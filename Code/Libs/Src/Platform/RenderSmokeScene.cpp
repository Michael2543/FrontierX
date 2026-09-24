#include "FrontierX/RenderSmokeScene.h"

#include "IRenderPrimitive.h"
#include "RPVertex1.h"
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
constexpr std::array<std::size_t, RenderSmokeIndexedVertexCount> kUniqueVertexPositions =
    {{0U, 1U, 2U, 3U, 4U, 5U, 8U}};
constexpr std::array<U16, RenderSmokeIndexCount> kIndices =
    {{0U, 1U, 2U, 3U, 4U, 5U, 3U, 5U, 6U}};

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
    const struct StageValue
    {
        U32 stage;
        D3DTEXTURESTAGESTATETYPE state;
        U32 value;
    } stages[] = {
        {0U, D3DTSS_COLOROP, D3DTOP_MODULATE},
        {0U, D3DTSS_COLORARG1, D3DTA_DIFFUSE},
        {0U, D3DTSS_COLORARG2, D3DTA_TEXTURE},
        {0U, D3DTSS_ALPHAOP, D3DTOP_MODULATE},
        {0U, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE},
        {0U, D3DTSS_ALPHAARG2, D3DTA_TEXTURE},
        {1U, D3DTSS_COLOROP, D3DTOP_DISABLE},
        {1U, D3DTSS_ALPHAOP, D3DTOP_DISABLE},
    };
    for (const StageValue & value : stages)
    {
        const GENRESULT result = target->set_texture_stage_state(
            value.stage, value.state, value.value);
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

template <typename RenderTarget>
GENRESULT SubmitIndexedScene(RenderTarget * target)
{
    if (!target) return GR_INVALID_PARMS;
    std::array<RPVertex, RenderSmokeIndexedVertexCount> vertices{};
    for (std::size_t index = 0U; index < vertices.size(); ++index)
    {
        const SmokeVertex & source = kVertices[kUniqueVertexPositions[index]];
        RPVertex & vertex = vertices[index];
        vertex.pos.set(source.x, source.y, source.z);
        vertex.b = source.blue;
        vertex.g = source.green;
        vertex.r = source.red;
        vertex.a = source.alpha;
        vertex.u = source.u;
        vertex.v = source.v;
    }
    return target->draw_indexed_primitive(D3DPT_TRIANGLELIST, D3DFVF_RPVERTEX,
        vertices.data(), static_cast<int>(vertices.size()),
        kIndices.data(), static_cast<int>(kIndices.size()), 0U);
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

GENRESULT SubmitIndexedRenderSmokeScene(IRenderPrimitive * primitive)
{
    return SubmitIndexedScene(primitive);
}

GENRESULT ConfigureRenderSmokeState(IRenderPipeline * pipeline)
{
    return ConfigureState(pipeline);
}

GENRESULT SubmitRenderSmokeScene(IRenderPipeline * pipeline)
{
    return SubmitScene(pipeline);
}

GENRESULT SubmitIndexedRenderSmokeScene(IRenderPipeline * pipeline)
{
    return SubmitIndexedScene(pipeline);
}
}
