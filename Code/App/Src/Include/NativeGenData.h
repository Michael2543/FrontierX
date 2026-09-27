#ifndef FRONTIERX_NATIVE_GEN_DATA_H
#define FRONTIERX_NATIVE_GEN_DATA_H

#if defined(_WIN32)
#error NativeGenData.h is only for the non-Windows App data owner
#endif

struct IFileSystem;
struct IGeneralData;

namespace FrontierX
{
// Source is borrowed; output owns one production IGeneralData reference.
bool CreateNativeGeneralData(IFileSystem * source, IGeneralData ** output,
                             unsigned int * globalValueBytes);
void ReleaseNativeGeneralData(IGeneralData * data) noexcept;
}

#endif
