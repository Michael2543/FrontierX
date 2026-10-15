#include "FrontierX/NativeWritableUtfArchive.h"

#include "FrontierX/EncodedHandle.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "FrontierX/UtfMetadata.h"
#include "IUTFWriter.h"

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <system_error>
#include <vector>

namespace FrontierX
{
namespace
{
constexpr DWORD InvalidPosition = 0xffffffffU;
constexpr DWORD MoveCurrent = 1;
constexpr DWORD MoveEnd = 2;

struct Entry
{
    bool directory = true;
    std::vector<unsigned char> bytes;
    std::uint64_t creationOrder = 0;
};

struct OpenFile
{
    std::string path;
    std::size_t position = 0;
    bool writable = true;
};

struct Search
{
    std::vector<std::string> names;
    std::size_t position = 0;
};

struct ArchiveState
{
    std::mutex mutex;
    std::map<std::string, Entry> entries{{"", {true, {}}}};
    std::uint64_t nextCreationOrder = 1;
    bool dirty = false;
    bool flushed = false;
};

bool ResolvePath(const std::string & current, const char * name,
                 bool allowRoot, std::string & result)
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
        result.clear();
        return true;
    }
    std::size_t start = 0;
    while (start < path.size())
    {
        const auto end = path.find('/', start);
        const auto part = path.substr(start, end - start);
        if (part.empty() || part == "." || part == ".." ||
            part.find('\0') != std::string::npos) return false;
        if (end == std::string::npos) break;
        start = end + 1;
        if (start == path.size()) return false;
    }
    result = absolute || current.empty() ? std::move(path) : current + "/" + path;
    return result.size() <= 4096;
}

bool DirectChild(const std::string & parent, const std::string & path,
                 std::string & name)
{
    const std::string prefix = parent.empty() ? "" : parent + "/";
    if (path.size() <= prefix.size() || path.compare(0, prefix.size(), prefix))
        return false;
    name = path.substr(prefix.size());
    return name.find('/') == std::string::npos;
}

bool Matches(const char * pattern, const std::string & name)
{
    if (!pattern) return false;
    if (std::strcmp(pattern, "*") == 0 || std::strcmp(pattern, "*.*") == 0)
        return true;
    const char * star = nullptr;
    std::size_t retry = 0;
    std::size_t index = 0;
    while (index < name.size())
    {
        if (*pattern == '*') { star = pattern++; retry = index; }
        else if (*pattern == '?' ||
                 (*pattern && *pattern == name[index]))
        { ++pattern; ++index; }
        else if (star) { pattern = star + 1; index = ++retry; }
        else return false;
    }
    while (*pattern == '*') ++pattern;
    return *pattern == '\0';
}

class WritableArchive final : public IFileSystem, public IUTFWriter
{
public:
    WritableArchive(std::filesystem::path path, bool saveDirectory)
        : path_(std::move(path)), state_(std::make_shared<ArchiveState>()),
          saveDirectory_(saveDirectory) {}

    explicit WritableArchive(std::filesystem::path path)
        : path_(std::move(path)), state_(std::make_shared<ArchiveState>()) {}

    WritableArchive(std::filesystem::path path, std::shared_ptr<ArchiveState> state,
                    std::string filePath, bool writable = true)
        : path_(std::move(path)), state_(std::move(state)), filePath_(std::move(filePath)),
          fileView_(true) { implicitFile_.writable = writable; }

    WritableArchive(std::filesystem::path path, std::shared_ptr<ArchiveState> state,
                    std::string directory, IFileSystem * parent)
        : path_(std::move(path)), state_(std::move(state)),
          directory_(std::move(directory)), directoryView_(true), parent_(parent)
    { if (parent_) parent_->AddRef(); }

    WritableArchive(std::filesystem::path path, IFileSystem * parent)
        : path_(std::move(path)), state_(std::make_shared<ArchiveState>()),
          parent_(parent)
    { if (parent_) parent_->AddRef(); }

