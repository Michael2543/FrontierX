#include "FrontierX/NativeUtfFileSystem.h"

#include "FrontierX/EncodedHandle.h"
#include "FrontierX/UtfMetadata.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <fstream>
#include <limits>
#include <mutex>
#include <memory>
#include <new>
#include <string>
#include <utility>
#include <vector>

namespace FrontierX
{
namespace
{
constexpr DWORD InvalidFileResult = 0xffffffffU;

char AsciiLower(char value)
{
    return value >= 'A' && value <= 'Z' ? static_cast<char>(value + ('a' - 'A')) : value;
}

bool MatchesSpec(const char * pattern, const std::string & name)
{
    if (!pattern) return false;
    if (std::strcmp(pattern, "*.*") == 0) return true;
    const char * star = nullptr;
    std::size_t retry = 0;
    std::size_t index = 0;
    while (index < name.size())
    {
        if (*pattern == '*' ) { star = pattern++; retry = index; }
        else if (*pattern == '?' ||
                 (*pattern && AsciiLower(*pattern) == AsciiLower(name[index])))
        { ++pattern; ++index; }
        else if (star) { pattern = star + 1; index = ++retry; }
        else return false;
    }
    while (*pattern == '*') ++pattern;
    return *pattern == '\0';
}

bool ResolveVirtualPath(const std::string & current, const char * name,
                        bool allowRoot, std::string & resolved,
                        const std::string & root = {})
{
    if (!name || !*name) return false;
    std::string path(name);
    if (path.size() > 4096) return false;
    std::replace(path.begin(), path.end(), '\\', '/');
    const bool absolute = path.front() == '/';
    if (absolute) path.erase(path.begin());
    if (path.empty())
    {
        if (!absolute || !allowRoot) return false;
        resolved = root;
        return true;
    }
    std::size_t start = 0;
    while (start < path.size())
    {
        const std::size_t end = path.find('/', start);
        const std::string part = path.substr(start, end - start);
        if (part.empty() || part == "." || part == "..") return false;
        if (end == std::string::npos) break;
        start = end + 1;
        if (start == path.size()) return false;
    }
    resolved = absolute ? (root.empty() ? std::move(path) : root + "/" + path) :
        current.empty() ? std::move(path) : current + "/" + path;
    return resolved.size() <= 4096;
}

class NativeUtfFileSystem final : public IFileSystem
{
public:
    explicit NativeUtfFileSystem(std::filesystem::path path, std::string directory = {},
        std::shared_ptr<const std::vector<unsigned char>> archive = {}, bool physical = false,
        IFileSystem * parent = nullptr,
        std::shared_ptr<const UtfArchiveIndex> diskIndex = {},
        U32 physicalFileLimit = static_cast<U32>(NativePhysicalFileLimit::Standard))
        : path_(std::move(path)), rootDirectory_(directory), directory_(std::move(directory)), archive_(std::move(archive)),
          diskIndex_(std::move(diskIndex)), physical_(physical), physicalFileLimit_(physicalFileLimit), parent_(parent)
    { if (parent_) parent_->AddRef(); }

    NativeUtfFileSystem(std::filesystem::path path, std::string entry,
                        std::vector<unsigned char> bytes)
        : path_(path / entry), directory_(std::move(entry)), fileView_(true)
    {
        implicitFile_.bytes = std::move(bytes);
        implicitFile_.open = true;
    }

