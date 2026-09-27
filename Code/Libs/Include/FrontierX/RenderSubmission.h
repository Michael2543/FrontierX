#ifndef FRONTIERX_RENDER_SUBMISSION_H
#define FRONTIERX_RENDER_SUBMISSION_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace FrontierX
{
// Runtime transfer layout matching the established RPVertex position/diffuse/UV shape.
struct RenderVertexP3DiffuseT2
{
    float x;
    float y;
    float z;
    std::uint32_t diffuseBgra;
    float u;
    float v;
};

struct RenderVertexP3DiffuseT4
{
    float x;
    float y;
    float z;
    std::uint32_t diffuseBgra;
    float u;
    float v;
    float u2;
    float v2;
};

// Runtime transfer layout for PolyMesh's MTVERTEX FVF.
struct RenderVertexP3DiffuseSpecularT4
{
    float x;
    float y;
    float z;
    std::uint32_t diffuseBgra;
    std::uint32_t specularBgra;
    float u;
    float v;
    float u2;
    float v2;
};

// Runtime transfer layout for the App MeshRender StandardVertex FVF.
struct RenderVertexP3N3DiffuseT5
{
    float x, y, z;
    float nx, ny, nz;
    std::uint32_t diffuseBgra;
    float u, v;
    float u2, v2;
    float tanX, tanY;
    float binX, binY;
    float tanZ, binZ;
};

static_assert(std::is_standard_layout<RenderVertexP3N3DiffuseT5>::value,
              "game standard render vertex must remain standard layout");
static_assert(sizeof(RenderVertexP3N3DiffuseT5) == 68U &&
                  offsetof(RenderVertexP3N3DiffuseT5, nx) == 12U &&
                  offsetof(RenderVertexP3N3DiffuseT5, diffuseBgra) == 24U &&
                  offsetof(RenderVertexP3N3DiffuseT5, u) == 28U &&
                  offsetof(RenderVertexP3N3DiffuseT5, tanX) == 44U &&
                  offsetof(RenderVertexP3N3DiffuseT5, binZ) + sizeof(float) == 68U,
              "unexpected game standard vertex layout");

static_assert(std::is_standard_layout<RenderVertexP3DiffuseSpecularT4>::value,
              "game render vertex must remain standard layout");
static_assert(sizeof(RenderVertexP3DiffuseSpecularT4) == 36U,
              "unexpected game render vertex size");
static_assert(offsetof(RenderVertexP3DiffuseSpecularT4, diffuseBgra) == 12U &&
                  offsetof(RenderVertexP3DiffuseSpecularT4, specularBgra) == 16U &&
                  offsetof(RenderVertexP3DiffuseSpecularT4, u) == 20U &&
                  offsetof(RenderVertexP3DiffuseSpecularT4, u2) == 28U &&
                  offsetof(RenderVertexP3DiffuseSpecularT4, v2) + sizeof(float) ==
                      sizeof(RenderVertexP3DiffuseSpecularT4),
              "unexpected game render vertex offsets");

static_assert(std::is_standard_layout<RenderVertexP3DiffuseT4>::value,
              "extended render vertex must remain standard layout");
static_assert(sizeof(RenderVertexP3DiffuseT4) == 32U,
              "unexpected extended render vertex size");
static_assert(offsetof(RenderVertexP3DiffuseT4, diffuseBgra) == 12U,
              "unexpected extended diffuse offset");
static_assert(offsetof(RenderVertexP3DiffuseT4, u) == 16U,
              "unexpected first extended texture offset");
static_assert(offsetof(RenderVertexP3DiffuseT4, u2) == 24U,
              "unexpected extended texture coordinate offset");
static_assert(offsetof(RenderVertexP3DiffuseT4, v2) + sizeof(float) ==
                  sizeof(RenderVertexP3DiffuseT4),
              "unexpected extended render vertex tail padding");

static_assert(std::is_standard_layout<RenderVertexP3DiffuseT2>::value,
              "render vertex must remain standard layout");
static_assert(sizeof(RenderVertexP3DiffuseT2) == 24U, "unexpected render vertex size");
static_assert(offsetof(RenderVertexP3DiffuseT2, diffuseBgra) == 12U,
              "unexpected diffuse offset");
static_assert(offsetof(RenderVertexP3DiffuseT2, u) == 16U, "unexpected texture offset");
static_assert(offsetof(RenderVertexP3DiffuseT2, v) + sizeof(float) ==
                  sizeof(RenderVertexP3DiffuseT2),
              "unexpected render vertex tail padding");

constexpr std::uint32_t PackDiffuseBgra(std::uint8_t red,
                                       std::uint8_t green,
                                       std::uint8_t blue,
                                       std::uint8_t alpha) noexcept
{
    return static_cast<std::uint32_t>(blue) |
           (static_cast<std::uint32_t>(green) << 8U) |
           (static_cast<std::uint32_t>(red) << 16U) |
           (static_cast<std::uint32_t>(alpha) << 24U);
}

enum class PrimitiveTopology : std::uint8_t
{
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
};

inline bool AreRenderElementsValid(PrimitiveTopology topology,
                                   std::size_t vertexCount,
                                   const std::uint16_t *indices,
                                   std::size_t indexCount) noexcept
{
    if (vertexCount == 0U ||
        vertexCount > (std::numeric_limits<std::uint32_t>::max)() ||
        indexCount > (std::numeric_limits<std::uint32_t>::max)())
    {
        return false;
    }
    if ((indices == nullptr) != (indexCount == 0U))
    {
        return false;
    }
    for (std::size_t index = 0U; index < indexCount; ++index)
    {
        if (indices[index] >= vertexCount)
        {
            return false;
        }
    }

    const std::size_t elementCount = indexCount == 0U ? vertexCount : indexCount;
    switch (topology)
    {
    case PrimitiveTopology::PointList:
        return true;
    case PrimitiveTopology::LineList:
        return elementCount >= 2U && (elementCount % 2U) == 0U;
    case PrimitiveTopology::LineStrip:
        return elementCount >= 2U;
    case PrimitiveTopology::TriangleList:
        return elementCount >= 3U && (elementCount % 3U) == 0U;
    case PrimitiveTopology::TriangleStrip:
        return elementCount >= 3U;
    }
    return false;
}

struct RenderSubmissionView
{
    PrimitiveTopology topology;
    const RenderVertexP3DiffuseT2 *vertices;
    std::size_t vertexCount;
    const std::uint16_t *indices;
    std::size_t indexCount;

    bool IsValid() const noexcept
    {
        return vertices != nullptr &&
               AreRenderElementsValid(topology, vertexCount, indices, indexCount);
    }

    std::size_t PrimitiveCount() const noexcept
    {
        if (!IsValid())
        {
            return 0U;
        }
        const std::size_t elementCount = indexCount == 0U ? vertexCount : indexCount;
        switch (topology)
        {
        case PrimitiveTopology::PointList:
            return elementCount;
        case PrimitiveTopology::LineList:
            return elementCount / 2U;
        case PrimitiveTopology::LineStrip:
            return elementCount - 1U;
        case PrimitiveTopology::TriangleList:
            return elementCount / 3U;
        case PrimitiveTopology::TriangleStrip:
            return elementCount - 2U;
        }
        return 0U;
    }
};

struct RenderSubmissionView2
{
    PrimitiveTopology topology;
    const RenderVertexP3DiffuseT4 *vertices;
    std::size_t vertexCount;
    const std::uint16_t *indices;
    std::size_t indexCount;

    bool IsValid() const noexcept
    {
        return vertices != nullptr &&
               AreRenderElementsValid(topology, vertexCount, indices, indexCount);
    }
};

struct RenderSubmissionView3
{
    PrimitiveTopology topology;
    const RenderVertexP3DiffuseSpecularT4 *vertices;
    std::size_t vertexCount;
    const std::uint16_t *indices;
    std::size_t indexCount;

    bool IsValid() const noexcept
    {
        return vertices != nullptr &&
               AreRenderElementsValid(topology, vertexCount, indices, indexCount);
    }
};

struct RenderSubmissionView4
{
    PrimitiveTopology topology;
    const RenderVertexP3N3DiffuseT5 *vertices;
    std::size_t vertexCount;
    const std::uint16_t *indices;
    std::size_t indexCount;

    bool IsValid() const noexcept
    {
        return vertices != nullptr &&
               AreRenderElementsValid(topology, vertexCount, indices, indexCount);
    }
};
} // namespace FrontierX

#endif
