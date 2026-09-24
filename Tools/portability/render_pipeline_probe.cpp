#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/RenderSmokeScene.h"
#include "rendpipeline.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <vector>

static_assert(std::is_base_of_v<IDAComponent, IRenderPipeline>,
              "IRenderPipeline must retain its DACOM base interface");
static_assert(std::is_base_of_v<IRenderPipeline, FrontierX::NativeRenderPipeline>,
              "native pipeline must implement the established renderer boundary");
static_assert(sizeof(IRP_VERTEXBUFFERHANDLE) == sizeof(U32),
              "render-pipeline vertex handles must remain 32-bit tokens");
#if !defined(_WIN32)
static_assert(sizeof(DDPIXELFORMAT) == 32U,
              "portable DirectDraw pixel declaration must match the legacy record");
static_assert(sizeof(RECT) == 16U,
              "portable render rectangles must retain four 32-bit fields");
#endif

namespace
{
using FrontierX::RenderVertexP3DiffuseT2;

constexpr std::uint32_t kVertexBufferA = 1U;
constexpr std::uint32_t kVertexBufferB = 2U;
constexpr std::uint32_t kVertexBuffer2 = 3U;
int vertexBufferOwner = 0;

bool Expect(bool condition, const char * message)
{
    if (!condition)
    {
        std::cerr << "render pipeline probe failed: " << message << '\n';
    }
    return condition;
}

class CapturingRenderPrimitive final : public IRenderPrimitive
{
public:
    GENRESULT COMAPI QueryInterface(const C8 *, void ** instance) override
    {
        *instance = nullptr;
        return GR_INTERFACE_UNSUPPORTED;
    }
    U32 COMAPI AddRef() override { return ++referenceCount; }
    U32 COMAPI Release() override { return --referenceCount; }
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
                                    const void * vertices,
                                    int vertexCount,
                                    U32) override
    {
        ++drawCount;
        type = primitiveType;
        format = vertexFormat;
        sourceCount = vertexCount;
        const std::size_t stride =
            vertexFormat == FrontierX::LegacyD3d9::kRpVertexFvf
                ? sizeof(FrontierX::RenderVertexP3DiffuseT2)
                : sizeof(FrontierX::RenderVertexP3DiffuseT4);
        sourceBytes.resize(static_cast<std::size_t>(vertexCount) * stride);
        std::memcpy(sourceBytes.data(), vertices, sourceBytes.size());
        return result;
    }

    GENRESULT COMAPI draw_indexed_primitive(D3DPRIMITIVETYPE primitiveType,
                                            U32 vertexFormat,
                                            const void * vertices,
                                            int vertexCount,
                                            const U16 * indices,
                                            int indexCount,
                                            U32) override
    {
        const GENRESULT drawResult =
            draw_primitive(primitiveType, vertexFormat, vertices, vertexCount, 0U);
        return drawResult == GR_OK ? StoreIndices(indices, indexCount) : drawResult;
    }