    GENRESULT QueryInterface(const C8 * name, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (name && (std::strcmp(name, IID_IFileSystem) == 0 ||
            std::strcmp(name, IID_IComponentFactory) == 0 ||
            std::strcmp(name, IID_IDAComponent) == 0))
        {
            *instance = static_cast<IFileSystem *>(this);
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override { return ++references_; }
    U32 Release() override
    {
        const U32 remaining = --references_;
        if (remaining == 0) delete this;
        return remaining;
    }

    GENRESULT CreateInstance(DACOMDESC * descriptor, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (!descriptor || descriptor->size < sizeof(DAFILEDESC) ||
            !descriptor->interface_name ||
            (std::strcmp(descriptor->interface_name, "IFileSystem") != 0 &&
             std::strcmp(descriptor->interface_name, IID_IFileSystem) != 0))
            return GR_INVALID_PARMS;
        const auto * file = static_cast<const DAFILEDESC *>(descriptor);
        if (fileView_ || file->dwCreationDistribution != OPEN_EXISTING ||
            (file->lpImplementation && std::strcmp(file->lpImplementation, "DOS") != 0 &&
             std::strcmp(file->lpImplementation, "UTF") != 0))
            return GR_INVALID_PARMS;
        // MScript asks the archive for a DOS root view before loading its table.
        if (file->lpFileName && std::strcmp(file->lpFileName, "\\") == 0)
        {
            if (!file->lpImplementation ||
                std::strcmp(file->lpImplementation, "DOS") != 0 ||
                file->dwDesiredAccess != GENERIC_READ ||
                file->dwCreationDistribution != OPEN_EXISTING || fileView_)
                return GR_INVALID_PARMS;
            try
            {
                *instance = static_cast<IFileSystem *>(new NativeUtfFileSystem(path_, rootDirectory_, archive_, physical_, nullptr, diskIndex_, physicalFileLimit_));
                return GR_OK;
            }
            catch (const std::bad_alloc &)
            {
                return GR_OUT_OF_MEMORY;
            }
        }
        if (!file->lpFileName || !*file->lpFileName ||
            (physical_ && (file->lpFileName[0] == '/' || file->lpFileName[0] == '\\')) ||
            file->dwDesiredAccess != GENERIC_READ)
            return GR_INVALID_PARMS;
        try
        {
            std::string directory;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (!ResolveVirtualPath(directory_, file->lpFileName, false, directory, rootDirectory_))
                    return GR_INVALID_PARMS;
            }
            std::vector<UtfDirectoryEntry> entries;
            std::string error;
            if (ListDirectory(directory, entries, error))
            {
                if (physical_)
                {
                    std::filesystem::path childRoot;
                    if (!ResolvePhysical(directory, childRoot, error))
                        return GR_DATA_NOT_FOUND;
                    *instance = static_cast<IFileSystem *>(
                        new NativeUtfFileSystem(std::move(childRoot), {}, {}, true, nullptr, {}, physicalFileLimit_));
                }
                else
                    *instance = static_cast<IFileSystem *>(
                        new NativeUtfFileSystem(path_, std::move(directory), archive_, false, nullptr, diskIndex_));
            }
            else
            {
                std::vector<unsigned char> bytes;
                if (!ReadEntry(directory, bytes, error) ||
                    bytes.size() > std::numeric_limits<DWORD>::max())
                    return GR_DATA_NOT_FOUND;
                const bool forceUtf = file->lpImplementation &&
                    std::strcmp(file->lpImplementation, "UTF") == 0;
                const bool forceDos = file->lpImplementation &&
                    std::strcmp(file->lpImplementation, "DOS") == 0;
                const bool utfSignature = bytes.size() >= 4 &&
                    std::memcmp(bytes.data(), "UTF ", 4) == 0;
                if (forceUtf || (!forceDos && utfSignature))
                {
                    UtfMetadata metadata;
                    if (!InspectUtfMetadata(bytes, metadata, error)) return GR_GENERIC;
                    auto nested = std::make_shared<const std::vector<unsigned char>>(std::move(bytes));
                    *instance = static_cast<IFileSystem *>(
                        new NativeUtfFileSystem(path_ / directory, {}, std::move(nested)));
                }
                else
                    *instance = static_cast<IFileSystem *>(
                        new NativeUtfFileSystem(path_, std::move(directory), std::move(bytes)));
            }
            return GR_OK;
        }
        catch (const std::bad_alloc &)
        {
            return GR_OUT_OF_MEMORY;
        }
    }

    BOOL CloseHandle(HANDLE handle) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (fileView_ && DecodeEncodedHandle(handle) == 4 && mappingOpen_)
        {
            mappingOpen_ = false;
            mapped_ = false;
            return TRUE;
        }
        Child * child = Resolve(handle);
        if (!child) return FALSE;
        child->open = false;
        child->bytes.clear();
        child->position = 0;
        return TRUE;
    }

