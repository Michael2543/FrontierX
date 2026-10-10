#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "RPUL/PrimitiveBuilder.h"
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif
#include "matrix4.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include "MyVertex.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <array>
#include <cstddef>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>
#include <vector>

namespace
{
using FrontierX::PrimitiveTopology;
using FrontierX::LegacySubmissionInput;
using FrontierX::LegacySubmissionStatus;
using FrontierX::RenderSubmissionView;
using FrontierX::RenderSubmissionView2;
using FrontierX::RenderSubmissionView3;
using FrontierX::RenderVertexP3DiffuseT2;
using FrontierX::RenderVertexP3DiffuseT4;
using FrontierX::RenderVertexP3DiffuseSpecularT4;

constexpr std::array<RenderVertexP3DiffuseT2, 4> kVertices = {{
    {-1.0F, -1.0F, 0.5F, FrontierX::PackDiffuseBgra(255U, 0U, 0U, 255U), 0.0F, 1.0F},
    {1.0F, -1.0F, 0.5F, FrontierX::PackDiffuseBgra(0U, 255U, 0U, 255U), 1.0F, 1.0F},
    {1.0F, 1.0F, 0.5F, FrontierX::PackDiffuseBgra(0U, 0U, 255U, 255U), 1.0F, 0.0F},
    {-1.0F, 1.0F, 0.5F, FrontierX::PackDiffuseBgra(255U, 255U, 255U, 255U), 0.0F, 0.0F},
}};

constexpr std::array<RenderVertexP3DiffuseT4, 4> kVertices2 = {{
    {-0.9F, -0.8F, 0.4F, FrontierX::PackDiffuseBgra(11U, 21U, 31U, 41U),
     0.0F, 1.0F, 0.1F, 0.9F},
    {0.9F, -0.8F, 0.4F, FrontierX::PackDiffuseBgra(51U, 61U, 71U, 81U),
     1.0F, 1.0F, 0.8F, 0.9F},
    {0.9F, 0.8F, 0.4F, FrontierX::PackDiffuseBgra(91U, 101U, 111U, 121U),
     1.0F, 0.0F, 0.8F, 0.2F},
    {-0.9F, 0.8F, 0.4F, FrontierX::PackDiffuseBgra(131U, 141U, 151U, 161U),
     0.0F, 0.0F, 0.1F, 0.2F},
}};

constexpr std::array<RenderVertexP3DiffuseSpecularT4, 3> kMtVertices = {{
    {-0.5F, -0.5F, 0.5F, 0xFF112233U, 0x00102030U,
     0.0F, 0.0F, 0.2F, 0.3F},
    {0.5F, -0.5F, 0.5F, 0xFF445566U, 0x00405060U,
     1.0F, 0.0F, 0.8F, 0.3F},
    {0.0F, 0.5F, 0.5F, 0xFF778899U, 0x00708090U,
     0.5F, 1.0F, 0.5F, 0.9F},
}};

bool Expect(bool condition, const char *message)
{
    if (!condition)
    {
        std::cerr << "render submission probe failed: " << message << '\n';
        return false;
    }
    return true;
}

class CapturingRenderPrimitive final : public IRenderPrimitive
{
public:
    GENRESULT COMAPI QueryInterface(const C8 *interfaceName, void **instance) override
    {
        if (interfaceName == nullptr || instance == nullptr)
        {
            return GR_INVALID_PARMS;
        }
        *instance = nullptr;
        if (std::strcmp(interfaceName, IID_IDAComponent) != 0 &&
            std::strcmp(interfaceName, IID_IRenderPrimitive) != 0)
        {
            return GR_INTERFACE_UNSUPPORTED;
        }
        *instance = static_cast<IRenderPrimitive *>(this);
        AddRef();
        return GR_OK;
    }

    U32 COMAPI AddRef() override
    {
        return ++referenceCount;
    }

    U32 COMAPI Release() override
    {
        return --referenceCount;
    }

