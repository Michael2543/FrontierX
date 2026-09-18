#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/ProfileDocument.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iterator>
#include <limits>
#include <new>
#include <string>
#include <string_view>
#include <vector>

namespace FrontierX
{
NativeComponentManager::~NativeComponentManager()
{
    if (ShutDown() != GR_OK)
    {
        // A module reported live objects. Keep its code mapped instead of
        // leaving those objects with invalid vtables during manager teardown.
        for (LoadedLibrary & loaded : libraries_) loaded.library.Abandon();
        registrations_.clear();
        libraries_.clear();
    }
}

GENRESULT NativeComponentManager::QueryInterface(const C8 * interfaceName, void ** instance)
{
    if (!instance) return GR_INVALID_PARMS;
    *instance = nullptr;
    if (!interfaceName || std::strcmp(interfaceName, "ICOManager") != 0)
        return GR_INTERFACE_UNSUPPORTED;
    *instance = static_cast<ICOManager *>(this);
    AddRef();
    return GR_OK;
}

U32 NativeComponentManager::AddRef()
{
    return 1;
}

U32 NativeComponentManager::Release()
{
    return 1;
}

GENRESULT NativeComponentManager::CreateInstance(DACOMDESC * descriptor, void ** instance)
{
    if (!instance) return GR_INVALID_PARMS;
    *instance = nullptr;
    if (!descriptor || !descriptor->interface_name ||
        std::strcmp(descriptor->interface_name, "ICOManager") == 0)
    {
        *instance = static_cast<ICOManager *>(this);
        AddRef();
        return GR_OK;
    }

    GENRESULT result = GR_INTERFACE_UNSUPPORTED;
    for (const Registration & registration : registrations_)
    {
        if (std::strcmp(registration.interfaceName, descriptor->interface_name) != 0)
            continue;
        result = registration.component->CreateInstance(descriptor, instance);
        if (result == GR_OK) break;
    }
    return result;
}

GENRESULT NativeComponentManager::RegisterComponent(IComponentFactory * component,
                                                     const C8 * interfaceName,
                                                     U32 priority)
{
    if (!component || !interfaceName) return GR_INVALID_PARMS;
    const auto position = std::find_if(registrations_.begin(), registrations_.end(),
        [priority](const Registration & registration)
        {
            return priority >= registration.priority;
        });
    try
    {
        registrations_.insert(position,
            Registration{component, interfaceName, priority, activeLibraryId_});
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_SPACE;
    }
    component->AddRef();
    return GR_OK;
}

GENRESULT NativeComponentManager::UnregisterComponent(IComponentFactory * component,
                                                       const C8 * interfaceName)
{
    if (!component) return GR_INVALID_PARMS;
    for (auto position = registrations_.rbegin(); position != registrations_.rend(); ++position)
    {
        if (position->component != component ||
            (interfaceName && std::strcmp(interfaceName, position->interfaceName) != 0))
            continue;
        component->Release();
        registrations_.erase(std::next(position).base());
        return GR_OK;
    }
    return GR_INVALID_PARMS;
}

GENRESULT NativeComponentManager::EnumerateComponents(const C8 * interfaceName,
                                                       DACOMENUMCALLBACK callback,
                                                       void * context)
{
    if (!callback) return GR_INVALID_PARMS;
    for (const Registration & registration : registrations_)
    {
        if (interfaceName && std::strcmp(interfaceName, registration.interfaceName) != 0)
            continue;
        if (!callback(registration.component, registration.interfaceName,
                      registration.priority, context))
            break;
    }
    return GR_OK;
}

GENRESULT NativeComponentManager::AddLibrary(const C8 * filename)
{
    if (!filename || !*filename) return GR_INVALID_PARMS;
    if (activeLibraryId_ != 0) return GR_GENERIC;

    const auto existing = std::find_if(libraries_.begin(), libraries_.end(),
        [filename](const LoadedLibrary & loaded)
        {
            return loaded.filename == filename;
        });
    if (existing != libraries_.end())
    {
        if (existing->references == std::numeric_limits<U32>::max())
            return GR_OUT_OF_SPACE;
        ++existing->references;
        return GR_OK;
    }
    if (nextLibraryId_ == 0 ||
        nextLibraryId_ == std::numeric_limits<std::size_t>::max())
        return GR_OUT_OF_SPACE;

    std::string error;
    DynamicLibrary library;
    if (!library.Open(filename, error)) return GR_GENERIC;

    void * registerSymbol = library.Symbol(NativeComponentRegisterSymbol, error);
    if (!registerSymbol) return GR_GENERIC;
    void * canUnloadSymbol = library.Symbol(NativeComponentCanUnloadSymbol, error);
    if (!canUnloadSymbol) return GR_GENERIC;

    NativeComponentRegister registerComponents = nullptr;
    NativeComponentCanUnload canUnload = nullptr;
    static_assert(sizeof(registerComponents) == sizeof(registerSymbol),
        "native module register pointer has an incompatible representation");
    static_assert(sizeof(canUnload) == sizeof(canUnloadSymbol),
        "native module unload pointer has an incompatible representation");
    std::memcpy(&registerComponents, &registerSymbol, sizeof(registerComponents));
    std::memcpy(&canUnload, &canUnloadSymbol, sizeof(canUnload));

    const std::size_t libraryId = nextLibraryId_++;
    try
    {
        libraries_.push_back(
            LoadedLibrary{filename, std::move(library), canUnload, libraryId, 1});
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_SPACE;
    }

    activeLibraryId_ = libraryId;
    GENRESULT result = GR_GENERIC;
    try
    {
        result = registerComponents(this);
    }
    catch (...)
    {
        result = GR_GENERIC;
    }
    activeLibraryId_ = 0;
    const bool registered = std::any_of(registrations_.begin(), registrations_.end(),
        [libraryId](const Registration & registration)
        {
            return registration.libraryId == libraryId;
        });
    if (result != GR_OK || !registered)
    {
        RemoveRegistrations(libraryId);
        if (!CanUnload(libraries_.back())) libraries_.back().library.Abandon();
        libraries_.pop_back();
        return result == GR_OK ? GR_GENERIC : result;
    }
    return GR_OK;
}

GENRESULT NativeComponentManager::RemoveLibrary(const C8 * filename)
{
    if (!filename || !*filename) return GR_INVALID_PARMS;
    const auto position = std::find_if(libraries_.begin(), libraries_.end(),
        [filename](const LoadedLibrary & loaded)
        {
            return loaded.filename == filename;
        });
    if (position == libraries_.end()) return GR_GENERIC;
    if (position->references > 1)
    {
        --position->references;
        return GR_OK;
    }
    if (!CanUnload(*position)) return GR_GENERIC;

    RemoveRegistrations(position->id);
    std::string error;
    if (!position->library.Close(error)) return GR_GENERIC;
    libraries_.erase(position);
    return GR_OK;
}

GENRESULT NativeComponentManager::ShutDown()
{
    for (const LoadedLibrary & loaded : libraries_)
    {
        if (!CanUnload(loaded)) return GR_GENERIC;
    }
    for (auto position = registrations_.rbegin(); position != registrations_.rend(); ++position)
        position->component->Release();
    registrations_.clear();

    GENRESULT result = GR_OK;
    std::string error;
    for (auto position = libraries_.rbegin(); position != libraries_.rend(); ++position)
    {
        if (!position->library.Close(error)) result = GR_GENERIC;
    }
    libraries_.erase(
        std::remove_if(libraries_.begin(), libraries_.end(),
            [](const LoadedLibrary & loaded) { return !loaded.library.IsOpen(); }),
        libraries_.end());
    if (result == GR_OK) configured_ = false;
    return result;
}

GENRESULT NativeComponentManager::SetINIConfig(const C8 * info, U32 flags)
{
    if (!info || (flags & (DACOM_INI_WRITABLE | DACOM_INI_STRING)) ==
        (DACOM_INI_WRITABLE | DACOM_INI_STRING))
        return GR_INVALID_PARMS;
    try
    {
        return ConfigureLibraries(info, flags);
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_SPACE;
    }
    catch (const std::filesystem::filesystem_error &)
    {
        return GR_INVALID_PARMS;
    }
    catch (...)
    {
        return GR_GENERIC;
    }
}

GENRESULT NativeComponentManager::ConfigureLibraries(const C8 * info, U32 flags)
{
    if (flags & ~(DACOM_INI_WRITABLE | DACOM_INI_STRING)) return GR_INVALID_PARMS;
    if (flags & DACOM_INI_WRITABLE) return GR_NOT_IMPLEMENTED;

    ProfileDocument document;
    std::filesystem::path baseDirectory;
    std::string error;
    if (flags & DACOM_INI_STRING)
    {
        std::size_t length = 0;
        while (length <= ProfileDocument::MaxBytes && info[length]) ++length;
        if (length > ProfileDocument::MaxBytes ||
            !document.LoadText(std::string_view(info, length), error))
            return GR_INVALID_PARMS;
    }
    else
    {
        const std::filesystem::path path(info);
        if (!document.LoadFile(path, error)) return GR_INVALID_PARMS;
        baseDirectory = path.parent_path();
    }

    bool hasLibraries = false;
    for (const std::string & section : document.Sections())
    {
        if (section.size() != 9) continue;
        const char * expected = "Libraries";
        bool equal = true;
        for (std::size_t index = 0; index < section.size(); ++index)
        {
            const unsigned char left = static_cast<unsigned char>(section[index]);
            const unsigned char right = static_cast<unsigned char>(expected[index]);
            const unsigned char folded = left >= 'A' && left <= 'Z' ?
                static_cast<unsigned char>(left + ('a' - 'A')) : left;
            const unsigned char foldedExpected = right >= 'A' && right <= 'Z' ?
                static_cast<unsigned char>(right + ('a' - 'A')) : right;
            if (folded != foldedExpected) { equal = false; break; }
        }
        if (equal) { hasLibraries = true; break; }
    }
    if (!hasLibraries) return GR_DATA_NOT_FOUND;
    if (configured_) return GR_OK;

    struct LibraryOperation
    {
        std::string filename;
        bool remove = false;
    };
    std::vector<LibraryOperation> operations;
    for (std::string line : document.Lines("Libraries"))
    {
        const std::size_t first = line.find_first_not_of(' ');
        if (first == std::string::npos) continue;
        line.erase(0, first);
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        bool remove = false;
        if (line[0] == '-')
        {
            remove = true;
            line.erase(0, 1);
            const std::size_t nameStart = line.find_first_not_of(' ');
            if (nameStart == std::string::npos) continue;
            line.erase(0, nameStart);
        }

        std::string filename;
        const std::size_t quote = line.find('"');
        if (quote != std::string::npos)
        {
            const std::size_t close = line.find('"', quote + 1);
            if (close == std::string::npos) return GR_INVALID_PARMS;
            filename = line.substr(quote + 1, close - quote - 1);
        }
        else
        {
            const std::size_t end = line.find_first_of(" \t\r");
            filename = line.substr(0, end);
        }
        if (filename.empty()) continue;

        std::filesystem::path libraryPath(filename);
        if (libraryPath.is_relative() && !baseDirectory.empty())
            libraryPath = baseDirectory / libraryPath;
        libraryPath = libraryPath.lexically_normal();
        try
        {
            operations.push_back(LibraryOperation{libraryPath.string(), remove});
        }
        catch (const std::bad_alloc &)
        {
            return GR_OUT_OF_SPACE;
        }
    }

    for (std::size_t index = 0; index < operations.size(); ++index)
    {
        const LibraryOperation & operation = operations[index];
        const GENRESULT result = operation.remove ? RemoveLibrary(operation.filename.c_str()) :
                                                    AddLibrary(operation.filename.c_str());
        if (result == GR_OK) continue;

        bool rollbackSucceeded = true;
        while (index > 0)
        {
            --index;
            const LibraryOperation & applied = operations[index];
            const GENRESULT rollback = applied.remove ? AddLibrary(applied.filename.c_str()) :
                                                        RemoveLibrary(applied.filename.c_str());
            if (rollback != GR_OK) rollbackSucceeded = false;
        }
        return rollbackSucceeded ? result : GR_GENERIC;
    }
    configured_ = true;
    return GR_OK;
}

std::size_t NativeComponentManager::RegisteredCount() const noexcept
{
    return registrations_.size();
}

void NativeComponentManager::RemoveRegistrations(std::size_t libraryId) noexcept
{
    for (auto position = registrations_.rbegin(); position != registrations_.rend();)
    {
        if (position->libraryId != libraryId)
        {
            ++position;
            continue;
        }
        position->component->Release();
        position = std::vector<Registration>::reverse_iterator(
            registrations_.erase(std::next(position).base()));
    }
}

bool NativeComponentManager::CanUnload(const LoadedLibrary & loaded) noexcept
{
    if (!loaded.canUnload) return false;
    try
    {
        return loaded.canUnload() != FALSE;
    }
    catch (...)
    {
        return false;
    }
}
}
