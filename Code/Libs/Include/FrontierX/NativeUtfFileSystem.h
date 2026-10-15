#ifndef FRONTIERX_NATIVE_UTF_FILE_SYSTEM_H
#define FRONTIERX_NATIVE_UTF_FILE_SYSTEM_H

#include "Filesys.h"

#include <filesystem>
#include <string>

namespace FrontierX
{
// Opens a validated UTF 1.01 container through the legacy read-only IFileSystem contract.
// The returned interface owns one reference and must be released by the caller.
bool OpenNativeUtfFileSystem(const std::filesystem::path & path,
                             IFileSystem ** output, std::string & error);
// Saved archives retain their writable directory factory for Mission::Save's
// reopen-on-write path. The returned reader owns one reference to parent.
bool OpenNativeUtfFileSystemWithParent(const std::filesystem::path & path,
                                      IFileSystem * parent, IFileSystem ** output,
                                      std::string & error);
// Read-only physical directory views for source material/shader data. Child UTF
// archives use the same nested factory contract; traversal outside the root fails.
enum class NativePhysicalFileLimit : U32
{
    Standard = 64U * 1024U * 1024U,
    Movies = 256U * 1024U * 1024U
};
bool OpenNativeDirectoryFileSystem(const std::filesystem::path & path,
                                   IFileSystem ** output, std::string & error,
                                   NativePhysicalFileLimit limit = NativePhysicalFileLimit::Standard);
}

#endif