    GENRESULT COMAPI set_state(RPRSTATE, U32) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_state(RPRSTATE, U32 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_modelview(const Transform &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_modelview(Transform &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_viewport(int, int, int, int) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_viewport(int *, int *, int *, int *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_ortho(float, float, float, float, float, float) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_perspective(float, float, float, float) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_projection(const Matrix4 &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_projection(Matrix4 &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_material(D3DMATERIAL9 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_material(D3DMATERIAL9 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI begin_scene() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI end_scene() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_render_state(D3DRENDERSTATETYPE, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_render_state(D3DRENDERSTATETYPE, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_texture_stage_state(U32, D3DTEXTURESTAGESTATETYPE, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_texture_stage_state(U32, D3DTEXTURESTAGESTATETYPE, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_sampler_state(U32, D3DSAMPLERSTATETYPE, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_sampler_state(U32, D3DSAMPLERSTATETYPE, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_texture_stage_texture(U32, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_texture_stage_texture(U32, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI verify_state() override { return GR_NOT_IMPLEMENTED; }

    GENRESULT COMAPI draw_primitive(D3DPRIMITIVETYPE primitiveType,
                                    U32 vertexFormat,
                                    const void *vertices,
                                    int vertexCount,
                                    U32) override
    {
        ++drawCount;
        if (vertexCount < 0)
        {
            status = LegacySubmissionStatus::InvalidInput;
            return GR_INVALID_PARMS;
        }

        LegacySubmissionInput input{
            static_cast<std::uint32_t>(primitiveType),
            vertexFormat,
            vertices,
            static_cast<std::size_t>(vertexCount),
            nullptr,
            0U,
        };
        status = FrontierX::AdaptLegacyRpVertexSubmission(
            input, converted.data(), converted.size(), submission);
        return status == LegacySubmissionStatus::Ok ? GR_OK : GR_INVALID_PARMS;
    }

    GENRESULT COMAPI draw_indexed_primitive(D3DPRIMITIVETYPE,
                                            U32,
                                            const void *,
                                            int,
                                            const U16 *,
                                            int,
                                            U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI draw_primitive_vb(D3DPRIMITIVETYPE,
                                       IRP_VERTEXBUFFERHANDLE,
                                       int,
                                       int,
                                       U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI draw_indexed_primitive_vb(D3DPRIMITIVETYPE,
                                               IRP_VERTEXBUFFERHANDLE,
                                               int,
                                               int,
                                               const U16 *,
                                               int,
                                               U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI flush(U32) override { return GR_NOT_IMPLEMENTED; }

    void ResetCapture()
    {
        status = LegacySubmissionStatus::InvalidInput;
        submission = {};
        converted = {};
        drawCount = 0U;
    }

    U32 referenceCount{1U};
    U32 drawCount{0U};
    LegacySubmissionStatus status{LegacySubmissionStatus::InvalidInput};
    RenderSubmissionView submission{};
    std::array<RenderVertexP3DiffuseT2, 8> converted{};
};

bool SamePosition(const RenderVertexP3DiffuseT2 &vertex, float x, float y, float z)
{
    return vertex.x == x && vertex.y == y && vertex.z == z;
}

RPVertex MakeRpVertex(float x, float y, float z, float u, float v)
{
    RPVertex vertex{};
    vertex.pos.set(x, y, z);
    vertex.color = 0xFFFFFFFFU;
    vertex.u = u;
    vertex.v = v;
    return vertex;
}

struct NativeOwnerCapture
{
    std::size_t calls = 0U;
    std::size_t calls2 = 0U;
    std::size_t calls3 = 0U;
    std::size_t calls4 = 0U;
    std::size_t lookupCalls = 0U;
    std::size_t stateCalls = 0U;
    std::size_t textureStageCalls = 0U;
    std::size_t samplerCalls = 0U;
    std::size_t projectionCalls = 0U;
    std::size_t viewportCalls = 0U;
    GENRESULT result = GR_OK;
    PrimitiveTopology topology = PrimitiveTopology::PointList;
    FrontierX::RenderStateSnapshot renderState{};
    FrontierX::TextureStageSnapshot textureStage{};
    FrontierX::SamplerStateSnapshot samplerState{};
    Matrix4 projection{true};
    std::array<int, 4> viewport{{0, 0, 0, 0}};
    std::vector<RenderVertexP3DiffuseT2> vertices;
    std::vector<RenderVertexP3DiffuseT4> vertices2;
    std::vector<RenderVertexP3DiffuseSpecularT4> vertices3;
    std::vector<FrontierX::RenderVertexP3N3DiffuseT5> vertices4;
    std::vector<std::uint16_t> indices;
};

GENRESULT DACOM_CDECL CaptureNativeSubmission(
    void * context, const RenderSubmissionView & submission)
{
    if (!context || !submission.IsValid())
    {
        return GR_INVALID_PARMS;
    }
    auto & capture = *static_cast<NativeOwnerCapture *>(context);
    ++capture.calls;
    capture.topology = submission.topology;
    capture.vertices.assign(
        submission.vertices, submission.vertices + submission.vertexCount);
    if (submission.indices)
    {
        capture.indices.assign(
            submission.indices, submission.indices + submission.indexCount);
    }
    else
    {
        capture.indices.clear();
    }
    return capture.result;
}

GENRESULT DACOM_CDECL CaptureNativeSubmission2(
    void * context, const RenderSubmissionView2 & submission)
{
    if (!context || !submission.IsValid())
    {
        return GR_INVALID_PARMS;
    }
    auto & capture = *static_cast<NativeOwnerCapture *>(context);
    ++capture.calls2;
    capture.topology = submission.topology;
    capture.vertices2.assign(
        submission.vertices, submission.vertices + submission.vertexCount);
    if (submission.indices)
    {
        capture.indices.assign(
            submission.indices, submission.indices + submission.indexCount);
    }
    else
    {
        capture.indices.clear();
    }
    return capture.result;
}

GENRESULT DACOM_CDECL CaptureNativeSubmission3(
    void * context, const RenderSubmissionView3 & submission)
{
    if (!context || !submission.IsValid()) return GR_INVALID_PARMS;
    auto & capture = *static_cast<NativeOwnerCapture *>(context);
    ++capture.calls3;
    capture.vertices3.assign(
        submission.vertices, submission.vertices + submission.vertexCount);
    if (submission.indices)
        capture.indices.assign(submission.indices,
                               submission.indices + submission.indexCount);
    else
        capture.indices.clear();
    return capture.result;
}

GENRESULT DACOM_CDECL CaptureNativeSubmission4(
    void * context, const FrontierX::RenderSubmissionView4 & submission)
{
    if (!context || !submission.IsValid()) return GR_INVALID_PARMS;
    auto & capture = *static_cast<NativeOwnerCapture *>(context);
    ++capture.calls4;
    capture.vertices4.assign(submission.vertices,
                             submission.vertices + submission.vertexCount);
    if (submission.indices)
        capture.indices.assign(submission.indices,
                               submission.indices + submission.indexCount);
    else
        capture.indices.clear();
    return capture.result;
}

GENRESULT DACOM_CDECL CaptureNativeRenderState(
    void * context, const FrontierX::RenderStateSnapshot & state)
{
    if (!context)
    {
        return GR_INVALID_PARMS;
    }
    auto & capture = *static_cast<NativeOwnerCapture *>(context);
    ++capture.stateCalls;
    if (capture.result == GR_OK)
    {
        capture.renderState = state;
    }
    return capture.result;
}

GENRESULT DACOM_CDECL CaptureNativeTextureStage(
    void * context, const FrontierX::TextureStageSnapshot & state)
{
    if (!context) return GR_INVALID_PARMS;
    auto & capture = *static_cast<NativeOwnerCapture *>(context);
    ++capture.textureStageCalls;
    if (capture.result == GR_OK) capture.textureStage = state;
    return capture.result;
}

GENRESULT DACOM_CDECL CaptureNativeSamplerState(
    void * context, const FrontierX::SamplerStateSnapshot & state)
{
    if (!context) return GR_INVALID_PARMS;
    auto & capture = *static_cast<NativeOwnerCapture *>(context);
    ++capture.samplerCalls;
    if (capture.result == GR_OK) capture.samplerState = state;
    return capture.result;
}

GENRESULT DACOM_CDECL CaptureNativeProjection(void * context,
                                              const Matrix4 & projection)
{
    auto & capture = *static_cast<NativeOwnerCapture *>(context);
    ++capture.projectionCalls;
    if (capture.result == GR_OK) capture.projection = projection;
    return capture.result;
}

GENRESULT DACOM_CDECL CaptureNativeViewport(void * context, int x, int y,
                                            int width, int height)
{
    auto & capture = *static_cast<NativeOwnerCapture *>(context);
    ++capture.viewportCalls;
    if (capture.result == GR_OK) capture.viewport = {{x, y, width, height}};
    return capture.result;
}

FrontierX::NativeVertexBufferView LookupNativeVertexBuffer(
    void * context, IRP_VERTEXBUFFERHANDLE handle)
{
    if (!context)
    {
        return {};
    }
    auto & capture = *static_cast<NativeOwnerCapture *>(context);
    ++capture.lookupCalls;
    switch (handle)
    {
    case 1U:
        return {kVertices.data(), sizeof(kVertices),
                FrontierX::LegacyD3d9::kRpVertexFvf};
    case 2U:
        return {kVertices2.data(), sizeof(kVertices2),
                FrontierX::LegacyD3d9::kRpVertex2Fvf};
    case 3U:
        return {kVertices.data(), sizeof(kVertices), 0xFFFFFFFFU};
    default:
        return {};
    }
}
} // namespace

int main()
{
    static_assert(std::is_standard_layout_v<RPVertex>);
    static_assert(sizeof(RPVertex) == sizeof(RenderVertexP3DiffuseT2));
    static_assert(offsetof(RPVertex, pos) == offsetof(RenderVertexP3DiffuseT2, x));
    static_assert(offsetof(RPVertex, color) == offsetof(RenderVertexP3DiffuseT2, diffuseBgra));
    static_assert(offsetof(RPVertex, u) == offsetof(RenderVertexP3DiffuseT2, u));
    static_assert(offsetof(RPVertex, v) == offsetof(RenderVertexP3DiffuseT2, v));
    static_assert(FrontierX::PackDiffuseBgra(0x33U, 0x22U, 0x11U, 0x44U) ==
                  0x44332211U);

    const RenderSubmissionView triangleList{
        PrimitiveTopology::TriangleList,
        kVertices.data(),
        3U,
        nullptr,
        0U,
    };
    if (!Expect(triangleList.IsValid(), "valid direct triangle list") ||
        !Expect(triangleList.PrimitiveCount() == 1U, "direct triangle count"))
    {
        return 1;
    }

    constexpr std::array<std::uint16_t, 6> kIndices = {{0U, 1U, 2U, 0U, 2U, 3U}};
    const RenderSubmissionView indexed{
        PrimitiveTopology::TriangleList,
        kVertices.data(),
        kVertices.size(),
        kIndices.data(),
        kIndices.size(),
    };
    if (!Expect(indexed.IsValid(), "valid indexed triangle list") ||
        !Expect(indexed.PrimitiveCount() == 2U, "indexed triangle count"))
    {
        return 1;
    }

    constexpr std::array<std::uint16_t, 3> kBadIndices = {{0U, 1U, 4U}};
    std::array<RenderVertexP3DiffuseSpecularT4, 3> convertedMt{};
    RenderSubmissionView3 mtSubmission{};
    const LegacySubmissionInput mtInput{
        FrontierX::LegacyD3d9::kTriangleList,
        FrontierX::LegacyD3d9::kMtVertexFvf,
        kMtVertices.data(), kMtVertices.size(), kIndices.data(), 3U};
    if (!Expect(FrontierX::AdaptLegacyMtVertexSubmission(
                    mtInput, convertedMt.data(), convertedMt.size(), mtSubmission) ==
                    LegacySubmissionStatus::Ok && mtSubmission.IsValid(),
                "PolyMesh indexed FVF adapts") ||
        !Expect(std::memcmp(convertedMt.data(), kMtVertices.data(),
                            sizeof(kMtVertices)) == 0 &&
                    mtSubmission.vertices[0].specularBgra == 0x00102030U &&
                    mtSubmission.vertices[2].u2 == 0.5F,
                "PolyMesh diffuse, specular and second UV preserved"))
        return 1;
    LegacySubmissionInput invalidMt = mtInput;
    invalidMt.indices = kBadIndices.data();
    if (!Expect(FrontierX::AdaptLegacyMtVertexSubmission(
                    invalidMt, convertedMt.data(), convertedMt.size(), mtSubmission) ==
                    LegacySubmissionStatus::InvalidInput && !mtSubmission.IsValid(),
                "PolyMesh out-of-range index rejected")) return 1;
    invalidMt = mtInput;
    invalidMt.vertexFormat = FrontierX::LegacyD3d9::kRpVertex2Fvf;
    if (!Expect(FrontierX::AdaptLegacyMtVertexSubmission(
                    invalidMt, convertedMt.data(), convertedMt.size(), mtSubmission) ==
                    LegacySubmissionStatus::UnsupportedVertexFormat,
                "PolyMesh FVF remains exact")) return 1;

    const std::array<RenderSubmissionView, 7> invalid = {{
        {PrimitiveTopology::TriangleList, nullptr, 3U, nullptr, 0U},
        {PrimitiveTopology::TriangleList, kVertices.data(), 0U, nullptr, 0U},
        {PrimitiveTopology::TriangleList, kVertices.data(), 4U, nullptr, 0U},
        {PrimitiveTopology::TriangleList, kVertices.data(), 3U, kIndices.data(), 0U},
        {PrimitiveTopology::TriangleList, kVertices.data(), 3U, nullptr, 3U},
        {PrimitiveTopology::TriangleList,
         kVertices.data(),
         kVertices.size(),
         kBadIndices.data(),
         kBadIndices.size()},
        {PrimitiveTopology::LineList, kVertices.data(), 3U, nullptr, 0U},
    }};
    for (const RenderSubmissionView &submission : invalid)
    {
        if (!Expect(!submission.IsValid(), "invalid submission accepted") ||
            !Expect(submission.PrimitiveCount() == 0U, "invalid primitive count"))
        {
            return 1;
        }
    }

    const std::array<RenderSubmissionView, 4> topologyCounts = {{
        {PrimitiveTopology::PointList, kVertices.data(), 4U, nullptr, 0U},
        {PrimitiveTopology::LineList, kVertices.data(), 4U, nullptr, 0U},
        {PrimitiveTopology::LineStrip, kVertices.data(), 4U, nullptr, 0U},
        {PrimitiveTopology::TriangleStrip, kVertices.data(), 4U, nullptr, 0U},
    }};
    constexpr std::array<std::size_t, 4> kExpectedCounts = {{4U, 2U, 3U, 2U}};
    for (std::size_t index = 0U; index < topologyCounts.size(); ++index)
    {
        if (!Expect(topologyCounts[index].IsValid(), "valid topology rejected") ||
            !Expect(topologyCounts[index].PrimitiveCount() == kExpectedCounts[index],
                    "topology primitive count"))
        {
            return 1;
        }
    }

    const RenderSubmissionView oversized{
        PrimitiveTopology::PointList,
        kVertices.data(),
        static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max()) + 1U,
        nullptr,
        0U,
    };
    if (!Expect(!oversized.IsValid(), "oversized vertex count accepted"))
    {
        return 1;
    }

    const RenderSubmissionView oversizedIndices{
        PrimitiveTopology::TriangleList,
        kVertices.data(),
        kVertices.size(),
        kIndices.data(),
        static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max()) + 1U,
    };
    const RenderSubmissionView unknownTopology{
        static_cast<PrimitiveTopology>(0xFFU),
        kVertices.data(),
        kVertices.size(),
        nullptr,
        0U,
    };
    if (!Expect(!oversizedIndices.IsValid(), "oversized index count accepted") ||
        !Expect(oversizedIndices.PrimitiveCount() == 0U,
                "oversized index primitive count") ||
        !Expect(!unknownTopology.IsValid(), "unknown topology accepted") ||
        !Expect(unknownTopology.PrimitiveCount() == 0U,
                "unknown topology primitive count"))
    {
        return 1;
    }

    std::array<unsigned char, sizeof(kVertices)> legacyVertexBytes{};
    std::memcpy(legacyVertexBytes.data(), kVertices.data(), legacyVertexBytes.size());
    std::array<RenderVertexP3DiffuseT2, kVertices.size()> converted{};
    RenderSubmissionView adapted{
        PrimitiveTopology::PointList,
        nullptr,
        0U,
        nullptr,
        0U,
    };
    const LegacySubmissionInput legacyIndexed{
        FrontierX::LegacyD3d9::kTriangleList,
        FrontierX::LegacyD3d9::kRpVertexFvf,
        legacyVertexBytes.data(),
        kVertices.size(),
        kIndices.data(),
        kIndices.size(),
    };
    if (!Expect(FrontierX::AdaptLegacyRpVertexSubmission(
                    legacyIndexed, converted.data(), converted.size(), adapted) ==
                    LegacySubmissionStatus::Ok,
                "legacy RPVertex submission adaptation") ||
        !Expect(adapted.IsValid(), "adapted submission validity") ||
        !Expect(adapted.topology == PrimitiveTopology::TriangleList,
                "adapted topology") ||
        !Expect(adapted.PrimitiveCount() == 2U, "adapted primitive count") ||
        !Expect(std::memcmp(converted.data(), kVertices.data(), sizeof(kVertices)) == 0,
                "adapted vertex bytes"))
    {
        return 1;
    }

    const LegacySubmissionInput legacyDirect{
        FrontierX::LegacyD3d9::kTriangleList,
        FrontierX::LegacyD3d9::kRpVertexFvf,
        legacyVertexBytes.data(),
        3U,
        nullptr,
        0U,
    };
    if (!Expect(FrontierX::AdaptLegacyRpVertexSubmission(
                    legacyDirect, converted.data(), converted.size(), adapted) ==
                    LegacySubmissionStatus::Ok,
                "direct legacy RPVertex submission adaptation") ||
        !Expect(adapted.IsValid() && adapted.indices == nullptr,
                "direct adapted submission validity") ||
        !Expect(adapted.PrimitiveCount() == 1U, "direct adapted primitive count"))
    {
        return 1;
    }

    PrimitiveTopology mappedTopology = PrimitiveTopology::PointList;
    if (!Expect(FrontierX::MapLegacyPrimitiveTopology(
                    FrontierX::LegacyD3d9::kLineStrip, mappedTopology),
                "legacy line-strip topology mapping") ||
        !Expect(mappedTopology == PrimitiveTopology::LineStrip,
                "mapped line-strip topology") ||
        !Expect(!FrontierX::MapLegacyPrimitiveTopology(
                    FrontierX::LegacyD3d9::kTriangleFan, mappedTopology),
                "legacy triangle fan unexpectedly mapped"))
    {
        return 1;
    }

    const auto ExpectAdapterFailure = [&](const LegacySubmissionInput &input,
                                          std::size_t capacity,
                                          LegacySubmissionStatus status,
                                          const char *message) {
        adapted = {PrimitiveTopology::TriangleList,
                   converted.data(),
                   converted.size(),
                   nullptr,
                   0U};
        const LegacySubmissionStatus actual = FrontierX::AdaptLegacyRpVertexSubmission(
            input, converted.data(), capacity, adapted);
        return Expect(actual == status, message) &&
               Expect(adapted.vertices == nullptr && adapted.vertexCount == 0U,
                      "failed adapter retained output");
    };

    LegacySubmissionInput invalidLegacy = legacyIndexed;
    invalidLegacy.primitiveType = FrontierX::LegacyD3d9::kTriangleFan;
    if (!ExpectAdapterFailure(invalidLegacy,
                              converted.size(),
                              LegacySubmissionStatus::UnsupportedPrimitiveType,
                              "triangle fan adapter status"))
    {
        return 1;
    }
    invalidLegacy = legacyIndexed;
    invalidLegacy.vertexFormat ^= FrontierX::LegacyD3d9::kFvfDiffuse;
    if (!ExpectAdapterFailure(invalidLegacy,
                              converted.size(),
                              LegacySubmissionStatus::UnsupportedVertexFormat,
                              "unsupported FVF adapter status"))
    {
        return 1;
    }
    invalidLegacy = legacyIndexed;
    invalidLegacy.primitiveType = FrontierX::LegacyD3d9::kPointList;
    if (!ExpectAdapterFailure(invalidLegacy,
                              converted.size(),
                              LegacySubmissionStatus::UnsupportedIndexedPointList,
                              "indexed point-list adapter status"))
    {
        return 1;
    }
    invalidLegacy = legacyIndexed;
    invalidLegacy.indices = kBadIndices.data();
    invalidLegacy.indexCount = kBadIndices.size();
    if (!ExpectAdapterFailure(invalidLegacy,
                              converted.size(),
                              LegacySubmissionStatus::InvalidInput,
                              "invalid legacy index adapter status") ||
        !ExpectAdapterFailure(legacyIndexed,
                              converted.size() - 1U,
                              LegacySubmissionStatus::InsufficientDestination,
                              "legacy destination capacity status"))
    {
        return 1;
    }

    CapturingRenderPrimitive capture;
    {
        PrimitiveBuilder builder(&capture, 4U);
        if (!Expect(capture.referenceCount == 2U,
                    "PrimitiveBuilder did not retain IRenderPrimitive"))
        {
            return 1;
        }

        builder.Begin(PB_TRIANGLES);
        builder.Color4ub(0x33U, 0x22U, 0x11U, 0x44U);
        builder.TexCoord2f(0.0F, 0.0F);
        builder.Vertex3f(-1.0F, -1.0F, 0.5F);
        builder.TexCoord2f(1.0F, 0.0F);
        builder.Vertex3f(1.0F, -1.0F, 0.5F);
        builder.TexCoord2f(0.0F, 1.0F);
        builder.Vertex3f(-1.0F, 1.0F, 0.5F);
        builder.End();
        if (!Expect(capture.drawCount == 1U, "production triangle draw count") ||
            !Expect(capture.status == LegacySubmissionStatus::Ok,
                    "production triangle adaptation") ||
            !Expect(capture.submission.topology == PrimitiveTopology::TriangleList,
                    "production triangle topology") ||
            !Expect(capture.submission.vertexCount == 3U,
                    "production triangle vertex count") ||
            !Expect(capture.submission.PrimitiveCount() == 1U,
                    "production triangle primitive count") ||
            !Expect(capture.converted[0].diffuseBgra == 0x44332211U,
                    "production triangle diffuse packing") ||
            !Expect(capture.converted[1].u == 1.0F && capture.converted[2].v == 1.0F,
                    "production triangle texture coordinates"))
        {
            return 1;
        }

        capture.ResetCapture();
        builder.Begin(PB_QUADS);
        builder.Vertex3f(-1.0F, -1.0F, 0.0F);
        builder.Vertex3f(1.0F, -1.0F, 0.0F);
        builder.Vertex3f(1.0F, 1.0F, 0.0F);
        builder.Vertex3f(-1.0F, 1.0F, 0.0F);
        builder.End();
        if (!Expect(capture.status == LegacySubmissionStatus::Ok,
                    "production quad adaptation") ||
            !Expect(capture.submission.vertexCount == 6U,
                    "production quad expansion count") ||
            !Expect(capture.submission.PrimitiveCount() == 2U,
                    "production quad primitive count") ||
            !Expect(SamePosition(capture.converted[0], -1.0F, -1.0F, 0.0F) &&
                        SamePosition(capture.converted[1], 1.0F, -1.0F, 0.0F) &&
                        SamePosition(capture.converted[2], 1.0F, 1.0F, 0.0F) &&
                        SamePosition(capture.converted[3], -1.0F, -1.0F, 0.0F) &&
                        SamePosition(capture.converted[4], 1.0F, 1.0F, 0.0F) &&
                        SamePosition(capture.converted[5], -1.0F, 1.0F, 0.0F),
                    "production quad expansion order"))
        {
            return 1;
        }

        capture.ResetCapture();
        builder.Begin(PB_TRIANGLE_FAN);
        builder.Vertex3f(0.0F, 0.0F, 0.0F);
        builder.Vertex3f(1.0F, 0.0F, 0.0F);
        builder.Vertex3f(0.0F, 1.0F, 0.0F);
        builder.End();
        if (!Expect(capture.drawCount == 1U, "production triangle-fan draw count") ||
            !Expect(capture.status == LegacySubmissionStatus::UnsupportedPrimitiveType,
                    "production triangle-fan rejection"))
        {
            return 1;
        }
    }
    if (!Expect(capture.referenceCount == 1U,
                "PrimitiveBuilder did not release IRenderPrimitive"))
    {
        return 1;
    }

    capture.ResetCapture();
    {
        PrimitiveBuilder2 appBuilder(4U);
        appBuilder.SetIRenderPrimitive(&capture);
        appBuilder.Begin(PB_QUADS, 4U);
        appBuilder.Color4ub(0x33U, 0x22U, 0x11U, 0x44U);
        appBuilder.Vertex3f(-1.0F, -1.0F, 0.0F);
        appBuilder.Vertex3f(1.0F, -1.0F, 0.0F);
        appBuilder.Vertex3f(1.0F, 1.0F, 0.0F);
        appBuilder.Vertex3f(-1.0F, 1.0F, 0.0F);
        appBuilder.End();
        if (!Expect(capture.referenceCount == 2U, "App builder receiver retention") ||
            !Expect(reinterpret_cast<std::uintptr_t>(appBuilder.vertex_buffer) % 32U == 0U,
                    "App builder native vertex alignment") ||
            !Expect(capture.status == LegacySubmissionStatus::Ok,
                    "App builder native draw adaptation") ||
            !Expect(capture.submission.vertexCount == 6U,
                    "App builder quad expansion") ||
            !Expect(capture.converted[0].diffuseBgra == 0x44332211U,
                    "App builder color packing"))
        {
            return 1;
        }
    }
    if (!Expect(capture.referenceCount == 1U, "App builder receiver release"))
    {
        return 1;
    }

    FrontierX::NativeComponentManager manager;
    FrontierX::RegisterNativeRenderPrimitive(&manager);
    if (!Expect(manager.RegisteredCount() == 2U,
                "native render-primitive direct and aggregate factory registration"))
    {
        return 1;
    }
    FrontierX::NativeRenderPrimitiveDesc invalidOwnerDescriptor;
    void * invalidOwner = nullptr;
    if (!Expect(manager.CreateInstance(&invalidOwnerDescriptor, &invalidOwner) ==
                    GR_INVALID_PARMS && !invalidOwner,
                "native render-primitive accepted missing callback"))
    {
        return 1;
    }
    NativeOwnerCapture ownerCapture;
    FrontierX::NativeRenderPrimitiveDesc basicOwnerDescriptor(
        CaptureNativeSubmission, &ownerCapture);
    void * rawBasicOwner = nullptr;
    if (!Expect(manager.CreateInstance(&basicOwnerDescriptor, &rawBasicOwner) == GR_OK &&
                    rawBasicOwner,
                "basic native render-primitive creation"))
    {
        return 1;
    }
    auto * basicOwner = static_cast<IRenderPrimitive *>(rawBasicOwner);
    if (!Expect(basicOwner->draw_primitive_vb(
                    D3DPT_TRIANGLELIST, 1U, 0, 3, 0U) == GR_NOT_IMPLEMENTED,
                "native owner without lookup accepted vertex-buffer draw") ||
        !Expect(basicOwner->draw_primitive(
                    D3DPT_TRIANGLELIST,
                    FrontierX::LegacyD3d9::kRpVertex2Fvf,
                    kVertices2.data(), 3, 0U) == GR_NOT_IMPLEMENTED,
                "native owner without extended callback accepted RPVERTEX2") ||
        !Expect(basicOwner->draw_primitive(
                    D3DPT_TRIANGLELIST,
                    FrontierX::LegacyD3d9::kGameStandardFvf,
                    kVertices2.data(), 3, 0U) == GR_NOT_IMPLEMENTED,
                "native owner without game callback accepted StandardVertex"))
    {
        basicOwner->Release();
        return 1;
    }
    basicOwner->Release();

    FrontierX::NativeRenderPrimitiveDesc ownerDescriptor(
        CaptureNativeSubmission, &ownerCapture, CaptureNativeSubmission2,
        CaptureNativeRenderState,
        LookupNativeVertexBuffer, &ownerCapture, CaptureNativeTextureStage, nullptr,
        nullptr, CaptureNativeProjection, CaptureNativeSubmission3, CaptureNativeSubmission4,
        CaptureNativeSamplerState, CaptureNativeViewport);
    void * rawOwner = nullptr;
    if (!Expect(manager.CreateInstance(&ownerDescriptor, &rawOwner) == GR_OK && rawOwner,
                "native render-primitive creation"))
    {
        return 1;
    }
    auto * nativeOwner = static_cast<IRenderPrimitive *>(rawOwner);
    int viewportX = 0, viewportY = 0, viewportWidth = 0, viewportHeight = 0;
    if (!Expect(nativeOwner->set_ortho(0.0F, 640.0F, 480.0F, 0.0F,
                                      -1.0F, 1.0F) == GR_OK &&
                    ownerCapture.projectionCalls == 1U &&
                    ownerCapture.projection.d[0][0] == 2.0F / 640.0F &&
                    ownerCapture.projection.d[1][1] == -2.0F / 480.0F &&
                    ownerCapture.projection.d[0][3] == -1.0F &&
                    ownerCapture.projection.d[1][3] == 1.0F,
                "native orthographic projection reaches production callback") ||
        !Expect(nativeOwner->set_ortho(0.0F, 0.0F, 480.0F, 0.0F,
                                      -1.0F, 1.0F) == GR_INVALID_PARMS &&
                    ownerCapture.projectionCalls == 1U,
                "invalid orthographic extent leaves projection unchanged") ||
        !Expect(nativeOwner->set_ortho(0.0F,
                                      std::numeric_limits<float>::denorm_min(),
                                      480.0F, 0.0F, -1.0F, 1.0F) ==
                    GR_INVALID_PARMS && ownerCapture.projectionCalls == 1U,
                "overflowing orthographic scale is rejected before callback") ||
        !Expect(nativeOwner->set_perspective(30.0F, 4.0F / 3.0F,
                                             100.0F, 1000.0F) == GR_OK &&
                    ownerCapture.projectionCalls == 2U &&
                    ownerCapture.projection.d[0][0] > 1.29F &&
                    ownerCapture.projection.d[0][0] < 1.30F &&
                    ownerCapture.projection.d[2][2] < -1.11F &&
                    ownerCapture.projection.d[2][2] > -1.12F &&
                    ownerCapture.projection.d[2][3] < -111.0F &&
                    ownerCapture.projection.d[2][3] > -112.0F &&
                    ownerCapture.projection.d[3][2] == -1.0F,
                "native game perspective reaches projection callback with camera -Z") ||
        !Expect(nativeOwner->set_perspective(0.0F, 4.0F / 3.0F,
                                             100.0F, 1000.0F) == GR_INVALID_PARMS &&
                    ownerCapture.projectionCalls == 2U,
                "invalid perspective leaves projection unchanged") ||
        !Expect(nativeOwner->set_viewport(10, 20, 100, 80) == GR_OK &&
                    nativeOwner->get_viewport(&viewportX, &viewportY,
                                              &viewportWidth, &viewportHeight) == GR_OK &&
                    ownerCapture.viewportCalls == 1U &&
                    ownerCapture.viewport == std::array<int, 4>{{10, 20, 100, 80}} &&
                    viewportX == 10 && viewportY == 20 &&
                    viewportWidth == 100 && viewportHeight == 80,
                "native viewport reaches production callback and round trips") ||
        !Expect(nativeOwner->set_viewport(10, 20, 0, 80) == GR_INVALID_PARMS &&
                    ownerCapture.viewportCalls == 1U,
                "invalid viewport leaves render state unchanged"))
    {
        nativeOwner->Release();
        return 1;
    }
    U32 stateValue = 0U;
    if (!Expect(nativeOwner->get_render_state(D3DRS_ZENABLE, &stateValue) == GR_OK &&
                    stateValue == 1U,
                "native owner default depth state") ||
        !Expect(nativeOwner->get_render_state(D3DRS_ALPHABLENDENABLE, &stateValue) == GR_OK &&
                    stateValue == 0U,
                "native owner default alpha state") ||
        !Expect(nativeOwner->set_render_state(D3DRS_ZWRITEENABLE, 0U) == GR_OK &&
                    ownerCapture.renderState.depthWriteEnabled == 0U,
                "native owner depth-write update") ||
        !Expect(nativeOwner->set_render_state(D3DRS_ZFUNC, D3DCMP_LESS) == GR_OK &&
                    ownerCapture.renderState.depthFunction == D3DCMP_LESS,
                "native owner depth-function update") ||
        !Expect(nativeOwner->set_render_state(D3DRS_ALPHABLENDENABLE, 1U) == GR_OK &&
                    ownerCapture.renderState.alphaBlendEnabled == 1U,
                "native owner alpha-enable update") ||
        !Expect(nativeOwner->set_render_state(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA) == GR_OK &&
                    ownerCapture.renderState.sourceBlend == D3DBLEND_SRCALPHA,
                "native owner source-blend update") ||
        !Expect(nativeOwner->set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA) == GR_OK &&
                    ownerCapture.renderState.destinationBlend == D3DBLEND_INVSRCALPHA,
                "native owner destination-blend update") ||
        !Expect(nativeOwner->set_render_state(D3DRS_SPECULARENABLE, 1U) == GR_OK &&
                    ownerCapture.renderState.specularEnabled == 1U &&
                    nativeOwner->get_render_state(D3DRS_SPECULARENABLE, &stateValue) == GR_OK &&
                    stateValue == 1U,
                "native owner specular state") ||
        !Expect(nativeOwner->set_render_state(D3DRS_SPECULARENABLE, 2U) == GR_INVALID_PARMS,
                "native owner invalid specular Boolean") ||
        !Expect(nativeOwner->set_render_state(D3DRS_COLORVERTEX, 1U) == GR_OK &&
                    nativeOwner->get_render_state(D3DRS_COLORVERTEX, &stateValue) == GR_OK &&
                    stateValue == 1U &&
                    nativeOwner->set_render_state(D3DRS_DIFFUSEMATERIALSOURCE,
                                                  D3DMCS_COLOR1) == GR_OK &&
                    nativeOwner->get_render_state(D3DRS_DIFFUSEMATERIALSOURCE,
                                                  &stateValue) == GR_OK &&
                    stateValue == D3DMCS_COLOR1 &&
                    nativeOwner->set_render_state(D3DRS_LIGHTING, 0U) == GR_OK &&
                    nativeOwner->get_render_state(D3DRS_LIGHTING, &stateValue) == GR_OK &&
                    stateValue == 0U,
                "native fixed vertex-color and unlit shader state") ||
        !Expect(nativeOwner->set_render_state(D3DRS_COLORVERTEX, 0U) == GR_NOT_IMPLEMENTED &&
                    nativeOwner->set_render_state(D3DRS_LIGHTING, 1U) == GR_NOT_IMPLEMENTED &&
                    nativeOwner->set_render_state(D3DRS_LIGHTING, 2U) == GR_INVALID_PARMS &&
                    nativeOwner->set_render_state(D3DRS_DIFFUSEMATERIALSOURCE, 0U) ==
                        GR_NOT_IMPLEMENTED,
                "native fixed shader rejects incompatible material state") ||
        !Expect(nativeOwner->set_texture_stage_state(2U, D3DTSS_COLOROP,
                                                     D3DTOP_DISABLE) == GR_OK &&
                    nativeOwner->set_texture_stage_state(3U, D3DTSS_ALPHAOP,
                                                         D3DTOP_DISABLE) == GR_OK &&
                    nativeOwner->set_texture_stage_texture(2U, 0U) == GR_OK &&
                    nativeOwner->set_texture_stage_texture(3U, 0U) == GR_OK &&
                    nativeOwner->get_texture_stage_texture(3U, &stateValue) == GR_OK &&
                    stateValue == 0U,
                "native unused texture stages remain disabled") ||
        !Expect(nativeOwner->set_texture_stage_state(2U, D3DTSS_COLOROP,
                                                     D3DTOP_MODULATE) == GR_NOT_IMPLEMENTED &&
                    nativeOwner->set_texture_stage_texture(2U, 1U) == GR_NOT_IMPLEMENTED &&
                    nativeOwner->set_texture_stage_state(4U, D3DTSS_COLOROP,
                                                         D3DTOP_DISABLE) == GR_NOT_IMPLEMENTED,
                "native unused texture stages reject active values") ||
        !Expect(nativeOwner->get_sampler_state(0U, D3DSAMP_MINFILTER,
                                               &stateValue) == GR_OK &&
                    stateValue == D3DTEXF_POINT &&
                    nativeOwner->set_sampler_state(0U, D3DSAMP_MINFILTER,
                                                   D3DTEXF_LINEAR) == GR_OK &&
                    nativeOwner->set_sampler_state(0U, D3DSAMP_MAGFILTER,
                                                   D3DTEXF_LINEAR) == GR_OK &&
                    nativeOwner->set_sampler_state(0U, D3DSAMP_MIPFILTER,
                                                   D3DTEXF_LINEAR) == GR_OK &&
                    nativeOwner->set_sampler_state(0U, D3DSAMP_ADDRESSU,
                                                   D3DTADDRESS_WRAP) == GR_OK &&
                    nativeOwner->set_sampler_state(0U, D3DSAMP_ADDRESSV,
                                                   D3DTADDRESS_WRAP) == GR_OK &&
                    ownerCapture.samplerState.minFilter == D3DTEXF_LINEAR &&
                    ownerCapture.samplerState.magFilter == D3DTEXF_LINEAR &&
                    ownerCapture.samplerState.mipFilter == D3DTEXF_LINEAR &&
                    ownerCapture.samplerState.addressU == D3DTADDRESS_WRAP &&
                    ownerCapture.samplerState.addressV == D3DTADDRESS_WRAP,
                "native sampler maps linear filtering and wrapping") ||
        !Expect(nativeOwner->set_sampler_state(1U, D3DSAMP_MINFILTER,
                                               D3DTEXF_LINEAR) == GR_NOT_IMPLEMENTED &&
                    nativeOwner->set_sampler_state(0U, D3DSAMP_MINFILTER,
                                                   999U) == GR_NOT_IMPLEMENTED &&
                    nativeOwner->set_sampler_state(0U, D3DSAMP_ADDRESSU,
                                                   999U) == GR_NOT_IMPLEMENTED,
                "native sampler rejects unsupported stage and values") ||
        !Expect(nativeOwner->set_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX, 1U) == GR_OK &&
                    ownerCapture.textureStage.textureCoordinateIndex == 1U &&
                    nativeOwner->get_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX,
                                                          &stateValue) == GR_OK &&
                    stateValue == 1U,
                "native owner texture coordinate selection") ||
        !Expect(nativeOwner->set_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX, 2U) ==
                    GR_NOT_IMPLEMENTED,
                "native owner unsupported texture coordinate index") ||
        !Expect(nativeOwner->set_render_state(D3DRS_ZENABLE, 2U) == GR_INVALID_PARMS,
                "native owner invalid Boolean state") ||
        !Expect(nativeOwner->set_render_state(
                    D3DRS_SRCBLEND, D3DBLEND_BOTHSRCALPHA) == GR_NOT_IMPLEMENTED,
                "native owner unsupported blend factor") ||
        !Expect(nativeOwner->set_render_state(
                    static_cast<D3DRENDERSTATETYPE>(999), 0U) == GR_NOT_IMPLEMENTED,
                "native owner unsupported render state"))
    {
        nativeOwner->Release();
        return 1;
    }
    const std::size_t committedStateCalls = ownerCapture.stateCalls;
    ownerCapture.result = GR_GENERIC;
    if (!Expect(nativeOwner->set_render_state(D3DRS_ZWRITEENABLE, 1U) == GR_GENERIC,
                "native owner state callback failure propagation") ||
        !Expect(nativeOwner->get_render_state(D3DRS_ZWRITEENABLE, &stateValue) == GR_OK &&
                    stateValue == 0U,
                "native owner failed state update rollback") ||
        !Expect(ownerCapture.stateCalls == committedStateCalls + 1U,
                "native owner state callback count"))
    {
        nativeOwner->Release();
        return 1;
    }
    ownerCapture.result = GR_OK;
    const std::size_t committedSamplerCalls = ownerCapture.samplerCalls;
    ownerCapture.result = GR_GENERIC;
    if (!Expect(nativeOwner->set_sampler_state(0U, D3DSAMP_ADDRESSU,
                                               D3DTADDRESS_CLAMP) == GR_GENERIC &&
                    nativeOwner->get_sampler_state(0U, D3DSAMP_ADDRESSU,
                                                   &stateValue) == GR_OK &&
                    stateValue == D3DTADDRESS_WRAP &&
                    ownerCapture.samplerCalls == committedSamplerCalls + 1U,
                "native sampler callback failure preserves prior state"))
    {
        nativeOwner->Release();
        return 1;
    }
    ownerCapture.result = GR_OK;
    const std::size_t committedTextureStageCalls = ownerCapture.textureStageCalls;
    ownerCapture.result = GR_GENERIC;
    if (!Expect(nativeOwner->set_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX, 0U) ==
                    GR_GENERIC,
                "native owner texture-stage callback failure propagation") ||
        !Expect(nativeOwner->get_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX,
                                                      &stateValue) == GR_OK &&
                    stateValue == 1U,
                "native owner failed texture-stage update rollback") ||
        !Expect(ownerCapture.textureStageCalls == committedTextureStageCalls + 1U,
                "native owner texture-stage callback count"))
    {
        nativeOwner->Release();
        return 1;
    }
    ownerCapture.result = GR_OK;
    {
        PrimitiveBuilder builder(nativeOwner, 4U);
        builder.Begin(PB_TRIANGLES);
        builder.Color4ub(0x33U, 0x22U, 0x11U, 0x44U);
        builder.TexCoord2f(0.0F, 0.0F);
        builder.Vertex3f(-1.0F, -1.0F, 0.5F);
        builder.TexCoord2f(1.0F, 0.0F);
        builder.Vertex3f(1.0F, -1.0F, 0.5F);
        builder.TexCoord2f(0.0F, 1.0F);
        builder.Vertex3f(-1.0F, 1.0F, 0.5F);
        builder.End();
    }
    if (!Expect(ownerCapture.calls == 1U, "native owner callback count") ||
        !Expect(ownerCapture.calls2 == 0U, "native owner unexpected extended callback") ||
        !Expect(ownerCapture.topology == PrimitiveTopology::TriangleList,
                "native owner callback topology") ||
        !Expect(ownerCapture.vertices.size() == 3U,
                "native owner callback vertex count") ||
        !Expect(ownerCapture.vertices[0].diffuseBgra == 0x44332211U,
                "native owner callback color") ||
        !Expect(ownerCapture.indices.empty(), "native owner direct submission indices"))
    {
        nativeOwner->Release();
        return 1;
    }

    if (!Expect(nativeOwner->draw_primitive(
                    D3DPT_TRIANGLELIST,
                    FrontierX::LegacyD3d9::kRpVertex2Fvf,
                    kVertices2.data(),
                    3,
                    0U) == GR_OK,
                "native owner extended direct submission") ||
        !Expect(ownerCapture.calls2 == 1U && ownerCapture.vertices2.size() == 3U,
                "native owner extended callback") ||
        !Expect(ownerCapture.vertices2[2].u2 == kVertices2[2].u2 &&
                    ownerCapture.vertices2[2].v2 == kVertices2[2].v2,
                "native owner extended texture coordinates"))
    {
        nativeOwner->Release();
        return 1;
    }

    if (!Expect(nativeOwner->draw_indexed_primitive(
                    D3DPT_TRIANGLELIST, FrontierX::LegacyD3d9::kMtVertexFvf,
                    kMtVertices.data(), static_cast<int>(kMtVertices.size()),
                    kIndices.data(), 3, 0U) == GR_OK &&
                    ownerCapture.calls3 == 1U &&
                    ownerCapture.vertices3.size() == 3U &&
                    ownerCapture.vertices3[0].specularBgra == 0x00102030U &&
                    ownerCapture.vertices3[2].u2 == 0.5F &&
                    ownerCapture.indices.size() == 3U,
                "native primitive indexed PolyMesh callback"))
    {
        nativeOwner->Release();
        return 1;
    }
    ownerCapture.result = GR_GENERIC;
    if (!Expect(nativeOwner->draw_indexed_primitive(
                    D3DPT_TRIANGLELIST, FrontierX::LegacyD3d9::kMtVertexFvf,
                    kMtVertices.data(), static_cast<int>(kMtVertices.size()),
                    kIndices.data(), 3, 0U) == GR_GENERIC,
                "native primitive propagates PolyMesh callback failure"))
    {
        nativeOwner->Release();
        return 1;
    }
    ownerCapture.result = GR_OK;

    std::array<FrontierX::RenderVertexP3N3DiffuseT5, 3U> gameVertices{};
    gameVertices[0].nx = 1.0F;
    gameVertices[0].diffuseBgra = 0x12345678U;
    gameVertices[1].u2 = 0.5F;
    gameVertices[2].binZ = 0.75F;
    if (!Expect(nativeOwner->draw_indexed_primitive(
                    D3DPT_TRIANGLELIST, FrontierX::LegacyD3d9::kGameStandardFvf,
                    gameVertices.data(), static_cast<int>(gameVertices.size()),
                    kIndices.data(), 3, 0U) == GR_OK &&
                    ownerCapture.calls4 == 1U &&
                    ownerCapture.vertices4.size() == gameVertices.size() &&
                    std::memcmp(ownerCapture.vertices4.data(), gameVertices.data(),
                                sizeof(gameVertices)) == 0,
                "native primitive preserves complete App StandardVertex bytes") ||
        !Expect(nativeOwner->draw_indexed_primitive(
                    D3DPT_TRIANGLELIST, FrontierX::LegacyD3d9::kGameStandardFvf,
                    gameVertices.data(), static_cast<int>(gameVertices.size()),
                    kBadIndices.data(), 3, 0U) == GR_INVALID_PARMS,
                "native primitive rejects out-of-range App index") ||
        !Expect(nativeOwner->draw_indexed_primitive(
                    D3DPT_TRIANGLESTRIP, FrontierX::LegacyD3d9::kGameStandardFvf,
                    gameVertices.data(), static_cast<int>(gameVertices.size()),
                    kIndices.data(), 3, 0U) == GR_NOT_IMPLEMENTED,
                "native primitive rejects unsupported App topology"))
    {
        nativeOwner->Release();
        return 1;
    }
    ownerCapture.result = GR_GENERIC;
    if (!Expect(nativeOwner->draw_indexed_primitive(
                    D3DPT_TRIANGLELIST, FrontierX::LegacyD3d9::kGameStandardFvf,
                    gameVertices.data(), static_cast<int>(gameVertices.size()),
                    kIndices.data(), 3, 0U) == GR_GENERIC,
                "native primitive propagates App callback failure"))
    {
        nativeOwner->Release();
        return 1;
    }
    ownerCapture.result = GR_OK;

    ownerCapture.calls = 0U;
    ownerCapture.calls2 = 0U;
    ownerCapture.lookupCalls = 0U;
    constexpr std::array<U16, 3> kSliceIndices = {{0U, 1U, 2U}};
    if (!Expect(nativeOwner->draw_primitive_vb(
                    D3DPT_TRIANGLELIST, 1U, 1, 3, 0U) == GR_OK,
                "registered owner direct vertex-buffer submission") ||
        !Expect(ownerCapture.calls == 1U && ownerCapture.vertices.size() == 3U,
                "registered owner direct vertex-buffer callback") ||
        !Expect(std::memcmp(ownerCapture.vertices.data(), kVertices.data() + 1U,
                            3U * sizeof(RenderVertexP3DiffuseT2)) == 0,
                "registered owner direct vertex-buffer slice") ||
        !Expect(nativeOwner->draw_indexed_primitive_vb(
                    D3DPT_TRIANGLELIST, 2U, 1, 3, kSliceIndices.data(),
                    static_cast<int>(kSliceIndices.size()), 0U) == GR_OK,
                "registered owner indexed extended vertex-buffer submission") ||
        !Expect(ownerCapture.calls2 == 1U && ownerCapture.vertices2.size() == 3U &&
                    ownerCapture.indices ==
                        std::vector<std::uint16_t>(kSliceIndices.begin(), kSliceIndices.end()),
                "registered owner indexed extended vertex-buffer callback") ||
        !Expect(std::memcmp(ownerCapture.vertices2.data(), kVertices2.data() + 1U,
                            3U * sizeof(RenderVertexP3DiffuseT4)) == 0,
                "registered owner indexed extended vertex-buffer slice") ||
        !Expect(ownerCapture.lookupCalls == 2U,
                "registered owner vertex-buffer lookup count") ||
        !Expect(nativeOwner->draw_primitive_vb(
                    D3DPT_TRIANGLELIST, 3U, 0, 3, 0U) == GR_NOT_IMPLEMENTED,
                "registered owner unsupported vertex-buffer FVF") ||
        !Expect(nativeOwner->draw_primitive_vb(
                    D3DPT_TRIANGLELIST, 99U, 0, 3, 0U) == GR_GENERIC,
                "registered owner missing vertex-buffer handle") ||
        !Expect(ownerCapture.calls == 1U && ownerCapture.calls2 == 1U,
                "registered owner invoked callback for rejected vertex buffer"))
    {
        nativeOwner->Release();
        return 1;
    }

    ownerCapture.calls = 0U;
    const std::array<RPVertex, 4> ownerVertices = {{
        MakeRpVertex(-1.0F, -1.0F, 0.5F, 0.0F, 1.0F),
        MakeRpVertex(1.0F, -1.0F, 0.5F, 1.0F, 1.0F),
        MakeRpVertex(1.0F, 1.0F, 0.5F, 1.0F, 0.0F),
        MakeRpVertex(-1.0F, 1.0F, 0.5F, 0.0F, 0.0F),
    }};
    if (!Expect(nativeOwner->draw_indexed_primitive(
                    D3DPT_TRIANGLELIST,
                    D3DFVF_RPVERTEX,
                    ownerVertices.data(),
                    static_cast<int>(ownerVertices.size()),
                    kIndices.data(),
                    static_cast<int>(kIndices.size()),
                    0U) == GR_OK,
                "native owner indexed submission") ||
        !Expect(ownerCapture.calls == 1U && ownerCapture.vertices.size() == 4U &&
                    ownerCapture.indices.size() == kIndices.size(),
                "native owner indexed callback") ||
        !Expect(nativeOwner->draw_primitive(
                    D3DPT_TRIANGLEFAN,
                    D3DFVF_RPVERTEX,
                    ownerVertices.data(),
                    4,
                    0U) == GR_NOT_IMPLEMENTED,
                "native owner triangle-fan rejection") ||
        !Expect(ownerCapture.calls == 1U,
                "native owner invoked callback for rejected topology"))
    {
        nativeOwner->Release();
        return 1;
    }
    ownerCapture.result = GR_GENERIC;
    if (!Expect(nativeOwner->draw_primitive(
                    D3DPT_TRIANGLELIST,
                    D3DFVF_RPVERTEX,
                    ownerVertices.data(),
                    3,
                    0U) == GR_GENERIC,
                "native owner callback failure propagation"))
    {
        nativeOwner->Release();
        return 1;
    }
    nativeOwner->Release();

    ownerCapture.result = GR_OK;
    ownerCapture.calls = 0U;
    ownerCapture.indices.clear();
    FrontierX::NativeRenderPipeline ownedResources{nullptr};
    IRP_VERTEXBUFFERHANDLE ownedHandle = 0U;
    void * ownedVertices = nullptr;
    U32 ownedCapacity = 0U;
    if (!Expect(ownedResources.create_vertex_buffer(
                    D3DFVF_RPVERTEX, static_cast<int>(ownerVertices.size()), 0U,
                    &ownedHandle) == GR_OK && ownedHandle != 0U,
                "registered owner resource buffer creation") ||
        !Expect(ownedResources.lock_vertex_buffer(
                    ownedHandle, 0U, &ownedVertices, &ownedCapacity) == GR_OK &&
                    ownedVertices && ownedCapacity == ownerVertices.size(),
                "registered owner resource buffer lock"))
    {
        return 1;
    }
    std::memcpy(ownedVertices, ownerVertices.data(), sizeof(ownerVertices));
    if (!Expect(ownedResources.unlock_vertex_buffer(ownedHandle) == GR_OK,
                "registered owner resource buffer unlock"))
    {
        return 1;
    }
    FrontierX::NativeRenderPrimitiveDesc resourceOwnerDescriptor(
        CaptureNativeSubmission, &ownerCapture, nullptr, nullptr,
        FrontierX::NativeRenderPipeline::OwnedVertexBufferLookup, &ownedResources);
    void * rawResourceOwner = nullptr;
    if (!Expect(manager.CreateInstance(&resourceOwnerDescriptor, &rawResourceOwner) == GR_OK &&
                    rawResourceOwner,
                "registered resource-backed owner creation"))
    {
        return 1;
    }
    auto * resourceOwner = static_cast<IRenderPrimitive *>(rawResourceOwner);
    if (!Expect(resourceOwner->draw_indexed_primitive_vb(
                    D3DPT_TRIANGLELIST, ownedHandle, 0,
                    static_cast<int>(ownerVertices.size()), kIndices.data(),
                    static_cast<int>(kIndices.size()), 0U) == GR_OK,
                "registered resource-backed indexed draw") ||
        !Expect(ownerCapture.calls == 1U &&
                    ownerCapture.vertices.size() == ownerVertices.size() &&
                    ownerCapture.indices.size() == kIndices.size(),
                "registered resource-backed callback") ||
        !Expect(ownedResources.destroy_vertex_buffer(ownedHandle) == GR_OK,
                "registered owner resource buffer destroy") ||
        !Expect(resourceOwner->draw_primitive_vb(
                    D3DPT_TRIANGLELIST, ownedHandle, 0, 3, 0U) == GR_GENERIC,
                "registered owner rejects destroyed resource handle"))
    {
        resourceOwner->Release();
        return 1;
    }
    resourceOwner->Release();
    if (!Expect(manager.ShutDown() == GR_OK,
                "native render-primitive manager shutdown"))
    {
        return 1;
    }

    std::cout << "render submission probe passed\n";
    return 0;
}