    BOOL ReadFile(HANDLE handle, LPVOID buffer, DWORD count, LPDWORD bytesRead,
                  LPOVERLAPPED overlapped) override
    {
        if (bytesRead) *bytesRead = 0;
        if (overlapped || (count != 0 && !buffer)) return FALSE;
        std::lock_guard<std::mutex> lock(mutex_);
        Child * child = Resolve(handle);
        if (!child) return FALSE;
        const std::size_t available = child->bytes.size() - child->position;
        const std::size_t copied = std::min<std::size_t>(available, count);
        if (copied != 0)
            std::memcpy(buffer, child->bytes.data() + child->position, copied);
        child->position += copied;
        if (bytesRead) *bytesRead = static_cast<DWORD>(copied);
        return TRUE;
    }

    BOOL WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED) override { return FALSE; }
    BOOL GetOverlappedResult(HANDLE, LPOVERLAPPED, LPDWORD, BOOL) override { return FALSE; }

    DWORD SetFilePointer(HANDLE handle, LONG distance, PLONG high, DWORD method) override
    {
        if ((high && *high != 0) || method != FILE_BEGIN || distance < 0)
            return InvalidFileResult;
        std::lock_guard<std::mutex> lock(mutex_);
        Child * child = Resolve(handle);
        if (!child || static_cast<std::size_t>(distance) > child->bytes.size())
            return InvalidFileResult;
        child->position = static_cast<std::size_t>(distance);
        if (high) *high = 0;
        return static_cast<DWORD>(child->position);
    }

