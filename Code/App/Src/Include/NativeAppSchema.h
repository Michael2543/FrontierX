#ifndef FRONTIERX_NATIVE_APP_SCHEMA_H
#define FRONTIERX_NATIVE_APP_SCHEMA_H

#if defined(_WIN32)
#error NativeAppSchema.h is only for the non-Windows App resource path
#endif

#include "ViewCnst.h"

struct IFileSystem;

namespace FrontierX
{
// Mirrors the stable resource bytes returned by Win32 LockResource.
const void *GetNativeAppSchema(int & length) noexcept;
bool ParseNativeAppSchema(IViewConstructor2 * parser);
// Mirrors Mission::LoadParseData ownership: caller destroys a successful list.
bool LoadNativeMissionSchema(IFileSystem * source, IViewConstructor2 * parser,
                             HANDLE & symbols, U32 * byteCount = nullptr);
}

#endif
