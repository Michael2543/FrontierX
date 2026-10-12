#ifndef FRONTIERX_NATIVE_WRITABLE_UTF_ARCHIVE_H
#define FRONTIERX_NATIVE_WRITABLE_UTF_ARCHIVE_H

#if defined(_WIN32)
#error NativeWritableUtfArchive.h is only for the native Linux port
#endif

#include "Filesys.h"

#include <filesystem>
#include <string>

namespace FrontierX
{
// Creates a writable UTF archive at an explicit output path. No bytes are
// published until IUTFWriter::Flush succeeds; the caller owns one reference.
bool OpenNativeWritableUtfArchive(const std::filesystem::path & path,
                                  IFileSystem ** output, std::string & error);
// Opens an existing save directory. CREATE_ALWAYS UTF children use the
// writable archive owner; OPEN_EXISTING children use the production reader.
// Explicit DOS children preserve raw records such as GameProgress's player.gpf.
// DOS writes publish their bytes through WriteFile without UTF wrapping.
bool OpenNativeSaveDirectory(const std::filesystem::path & path,
                             IFileSystem ** output, std::string & error);
}

#endif
