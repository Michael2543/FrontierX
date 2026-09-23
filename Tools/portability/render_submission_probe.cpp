#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "RPUL/PrimitiveBuilder.h"

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
using FrontierX::RenderVertexP3DiffuseT2;
using FrontierX::RenderVertexP3DiffuseT4;

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
    std::size_t lookupCalls = 0U;
    std::size_t stateCalls = 0U;
    GENRESULT result = GR_OK;
    PrimitiveTopology topology = PrimitiveTopology::PointList;
    FrontierX::RenderStateSnapshot renderState{};
    std::vector<RenderVertexP3DiffuseT2> vertices;
    std::vector<RenderVertexP3DiffuseT4> vertices2;
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

    FrontierX::NativeComponentManager manager;
    FrontierX::RegisterNativeRenderPrimitive(&manager);
    if (!Expect(manager.RegisteredCount() == 1U,
                "native render-primitive factory registration"))
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
                "native owner without extended callback accepted RPVERTEX2"))
    {
        basicOwner->Release();
        return 1;
    }
    basicOwner->Release();

    FrontierX::NativeRenderPrimitiveDesc ownerDescriptor(
        CaptureNativeSubmission, &ownerCapture, CaptureNativeSubmission2,
        CaptureNativeRenderState,
        LookupNativeVertexBuffer, &ownerCapture);
    void * rawOwner = nullptr;
    if (!Expect(manager.CreateInstance(&ownerDescriptor, &rawOwner) == GR_OK && rawOwner,
                "native render-primitive creation"))
    {
        return 1;
    }
    auto * nativeOwner = static_cast<IRenderPrimitive *>(rawOwner);
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
