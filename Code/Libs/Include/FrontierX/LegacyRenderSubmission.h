#ifndef FRONTIERX_LEGACY_RENDER_SUBMISSION_H
#define FRONTIERX_LEGACY_RENDER_SUBMISSION_H

#include "FrontierX/RenderSubmission.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace FrontierX
{
namespace LegacyD3d9
{
// Stable D3D9 values used at the existing IRenderPrimitive boundary.
constexpr std::uint32_t kPointList = 1U;
constexpr std::uint32_t kLineList = 2U;
constexpr std::uint32_t kLineStrip = 3U;
constexpr std::uint32_t kTriangleList = 4U;
constexpr std::uint32_t kTriangleStrip = 5U;
constexpr std::uint32_t kTriangleFan = 6U;

constexpr std::uint32_t kFvfXyz = 0x002U;
constexpr std::uint32_t kFvfDiffuse = 0x040U;
constexpr std::uint32_t kFvfTex1 = 0x100U;
constexpr std::uint32_t kRpVertexFvf = kFvfXyz | kFvfDiffuse | kFvfTex1;
} // namespace LegacyD3d9

enum class LegacySubmissionStatus : std::uint8_t
{
    Ok,
    UnsupportedPrimitiveType,
    UnsupportedVertexFormat,
    UnsupportedIndexedPointList,
    InvalidInput,
    InsufficientDestination,
};

struct LegacySubmissionInput
{
    std::uint32_t primitiveType;
    std::uint32_t vertexFormat;
    const void *vertices;
    std::size_t vertexCount;
    const std::uint16_t *indices;
    std::size_t indexCount;
};

inline bool MapLegacyPrimitiveTopology(std::uint32_t primitiveType,
                                       PrimitiveTopology &topology) noexcept
{
    switch (primitiveType)
    {
    case LegacyD3d9::kPointList:
        topology = PrimitiveTopology::PointList;
        return true;
    case LegacyD3d9::kLineList:
        topology = PrimitiveTopology::LineList;
        return true;
    case LegacyD3d9::kLineStrip:
        topology = PrimitiveTopology::LineStrip;
        return true;
    case LegacyD3d9::kTriangleList:
        topology = PrimitiveTopology::TriangleList;
        return true;
    case LegacyD3d9::kTriangleStrip:
        topology = PrimitiveTopology::TriangleStrip;
        return true;
    default:
        return false;
    }
}

inline LegacySubmissionStatus AdaptLegacyRpVertexSubmission(
    const LegacySubmissionInput &input,
    RenderVertexP3DiffuseT2 *destination,
    std::size_t destinationCapacity,
    RenderSubmissionView &output) noexcept
{
    output = {PrimitiveTopology::PointList, nullptr, 0U, nullptr, 0U};

    PrimitiveTopology topology = PrimitiveTopology::PointList;
    if (!MapLegacyPrimitiveTopology(input.primitiveType, topology))
    {
        return LegacySubmissionStatus::UnsupportedPrimitiveType;
    }
    if (input.vertexFormat != LegacyD3d9::kRpVertexFvf)
    {
        return LegacySubmissionStatus::UnsupportedVertexFormat;
    }
    if (topology == PrimitiveTopology::PointList && input.indexCount != 0U)
    {
        return LegacySubmissionStatus::UnsupportedIndexedPointList;
    }
    if (input.vertices == nullptr || input.vertexCount == 0U || destination == nullptr ||
        (input.indices == nullptr) != (input.indexCount == 0U))
    {
        return LegacySubmissionStatus::InvalidInput;
    }
    if (destinationCapacity < input.vertexCount)
    {
        return LegacySubmissionStatus::InsufficientDestination;
    }

    const RenderSubmissionView candidate{
        topology,
        destination,
        input.vertexCount,
        input.indices,
        input.indexCount,
    };
    if (!candidate.IsValid())
    {
        return LegacySubmissionStatus::InvalidInput;
    }

    std::memmove(destination,
                 input.vertices,
                 input.vertexCount * sizeof(RenderVertexP3DiffuseT2));
    output = candidate;
    return LegacySubmissionStatus::Ok;
}
} // namespace FrontierX

#endif