    WritableArchive(std::filesystem::path path, IFileSystem * parent, bool rawFile)
        : path_(std::move(path)), state_(std::make_shared<ArchiveState>()),
          fileView_(true), rawFile_(rawFile), parent_(parent)
    {
        state_->entries[""] = Entry{false, {}};
        if (parent_) parent_->AddRef();
    }

    GENRESULT QueryInterface(const C8 * name, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (!name) return GR_INTERFACE_UNSUPPORTED;
        if (std::strcmp(name, IID_IFileSystem) == 0 ||
            std::strcmp(name, IID_IComponentFactory) == 0 ||
            std::strcmp(name, IID_IDAComponent) == 0)
            *instance = static_cast<IFileSystem *>(this);
        else if (!fileView_ && !saveDirectory_ && !directoryView_ &&
                 (std::strcmp(name, IID_IUTFWriter) == 0 ||
                  std::strcmp(name, "IUTFWriter") == 0))
            *instance = static_cast<IUTFWriter *>(this);
        if (!*instance) return GR_INTERFACE_UNSUPPORTED;
        AddRef();
        return GR_OK;
    }
    U32 AddRef() override { return ++references_; }
    U32 Release() override
    {
        const U32 remaining = --references_;
        if (!remaining) delete this;
        return remaining;
    }

