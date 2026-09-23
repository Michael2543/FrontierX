#ifndef FRONTIERX_RENDER_SMOKE_SCENE_H
#define FRONTIERX_RENDER_SMOKE_SCENE_H

#include "DACOM.H"

#include <array>
#include <cstdint>

struct IRenderPrimitive;
struct IRenderPipeline;

namespace FrontierX
{
inline constexpr std::uint32_t RenderSmokeTextureWidth = 2U;
inline constexpr std::uint32_t RenderSmokeTextureHeight = 2U;
inline constexpr std::uint32_t RenderSmokeVertexCount = 9U;
inline constexpr std::array<std::uint8_t, 16> RenderSmokeTextureRgba = {{
    255U, 255U, 255U, 255U,
    42U, 90U, 220U, 255U,
    220U, 52U, 38U, 255U,
    245U, 190U, 32U, 255U,
}};

// Submits the deterministic repository-owned triangle-and-quad fixture through
// the production PrimitiveBuilder and either established renderer boundary.
GENRESULT ConfigureRenderSmokeState(IRenderPrimitive * primitive);
GENRESULT SubmitRenderSmokeScene(IRenderPrimitive * primitive);
GENRESULT ConfigureRenderSmokeState(IRenderPipeline * pipeline);
GENRESULT SubmitRenderSmokeScene(IRenderPipeline * pipeline);
}

#endif
