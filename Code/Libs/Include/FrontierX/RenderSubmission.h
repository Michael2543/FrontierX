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

struct RenderSubmissionView
{
    PrimitiveTopology topology;
    const RenderVertexP3DiffuseT2 *vertices;
    std::size_t vertexCount;
    const std::uint16_t *indices;
    std::size_t indexCount;

    bool IsValid() const noexcept
    {
        if (vertices == nullptr || vertexCount == 0U ||
            vertexCount > std::numeric_limits<std::uint32_t>::max() ||
            indexCount > std::numeric_limits<std::uint32_t>::max())
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
} // namespace FrontierX

#endif
