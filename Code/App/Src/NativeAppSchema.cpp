#include "Include/NativeAppSchema.h"
#include "ViewCnst.h"
#include "Filesys.h"

#include <climits>
#include <cstring>
#include <vector>

#include "NativeAppSchema.inc"

static_assert(sizeof(appSchemaBytes) > 0 && sizeof(appSchemaBytes) <= INT_MAX,
              "App parser resource size exceeds the Mission caller contract");

namespace FrontierX
{
const void *GetNativeAppSchema(int & length) noexcept
{
    length = static_cast<int>(sizeof(appSchemaBytes));
    return appSchemaBytes;
}

bool ParseNativeAppSchema(IViewConstructor2 * parser)
{
    if (!parser) return false;
    int length = 0;
    const void *resource = GetNativeAppSchema(length);
    if (!resource || length <= 0) return false;
    std::vector<char> terminated(static_cast<std::size_t>(length) + 1U, '\0');
    std::memcpy(terminated.data(), resource, static_cast<std::size_t>(length));
    return parser->ParseMemory(terminated.data()) == GR_OK;
}

bool LoadNativeMissionSchema(IFileSystem * source, IViewConstructor2 * parser,
                             HANDLE & symbols, U32 * byteCount)
{
    if (byteCount) *byteCount = 0;
    if (!source || !parser || symbols) return false;
    DAFILEDESC descriptor("\\ParseData\\Data.h");
    const HANDLE file = source->OpenChild(&descriptor);
    if (file == INVALID_HANDLE_VALUE) return false;
    const DWORD length = source->GetFileSize(file, nullptr);
    if (!length || length == DWORD(-1) || length > 4U * 1024U * 1024U)
    {
        source->CloseHandle(file);
        return false;
    }
    std::vector<char> terminated(static_cast<std::size_t>(length) + 1U, '\0');
    DWORD read = 0;
    const bool loaded = source->ReadFile(file, terminated.data(), length,
                                         &read, nullptr) && read == length;
    const bool closed = source->CloseHandle(file) != 0;
    if (!loaded || !closed) return false;
    const HANDLE parsed = parser->ParseNewMemory(terminated.data());
    if (!parsed) return false;
    if (!parser->GetSymbol(parsed, "MT_GlobalData"))
    {
        parser->DestroySymbols(parsed);
        return false;
    }
    symbols = parsed;
    if (byteCount) *byteCount = length;
    return true;
}
}