    BOOL SetEndOfFile(HANDLE) override { return FALSE; }
    DWORD GetFileSize(HANDLE handle, LPDWORD high) override
    {
        if (high) *high = 0;
        std::lock_guard<std::mutex> lock(mutex_);
        Child * child = Resolve(handle);
        return child ? static_cast<DWORD>(child->bytes.size()) : InvalidFileResult;
    }
    BOOL LockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return FALSE; }
    BOOL UnlockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return FALSE; }
    BOOL GetFileTime(HANDLE, LPFILETIME, LPFILETIME, LPFILETIME) override { return FALSE; }
    BOOL SetFileTime(HANDLE, const FILETIME *, const FILETIME *, const FILETIME *) override
    {
        return FALSE;
    }
    HANDLE CreateFileMapping(HANDLE file, LPSECURITY_ATTRIBUTES attributes, DWORD protection,
                             DWORD sizeHigh, DWORD sizeLow, LPCTSTR name) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!fileView_ || file || attributes || protection != PAGE_READONLY ||
            sizeHigh || sizeLow || name || !implicitFile_.open ||
            implicitFile_.bytes.empty() || mappingOpen_)
            return INVALID_HANDLE_VALUE;
        mappingOpen_ = true;
        return EncodeHandleValue(4);
    }
    LPVOID MapViewOfFile(HANDLE mapping, DWORD access, DWORD offsetHigh,
                         DWORD offsetLow, DWORD count) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!mappingOpen_ || DecodeEncodedHandle(mapping) != 4 ||
            access != FILE_MAP_READ || offsetHigh || offsetLow ||
            (count && count > implicitFile_.bytes.size()))
            return nullptr;
        mapped_ = true;
        return implicitFile_.bytes.data();
    }
    BOOL UnmapViewOfFile(LPCVOID address) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!mappingOpen_ || !mapped_ || address != implicitFile_.bytes.data())
            return FALSE;
        mapped_ = false;
        return TRUE;
    }
    HANDLE FindFirstFile(LPCTSTR pattern, LPWIN32_FIND_DATA data) override
    {
        if (!pattern || !data || fileView_) return INVALID_HANDLE_VALUE;
        try
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::vector<UtfDirectoryEntry> entries;
            std::string error;
            if (!ListDirectory(directory_, entries, error))
            {
                last_error_ = 3;
                return INVALID_HANDLE_VALUE;
            }
            for (const UtfDirectoryEntry & entry : entries)
                if (entry.name.size() >= sizeof(data->cFileName))
                {
                    last_error_ = 3;
                    return INVALID_HANDLE_VALUE;
                }
            entries.erase(std::remove_if(entries.begin(), entries.end(),
                [&](const UtfDirectoryEntry & entry) {
                    return !MatchesSpec(pattern, entry.name);
                }), entries.end());
            if (entries.empty())
            {
                last_error_ = ERROR_FILE_NOT_FOUND;
                return INVALID_HANDLE_VALUE;
            }
            std::size_t index = 0;
            for (; index < searches_.size(); ++index)
                if (!searches_[index].open) break;
            if (index == searches_.size()) searches_.emplace_back();
            if (nextSearchToken_ > (std::numeric_limits<EncodedHandleValue>::max)() - 2)
                return INVALID_HANDLE_VALUE;
            searches_[index].entries = std::move(entries);
            searches_[index].position = 0;
            searches_[index].open = true;
            searches_[index].token = nextSearchToken_;
            nextSearchToken_ += 2;
            FillFindData(searches_[index].entries[0], data);
            last_error_ = 0;
            return EncodeHandleValue(searches_[index].token);
        }
        catch (const std::bad_alloc &)
        {
            last_error_ = 3;
            return INVALID_HANDLE_VALUE;
        }
    }
    BOOL FindNextFile(HANDLE handle, LPWIN32_FIND_DATA data) override
    {
        if (!data) return FALSE;
        std::lock_guard<std::mutex> lock(mutex_);
        Search * search = ResolveSearch(handle);
        if (!search) return FALSE;
        if (search->position + 1 >= search->entries.size())
        {
            last_error_ = ERROR_NO_MORE_FILES;
            return FALSE;
        }
        FillFindData(search->entries[++search->position], data);
        last_error_ = 0;
        return TRUE;
    }
    BOOL FindClose(HANDLE handle) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        Search * search = ResolveSearch(handle);
        if (!search) return FALSE;
        search->entries.clear();
        search->open = false;
        return TRUE;
    }
    BOOL CreateDirectory(LPCTSTR, LPSECURITY_ATTRIBUTES) override { return FALSE; }
    BOOL RemoveDirectory(LPCTSTR) override { return FALSE; }
    DWORD GetCurrentDirectory(DWORD size, LPTSTR buffer) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const std::string relative = directory_.substr(rootDirectory_.size());
        const std::string current = relative.empty() ? "/" :
            relative.front() == '/' ? relative : "/" + relative;
        if (!buffer || size <= current.size()) return static_cast<DWORD>(current.size() + 1);
        std::memcpy(buffer, current.c_str(), current.size() + 1);
        return static_cast<DWORD>(current.size());
    }
    BOOL SetCurrentDirectory(LPCTSTR name) override
    {
        if (!name || !*name || fileView_) return FALSE;
        try
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::string candidate;
            if (std::strcmp(name, "/") == 0 ||
                std::strcmp(name, "\\") == 0) candidate = rootDirectory_;
            else if (std::strcmp(name, "..") == 0)
            {
                if (directory_ == rootDirectory_) return FALSE;
                const auto slash = directory_.rfind('/');
                candidate = slash == std::string::npos ? "" : directory_.substr(0, slash);
            }
            else
            {
                if (!ResolveVirtualPath(directory_, name, true, candidate, rootDirectory_))
                    return FALSE;
            }
            std::vector<UtfDirectoryEntry> entries;
            std::string error;
            if (!ListDirectory(candidate, entries, error)) return FALSE;
            directory_ = std::move(candidate);
            return TRUE;
        }
        catch (const std::bad_alloc &)
        {
            last_error_ = 3;
            return FALSE;
        }
    }
    BOOL DeleteFile(LPCTSTR) override { return FALSE; }
    BOOL CopyFile(LPCTSTR, LPCTSTR, BOOL) override { return FALSE; }
    BOOL MoveFile(LPCTSTR, LPCTSTR) override { return FALSE; }
    DWORD GetFileAttributes(LPCTSTR) override { return InvalidFileResult; }
    BOOL SetFileAttributes(LPCTSTR, DWORD) override { return FALSE; }
    DWORD GetLastError() override { return last_error_; }

    HANDLE OpenChild(DAFILEDESC * descriptor) override
    {
        if (fileView_ || !descriptor || !descriptor->lpFileName || !descriptor->lpFileName[0] ||
            descriptor->dwDesiredAccess != GENERIC_READ)
        {
            last_error_ = 1;
            return INVALID_HANDLE_VALUE;
        }

        std::vector<unsigned char> bytes;
        std::string error;
        try
        {
            std::string entry;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (!ResolveVirtualPath(directory_, descriptor->lpFileName,
                        false, entry, rootDirectory_))
                {
                    last_error_ = 1;
                    return INVALID_HANDLE_VALUE;
                }
            }
            if (!ReadEntry(entry, bytes, error) ||
                bytes.size() > std::numeric_limits<DWORD>::max())
            {
                last_error_ = 2;
                return INVALID_HANDLE_VALUE;
            }
        }
        catch (const std::bad_alloc &)
        {
            last_error_ = 3;
            return INVALID_HANDLE_VALUE;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        std::size_t index = 0;
        for (; index < children_.size(); ++index)
            if (!children_[index].open) break;
        if (index == children_.size())
        {
            try
            {
                children_.emplace_back();
            }
            catch (const std::bad_alloc &)
            {
                last_error_ = 3;
                return INVALID_HANDLE_VALUE;
            }
        }
        children_[index].bytes = std::move(bytes);
        children_[index].position = 0;
        children_[index].open = true;
        if (nextChildToken_ > (std::numeric_limits<EncodedHandleValue>::max)() - 2)
        {
            children_[index].open = false;
            children_[index].bytes.clear();
            return INVALID_HANDLE_VALUE;
        }
        children_[index].token = nextChildToken_;
        nextChildToken_ += 2;
        last_error_ = 0;
        return EncodeHandleValue(children_[index].token);
    }

    DWORD GetFilePosition(HANDLE handle, PLONG high) override
    {
        if (high) *high = 0;
        std::lock_guard<std::mutex> lock(mutex_);
        Child * child = Resolve(handle);
        return child ? static_cast<DWORD>(child->position) : InvalidFileResult;
    }

    LONG GetFileName(LPSTR buffer, LONG bufferSize) override
    {
        const std::string name = path_.string();
        const std::size_t available = bufferSize > 0 ? static_cast<std::size_t>(bufferSize) : 0;
        const std::size_t copied = std::min(available, name.size() + 1);
        if (copied != 0 && buffer)
        {
            std::memcpy(buffer, name.c_str(), copied);
            buffer[copied - 1] = '\0';
        }
        return static_cast<LONG>(copied);
    }

    DWORD GetAccessType() override { return GENERIC_READ; }
    GENRESULT GetParentSystem(LPFILESYSTEM * parent) override
    {
        if (!parent) return GR_INVALID_PARMS;
        *parent = parent_;
        if (!parent_) return GR_DATA_NOT_FOUND;
        parent_->AddRef();
        return GR_OK;
    }
    GENRESULT SetPreference(DWORD, DWORD) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT GetPreference(DWORD, PDWORD) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT ReadDirectoryExtension(HANDLE, LPVOID, DWORD, LPDWORD, DWORD) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT WriteDirectoryExtension(HANDLE, LPCVOID, DWORD, LPDWORD, DWORD) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    LONG SerialCall(LPFILESYSTEM system, DAFILE_SERIAL_PROC procedure, void * context) override
    {
        return system && procedure ? (system->*procedure)(context) : 0;
    }
    BOOL GetAbsolutePath(char *, LPCTSTR, LONG) override { return FALSE; }

