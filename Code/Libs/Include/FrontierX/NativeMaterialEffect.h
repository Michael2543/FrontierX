#ifndef FRONTIERX_NATIVE_MATERIAL_EFFECT_H
#define FRONTIERX_NATIVE_MATERIAL_EFFECT_H

#if !defined(_WIN32)
#include "da_d3dtypes.h"

// Native-only pass contract for the existing IRenderPipeline effect slot.
// The supported Windows ID3DXEffect declaration comes from the DirectX SDK.
struct ID3DXEffect
{
    virtual bool Begin(UINT * passCount, U32 flags) = 0;
    virtual bool BeginPass(UINT pass) = 0;
    virtual bool EndPass() = 0;
    virtual bool End() = 0;
    virtual ~ID3DXEffect() = default;
};
#endif

#endif
