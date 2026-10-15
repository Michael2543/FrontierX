#include "Include/NativeAppGlobals.h"
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include "IMovieCamera.h"
#include "MGlobals.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#include "Filesys.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

// This probe exercises only MovieCameraManager's disk owner. MovieCamera's
// actual object factory shares that production translation unit but requires
// a live mission/UI; reject accidental entry instead of supplying fake IDs.
IStatusBarResource *STATUS = nullptr;
IHintResource *HINTBOX = nullptr;

U32 MGlobals::CreateNewPartID(U32)
{
	throw std::logic_error("camera save probe unexpectedly entered mission object creation");
}

U32 MGlobals::GetThisPlayer()
{
	throw std::logic_error("camera save probe unexpectedly requested a mission player");
}

namespace
{
void Require(bool condition, const char *message)
{
	if (!condition) throw std::runtime_error(message);
}

// A writable DOS child with the same IFileSystem calls used by MovieManager.
// The parent is stack-owned; CreateInstance children release themselves.
class CameraSaveFiles final : public IFileSystem
{
public:
	explicit CameraSaveFiles(std::vector<U8> initial = {})
		: bytes_(std::make_shared<std::vector<U8>>(std::move(initial))) {}

	GENRESULT QueryInterface(const C8 *name, void **instance) override
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
		if (!remaining && child_) delete this;
		return remaining;
	}
	GENRESULT CreateInstance(DACOMDESC *descriptor, void **instance) override
	{
		if (!instance) return GR_INVALID_PARMS;
		*instance = nullptr;
		if (child_ || !descriptor || descriptor->size < sizeof(DAFILEDESC))
			return GR_INVALID_PARMS;
		const auto *file = static_cast<const DAFILEDESC *>(descriptor);
		if (!file->lpFileName || std::strcmp(file->lpFileName, "MovieManager") != 0 ||
			(file->lpImplementation && std::strcmp(file->lpImplementation, "DOS") != 0))
			return GR_INVALID_PARMS;
		if (file->dwCreationDistribution == CREATE_ALWAYS)
			bytes_->clear();
		else if (file->dwCreationDistribution != OPEN_EXISTING)
			return GR_INVALID_PARMS;
		*instance = static_cast<IFileSystem *>(new CameraSaveFiles(bytes_));
		return GR_OK;
	}
	BOOL CloseHandle(HANDLE) override { return FALSE; }
	BOOL ReadFile(HANDLE handle, LPVOID buffer, DWORD count, LPDWORD read,
		LPOVERLAPPED) override
	{
		if (read) *read = 0;
		if (!child_ || handle != nullptr || (count && !buffer)) return FALSE;
		const auto available = bytes_->size() - position_;
		const auto copied = std::min<std::size_t>(available, count);
		if (copied) std::memcpy(buffer, bytes_->data() + position_, copied);
		position_ += copied;
		if (read) *read = static_cast<DWORD>(copied);
		return TRUE;
	}
	BOOL WriteFile(HANDLE handle, LPCVOID buffer, DWORD count, LPDWORD written,
		LPOVERLAPPED) override
	{
		if (written) *written = 0;
		if (!child_ || handle != nullptr || (count && !buffer)) return FALSE;
		const auto *first = static_cast<const U8 *>(buffer);
		bytes_->insert(bytes_->end(), first, first + count);
		position_ += count;
		if (written) *written = count;
		return TRUE;
	}
	BOOL GetOverlappedResult(HANDLE, LPOVERLAPPED, LPDWORD, BOOL) override { return FALSE; }
	DWORD SetFilePointer(HANDLE, LONG, PLONG, DWORD) override { return 0xffffffffU; }
	BOOL SetEndOfFile(HANDLE) override { return FALSE; }
	DWORD GetFileSize(HANDLE handle, LPDWORD high) override
	{
		if (high) *high = 0;
		return child_ && handle == nullptr ? static_cast<DWORD>(bytes_->size()) : 0xffffffffU;
	}
	BOOL LockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return FALSE; }
	BOOL UnlockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) override { return FALSE; }
	BOOL GetFileTime(HANDLE, LPFILETIME, LPFILETIME, LPFILETIME) override { return FALSE; }
	BOOL SetFileTime(HANDLE, const FILETIME *, const FILETIME *, const FILETIME *) override { return FALSE; }
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
	BOOL SetCurrentDirectory(LPCTSTR name) override
	{
		return !child_ && name && std::strcmp(name, "\\") == 0;
	}
	BOOL DeleteFile(LPCTSTR) override { return FALSE; }
	BOOL CopyFile(LPCTSTR, LPCTSTR, BOOL) override { return FALSE; }
	BOOL MoveFile(LPCTSTR, LPCTSTR) override { return FALSE; }
	DWORD GetFileAttributes(LPCTSTR) override { return 0xffffffffU; }
	BOOL SetFileAttributes(LPCTSTR, DWORD) override { return FALSE; }
	DWORD GetLastError() override { return 0; }
	HANDLE OpenChild(DAFILEDESC *) override { return INVALID_HANDLE_VALUE; }
	DWORD GetFilePosition(HANDLE, PLONG) override { return 0xffffffffU; }
	LONG GetFileName(LPSTR, LONG) override { return 0; }
	DWORD GetAccessType() override { return GENERIC_READ | GENERIC_WRITE; }
	GENRESULT GetParentSystem(LPFILESYSTEM *parent) override
	{
		if (parent) *parent = nullptr;
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
	LONG SerialCall(LPFILESYSTEM, DAFILE_SERIAL_PROC, void *) override { return 0; }
	BOOL GetAbsolutePath(char *, LPCTSTR, LONG) override { return FALSE; }

	const std::vector<U8> &Bytes() const { return *bytes_; }

private:
	explicit CameraSaveFiles(std::shared_ptr<std::vector<U8>> bytes)
		: bytes_(std::move(bytes)), child_(true) {}
	std::shared_ptr<std::vector<U8>> bytes_;
	std::size_t position_ = 0;
	U32 references_ = 1;
	bool child_ = false;
};