private:
    ~NativeUtfFileSystem() { if (parent_) parent_->Release(); }

    struct Child
    {
        std::vector<unsigned char> bytes;
        std::size_t position = 0;
        EncodedHandleValue token = 0;
        bool open = false;
    };
    struct Search
    {
        std::vector<UtfDirectoryEntry> entries;
        std::size_t position = 0;
        EncodedHandleValue token = 0;
        bool open = false;
    };

    static void FillFindData(const UtfDirectoryEntry & entry, WIN32_FIND_DATA * data)
    {
        *data = {};
        data->dwFileAttributes = entry.directory ? FILE_ATTRIBUTE_DIRECTORY :
            FILE_ATTRIBUTE_NORMAL;
        data->nFileSizeLow = entry.size;
        std::memcpy(data->cFileName, entry.name.c_str(), entry.name.size() + 1);
    }

    Child * Resolve(HANDLE handle)
    {
        if (!handle && fileView_ && implicitFile_.open) return &implicitFile_;
        const auto value = DecodeEncodedHandle(handle);
        if (value == 0 || (value & 1U) == 0) return nullptr;
        for (Child & child : children_)
            if (child.open && child.token == value) return &child;
        return nullptr;
    }
    Search * ResolveSearch(HANDLE handle)
    {
        const auto value = DecodeEncodedHandle(handle);
        if (value < 2 || (value & 1U) != 0) return nullptr;
        for (Search & search : searches_)
            if (search.open && search.token == value) return &search;
        return nullptr;
    }

    std::atomic<U32> references_{1};
    std::filesystem::path path_;
    std::mutex mutex_;
    // Match BaseUTF iRootIndex: each directory view owns its virtual root.
    std::string rootDirectory_;
    std::string directory_;
    std::shared_ptr<const std::vector<unsigned char>> archive_;
    std::shared_ptr<const UtfArchiveIndex> diskIndex_;
    bool physical_ = false;
    U32 physicalFileLimit_ = static_cast<U32>(NativePhysicalFileLimit::Standard);
    IFileSystem * parent_ = nullptr;
    bool ResolvePhysical(const std::string & relative, std::filesystem::path & resolved,
                         std::string & error) const
    {
        const std::filesystem::path requested(relative);
        if (requested.is_absolute() || relative.find('\\') != std::string::npos)
        { error = "invalid physical child path"; return false; }
        for (const auto &part : requested)
            if (part == "." || part == "..")
            { error = "physical child traversal is unsupported"; return false; }
        std::error_code status;
        resolved = std::filesystem::canonical(path_ / requested, status);
        if (status || std::mismatch(path_.begin(), path_.end(),
                resolved.begin(), resolved.end()).first != path_.end())
        { error = "physical child is missing or outside its root"; return false; }
        return true;
    }
    bool ListDirectory(const std::string & directory, std::vector<UtfDirectoryEntry> & entries,
                       std::string & error) const
    {
        if (physical_)
        {
            std::filesystem::path resolved;
            if (!ResolvePhysical(directory, resolved, error)) return false;
            std::error_code status;
            std::vector<UtfDirectoryEntry> found;
            for (std::filesystem::directory_iterator i(resolved, status), end;
                 !status && i != end; i.increment(status))
            {
                const auto kind = i->status(status);
                if (status) break;
                if (!std::filesystem::is_directory(kind) && !std::filesystem::is_regular_file(kind))
                    continue;
                if (found.size() >= 65536)
                { error = "physical directory exceeds entry limit"; return false; }
                found.push_back({i->path().filename().string(), std::filesystem::is_directory(kind)});
            }
            if (status) { error = "physical directory enumeration failed"; return false; }
            std::sort(found.begin(), found.end(), [](const auto &left, const auto &right) {
                return left.name < right.name;
            });
            entries.swap(found);
            error.clear();
            return true;
        }
        if (diskIndex_ && !archive_)
        {
            std::string exact;
            if (!ResolveIndexedUtfDirectory(*diskIndex_, directory, exact))
            { error = "UTF directory not found"; return false; }
            return ListIndexedUtfDirectory(*diskIndex_, exact, entries, error);
        }
        return archive_ ? ListUtfDirectory(*archive_, directory, entries, error) :
                          ListUtfDirectory(path_, directory, entries, error);
    }
    bool ReadEntry(const std::string & entry, std::vector<unsigned char> & bytes,
                   std::string & error) const
    {
        if (physical_)
        {
            std::filesystem::path resolved;
            if (!ResolvePhysical(entry, resolved, error)) return false;
            std::error_code status;
            if (!std::filesystem::is_regular_file(resolved, status) || status)
            { error = "physical child is not a regular file"; return false; }
            std::ifstream input(resolved, std::ios::binary | std::ios::ate);
            const auto size = input.tellg();
            if (!input || size < 0 || size > static_cast<std::streamoff>(physicalFileLimit_))
            { error = "physical file is unreadable or exceeds its directory read limit"; return false; }
            std::vector<unsigned char> data(static_cast<std::size_t>(size));
            input.seekg(0);
            if (!data.empty() && !input.read(reinterpret_cast<char *>(data.data()),
                    static_cast<std::streamsize>(data.size())))
            { error = "physical file read failed"; return false; }
            bytes.swap(data);
            error.clear();
            return true;
        }
        if (diskIndex_ && !archive_)
        {
            std::string exact;
            if (!ResolveIndexedUtfFile(*diskIndex_, entry, exact))
            { error = "UTF entry not found"; return false; }
            return ReadIndexedUtfEntry(path_, *diskIndex_, exact, bytes, error);
        }
        return archive_ ? ReadUtfEntry(*archive_, entry, bytes, error) :
                          ReadUtfEntry(path_, entry, bytes, error);
    }
    bool fileView_ = false;
    Child implicitFile_;
    bool mappingOpen_ = false;
    bool mapped_ = false;
    std::vector<Child> children_;
    std::vector<Search> searches_;
    EncodedHandleValue nextChildToken_ = 1;
    EncodedHandleValue nextSearchToken_ = 2;
    std::atomic<DWORD> last_error_{0};
};
}