    GENRESULT CreateInstance(DACOMDESC * descriptor, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (fileView_ || !descriptor || descriptor->size < sizeof(DAFILEDESC) ||
            !descriptor->interface_name ||
            (std::strcmp(descriptor->interface_name, "IFileSystem") != 0 &&
             std::strcmp(descriptor->interface_name, IID_IFileSystem) != 0))
            return GR_INVALID_PARMS;
        const auto * file = static_cast<const DAFILEDESC *>(descriptor);
        if (saveDirectory_)
        {
            if (!file->lpFileName || !*file->lpFileName ||
                std::strchr(file->lpFileName, '/') ||
                std::strchr(file->lpFileName, '\\') ||
                std::strcmp(file->lpFileName, ".") == 0 ||
                std::strcmp(file->lpFileName, "..") == 0)
                return GR_INVALID_PARMS;
            const auto childPath = path_ / file->lpFileName;
            if (file->lpImplementation && std::strcmp(file->lpImplementation, "DOS") == 0)
            {
                if (file->dwCreationDistribution == OPEN_EXISTING &&
                    file->dwDesiredAccess == GENERIC_READ)
                {
                    std::string error;
                    IFileSystem * directory = nullptr;
                    if (!OpenNativeDirectoryFileSystem(path_, &directory, error))
                        return GR_DATA_NOT_FOUND;
                    const auto result = directory->CreateInstance(descriptor, instance);
                    directory->Release();
                    return result;
                }
                if (file->dwCreationDistribution != CREATE_ALWAYS ||
                    file->dwDesiredAccess != (GENERIC_READ | GENERIC_WRITE))
                    return GR_NOT_IMPLEMENTED;
                std::error_code status;
                const auto kind = std::filesystem::symlink_status(childPath, status).type();
                if (status == std::errc::no_such_file_or_directory) status.clear();
                if (status || (kind != std::filesystem::file_type::not_found &&
                    kind != std::filesystem::file_type::regular)) return GR_GENERIC;
                std::ofstream output(childPath, std::ios::binary | std::ios::trunc);
                output.close();
                if (!output) return GR_GENERIC;
                try { *instance = static_cast<IFileSystem *>(
                    new WritableArchive(childPath, this, true)); }
                catch (const std::bad_alloc &) { return GR_OUT_OF_MEMORY; }
                return GR_OK;
            }
            if (file->lpImplementation &&
                std::strcmp(file->lpImplementation, "UTF") != 0)
                return GR_NOT_IMPLEMENTED;
            if (file->dwCreationDistribution == CREATE_ALWAYS &&
                file->dwDesiredAccess == (GENERIC_READ | GENERIC_WRITE))
            {
                try { *instance = static_cast<IFileSystem *>(
                    new WritableArchive(childPath, static_cast<IFileSystem *>(this))); }
                catch (const std::bad_alloc &) { return GR_OUT_OF_MEMORY; }
                return GR_OK;
            }
            if (file->dwCreationDistribution == OPEN_EXISTING &&
                file->dwDesiredAccess == GENERIC_READ)
            {
                std::string error;
                IFileSystem * reader = nullptr;
                if (!OpenNativeUtfFileSystemWithParent(childPath, this, &reader, error))
                    return GR_DATA_NOT_FOUND;
                *instance = reader;
                return GR_OK;
            }
            return GR_NOT_IMPLEMENTED;
        }
        const bool create = file->dwDesiredAccess == (GENERIC_READ | GENERIC_WRITE) &&
            file->dwCreationDistribution == CREATE_ALWAYS;
        const bool read = file->dwDesiredAccess == GENERIC_READ &&
            file->dwCreationDistribution == OPEN_EXISTING;
        if ((file->lpImplementation &&
             std::strcmp(file->lpImplementation, "DOS") != 0) ||
            (!create && !read))
            return GR_NOT_IMPLEMENTED;
        std::string resolved;
        if (!ResolvePath(directory_, file->lpFileName, false, resolved))
            return GR_INVALID_PARMS;
        try
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            auto found = state_->entries.find(resolved);
            if (read && found == state_->entries.end()) return GR_DATA_NOT_FOUND;
            if (create && !HasParentDirectory(resolved)) return GR_DATA_NOT_FOUND;
            if (found != state_->entries.end() && found->second.directory)
            {
                *instance = static_cast<IFileSystem *>(
                    new WritableArchive(path_, state_, resolved,
                                        static_cast<IFileSystem *>(this)));
                return GR_OK;
            }
            if (read)
            {
                *instance = static_cast<IFileSystem *>(
                    new WritableArchive(path_, state_, resolved, false));
                return GR_OK;
            }
            if (found == state_->entries.end())
                state_->entries.emplace(resolved, Entry{false, {}, state_->nextCreationOrder++});
            else
                found->second.bytes.clear();
            state_->dirty = true;
            state_->flushed = false;
            *instance = static_cast<IFileSystem *>(
                new WritableArchive(path_, state_, std::move(resolved)));
            return GR_OK;
        }
        catch (const std::bad_alloc &)
        { return GR_OUT_OF_MEMORY; }
    }

    BOOL CloseHandle(HANDLE handle) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return files_.erase(DecodeEncodedHandle(handle)) != 0;
    }
    BOOL ReadFile(HANDLE handle, LPVOID buffer, DWORD count, LPDWORD read,
                  LPOVERLAPPED overlapped) override
    {
        if (read) *read = 0;
        if (overlapped || (count && !buffer)) return FALSE;
        std::lock_guard<std::mutex> lock(mutex_);
        OpenFile * open = ResolveOpen(handle);
        if (!open) return FALSE;
        std::lock_guard<std::mutex> treeLock(state_->mutex);
        const auto entry = state_->entries.find(open->path);
        if (entry == state_->entries.end() || entry->second.directory ||
            open->position > entry->second.bytes.size()) return FALSE;
        const auto copied = std::min<std::size_t>(count,
            entry->second.bytes.size() - open->position);
        if (copied) std::memcpy(buffer, entry->second.bytes.data() + open->position, copied);
        open->position += copied;
        if (read) *read = static_cast<DWORD>(copied);
        return TRUE;
    }
    BOOL WriteFile(HANDLE handle, LPCVOID buffer, DWORD count, LPDWORD written,
                   LPOVERLAPPED overlapped) override
    {
        if (written) *written = 0;
        if (overlapped || (count && !buffer)) return FALSE;
        std::lock_guard<std::mutex> lock(mutex_);
        OpenFile * open = ResolveOpen(handle);
        if (!open || !open->writable) return FALSE;
        std::lock_guard<std::mutex> treeLock(state_->mutex);
        auto entry = state_->entries.find(open->path);
        if (entry == state_->entries.end() || entry->second.directory ||
            open->position > 64U * 1024U * 1024U ||
            count > 64U * 1024U * 1024U - open->position) return FALSE;
        try
        {
            const std::size_t end = open->position + count;
            if (end > entry->second.bytes.size()) entry->second.bytes.resize(end);
            if (count) std::memcpy(entry->second.bytes.data() + open->position, buffer, count);
            open->position = end;
            state_->dirty = true;
            state_->flushed = false;
            if (rawFile_)
            {
                std::ofstream output(path_, std::ios::binary | std::ios::trunc);
                if (!output || !output.write(
                    reinterpret_cast<const char *>(entry->second.bytes.data()),
                    static_cast<std::streamsize>(entry->second.bytes.size())) ||
                    !output.flush()) return FALSE;
                output.close();
                if (!output) return FALSE;
            }
            if (written) *written = count;
            return TRUE;
        }
        catch (const std::bad_alloc &)
        { return FALSE; }
    }
    BOOL GetOverlappedResult(HANDLE, LPOVERLAPPED, LPDWORD, BOOL) override { return FALSE; }
    DWORD SetFilePointer(HANDLE handle, LONG distance, PLONG high, DWORD method) override
    {
        if (high && *high) return InvalidPosition;
        std::lock_guard<std::mutex> lock(mutex_);
        OpenFile * open = ResolveOpen(handle);
        if (!open) return InvalidPosition;
        std::lock_guard<std::mutex> treeLock(state_->mutex);
        const auto entry = state_->entries.find(open->path);
        if (entry == state_->entries.end() || entry->second.directory) return InvalidPosition;
        std::int64_t base = 0;
        if (method == MoveCurrent) base = static_cast<std::int64_t>(open->position);
        else if (method == MoveEnd) base = static_cast<std::int64_t>(entry->second.bytes.size());
        else if (method != FILE_BEGIN) return InvalidPosition;
        const auto next = base + distance;
        if (next < 0 || next > 64 * 1024 * 1024) return InvalidPosition;
        open->position = static_cast<std::size_t>(next);
        if (high) *high = 0;
        return static_cast<DWORD>(open->position);
    }
    BOOL SetEndOfFile(HANDLE handle) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        OpenFile * open = ResolveOpen(handle);
        if (!open || !open->writable) return FALSE;
        std::lock_guard<std::mutex> treeLock(state_->mutex);
        auto entry = state_->entries.find(open->path);
        if (entry == state_->entries.end() || entry->second.directory) return FALSE;
        try { entry->second.bytes.resize(open->position); }
        catch (const std::bad_alloc &) { return FALSE; }
        state_->dirty = true;
        state_->flushed = false;
        return TRUE;
    }
    DWORD GetFileSize(HANDLE handle, LPDWORD high) override
    {
        if (high) *high = 0;
        std::lock_guard<std::mutex> lock(mutex_);
        OpenFile * open = ResolveOpen(handle);
        if (!open) return InvalidPosition;
        std::lock_guard<std::mutex> treeLock(state_->mutex);
        const auto entry = state_->entries.find(open->path);
        return entry == state_->entries.end() || entry->second.directory ?
            InvalidPosition : static_cast<DWORD>(entry->second.bytes.size());
    }
    BOOL LockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return FALSE; }
    BOOL UnlockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return FALSE; }
    BOOL GetFileTime(HANDLE, LPFILETIME, LPFILETIME, LPFILETIME) override { return FALSE; }
    BOOL SetFileTime(HANDLE, const FILETIME *, const FILETIME *, const FILETIME *) override
    { return FALSE; }
    HANDLE CreateFileMapping(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD,
                             LPCTSTR) override { return INVALID_HANDLE_VALUE; }
    LPVOID MapViewOfFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return nullptr; }
    BOOL UnmapViewOfFile(LPCVOID) override { return FALSE; }

    HANDLE FindFirstFile(LPCTSTR pattern, LPWIN32_FIND_DATA data) override
    {
        if (fileView_ || !pattern || !data) return INVALID_HANDLE_VALUE;
        std::lock_guard<std::mutex> lock(mutex_);
        if (saveDirectory_)
        {
            Search search;
            std::error_code error;
            for (std::filesystem::directory_iterator it(path_, error), end;
                 !error && it != end; it.increment(error))
            {
                if (!std::filesystem::is_regular_file(it->symlink_status(error)) || error)
                    continue;
                const auto name = it->path().filename().string();
                if (name.size() < sizeof(data->cFileName) && Matches(pattern, name))
                    search.names.push_back(name);
            }
            if (error || search.names.empty())
            { lastError_ = ERROR_FILE_NOT_FOUND; return INVALID_HANDLE_VALUE; }
            std::sort(search.names.begin(), search.names.end());
            const auto token = nextSearchToken_;
            nextSearchToken_ += 2;
            searches_.emplace(token, std::move(search));
            if (!FillPhysicalFindData(searches_.at(token).names[0], data))
            { searches_.erase(token); lastError_ = ERROR_FILE_NOT_FOUND;
              return INVALID_HANDLE_VALUE; }
            lastError_ = 0;
            return EncodeHandleValue(token);
        }
        std::lock_guard<std::mutex> treeLock(state_->mutex);
        Search search;
        for (const auto & entry : state_->entries)
        {
            std::string name;
            if (DirectChild(directory_, entry.first, name) && Matches(pattern, name))
                search.names.push_back(entry.first);
        }
        std::sort(search.names.begin(), search.names.end(), [&](const auto &a, const auto &b) {
            return state_->entries.at(a).creationOrder < state_->entries.at(b).creationOrder;
        });
        if (search.names.empty())
        { lastError_ = ERROR_FILE_NOT_FOUND; return INVALID_HANDLE_VALUE; }
        const auto token = nextSearchToken_;
        nextSearchToken_ += 2;
        searches_.emplace(token, std::move(search));
        FillFindData(searches_.at(token).names[0], data);
        lastError_ = 0;
        return EncodeHandleValue(token);
    }
    BOOL FindNextFile(HANDLE handle, LPWIN32_FIND_DATA data) override
    {
        if (!data) return FALSE;
        std::lock_guard<std::mutex> lock(mutex_);
        const auto found = searches_.find(DecodeEncodedHandle(handle));
        if (found == searches_.end()) return FALSE;
        if (++found->second.position >= found->second.names.size())
        { lastError_ = ERROR_NO_MORE_FILES; return FALSE; }
        if (saveDirectory_)
        {
            if (!FillPhysicalFindData(found->second.names[found->second.position], data))
            { lastError_ = ERROR_FILE_NOT_FOUND; return FALSE; }
        }
        else
        {
            std::lock_guard<std::mutex> treeLock(state_->mutex);
            FillFindData(found->second.names[found->second.position], data);
        }
        lastError_ = 0;
        return TRUE;
    }
    BOOL FindClose(HANDLE handle) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return searches_.erase(DecodeEncodedHandle(handle)) != 0;
    }
    BOOL CreateDirectory(LPCTSTR name, LPSECURITY_ATTRIBUTES attributes) override
    {
        if (fileView_ || attributes) return FALSE;
        std::string resolved;
        if (!ResolvePath(directory_, name, false, resolved)) return FALSE;
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (!HasParentDirectory(resolved) || state_->entries.count(resolved)) return FALSE;
        state_->entries.emplace(resolved, Entry{true, {}, state_->nextCreationOrder++});
        state_->dirty = true;
        state_->flushed = false;
        return TRUE;
    }
    BOOL RemoveDirectory(LPCTSTR name) override
    {
        if (fileView_) return FALSE;
        std::string resolved;
        if (!ResolvePath(directory_, name, false, resolved)) return FALSE;
        std::lock_guard<std::mutex> lock(state_->mutex);
        auto found = state_->entries.find(resolved);
        if (found == state_->entries.end() || !found->second.directory) return FALSE;
        for (const auto & entry : state_->entries)
            if (entry.first.size() > resolved.size() &&
                entry.first.compare(0, resolved.size(), resolved) == 0 &&
                entry.first[resolved.size()] == '/') return FALSE;
        state_->entries.erase(found);
        state_->dirty = true;
        state_->flushed = false;
        return TRUE;
    }
    DWORD GetCurrentDirectory(DWORD size, LPTSTR buffer) override
    {
        const std::string current = directory_.empty() ? "\\" : "\\" + directory_;
        if (!buffer || size <= current.size()) return static_cast<DWORD>(current.size() + 1);
        std::memcpy(buffer, current.c_str(), current.size() + 1);
        return static_cast<DWORD>(current.size());
    }
    BOOL SetCurrentDirectory(LPCTSTR name) override
    {
        if (fileView_ || !name) return FALSE;
        std::string resolved;
        if (std::strcmp(name, "..") == 0)
        {
            if (directory_.empty()) return FALSE;
            const auto slash = directory_.rfind('/');
            resolved = slash == std::string::npos ? "" : directory_.substr(0, slash);
        }
        else if (!ResolvePath(directory_, name, true, resolved)) return FALSE;
        std::lock_guard<std::mutex> lock(state_->mutex);
        const auto found = state_->entries.find(resolved);
        if (found == state_->entries.end() || !found->second.directory) return FALSE;
        directory_ = std::move(resolved);
        return TRUE;
    }
    BOOL DeleteFile(LPCTSTR name) override
    {
        if (fileView_) return FALSE;
        if (saveDirectory_)
        {
            // Match save-directory CreateInstance: only a regular leaf file.
            if (!name || !*name || std::strpbrk(name, "/\\") ||
                std::strcmp(name, ".") == 0 || std::strcmp(name, "..") == 0)
                return FALSE;
            std::error_code error;
            const auto file = path_ / name;
            if (std::filesystem::symlink_status(file, error).type() !=
                    std::filesystem::file_type::regular || error) return FALSE;
            return std::filesystem::remove(file, error) && !error ? TRUE : FALSE;
        }
        std::string resolved;
        if (!ResolvePath(directory_, name, false, resolved)) return FALSE;
        std::lock_guard<std::mutex> lock(state_->mutex);
        auto found = state_->entries.find(resolved);
        if (found == state_->entries.end() || found->second.directory) return FALSE;
        state_->entries.erase(found);
        state_->dirty = true;
        state_->flushed = false;
        return TRUE;
    }
    BOOL CopyFile(LPCTSTR, LPCTSTR, BOOL) override { return FALSE; }
    BOOL MoveFile(LPCTSTR, LPCTSTR) override { return FALSE; }
    DWORD GetFileAttributes(LPCTSTR) override { return InvalidPosition; }
    BOOL SetFileAttributes(LPCTSTR, DWORD) override { return FALSE; }
    DWORD GetLastError() override { return lastError_; }

    HANDLE OpenChild(DAFILEDESC * descriptor) override
    {
        if (fileView_ || !descriptor) return INVALID_HANDLE_VALUE;
        const bool create = descriptor->dwCreationDistribution == CREATE_ALWAYS &&
            descriptor->dwDesiredAccess == (GENERIC_READ | GENERIC_WRITE);
        const bool read = descriptor->dwCreationDistribution == OPEN_EXISTING &&
            descriptor->dwDesiredAccess == GENERIC_READ;
        if (!create && !read) return INVALID_HANDLE_VALUE;
        std::string resolved;
        if (!ResolvePath(directory_, descriptor->lpFileName, false, resolved))
            return INVALID_HANDLE_VALUE;
        std::lock_guard<std::mutex> lock(mutex_);
        std::lock_guard<std::mutex> treeLock(state_->mutex);
        if (create)
        {
            if (!HasParentDirectory(resolved)) return INVALID_HANDLE_VALUE;
            auto existing = state_->entries.find(resolved);
            if (existing != state_->entries.end() && existing->second.directory)
                return INVALID_HANDLE_VALUE;
            if (existing == state_->entries.end())
                state_->entries.emplace(resolved, Entry{false, {}, state_->nextCreationOrder++});
            else
                existing->second.bytes.clear();
            state_->dirty = true;
            state_->flushed = false;
        }
        else
        {
            const auto existing = state_->entries.find(resolved);
            if (existing == state_->entries.end() || existing->second.directory)
                return INVALID_HANDLE_VALUE;
        }
        const auto token = nextFileToken_;
        nextFileToken_ += 2;
        files_.emplace(token, OpenFile{std::move(resolved), 0, create});
        return EncodeHandleValue(token);
    }
    DWORD GetFilePosition(HANDLE handle, PLONG high) override
    {
        if (high) *high = 0;
        std::lock_guard<std::mutex> lock(mutex_);
        OpenFile * open = ResolveOpen(handle);
        return open ? static_cast<DWORD>(open->position) : InvalidPosition;
    }
    LONG GetFileName(LPSTR buffer, LONG size) override
    {
        const auto filename = path_.filename().string();
        if (!buffer || size <= 0) return 0;
        const auto copied = std::min<std::size_t>(filename.size(),
            static_cast<std::size_t>(size - 1));
        std::memcpy(buffer, filename.data(), copied);
        buffer[copied] = '\0';
        return static_cast<LONG>(copied + 1);
    }
    DWORD GetAccessType() override { return GENERIC_READ | GENERIC_WRITE; }
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
    { return GR_NOT_IMPLEMENTED; }
    GENRESULT WriteDirectoryExtension(HANDLE, LPCVOID, DWORD, LPDWORD, DWORD) override
    { return GR_NOT_IMPLEMENTED; }
    LONG SerialCall(LPFILESYSTEM system, DAFILE_SERIAL_PROC procedure, void * context) override
    { return system && procedure ? (system->*procedure)(context) : 0; }
    BOOL GetAbsolutePath(char *, LPCTSTR, LONG) override { return FALSE; }

    void Flush() override
    {
        if (fileView_ || saveDirectory_ || directoryView_)
            Fail("non-archive cannot flush UTF archive");
        std::vector<UtfArchiveItem> items;
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            std::vector<const std::pair<const std::string, Entry> *> ordered;
            for (const auto &entry : state_->entries)
                if (!entry.first.empty()) ordered.push_back(&entry);
            std::sort(ordered.begin(), ordered.end(), [](const auto *a, const auto *b) {
                return a->second.creationOrder < b->second.creationOrder;
            });
            for (const auto *entry : ordered)
                items.push_back({entry->first, entry->second.directory, entry->second.bytes});
        }
        std::vector<unsigned char> encoded;
        std::string error;
        if (!EncodeUtfArchive(items, encoded, error)) Fail(error.c_str());
        auto temporary = path_;
        temporary += ".frontierx-tmp";
        std::error_code status;
        const auto temporaryStatus = std::filesystem::symlink_status(temporary, status);
        if (status == std::errc::no_such_file_or_directory)
            status.clear();
        if (status || temporaryStatus.type() != std::filesystem::file_type::not_found)
            Fail("UTF save temporary path already exists or cannot be checked");
        {
            std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
            if (!output || !output.write(reinterpret_cast<const char *>(encoded.data()),
                static_cast<std::streamsize>(encoded.size())) || !output.flush())
            {
                output.close();
                std::filesystem::remove(temporary, status);
                Fail("UTF save temporary write failed");
            }
            output.close();
            if (!output)
            {
                std::filesystem::remove(temporary, status);
                Fail("UTF save temporary close failed");
            }
        }
        UtfMetadata checked;
        if (!InspectUtfMetadata(temporary, checked, error))
        {
            std::filesystem::remove(temporary, status);
            Fail(error.c_str());
        }
        std::filesystem::rename(temporary, path_, status);
        if (status)
        {
            std::filesystem::remove(temporary, status);
            Fail("UTF save atomic publish failed");
        }
        std::lock_guard<std::mutex> lock(state_->mutex);
        state_->dirty = false;
        state_->flushed = true;
    }
    void AddDirEntries(U32) override {}
    void AddNameSpace(U32) override {}
    void HintExpansionDirEntries(U32) override {}
    void HintExpansionNameSpace(U32) override {}

