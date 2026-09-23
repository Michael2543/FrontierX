#include "FrontierX/NativeRenderPipeline.h"

#include "FrontierX/LegacyRenderSubmission.h"

#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>

namespace FrontierX
{
namespace
{
// Stable IRP_VBF_READ, IRP_VBF_NO_CLIP and IRP_VBF_SYSTEM bits.
constexpr U32 kSupportedVertexBufferFlags = 0x7U;

GENRESULT StatusResult(LegacySubmissionStatus status)
{
    switch (status)
    {
    case LegacySubmissionStatus::Ok:
        return GR_OK;
    case LegacySubmissionStatus::UnsupportedPrimitiveType:
    case LegacySubmissionStatus::UnsupportedVertexFormat:
    case LegacySubmissionStatus::UnsupportedIndexedPointList:
        return GR_NOT_IMPLEMENTED;
    case LegacySubmissionStatus::InvalidInput:
        return GR_INVALID_PARMS;
    case LegacySubmissionStatus::InsufficientDestination:
        return GR_OUT_OF_MEMORY;
    }
    return GR_GENERIC;
}
} // namespace

NativeRenderPipeline::NativeRenderPipeline(IRenderPrimitive * drawTarget) noexcept
    : drawTarget_(drawTarget)
{
}

NativeRenderPipeline::NativeRenderPipeline(NativeVertexBufferLookup lookup,
                                           void * lookupContext,
                                           IRenderPrimitive * drawTarget) noexcept
    : lookup_(lookup), lookupContext_(lookupContext), drawTarget_(drawTarget)
{
}

GENRESULT NativeRenderPipeline::QueryInterface(const C8 * interfaceName, void ** instance)
{
    if (!instance)
    {
        return GR_INVALID_PARMS;
    }
    *instance = nullptr;
    if (!interfaceName)
    {
        return GR_INTERFACE_UNSUPPORTED;
    }
    if (std::strcmp(interfaceName, IID_IRenderPipeline) == 0)
    {
        *instance = static_cast<IRenderPipeline *>(this);
    }
    else if (std::strcmp(interfaceName, IID_IDAComponent) == 0)
    {
        *instance = static_cast<IDAComponent *>(this);
    }
    else
    {
        return GR_INTERFACE_UNSUPPORTED;
    }
    AddRef();
    return GR_OK;
}

U32 NativeRenderPipeline::AddRef()
{
    return referenceCount_.fetch_add(1U, std::memory_order_relaxed) + 1U;
}

U32 NativeRenderPipeline::Release()
{
    const U32 remaining = referenceCount_.fetch_sub(1U, std::memory_order_acq_rel) - 1U;
    if (remaining == 0U)
    {
        delete this;
    }
    return remaining;
}

GENRESULT NativeRenderPipeline::query_device_ability(RPDEVICEABILITY ability,
                                                      U32 * outAnswer)
{
    if (!outAnswer)
    {
        return GR_INVALID_PARMS;
    }
    *outAnswer = 0U;
    return ability == RP_A_DEPTH_BIAS ? GR_OK : GR_NOT_IMPLEMENTED;
}

GENRESULT NativeRenderPipeline::set_modelview(const Transform & modelview)
{
    return drawTarget_ ? drawTarget_->set_modelview(modelview) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_modelview(Transform & modelview)
{
    return drawTarget_ ? drawTarget_->get_modelview(modelview) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_projection(const Matrix4 & projection)
{
    return drawTarget_ ? drawTarget_->set_projection(projection) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_projection(Matrix4 & projection)
{
    return drawTarget_ ? drawTarget_->get_projection(projection) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_viewport(int x, int y, int width, int height)
{
    return drawTarget_ ? drawTarget_->set_viewport(x, y, width, height) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_viewport(int * x, int * y, int * width, int * height)
{
    return drawTarget_ ? drawTarget_->get_viewport(x, y, width, height) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_ortho(float left, float right, float bottom, float top,
                                          float nearValue, float farValue)
{
    return drawTarget_ ? drawTarget_->set_ortho(
                             left, right, bottom, top, nearValue, farValue)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_perspective(float fieldOfView, float aspect,
                                                float nearValue, float farValue)
{
    return drawTarget_ ? drawTarget_->set_perspective(
                             fieldOfView, aspect, nearValue, farValue)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_material(D3DMATERIAL9 * material)
{
    return drawTarget_ ? drawTarget_->set_material(material) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_material(D3DMATERIAL9 * material)
{
    return drawTarget_ ? drawTarget_->get_material(material) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::begin_scene()
{
    return drawTarget_ ? drawTarget_->begin_scene() : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::end_scene()
{
    return drawTarget_ ? drawTarget_->end_scene() : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_render_state(D3DRENDERSTATETYPE state, U32 value)
{
    return drawTarget_ ? drawTarget_->set_render_state(state, value) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_render_state(D3DRENDERSTATETYPE state, U32 * value)
{
    return drawTarget_ ? drawTarget_->get_render_state(state, value) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_sampler_state(U32 stage, D3DSAMPLERSTATETYPE state,
                                                  U32 value)
{
    return drawTarget_ ? drawTarget_->set_sampler_state(stage, state, value) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_sampler_state(U32 stage, D3DSAMPLERSTATETYPE state,
                                                  U32 * value)
{
    return drawTarget_ ? drawTarget_->get_sampler_state(stage, state, value) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_texture_stage_state(
    U32 stage, D3DTEXTURESTAGESTATETYPE state, U32 value)
{
    return drawTarget_ ? drawTarget_->set_texture_stage_state(stage, state, value)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_texture_stage_state(
    U32 stage, D3DTEXTURESTAGESTATETYPE state, U32 * value)
{
    return drawTarget_ ? drawTarget_->get_texture_stage_state(stage, state, value)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_texture_stage_texture(U32 stage, U32 texture)
{
    return drawTarget_ ? drawTarget_->set_texture_stage_texture(stage, texture) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_texture_stage_texture(U32 stage, U32 * texture)
{
    return drawTarget_ ? drawTarget_->get_texture_stage_texture(stage, texture) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::verify_state()
{
    return drawTarget_ ? drawTarget_->verify_state() : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::draw_primitive(D3DPRIMITIVETYPE primitiveType,
                                               U32 vertexFormat,
                                               const void * vertices,
                                               int vertexCount,
                                               U32 flags)
{
    return drawTarget_ ? drawTarget_->draw_primitive(
                             primitiveType, vertexFormat, vertices, vertexCount, flags)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::draw_indexed_primitive(
    D3DPRIMITIVETYPE primitiveType, U32 vertexFormat, const void * vertices,
    int vertexCount, const U16 * indices, int indexCount, U32 flags)
{
    return drawTarget_ ? drawTarget_->draw_indexed_primitive(
                             primitiveType, vertexFormat, vertices, vertexCount,
                             indices, indexCount, flags)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::create_vertex_buffer(
    U32 vertexFormat,
    int vertexCount,
    U32 flags,
    IRP_VERTEXBUFFERHANDLE * outHandle)
{
    if (!outHandle)
    {
        return GR_INVALID_PARMS;
    }
    *outHandle = 0U;
    if (lookup_)
    {
        return GR_NOT_IMPLEMENTED;
    }
    if (vertexCount <= 0)
    {
        return GR_INVALID_PARMS;
    }
    if ((flags & ~kSupportedVertexBufferFlags) != 0U)
    {
        return GR_NOT_IMPLEMENTED;
    }
    const bool extended = vertexFormat == LegacyD3d9::kRpVertex2Fvf;
    if (vertexFormat != LegacyD3d9::kRpVertexFvf && !extended)
    {
        return GR_NOT_IMPLEMENTED;
    }
    const std::size_t stride = extended ? sizeof(RenderVertexP3DiffuseT4)
                                        : sizeof(RenderVertexP3DiffuseT2);
    const auto count = static_cast<std::size_t>(vertexCount);
    if (count > std::numeric_limits<std::size_t>::max() / stride || nextHandle_ == 0U)
    {
        return GR_OUT_OF_MEMORY;
    }

    OwnedVertexBuffer buffer;
    buffer.vertexFormat = vertexFormat;
    buffer.vertexCount = static_cast<U32>(vertexCount);
    try
    {
        buffer.bytes.resize(count * stride);
        const auto inserted = ownedBuffers_.emplace(nextHandle_, std::move(buffer));
        if (!inserted.second)
        {
            return GR_GENERIC;
        }
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_MEMORY;
    }
    catch (const std::length_error &)
    {
        return GR_OUT_OF_MEMORY;
    }
    *outHandle = nextHandle_++;
    return GR_OK;
}

GENRESULT NativeRenderPipeline::destroy_vertex_buffer(IRP_VERTEXBUFFERHANDLE handle)
{
    const auto found = ownedBuffers_.find(handle);
    if (found == ownedBuffers_.end())
    {
        return GR_INVALID_PARMS;
    }
    if (found->second.locked)
    {
        return GR_GENERIC;
    }
    ownedBuffers_.erase(found);
    return GR_OK;
}

GENRESULT NativeRenderPipeline::lock_vertex_buffer(
    IRP_VERTEXBUFFERHANDLE handle,
    U32,
    void ** outVertices,
    U32 * outVertexCount)
{
    if (!outVertices || !outVertexCount)
    {
        return GR_INVALID_PARMS;
    }
    *outVertices = nullptr;
    *outVertexCount = 0U;
    const auto found = ownedBuffers_.find(handle);
    if (found == ownedBuffers_.end() || found->second.locked)
    {
        return GR_INVALID_PARMS;
    }
    found->second.locked = true;
    *outVertices = found->second.bytes.data();
    *outVertexCount = found->second.vertexCount;
    return GR_OK;
}

GENRESULT NativeRenderPipeline::unlock_vertex_buffer(IRP_VERTEXBUFFERHANDLE handle)
{
    const auto found = ownedBuffers_.find(handle);
    if (found == ownedBuffers_.end() || !found->second.locked)
    {
        return GR_INVALID_PARMS;
    }
    found->second.locked = false;
    return GR_OK;
}

NativeVertexBufferView NativeRenderPipeline::OwnedVertexBufferLookup(
    void * context, IRP_VERTEXBUFFERHANDLE handle) noexcept
{
    return context
        ? static_cast<const NativeRenderPipeline *>(context)->LookupOwnedVertexBuffer(handle)
        : NativeVertexBufferView{};
}

NativeVertexBufferView NativeRenderPipeline::LookupOwnedVertexBuffer(
    IRP_VERTEXBUFFERHANDLE handle) const noexcept
{
    const auto found = ownedBuffers_.find(handle);
    if (found == ownedBuffers_.end() || found->second.locked)
    {
        return {};
    }
    return {found->second.bytes.data(), found->second.bytes.size(),
            found->second.vertexFormat};
}

GENRESULT NativeRenderPipeline::draw_primitive_vb(D3DPRIMITIVETYPE primitiveType,
                                                  IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
                                                  int startVertex,
                                                  int vertexCount,
                                                  U32 flags)
{
    return Submit(primitiveType, vertexBufferHandle, startVertex, vertexCount,
                  nullptr, 0, flags);
}

GENRESULT NativeRenderPipeline::draw_indexed_primitive_vb(
    D3DPRIMITIVETYPE primitiveType,
    IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
    int startVertex,
    int vertexCount,
    const U16 * indices,
    int indexCount,
    U32 flags)
{
    if (!indices || indexCount <= 0)
    {
        return GR_INVALID_PARMS;
    }
    return Submit(primitiveType, vertexBufferHandle, startVertex, vertexCount,
                  indices, indexCount, flags);
}

GENRESULT NativeRenderPipeline::Submit(D3DPRIMITIVETYPE primitiveType,
                                       IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
                                       int startVertex,
                                       int vertexCount,
                                       const U16 * indices,
                                       int indexCount,
                                       U32 flags)
{
    if (startVertex < 0 || vertexCount <= 0 || vertexBufferHandle == 0U)
    {
        return GR_INVALID_PARMS;
    }
    if (!drawTarget_)
    {
        return GR_GENERIC;
    }
    const NativeVertexBufferView buffer = lookup_
        ? lookup_(lookupContext_, vertexBufferHandle)
        : LookupOwnedVertexBuffer(vertexBufferHandle);
    if (!buffer.vertices)
    {
        return GR_GENERIC;
    }
    if (buffer.vertexFormat != LegacyD3d9::kRpVertexFvf &&
        buffer.vertexFormat != LegacyD3d9::kRpVertex2Fvf)
    {
        return GR_NOT_IMPLEMENTED;
    }
    const std::uint32_t vertexFormat = buffer.vertexFormat;
    const bool extended = vertexFormat == LegacyD3d9::kRpVertex2Fvf;
    const std::size_t stride = extended ? sizeof(RenderVertexP3DiffuseT4)
                                        : sizeof(RenderVertexP3DiffuseT2);
    const std::size_t capacity = buffer.byteSize / stride;
    const std::size_t base = static_cast<std::size_t>(startVertex);
    const std::size_t count = static_cast<std::size_t>(vertexCount);
    if (base > capacity || count > capacity - base)
    {
        return GR_INVALID_PARMS;
    }

    // Windows uses start_vert as BaseVertexIndex. Indices remain slice-relative.
    const LegacySubmissionInput input{
        static_cast<std::uint32_t>(primitiveType),
        vertexFormat,
        static_cast<const unsigned char *>(buffer.vertices) + base * stride,
        count,
        indices,
        static_cast<std::size_t>(indexCount),
    };
    LegacySubmissionStatus status;
    const void * vertices = nullptr;
    try
    {
        if (extended)
        {
            converted2_.resize(count);
            RenderSubmissionView2 submission{};
            status = AdaptLegacyRpVertex2Submission(
                input, converted2_.data(), converted2_.size(), submission);
            vertices = submission.vertices;
        }
        else
        {
            converted_.resize(count);
            RenderSubmissionView submission{};
            status = AdaptLegacyRpVertexSubmission(
                input, converted_.data(), converted_.size(), submission);
            vertices = submission.vertices;
        }
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_MEMORY;
    }
    if (status != LegacySubmissionStatus::Ok)
    {
        return StatusResult(status);
    }
    return indices ? drawTarget_->draw_indexed_primitive(
                         primitiveType, vertexFormat, vertices, vertexCount,
                         indices, indexCount, flags)
                   : drawTarget_->draw_primitive(
                         primitiveType, vertexFormat, vertices, vertexCount, flags);
}
} // namespace FrontierX