bool OpenNativeUtfFileSystem(const std::filesystem::path & path,
                             IFileSystem ** output, std::string & error)
{
    if (!output)
    {
        error = "native UTF output interface is null";
        return false;
    }
    *output = nullptr;
    auto index = std::make_shared<UtfArchiveIndex>();
    if (!BuildUtfArchiveIndex(path, *index, error)) return false;
    try
    {
        *output = new NativeUtfFileSystem(path, {}, {}, false, nullptr, index);
    }
    catch (const std::bad_alloc &)
    {
        error = "cannot allocate native UTF file system";
        return false;
    }
    error.clear();
    return true;
}

bool OpenNativeUtfFileSystemWithParent(const std::filesystem::path & path,
                                      IFileSystem * parent, IFileSystem ** output,
                                      std::string & error)
{
    if (!output || !parent)
    { error = "saved UTF reader requires output and parent"; return false; }
    *output = nullptr;
    auto index = std::make_shared<UtfArchiveIndex>();
    if (!BuildUtfArchiveIndex(path, *index, error)) return false;
    try { *output = new NativeUtfFileSystem(path, {}, {}, false, parent, index); }
    catch (const std::bad_alloc &)
    { error = "cannot allocate saved UTF reader"; return false; }
    error.clear();
    return true;
}

bool OpenNativeDirectoryFileSystem(const std::filesystem::path & path,
                                   IFileSystem ** output, std::string & error,
                                   NativePhysicalFileLimit limit)
{
    if (!output) { error = "native directory output is null"; return false; }
    *output = nullptr;
    if (limit != NativePhysicalFileLimit::Standard && limit != NativePhysicalFileLimit::Movies)
    { error = "invalid native physical directory read limit"; return false; }
    std::error_code status;
    const auto root = std::filesystem::canonical(path, status);
    if (status || !std::filesystem::is_directory(root, status) || status)
    { error = "native directory root is inaccessible"; return false; }
    try { *output = new NativeUtfFileSystem(root, {}, {}, true, nullptr, {}, static_cast<U32>(limit)); }
    catch (const std::bad_alloc &)
    { error = "cannot allocate native directory file system"; return false; }
    error.clear();
    return true;
}
}
