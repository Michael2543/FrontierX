#ifndef FRONTIERX_NATIVE_POLY_MESH_H
#define FRONTIERX_NATIVE_POLY_MESH_H

#include "TYPEDEFS.H"

struct ICOManager;

namespace FrontierX
{
bool RegisterNativePolyMesh(ICOManager *manager);

#if !defined(_WIN32)
// Scoped render-instance flags only; never stored in mesh/material data.
constexpr U32 NativeInstanceOpacityFlag = U32(1) << 31;
constexpr U32 NativeInstanceOpacityMask = U32(255) << 16;

constexpr U32 EncodeNativeInstanceOpacity(U8 opacity)
{
    return NativeInstanceOpacityFlag | (U32(opacity) << 16);
}

constexpr U8 NativeInstanceOpacity(U32 flags)
{
    return (flags & NativeInstanceOpacityFlag) ? U8((flags >> 16) & 255) : U8(255);
}
#endif
}

#endif