private:
    ~WritableArchive() { if (parent_) parent_->Release(); }

    [[noreturn]] static void Fail(const char * message)
    {
        std::fprintf(stderr, "Native UTF save failed: %s\n", message);
        std::abort();
    }
    bool HasParentDirectory(const std::string & path) const
    {
        const auto slash = path.rfind('/');
        const std::string parent = slash == std::string::npos ? "" : path.substr(0, slash);
        const auto found = state_->entries.find(parent);
        return found != state_->entries.end() && found->second.directory;
    }
    OpenFile * ResolveOpen(HANDLE handle)
    {
        if (fileView_ && !handle)
        {
            implicitFile_.path = filePath_;
            return &implicitFile_;
        }
        const auto token = DecodeEncodedHandle(handle);
        if (!token || (token & 1U) == 0) return nullptr;
        const auto found = files_.find(token);
        return found == files_.end() ? nullptr : &found->second;
    }
    void FillFindData(const std::string & path, WIN32_FIND_DATA * data)
    {
        *data = {};
        const auto found = state_->entries.find(path);
        if (found == state_->entries.end()) return;
        data->dwFileAttributes = found->second.directory ?
            FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
        data->nFileSizeLow = static_cast<DWORD>(found->second.bytes.size());
        const auto slash = path.rfind('/');
        const std::string name = slash == std::string::npos ? path : path.substr(slash + 1);
        if (name.size() < sizeof(data->cFileName))
            std::memcpy(data->cFileName, name.c_str(), name.size() + 1);
    }
    bool FillPhysicalFindData(const std::string & name, WIN32_FIND_DATA * data)
    {
        *data = {};
        data->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
        std::error_code error;
        const auto size = std::filesystem::file_size(path_ / name, error);
        if (error) return false;
        data->nFileSizeLow = size > std::numeric_limits<DWORD>::max() ?
            InvalidPosition : static_cast<DWORD>(size);
        std::memcpy(data->cFileName, name.c_str(), name.size() + 1);
        return true;
    }

    std::atomic<U32> references_{1};
    std::filesystem::path path_;
    std::shared_ptr<ArchiveState> state_;
    std::string directory_;
    std::string filePath_;
    bool fileView_ = false;
    bool rawFile_ = false;
    bool directoryView_ = false;
    bool saveDirectory_ = false;
    IFileSystem * parent_ = nullptr;
    OpenFile implicitFile_;
    std::mutex mutex_;
    std::map<EncodedHandleValue, OpenFile> files_;
    std::map<EncodedHandleValue, Search> searches_;
    EncodedHandleValue nextFileToken_ = 1;
    EncodedHandleValue nextSearchToken_ = 2;
    DWORD lastError_ = 0;
};
}