    GENRESULT COMAPI draw_primitive_vb(D3DPRIMITIVETYPE, IRP_VERTEXBUFFERHANDLE, int,
                                       int, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI draw_indexed_primitive_vb(D3DPRIMITIVETYPE, IRP_VERTEXBUFFERHANDLE,
                                               int, int, const U16 *, int, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI flush(U32) override { return GR_NOT_IMPLEMENTED; }

    GENRESULT StoreIndices(const U16 * source, int count)
    {
        if (!source || count <= 0)
        {
            return GR_INVALID_PARMS;
        }
        indices.assign(source, source + count);
        return GR_OK;
    }

    int drawCount = 0;
    int sourceCount = 0;
    D3DPRIMITIVETYPE type{};
    U32 format = 0U;
    GENRESULT result = GR_OK;
    std::vector<unsigned char> sourceBytes;
    std::vector<U16> indices;
    U32 referenceCount = 1U;
};

std::array<RenderVertexP3DiffuseT2, 3> vertexBufferA{{
    {-1.0F, -1.0F, 0.5F, FrontierX::PackDiffuseBgra(255U, 0U, 0U, 255U), 0.0F, 1.0F},
    {1.0F, -1.0F, 0.5F, FrontierX::PackDiffuseBgra(0U, 255U, 0U, 255U), 1.0F, 1.0F},
    {1.0F, 1.0F, 0.5F, FrontierX::PackDiffuseBgra(0U, 0U, 255U, 255U), 1.0F, 0.0F},
}};

std::array<RenderVertexP3DiffuseT2, 4> vertexBufferB{{
    {-0.5F, -0.5F, 0.2F, FrontierX::PackDiffuseBgra(10U, 20U, 30U, 40U), 0.0F, 1.0F},
    {0.5F, -0.5F, 0.2F, FrontierX::PackDiffuseBgra(50U, 60U, 70U, 80U), 1.0F, 1.0F},
    {0.5F, 0.5F, 0.2F, FrontierX::PackDiffuseBgra(90U, 100U, 110U, 120U), 1.0F, 0.0F},
    {-0.5F, 0.5F, 0.2F, FrontierX::PackDiffuseBgra(130U, 140U, 150U, 160U), 0.0F, 0.0F},
}};

std::array<FrontierX::RenderVertexP3DiffuseT4, 3> vertexBuffer2{{
    {-0.6F, -0.6F, 0.1F, FrontierX::PackDiffuseBgra(15U, 25U, 35U, 45U), 0.0F, 1.0F, 0.0F, 0.0F},
    {0.6F, -0.6F, 0.1F, FrontierX::PackDiffuseBgra(55U, 65U, 75U, 85U), 1.0F, 1.0F, 0.0F, 1.0F},
    {0.6F, 0.6F, 0.1F, FrontierX::PackDiffuseBgra(95U, 105U, 115U, 125U), 1.0F, 0.0F, 1.0F, 1.0F},
}};

FrontierX::NativeVertexBufferView LookupVertexBuffer(void * context,
                                                     IRP_VERTEXBUFFERHANDLE handle)
{
    using namespace FrontierX;
    if (context != &vertexBufferOwner)
    {
        return {};
    }
    switch (handle)
    {
    case kVertexBufferA:
        return {vertexBufferA.data(), sizeof(vertexBufferA), LegacyD3d9::kRpVertexFvf};
    case kVertexBufferB:
        return {vertexBufferB.data(), sizeof(vertexBufferB), LegacyD3d9::kRpVertexFvf};
    case kVertexBuffer2:
        return {vertexBuffer2.data(), sizeof(vertexBuffer2), LegacyD3d9::kRpVertex2Fvf};
    case 4U:
        return {vertexBufferA.data(), sizeof(vertexBufferA) - 1U, LegacyD3d9::kRpVertexFvf};
    case 5U:
        return {vertexBuffer2.data(), sizeof(vertexBuffer2) - 1U, LegacyD3d9::kRpVertex2Fvf};
    case 6U:
        return {vertexBufferA.data(), sizeof(vertexBufferA), 0xFFFFFFFFU};
    default:
        return {};
    }
}

bool CheckInterfaceRegressions()
{
    using namespace FrontierX;
    CapturingRenderPrimitive capture;
    NativeRenderPipeline pipeline{&capture};
    IRenderPipeline * interface = nullptr;
    IDAComponent * component = nullptr;
    void * unsupported = reinterpret_cast<void *>(1U);
    U32 ability = 99U;
    if (!Expect(pipeline.QueryInterface(IID_IRenderPipeline,
                                        reinterpret_cast<void **>(&interface)) == GR_OK &&
                    interface == static_cast<IRenderPipeline *>(&pipeline),
                "native IRenderPipeline query") ||
        !Expect(interface->Release() == 1U, "queried pipeline reference released") ||
        !Expect(pipeline.QueryInterface(IID_IDAComponent,
                                        reinterpret_cast<void **>(&component)) == GR_OK &&
                    component == static_cast<IDAComponent *>(&pipeline),
                "native base-component query") ||
        !Expect(component->Release() == 1U, "queried base-component reference released") ||
        !Expect(pipeline.QueryInterface("unsupported", &unsupported) ==
                    GR_INTERFACE_UNSUPPORTED && !unsupported,
                "unsupported pipeline query cleared") ||
        !Expect(pipeline.query_device_ability(RP_A_DEPTH_BIAS, &ability) == GR_OK &&
                    ability == 0U,
                "unsupported depth bias reported as absent") ||
        !Expect(pipeline.query_device_ability(RP_A_DEVICE_MEMORY, &ability) ==
                    GR_NOT_IMPLEMENTED && ability == 0U,
                "unknown native ability rejected deterministically") ||
        !Expect(interface->startup() == GR_NOT_IMPLEMENTED,
                "unimplemented pipeline lifecycle fails explicitly") ||
        !Expect(interface->draw_primitive(
                    D3DPT_TRIANGLELIST, LegacyD3d9::kRpVertexFvf,
                    vertexBufferA.data(), static_cast<int>(vertexBufferA.size()), 0U) == GR_OK &&
                    capture.drawCount == 1,
                "direct draw crosses IRenderPipeline vtable") ||
        !Expect(SubmitRenderSmokeScene(interface) == GR_OK &&
                    capture.drawCount == 2 &&
                    capture.sourceCount == static_cast<int>(RenderSmokeVertexCount),
                "production PrimitiveBuilder crosses IRenderPipeline vtable"))
    {
        return false;
    }
    const U16 indices[] = {0U, 1U, 2U};
    return Expect(interface->draw_indexed_primitive(
                      D3DPT_TRIANGLELIST, LegacyD3d9::kRpVertexFvf,
                      vertexBufferA.data(), static_cast<int>(vertexBufferA.size()),
                      indices, 3, 0U) == GR_OK &&
                      capture.drawCount == 3 && capture.indices.size() == 3U,
                  "indexed draw crosses IRenderPipeline vtable");
}

// Regression for RenderBatcher's nonzero start_vert draws and the D3D9
// BaseVertexIndex contract. The existing submission probe has no buffer view.
bool CheckPipelineRegressions()
{
    using namespace FrontierX;
    CapturingRenderPrimitive capture;
    NativeRenderPipeline pipeline{LookupVertexBuffer, &vertexBufferOwner, &capture};
    const std::array<std::uint32_t, 2> handles{kVertexBufferA, kVertexBuffer2};
    const U16 valid[] = {0U, 1U, 2U};
    const U16 invalid[] = {0U, 1U, 3U};
    for (std::size_t format = 0; format < handles.size(); ++format)
    {
        const auto handle = handles[format];
        const auto truncatedHandle = static_cast<std::uint32_t>(4U + format);
        capture.drawCount = 0;
        if (!Expect(pipeline.draw_indexed_primitive_vb(D3DPT_TRIANGLELIST, handle, 0, 3,
                                                       nullptr, 3, 0U) == GR_INVALID_PARMS,
                    "null indices rejected") ||
            !Expect(pipeline.draw_indexed_primitive_vb(D3DPT_TRIANGLELIST, handle, 0, 3,
                                                       valid, 0, 0U) == GR_INVALID_PARMS,
                    "empty indexed draw rejected") ||
            !Expect(pipeline.draw_indexed_primitive_vb(D3DPT_TRIANGLELIST, handle, 0, 3,
                                                       invalid, 3, 0U) == GR_INVALID_PARMS,
                    "relative index bounds rejected") ||
            !Expect(pipeline.draw_primitive_vb(D3DPT_TRIANGLELIST, truncatedHandle,
                                               0, 3, 0U) == GR_INVALID_PARMS,
                    "truncated direct buffer rejected") ||
            !Expect(pipeline.draw_indexed_primitive_vb(D3DPT_TRIANGLELIST, truncatedHandle,
                                                       0, 3, valid, 3, 0U) == GR_INVALID_PARMS,
                    "truncated indexed buffer rejected") ||
            !Expect(pipeline.draw_primitive_vb(D3DPT_TRIANGLELIST, handle,
                                               0, 1, 0U) == GR_INVALID_PARMS,
                    "incomplete direct triangle rejected") ||
            !Expect(pipeline.draw_indexed_primitive_vb(D3DPT_TRIANGLELIST, handle,
                                                       0, 3, valid, 2, 0U) == GR_INVALID_PARMS,
                    "incomplete indexed triangle rejected") ||
            !Expect(capture.drawCount == 0, "invalid draws never reach owner"))
        {
            return false;
        }
        if (!Expect(pipeline.draw_indexed_primitive_vb(D3DPT_LINESTRIP, handle, 1, 2,
                                                       valid, 2, 0U) == GR_OK,
                    "nonzero indexed slice accepted") ||
            !Expect(capture.indices == std::vector<U16>({0U, 1U}),
                    "slice-relative indices preserved"))
        {
            return false;
        }
        const auto view = LookupVertexBuffer(&vertexBufferOwner, handle);
        const std::size_t stride = format == 0 ? sizeof(RenderVertexP3DiffuseT2)
                                               : sizeof(RenderVertexP3DiffuseT4);
        if (!Expect(std::memcmp(capture.sourceBytes.data(),
                                static_cast<const unsigned char *>(view.vertices) + stride,
                                2U * stride) == 0, "nonzero slice bytes preserved"))
        {
            return false;
        }
        capture.result = GR_GENERIC;
        if (!Expect(pipeline.draw_primitive_vb(D3DPT_TRIANGLELIST, handle,
                                               0, 3, 0U) == GR_GENERIC,
                    "owner failure propagated"))
        {
            return false;
        }
        capture.result = GR_OK;
    }
    NativeRenderPipeline missingOwner{LookupVertexBuffer, &vertexBufferOwner, nullptr};
    NativeRenderPipeline missingLookup{nullptr, &vertexBufferOwner, &capture};
    NativeRenderPipeline wrongContext{LookupVertexBuffer, nullptr, &capture};
    return Expect(missingOwner.draw_primitive_vb(D3DPT_TRIANGLELIST, kVertexBufferA,
                                                 0, 3, 0U) == GR_GENERIC,
                  "missing owner rejected") &&
           Expect(missingLookup.draw_primitive_vb(D3DPT_TRIANGLELIST, kVertexBufferA,
                                                  0, 3, 0U) == GR_GENERIC,
                  "missing lookup rejected") &&
           Expect(wrongContext.draw_primitive_vb(D3DPT_TRIANGLELIST, kVertexBufferA,
                                                 0, 3, 0U) == GR_GENERIC,
                  "lookup context forwarded") &&
           Expect(pipeline.draw_primitive_vb(D3DPT_TRIANGLELIST, 6U,
                                             0, 3, 0U) == GR_NOT_IMPLEMENTED,
                  "unsupported buffer format rejected");
}

// Regression for RenderBatcher's create/lock/fill/unlock/draw/relock/destroy
// ownership sequence. The external lookup checks above cannot exercise lifetime.
bool CheckOwnedVertexBufferRegressions()
{
    using namespace FrontierX;
    CapturingRenderPrimitive capture;
    NativeRenderPipeline pipeline{&capture};
    IRP_VERTEXBUFFERHANDLE handle = 99U;
    if (!Expect(pipeline.create_vertex_buffer(
                    LegacyD3d9::kRpVertexFvf, 4, 0U, nullptr) == GR_INVALID_PARMS,
                "owned create requires output") ||
        !Expect(pipeline.create_vertex_buffer(
                    LegacyD3d9::kRpVertexFvf, 0, 0U, &handle) == GR_INVALID_PARMS &&
                    handle == 0U,
                "owned create rejects empty buffer") ||
        !Expect(pipeline.create_vertex_buffer(
                    0xFFFFFFFFU, 4, 0U, &handle) == GR_NOT_IMPLEMENTED && handle == 0U,
                "owned create rejects unsupported format") ||
        !Expect(pipeline.create_vertex_buffer(
                    LegacyD3d9::kRpVertexFvf, 4, 8U, &handle) == GR_NOT_IMPLEMENTED &&
                    handle == 0U,
                "owned create rejects unknown flags") ||
        !Expect(pipeline.create_vertex_buffer(
                    LegacyD3d9::kRpVertexFvf, 4, 7U, &handle) == GR_OK && handle != 0U,
                "owned RPVertex buffer created"))
    {
        return false;
    }

    void * vertices = nullptr;
    U32 vertexCount = 0U;
    if (!Expect(pipeline.lock_vertex_buffer(handle, 0U, &vertices, &vertexCount) == GR_OK &&
                    vertices && vertexCount == vertexBufferB.size(),
                "owned RPVertex buffer locked") ||
        !Expect(pipeline.draw_primitive_vb(
                    D3DPT_TRIANGLELIST, handle, 1, 3, 0U) == GR_GENERIC,
                "locked owned buffer cannot draw"))
    {
        return false;
    }
    std::memcpy(vertices, vertexBufferB.data(), sizeof(vertexBufferB));
    vertices = reinterpret_cast<void *>(1U);
    vertexCount = 99U;
    if (!Expect(pipeline.lock_vertex_buffer(handle, 0U, &vertices, &vertexCount) ==
                    GR_INVALID_PARMS && !vertices && vertexCount == 0U,
                "owned buffer rejects double lock") ||
        !Expect(pipeline.destroy_vertex_buffer(handle) == GR_GENERIC,
                "owned buffer rejects destroy while locked") ||
        !Expect(pipeline.unlock_vertex_buffer(handle) == GR_OK,
                "owned RPVertex buffer unlocked") ||
        !Expect(pipeline.unlock_vertex_buffer(handle) == GR_INVALID_PARMS,
                "owned buffer rejects double unlock") ||
        !Expect(pipeline.draw_primitive_vb(
                    D3DPT_TRIANGLELIST, handle, 1, 3, 0U) == GR_OK,
                "owned RPVertex nonzero slice drawn") ||
        !Expect(capture.drawCount == 1 &&
                    std::memcmp(capture.sourceBytes.data(), vertexBufferB.data() + 1,
                                3U * sizeof(RenderVertexP3DiffuseT2)) == 0,
                "owned RPVertex bytes preserved"))
    {
        return false;
    }

    const U16 indices[] = {0U, 1U, 2U};
    if (!Expect(pipeline.draw_indexed_primitive_vb(
                    D3DPT_TRIANGLELIST, handle, 1, 3, indices, 3, 0U) == GR_OK,
                "owned RPVertex indexed draw") ||
        !Expect(capture.indices == std::vector<U16>({0U, 1U, 2U}),
                "owned indices remain slice-relative") ||
        !Expect(pipeline.lock_vertex_buffer(handle, 0U, &vertices, &vertexCount) == GR_OK &&
                    vertexCount == vertexBufferB.size(),
                "owned buffer relocked after draw"))
    {
        return false;
    }
    std::memcpy(vertices, vertexBufferB.data(), sizeof(vertexBufferB));
    if (!Expect(pipeline.unlock_vertex_buffer(handle) == GR_OK,
                "owned buffer second unlock") ||
        !Expect(pipeline.destroy_vertex_buffer(handle) == GR_OK,
                "owned RPVertex buffer destroyed"))
    {
        return false;
    }
    vertices = reinterpret_cast<void *>(1U);
    vertexCount = 99U;
    if (!Expect(pipeline.lock_vertex_buffer(handle, 0U, &vertices, &vertexCount) ==
                    GR_INVALID_PARMS && !vertices && vertexCount == 0U,
                "stale owned handle cannot lock") ||
        !Expect(pipeline.draw_primitive_vb(
                    D3DPT_TRIANGLELIST, handle, 0, 3, 0U) == GR_GENERIC,
                "stale owned handle cannot draw") ||
        !Expect(pipeline.destroy_vertex_buffer(handle) == GR_INVALID_PARMS,
                "stale owned handle cannot destroy"))
    {
        return false;
    }

    IRP_VERTEXBUFFERHANDLE extendedHandle = 0U;
    if (!Expect(pipeline.create_vertex_buffer(
                    LegacyD3d9::kRpVertex2Fvf, 3, 0U, &extendedHandle) == GR_OK &&
                    extendedHandle > handle,
                "owned RPVERTEX2 buffer created with fresh handle") ||
        !Expect(pipeline.lock_vertex_buffer(
                    extendedHandle, 0U, &vertices, &vertexCount) == GR_OK &&
                    vertexCount == vertexBuffer2.size(),
                "owned RPVERTEX2 buffer locked"))
    {
        return false;
    }
    std::memcpy(vertices, vertexBuffer2.data(), sizeof(vertexBuffer2));
    if (!Expect(pipeline.unlock_vertex_buffer(extendedHandle) == GR_OK,
                "owned RPVERTEX2 buffer unlocked") ||
        !Expect(pipeline.draw_indexed_primitive_vb(
                    D3DPT_TRIANGLELIST, extendedHandle, 0, 3, indices, 3, 0U) == GR_OK,
                "owned RPVERTEX2 indexed draw") ||
        !Expect(capture.format == LegacyD3d9::kRpVertex2Fvf &&
                    std::memcmp(capture.sourceBytes.data(), vertexBuffer2.data(),
                                sizeof(vertexBuffer2)) == 0,
                "owned RPVERTEX2 bytes preserved") ||
        !Expect(pipeline.destroy_vertex_buffer(extendedHandle) == GR_OK,
                "owned RPVERTEX2 buffer destroyed"))
    {
        return false;
    }

    NativeRenderPipeline external{LookupVertexBuffer, &vertexBufferOwner, &capture};
    return Expect(external.create_vertex_buffer(
                      LegacyD3d9::kRpVertexFvf, 3, 0U, &handle) == GR_NOT_IMPLEMENTED,
                  "external lookup ownership remains separate");
}

bool CheckExtendedSubmissionRegressions()
{
    using namespace FrontierX;
    RenderVertexP3DiffuseT4 destination[3]{};
    // Deliberately unaligned legacy bytes must be copied, never read as typed vertices.
    std::array<unsigned char, sizeof(destination) + 1U> source{};
    std::memcpy(source.data() + 1U, vertexBuffer2.data(), sizeof(destination));
    LegacySubmissionInput input{LegacyD3d9::kTriangleList, LegacyD3d9::kRpVertex2Fvf,
                                source.data() + 1U, 3U, nullptr, 0U};
    RenderSubmissionView2 output{};
    if (!Expect(AdaptLegacyRpVertex2Submission(input, destination, 3U, output) ==
                    LegacySubmissionStatus::Ok, "unaligned legacy bytes accepted") ||
        !Expect(output.vertices == destination &&
                    std::memcmp(destination, vertexBuffer2.data(), sizeof(destination)) == 0,
                "extended adapter copies into caller storage"))
    {
        return false;
    }
    source.fill(0U);
    if (!Expect(std::memcmp(output.vertices, vertexBuffer2.data(), sizeof(destination)) == 0,
                "extended output independent of source lifetime"))
    {
        return false;
    }
    const U16 invalid[] = {0U, 1U, 99U};
    input.indices = invalid;
    input.indexCount = 3U;
    if (!Expect(AdaptLegacyRpVertex2Submission(input, destination, 3U, output) ==
                    LegacySubmissionStatus::InvalidInput && output.vertices == nullptr,
                "extended adapter rejects out-of-range indices") ||
        !Expect(std::memcmp(destination, vertexBuffer2.data(), sizeof(destination)) == 0,
                "failed validation does not copy"))
    {
        return false;
    }
    // Expected validity for element counts 1 through 6, independent of implementation.
    struct TopologyCase { PrimitiveTopology topology; std::array<bool, 6> valid; };
    const TopologyCase cases[] = {
        {PrimitiveTopology::PointList, {true, true, true, true, true, true}},
        {PrimitiveTopology::LineList, {false, true, false, true, false, true}},
        {PrimitiveTopology::LineStrip, {false, true, true, true, true, true}},
        {PrimitiveTopology::TriangleList, {false, false, true, false, false, true}},
        {PrimitiveTopology::TriangleStrip, {false, false, true, true, true, true}},
        {static_cast<PrimitiveTopology>(255), {false, false, false, false, false, false}},
    };
    const U16 indices[] = {0U, 1U, 2U, 0U, 1U, 2U};
    for (const auto & test : cases)
    {
        for (std::size_t count = 1U; count <= test.valid.size(); ++count)
        {
            const RenderSubmissionView2 direct{test.topology, destination, count, nullptr, 0U};
            const RenderSubmissionView2 indexed{test.topology, destination, 3U, indices, count};
            if (!Expect(direct.IsValid() == test.valid[count - 1U] &&
                            indexed.IsValid() == test.valid[count - 1U],
                        "extended topology element counts"))
            {
                return false;
            }
        }
    }
    return true;
}

struct ReusedTextureHandle
{
    unsigned creates = 0U;
    unsigned destroys = 0U;
};

GENRESULT CreateReusedTexture(void * context, U32, U32, U32 & handle)
{
    ++static_cast<ReusedTextureHandle *>(context)->creates;
    handle = 41U;
    return GR_OK;
}

GENRESULT DestroyReusedTexture(void * context, U32 handle)
{
    if (handle != 41U) return GR_INVALID_PARMS;
    ++static_cast<ReusedTextureHandle *>(context)->destroys;
    return GR_OK;
}

bool CheckTextureHandleCollision()
{
    ReusedTextureHandle owner;
    FrontierX::NativeTextureOperations operations{};
    operations.context = &owner;
    operations.create = CreateReusedTexture;
    operations.destroy = DestroyReusedTexture;
    FrontierX::NativeRenderPipeline pipeline{nullptr, operations};
    const PixelFormat format(PF_RGBA8);
    U32 first = 0U;
    U32 duplicate = 99U;
    U32 width = 0U;
    U32 height = 0U;
    U32 levels = 0U;
    return Expect(pipeline.create_texture(2, 2, format, 0, 0U, first) == GR_OK &&
                      first == 41U, "first native texture handle") &&
        Expect(pipeline.create_texture(4, 4, format, 0, 0U, duplicate) == GR_GENERIC &&
                   duplicate == 0U, "reused native texture handle rejected") &&
        Expect(pipeline.get_texture_dim(first, &width, &height, &levels) == GR_OK &&
                   width == 2U && height == 2U && levels == 1U,
               "collision preserves original texture metadata") &&
        Expect(pipeline.destroy_texture(first) == GR_OK && owner.creates == 2U &&
                   owner.destroys == 1U, "original texture remains destroyable");
}

struct LockTextureBackend
{
    std::array<std::uint8_t, 16> committed{};
    bool failUpload = false;
    unsigned uploads = 0U;
    unsigned destroys = 0U;
};

GENRESULT CreateLockTexture(void *, U32 width, U32 height, U32 & handle)
{
    if (width != 2U || height != 2U) return GR_INVALID_PARMS;
    handle = 52U;
    return GR_OK;
}

GENRESULT UploadLockTexture(void * context, U32 handle, const std::uint8_t * pixels,
                            U32 width, U32 height, std::size_t byteCount)
{
    auto & backend = *static_cast<LockTextureBackend *>(context);
    if (handle != 52U || width != 2U || height != 2U || byteCount != 16U)
        return GR_INVALID_PARMS;
    ++backend.uploads;
    if (backend.failUpload) return GR_GENERIC;
    std::memcpy(backend.committed.data(), pixels, byteCount);
    return GR_OK;
}

GENRESULT DestroyLockTexture(void * context, U32 handle)
{
    if (handle != 52U) return GR_INVALID_PARMS;
    ++static_cast<LockTextureBackend *>(context)->destroys;
    return GR_OK;
}

bool CheckTextureLockLifecycle()
{
    LockTextureBackend backend;
    FrontierX::NativeTextureOperations operations{};
    operations.context = &backend;
    operations.create = CreateLockTexture;
    operations.upload = UploadLockTexture;
    operations.destroy = DestroyLockTexture;
    FrontierX::NativeRenderPipeline pipeline{nullptr, operations};
    const PixelFormat format(PF_RGBA8);
    U32 handle = 0U;
    const std::array<std::uint8_t, 16> original{{
        1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U,
        9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U}};
    if (!Expect(pipeline.create_texture(2, 2, format, 0, 0U, handle) == GR_OK &&
                    handle == 52U, "lockable native texture created") ||
        !Expect(pipeline.set_texture_level_data(handle, 0, 2, 2, 8, format,
                    original.data(), nullptr, nullptr) == GR_OK &&
                    backend.committed == original, "initial upload populates texture shadow"))
        return false;
    const std::array<std::uint8_t, 16> rejected{};
    backend.failUpload = true;
    if (!Expect(pipeline.set_texture_level_data(handle, 0, 2, 2, 8, format,
                    rejected.data(), nullptr, nullptr) == GR_GENERIC &&
                    backend.committed == original,
                "failed replacement preserves committed texels"))
        return false;
    backend.failUpload = false;
    RPLOCKDATA locked{};
    if (!Expect(pipeline.lock_texture(handle, 0, &locked) == GR_OK &&
                    locked.width == 2U && locked.height == 2U && locked.pitch == 8U &&
                    locked.pixels != nullptr && locked.pf.num_bits() == 32 &&
                    std::memcmp(locked.pixels, original.data(), original.size()) == 0,
                "RGBA8 lock returns bounded CPU texels") ||
        !Expect(pipeline.lock_texture(handle, 0, &locked) == GR_GENERIC &&
                    pipeline.destroy_texture(handle) == GR_GENERIC &&
                    pipeline.set_texture_level_data(handle, 0, 2, 2, 8, format,
                        original.data(), nullptr, nullptr) == GR_NOT_IMPLEMENTED,
                "locked texture rejects conflicting operations"))
        return false;
    static_cast<std::uint8_t *>(locked.pixels)[0] = 99U;
    backend.failUpload = true;
    if (!Expect(pipeline.unlock_texture(handle, 0) == GR_GENERIC &&
                    backend.committed == original &&
                    pipeline.destroy_texture(handle) == GR_GENERIC,
                "failed upload preserves GPU data and lock for retry"))
        return false;
    backend.failUpload = false;
    RPLOCKDATA reopened{};
    return Expect(pipeline.unlock_texture(handle, 0) == GR_OK &&
                      backend.committed[0] == 99U && backend.uploads == 4U,
                  "retry commits modified texel") &&
        Expect(pipeline.unlock_texture(handle, 0) == GR_INVALID_PARMS &&
                   pipeline.lock_texture(handle, 1, &reopened) == GR_INVALID_PARMS &&
                   pipeline.lock_texture(handle, 0, nullptr) == GR_INVALID_PARMS,
               "invalid unlock, level and output rejected") &&
        Expect(pipeline.lock_texture(handle, 0, &reopened) == GR_OK &&
                   static_cast<std::uint8_t *>(reopened.pixels)[0] == 99U &&
                   pipeline.unlock_texture(handle, 0) == GR_OK &&
                   pipeline.destroy_texture(handle) == GR_OK &&
                   backend.destroys == 1U,
               "relock sees committed bytes and texture is destroyed");
}

struct BlitTextureBackend
{
    std::array<std::array<std::uint8_t, 16>, 2> committed{};
    U32 nextHandle = 60U;
    bool failUpload = false;
};

GENRESULT CreateBlitTexture(void * context, U32 width, U32 height, U32 & handle)
{
    if (width != 2U || height != 2U) return GR_INVALID_PARMS;
    handle = static_cast<BlitTextureBackend *>(context)->nextHandle++;
    return GR_OK;
}

GENRESULT UploadBlitTexture(void * context, U32 handle, const std::uint8_t * pixels,
                            U32 width, U32 height, std::size_t byteCount)
{
    auto & backend = *static_cast<BlitTextureBackend *>(context);
    if (handle < 60U || handle > 61U || width != 2U || height != 2U ||
        byteCount != 16U) return GR_INVALID_PARMS;
    if (backend.failUpload) return GR_GENERIC;
    std::memcpy(backend.committed[handle - 60U].data(), pixels, byteCount);
    return GR_OK;
}

GENRESULT DestroyBlitTexture(void *, U32 handle)
{
    return handle == 60U || handle == 61U ? GR_OK : GR_INVALID_PARMS;
}

bool CheckTextureBlit()
{
    BlitTextureBackend backend;
    FrontierX::NativeTextureOperations operations{};
    operations.context = &backend;
    operations.create = CreateBlitTexture;
    operations.upload = UploadBlitTexture;
    operations.destroy = DestroyBlitTexture;
    FrontierX::NativeRenderPipeline pipeline{nullptr, operations};
    const PixelFormat format(PF_RGBA8);
    U32 source = 0U, destination = 0U;
    const std::array<std::uint8_t, 16> pixels{{
        1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U,
        9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U}};
    const RECT full{0, 0, 2, 2};
    const RECT partial{0, 0, 1, 2};
    if (!Expect(pipeline.create_texture(2, 2, format, 0, 0U, source) == GR_OK &&
                    pipeline.create_texture(2, 2, format, 0, 0U, destination) == GR_OK &&
                    pipeline.set_texture_level_data(source, 0, 2, 2, 8, format,
                        pixels.data(), nullptr, nullptr) == GR_OK,
                "blit source and destination created")) return false;
    backend.failUpload = true;
    if (!Expect(pipeline.blit_texture(destination, 0, full, source, 0, full) == GR_GENERIC &&
                    backend.committed[1] != pixels,
                "failed blit preserves destination backend")) return false;
    RPLOCKDATA failedDestination{};
    if (!Expect(pipeline.lock_texture(destination, 0, &failedDestination) == GR_OK &&
                    std::memcmp(failedDestination.pixels, pixels.data(), pixels.size()) != 0,
                "failed blit preserves destination shadow")) return false;
    backend.failUpload = false;
    if (!Expect(pipeline.unlock_texture(destination, 0) == GR_OK,
                "destination unlock after failed blit")) return false;
    if (!Expect(pipeline.blit_texture(destination, 0, partial, source, 0, full) ==
                    GR_NOT_IMPLEMENTED &&
                    pipeline.blit_texture(destination, 1, full, source, 0, full) ==
                    GR_INVALID_PARMS &&
                    pipeline.blit_texture(destination, 0, full, source, 0, full) == GR_OK &&
                    backend.committed[1] == pixels,
                "bounded RGBA8 full copy and explicit unsupported rectangles")) return false;
    RPLOCKDATA locked{};
    return Expect(pipeline.lock_texture(destination, 0, &locked) == GR_OK &&
                      std::memcmp(locked.pixels, pixels.data(), pixels.size()) == 0 &&
                      pipeline.blit_texture(destination, 0, full, source, 0, full) ==
                          GR_INVALID_PARMS &&
                      pipeline.unlock_texture(destination, 0) == GR_OK,
                  "blit updates shadow and rejects locked destination") &&
        Expect(pipeline.destroy_texture(destination) == GR_OK &&
                   pipeline.destroy_texture(source) == GR_OK,
               "blit resources destroyed");
}

} // namespace

int main()
{
    if (!CheckInterfaceRegressions() || !CheckPipelineRegressions() ||
        !CheckOwnedVertexBufferRegressions() ||
        !CheckExtendedSubmissionRegressions() || !CheckTextureHandleCollision() ||
        !CheckTextureLockLifecycle() || !CheckTextureBlit())
    {
        return 1;
    }
    CapturingRenderPrimitive capture;
    FrontierX::NativeRenderPipeline pipeline{
        LookupVertexBuffer, &vertexBufferOwner, &capture};
    if (!Expect(pipeline.draw_primitive_vb(D3DPT_TRIANGLELIST, kVertexBufferA, 0, 3, 0U) ==
                    GR_OK,
                "direct vertex-buffer draw accepted") ||
        !Expect(capture.drawCount == 1 &&
                    capture.type == D3DPT_TRIANGLELIST &&
                    capture.format == FrontierX::LegacyD3d9::kRpVertexFvf &&
                    capture.sourceCount == 3,
                "direct vertex-buffer legacy draw"))
    {
        return 1;
    }

    constexpr std::array<U16, 6> quadIndices{{0U, 1U, 2U, 0U, 2U, 1U}};
    if (!Expect(pipeline.draw_indexed_primitive_vb(
                    D3DPT_TRIANGLELIST, kVertexBufferB, 1, 3, quadIndices.data(),
                    static_cast<int>(quadIndices.size()), 0U) == GR_OK,
                "indexed vertex-buffer draw accepted") ||
        !Expect(capture.drawCount == 2 && capture.sourceCount == 3 &&
                    capture.indices.size() == quadIndices.size(),
                "indexed vertex-buffer legacy draw"))
    {
        return 1;
    }

    if (!Expect(std::memcmp(capture.sourceBytes.data(), vertexBufferB.data() + 1,
                            sizeof(RenderVertexP3DiffuseT2) * 3) == 0,
                "vertex-buffer slice bytes") ||
        !Expect(capture.indices[0] == 0U && capture.indices[5] == 1U,
                "indexed bounds"))
    {
        return 1;
    }

    const auto ExpectFailure = [&](GENRESULT expected,
                                   D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST,
                                   std::uint32_t vertexBuffer = kVertexBufferA,
                                   int start = 0,
                                   int count = 3,
                                   const U16 * indices = nullptr,
                                   int indexCount = 0) {
        capture.drawCount = 0;
        const GENRESULT result =
            indices ? pipeline.draw_indexed_primitive_vb(primitiveType,
                                                        vertexBuffer,
                                                        start,
                                                        count,
                                                        indices,
                                                        indexCount,
                                                        0U)
                    : pipeline.draw_primitive_vb(primitiveType, vertexBuffer, start,
                                                count, 0U);
        return Expect(result == expected, "vertex-buffer failure status") &&
               Expect(capture.drawCount == 0, "failed draw reached owner");
    };

    if (!ExpectFailure(GR_NOT_IMPLEMENTED, D3DPT_TRIANGLEFAN) ||
        !ExpectFailure(GR_INVALID_PARMS, D3DPT_TRIANGLELIST, 0U) ||
        !ExpectFailure(GR_INVALID_PARMS, D3DPT_TRIANGLELIST, kVertexBufferA, 2, 2) ||
        !ExpectFailure(GR_INVALID_PARMS, D3DPT_TRIANGLELIST, kVertexBufferA, 0, 4))
    {
        return 1;
    }

    if (!Expect(pipeline.draw_primitive_vb(
                    D3DPT_TRIANGLELIST, kVertexBuffer2, 0, 3, 0U) == GR_OK,
                "RPVERTEX2 direct vertex-buffer draw accepted") ||
        !Expect(capture.drawCount == 1 &&
                    capture.format == FrontierX::LegacyD3d9::kRpVertex2Fvf &&
                    capture.sourceCount == 3,
                "RPVERTEX2 legacy draw"))
    {
        return 1;
    }

    if (!Expect(std::memcmp(capture.sourceBytes.data(), vertexBuffer2.data(),
                            sizeof(vertexBuffer2)) == 0,
                "RPVERTEX2 slice bytes"))
    {
        return 1;
    }

    constexpr std::array<U16, 3> triangle2Indices{{0U, 1U, 2U}};
    if (!Expect(pipeline.draw_indexed_primitive_vb(
                    D3DPT_TRIANGLELIST,
                    kVertexBuffer2,
                    0,
                    3,
                    triangle2Indices.data(),
                    static_cast<int>(triangle2Indices.size()),
                    0U) == GR_OK,
                "RPVERTEX2 indexed vertex-buffer draw accepted") ||
        !Expect(capture.drawCount == 2 && capture.sourceCount == 3 &&
                    capture.format == FrontierX::LegacyD3d9::kRpVertex2Fvf &&
                    capture.indices.size() == triangle2Indices.size(),
                "RPVERTEX2 indexed legacy draw"))
    {
        return 1;
    }

    if (!Expect(pipeline.draw_indexed_primitive_vb(
                    D3DPT_LINESTRIP,
                    kVertexBuffer2,
                    0,
                    3,
                    triangle2Indices.data(),
                    static_cast<int>(triangle2Indices.size()),
                    0U) == GR_OK,
                "RPVERTEX2 indexed line-strip accepted") ||
        !Expect(capture.drawCount == 3 && capture.sourceCount == 3 &&
                    capture.indices.size() == triangle2Indices.size(),
                "RPVERTEX2 indexed line-strip legacy draw"))
    {
        return 1;
    }

    constexpr std::array<U16, 3> badTriangle2Indices{{0U, 1U, 3U}};
    if (!Expect(pipeline.draw_indexed_primitive_vb(
                    D3DPT_TRIANGLELIST,
                    kVertexBuffer2,
                    0,
                    3,
                    badTriangle2Indices.data(),
                    static_cast<int>(badTriangle2Indices.size()),
                    0U) == GR_INVALID_PARMS,
                "RPVERTEX2 indexed out-of-slice rejection"))
    {
        return 1;
    }

    std::cout << "render pipeline probe passed\n";
    return 0;
}