template<class T>
void Put(std::vector<U8> &bytes, std::size_t offset, const T &value)
{
	Require(offset + sizeof(value) <= bytes.size(), "camera test field overrun");
	std::memcpy(bytes.data() + offset, &value, sizeof(value));
}
}

bool CheckNativeMovieCameraAim();

void CheckMovieCameraSaveRoundTrip()
{
	// The existing disk round trip cannot detect TM2's camera facing away after
	// cinematic handoff. Exercise its production conversion before disk checks.
	Require(CheckNativeMovieCameraAim(), "native cinematic camera did not aim at its requested target");
	Require(CAMERAMANAGER != nullptr, "camera manager missing for save test");
	// 44-byte position + 25-byte scalar header + 4-byte count + 1-byte flag.
	std::vector<U8> serialized(111, 0);
	Put(serialized, 4, 2.0F);        // saved camera duration
	Put(serialized, 12, 17.5F);      // saved camera position.x
	Put(serialized, 36, U32{3});     // saved camera system
	Put(serialized, 69, U32{1});     // one queued CamSpot
	Put(serialized, 73, 1.25F);      // queued spot duration
	Put(serialized, 81, 23.0F);      // queued spot position.x
	Put(serialized, 105, U32{4});    // queued spot system
	serialized[109] = 1;            // regular metrics
	CameraSaveFiles input(serialized);
	Require(CAMERAMANAGER->Load(&input) == TRUE,
		"production camera manager rejected its Win32-shaped save");
	CameraSaveFiles output;
	Require(CAMERAMANAGER->Save(&output) == TRUE && output.Bytes() == serialized,
		"production camera manager changed its 44-byte disk record or queued spot");

	auto truncated = serialized;
	truncated.pop_back();
	CameraSaveFiles shortInput(std::move(truncated));
	Require(CAMERAMANAGER->Load(&shortInput) == FALSE,
		"production camera manager accepted a truncated queued spot");
	CameraSaveFiles emptyAfterFailure;
	Require(CAMERAMANAGER->Save(&emptyAfterFailure) == TRUE &&
		emptyAfterFailure.Bytes().size() == 74,
		"failed camera load retained a partial queue");

	auto extra = serialized;
	extra.push_back(0);
	CameraSaveFiles longInput(std::move(extra));
	Require(CAMERAMANAGER->Load(&longInput) == FALSE,
		"production camera manager accepted trailing save bytes");
	serialized[110] = 2;
	CameraSaveFiles badFlag(std::move(serialized));
	Require(CAMERAMANAGER->Load(&badFlag) == FALSE,
		"production camera manager accepted an invalid last-spot flag");
}
