#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
#include <cstring>

#include "NativeAppVersion.inc"

U32 __stdcall GetBuildVersion(char *buffer, U32 bufferSize)
{
    if (!buffer || bufferSize == 0) return 0;
    buffer[0] = '\0';
    if (bufferSize <= 2) return 0;
    const U32 length = static_cast<U32>(sizeof(NativeAppVersionString) - 1U);
    const U32 written = length < bufferSize ? length : bufferSize - 1U;
    std::memcpy(buffer, NativeAppVersionString, written);
    buffer[written] = '\0';
    return written;
}

U32 __stdcall GetBuildVersion(void)
{
    return NativeAppBuildVersion;
}
U32 __stdcall GetProductVersion(void) { return GetBuildVersion(); }
U32 __stdcall GetProductVersion(char *buffer, U32 capacity) { return GetBuildVersion(buffer, capacity); }
#endif
