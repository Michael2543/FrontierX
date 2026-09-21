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
    float u;
    float v;
};

constexpr std::array<SmokeVertex, 9> kVertices = {{
    {0.0F, -0.52F, 0.20F, 255U, 87U, 46U, 0.5F, 1.0F},
    {0.58F, 0.48F, 0.20F, 41U, 255U, 87U, 1.0F, 0.0F},
    {-0.58F, 0.48F, 0.20F, 51U, 107U, 255U, 0.0F, 0.0F},
    {-0.82F, -0.74F, 0.70F, 209U, 209U, 209U, 0.0F, 0.0F},
    {0.82F, -0.74F, 0.70F, 209U, 209U, 209U, 1.0F, 0.0F},
    {0.82F, 0.74F, 0.70F, 209U, 209U, 209U, 1.0F, 1.0F},
    {-0.82F, -0.74F, 0.70F, 209U, 209U, 209U, 0.0F, 0.0F},
    {0.82F, 0.74F, 0.70F, 209U, 209U, 209U, 1.0F, 1.0F},
    {-0.82F, 0.74F, 0.70F, 209U, 209U, 209U, 0.0F, 1.0F},
}};
}

GENRESULT SubmitRenderSmokeScene(IRenderPrimitive * primitive)
{
    if (!primitive) return GR_INVALID_PARMS;
    PrimitiveBuilder builder(primitive, 8U);
    builder.Begin(PB_TRIANGLES);
    for (const SmokeVertex & vertex : kVertices)
    {
        builder.Color4ub(vertex.red, vertex.green, vertex.blue, 255U);
        builder.TexCoord2f(vertex.u, vertex.v);
        builder.Vertex3f(vertex.x, vertex.y, vertex.z);
    }
    builder.End();
    return GR_OK;
}
}