bool OpenNativeWritableUtfArchive(const std::filesystem::path & path,
                                  IFileSystem ** output, std::string & error)
{
    if (!output) { error = "writable UTF output is null"; return false; }
    *output = nullptr;
    if (path.empty() || path.filename().empty() ||
        path.filename() == "." || path.filename() == "..")
    { error = "writable UTF output path is invalid"; return false; }
    std::error_code status;
    const auto parent = std::filesystem::canonical(path.parent_path(), status);
    if (status || !std::filesystem::is_directory(parent, status) || status)
    { error = "writable UTF parent directory is unavailable"; return false; }
    try
    {
        *output = new WritableArchive(parent / path.filename());
        error.clear();
        return true;
    }
    catch (const std::bad_alloc &)
    { error = "writable UTF archive allocation failed"; return false; }
}

bool OpenNativeSaveDirectory(const std::filesystem::path & path,
                             IFileSystem ** output, std::string & error)
{
    if (!output) { error = "save directory output is null"; return false; }
    *output = nullptr;
    std::error_code status;
    const auto root = std::filesystem::canonical(path, status);
    if (status || !std::filesystem::is_directory(root, status) || status)
    { error = "save directory is inaccessible"; return false; }
    try
    {
        *output = new WritableArchive(root, true);
        error.clear();
        return true;
    }
    catch (const std::bad_alloc &)
    { error = "save directory allocation failed"; return false; }
}
}
