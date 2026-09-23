#include "FrontierX/RenderSmokeScene.h"

#include "IRenderPrimitive.h"
#include "RPUL/PrimitiveBuilder.h"

#include <array>

namespace FrontierX
{
namespace
{
struct SmokeVertex
{
    float x;
    float y;
    float z;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
    float u;
    float v;
};

constexpr std::array<SmokeVertex, 9> kVertices = {{
    {0.0F, -0.52F, 0.20F, 255U, 87U, 46U, 210U, 0.5F, 1.0F},
    {0.58F, 0.48F, 0.20F, 41U, 255U, 87U, 210U, 1.0F, 0.0F},
    {-0.58F, 0.48F, 0.20F, 51U, 107U, 255U, 210U, 0.0F, 0.0F},
    {-0.82F, -0.74F, 0.70F, 209U, 209U, 209U, 160U, 0.0F, 0.0F},
    {0.82F, -0.74F, 0.70F, 209U, 209U, 209U, 160U, 1.0F, 0.0F},
    {0.82F, 0.74F, 0.70F, 209U, 209U, 209U, 160U, 1.0F, 1.0F},
    {-0.82F, -0.74F, 0.70F, 209U, 209U, 209U, 160U, 0.0F, 0.0F},
    {0.82F, 0.74F, 0.70F, 209U, 209U, 209U, 160U, 1.0F, 1.0F},
    {-0.82F, 0.74F, 0.70F, 209U, 209U, 209U, 160U, 0.0F, 1.0F},
}};

template <typename RenderTarget>
GENRESULT ConfigureState(RenderTarget * target)
{
    if (!target) return GR_INVALID_PARMS;
    const struct StateValue
    {
        D3DRENDERSTATETYPE state;
        U32 value;
    } values[] = {
        {D3DRS_ZENABLE, 1U},
        {D3DRS_ZWRITEENABLE, 1U},
        {D3DRS_ZFUNC, D3DCMP_LESSEQUAL},
        {D3DRS_ALPHABLENDENABLE, 1U},
        {D3DRS_SRCBLEND, D3DBLEND_SRCALPHA},
        {D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA},
    };
    for (const StateValue & value : values)
    {
        const GENRESULT result = target->set_render_state(value.state, value.value);
        if (result != GR_OK) return result;
    }
    return GR_OK;
}

template <typename RenderTarget>
GENRESULT SubmitScene(RenderTarget * target)
{
    if (!target) return GR_INVALID_PARMS;
    PrimitiveBuilder builder(target, 8U);
    builder.Begin(PB_TRIANGLES);
    for (const SmokeVertex & vertex : kVertices)
    {
        builder.Color4ub(vertex.red, vertex.green, vertex.blue, vertex.alpha);
        builder.TexCoord2f(vertex.u, vertex.v);
        builder.Vertex3f(vertex.x, vertex.y, vertex.z);
    }
    builder.End();
    return GR_OK;
}
}

GENRESULT ConfigureRenderSmokeState(IRenderPrimitive * primitive)
{
    return ConfigureState(primitive);
}

GENRESULT SubmitRenderSmokeScene(IRenderPrimitive * primitive)
{
    return SubmitScene(primitive);
}

GENRESULT ConfigureRenderSmokeState(IRenderPipeline * pipeline)
{
    return ConfigureState(pipeline);
}

GENRESULT SubmitRenderSmokeScene(IRenderPipeline * pipeline)
{
    return SubmitScene(pipeline);
}
}
