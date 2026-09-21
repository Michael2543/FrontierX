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
}

#endif
