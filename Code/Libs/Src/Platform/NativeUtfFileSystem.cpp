#include "FrontierX/NativeUtfFileSystem.h"

#include "FrontierX/EncodedHandle.h"
#include "FrontierX/UtfMetadata.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <limits>
#include <mutex>
#include <new>
#include <string>
#include <utility>
#include <vector>

namespace FrontierX
{
namespace
{
constexpr EncodedHandleValue FirstChildHandle = 1;
constexpr DWORD InvalidFileResult = 0xffffffffU;

class NativeUtfFileSystem final : public IFileSystem
{
public:
    explicit NativeUtfFileSystem(std::filesystem::path path) : path_(std::move(path)) {}

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

    GENRESULT CreateInstance(DACOMDESC *, void ** instance) override
    {
        if (instance) *instance = nullptr;
        return GR_NOT_IMPLEMENTED;
    }

    BOOL CloseHandle(HANDLE handle) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
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
    HANDLE CreateFileMapping(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD,
                             LPCTSTR) override { return INVALID_HANDLE_VALUE; }
    LPVOID MapViewOfFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return nullptr; }
    BOOL UnmapViewOfFile(LPCVOID) override { return FALSE; }
    HANDLE FindFirstFile(LPCTSTR, LPWIN32_FIND_DATA) override { return INVALID_HANDLE_VALUE; }
    BOOL FindNextFile(HANDLE, LPWIN32_FIND_DATA) override { return FALSE; }
    BOOL FindClose(HANDLE) override { return FALSE; }
    BOOL CreateDirectory(LPCTSTR, LPSECURITY_ATTRIBUTES) override { return FALSE; }
    BOOL RemoveDirectory(LPCTSTR) override { return FALSE; }
    DWORD GetCurrentDirectory(DWORD, LPTSTR) override { return 0; }
    BOOL SetCurrentDirectory(LPCTSTR) override { return FALSE; }
    BOOL DeleteFile(LPCTSTR) override { return FALSE; }
    BOOL CopyFile(LPCTSTR, LPCTSTR, BOOL) override { return FALSE; }
    BOOL MoveFile(LPCTSTR, LPCTSTR) override { return FALSE; }
    DWORD GetFileAttributes(LPCTSTR) override { return InvalidFileResult; }
    BOOL SetFileAttributes(LPCTSTR, DWORD) override { return FALSE; }
    DWORD GetLastError() override { return last_error_; }

    HANDLE OpenChild(DAFILEDESC * descriptor) override
    {
        if (!descriptor || !descriptor->lpFileName || !descriptor->lpFileName[0] ||
            descriptor->dwDesiredAccess != GENERIC_READ)
        {
            last_error_ = 1;
            return INVALID_HANDLE_VALUE;
        }

        std::vector<unsigned char> bytes;
        std::string error;
        try
        {
            if (!ReadUtfEntry(path_, descriptor->lpFileName, bytes, error) ||
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
        last_error_ = 0;
        return EncodeHandleValue(FirstChildHandle + index);
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
        *parent = nullptr;
        return GR_DATA_NOT_FOUND;
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
    ~NativeUtfFileSystem() = default;

    struct Child
    {
        std::vector<unsigned char> bytes;
        std::size_t position = 0;
        bool open = false;
    };

    Child * Resolve(HANDLE handle)
    {
        std::size_t index = 0;
        if (!TryDecodeOffsetHandleIndex(handle, FirstChildHandle, children_.size(), index) ||
            !children_[index].open)
            return nullptr;
        return &children_[index];
    }

    std::atomic<U32> references_{1};
    std::filesystem::path path_;
    std::mutex mutex_;
    std::vector<Child> children_;
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
    UtfMetadata metadata;
    if (!InspectUtfMetadata(path, metadata, error)) return false;
    try
    {
        *output = new NativeUtfFileSystem(path);
    }
    catch (const std::bad_alloc &)
    {
        error = "cannot allocate native UTF file system";
        return false;
    }
    error.clear();
    return true;
}
}
