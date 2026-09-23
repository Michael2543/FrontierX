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
