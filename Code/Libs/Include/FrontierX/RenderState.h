#ifndef FRONTIERX_RENDER_STATE_H
#define FRONTIERX_RENDER_STATE_H

#include "da_d3dtypes.h"

namespace FrontierX
{
struct RenderStateSnapshot
{
    U32 depthEnabled = 1U;
    U32 depthWriteEnabled = 1U;
    U32 depthFunction = D3DCMP_LESSEQUAL;
    U32 alphaBlendEnabled = 0U;
    U32 sourceBlend = D3DBLEND_ONE;
    U32 destinationBlend = D3DBLEND_ZERO;
};

struct TextureStageSnapshot
{
    U32 colorOperation = D3DTOP_MODULATE;
    U32 colorArgument1 = D3DTA_TEXTURE;
    U32 colorArgument2 = D3DTA_CURRENT;
    U32 alphaOperation = D3DTOP_SELECTARG1;
    U32 alphaArgument1 = D3DTA_TEXTURE;
    U32 alphaArgument2 = D3DTA_CURRENT;
};

inline bool IsTextureStageArgument(U32 value) noexcept
{
    return value == D3DTA_DIFFUSE || value == D3DTA_CURRENT ||
           value == D3DTA_TEXTURE;
}

inline bool IsTextureStageOperation(U32 value) noexcept
{
    return value == D3DTOP_SELECTARG1 || value == D3DTOP_SELECTARG2 ||
           value == D3DTOP_MODULATE;
}

inline bool IsRenderBoolean(U32 value) noexcept
{
    return value <= 1U;
}

inline bool IsRenderDepthFunction(U32 value) noexcept
{
    return value >= D3DCMP_NEVER && value <= D3DCMP_ALWAYS;
}

inline bool IsRenderBlendFactor(U32 value) noexcept
{
    return value >= D3DBLEND_ZERO && value <= D3DBLEND_SRCALPHASAT;
}
} // namespace FrontierX

#endif
