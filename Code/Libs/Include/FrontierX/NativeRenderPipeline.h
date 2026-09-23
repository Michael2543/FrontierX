#ifndef FRONTIERX_NATIVE_RENDER_PIPELINE_H
#define FRONTIERX_NATIVE_RENDER_PIPELINE_H

#include "IRenderPrimitive.h"
#include "FrontierX/RenderPipelineAdapter.h"
#include "FrontierX/RenderSubmission.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

namespace FrontierX
{

// Borrowed CPU storage, valid through the synchronous draw. No ownership transfer.
struct NativeVertexBufferView
{
    const void * vertices = nullptr;
    std::size_t byteSize = 0U;
    std::uint32_t vertexFormat = 0U;
};

using NativeVertexBufferLookup = NativeVertexBufferView (*)(void * context,
                                                            IRP_VERTEXBUFFERHANDLE handle);

// Native implementation of the established IRenderPipeline boundary. The
// implemented slots forward to the checked IRenderPrimitive owner or manage
// CPU vertex buffers; every other slot fails in RenderPipelineAdapter.
class NativeRenderPipeline final : public RenderPipelineAdapter
{
public:
    explicit NativeRenderPipeline(IRenderPrimitive * drawTarget) noexcept;
    NativeRenderPipeline(NativeVertexBufferLookup lookup,
                         void * lookupContext,
                         IRenderPrimitive * drawTarget) noexcept;

    GENRESULT COMAPI QueryInterface(const C8 * interfaceName, void ** instance) override;
    U32 COMAPI AddRef() override;
    U32 COMAPI Release() override;

    GENRESULT COMAPI query_device_ability(RPDEVICEABILITY ability, U32 * outAnswer) override;
    GENRESULT COMAPI create_vertex_buffer(U32 vertexFormat, int vertexCount, U32 flags,
                                          IRP_VERTEXBUFFERHANDLE * outHandle) override;
    GENRESULT COMAPI destroy_vertex_buffer(IRP_VERTEXBUFFERHANDLE handle) override;
    GENRESULT COMAPI lock_vertex_buffer(IRP_VERTEXBUFFERHANDLE handle, U32 lockFlags,
                                        void ** outVertices, U32 * outVertexCount) override;
    GENRESULT COMAPI unlock_vertex_buffer(IRP_VERTEXBUFFERHANDLE handle) override;

    GENRESULT COMAPI set_modelview(const Transform & modelview) override;
    GENRESULT COMAPI get_modelview(Transform & modelview) override;
    GENRESULT COMAPI set_projection(const Matrix4 & projection) override;
    GENRESULT COMAPI get_projection(Matrix4 & projection) override;
    GENRESULT COMAPI set_viewport(int x, int y, int width, int height) override;
    GENRESULT COMAPI get_viewport(int * x, int * y, int * width, int * height) override;
    GENRESULT COMAPI set_ortho(float left, float right, float bottom, float top,
                               float nearValue, float farValue) override;
    GENRESULT COMAPI set_perspective(float fieldOfView, float aspect,
                                     float nearValue, float farValue) override;
    GENRESULT COMAPI set_material(D3DMATERIAL9 * material) override;
    GENRESULT COMAPI get_material(D3DMATERIAL9 * material) override;
    GENRESULT COMAPI begin_scene() override;
    GENRESULT COMAPI end_scene() override;
    GENRESULT COMAPI set_render_state(D3DRENDERSTATETYPE state, U32 value) override;
    GENRESULT COMAPI get_render_state(D3DRENDERSTATETYPE state, U32 * value) override;
    GENRESULT COMAPI set_sampler_state(U32 stage, D3DSAMPLERSTATETYPE state,
                                       U32 value) override;
    GENRESULT COMAPI get_sampler_state(U32 stage, D3DSAMPLERSTATETYPE state,
                                       U32 * value) override;
    GENRESULT COMAPI set_texture_stage_state(U32 stage, D3DTEXTURESTAGESTATETYPE state,
                                             U32 value) override;
    GENRESULT COMAPI get_texture_stage_state(U32 stage, D3DTEXTURESTAGESTATETYPE state,
                                             U32 * value) override;
    GENRESULT COMAPI set_texture_stage_texture(U32 stage, U32 texture) override;
    GENRESULT COMAPI get_texture_stage_texture(U32 stage, U32 * texture) override;
    GENRESULT COMAPI verify_state() override;
    GENRESULT COMAPI draw_primitive(D3DPRIMITIVETYPE primitiveType, U32 vertexFormat,
                                    const void * vertices, int vertexCount,
                                    U32 flags) override;
    GENRESULT COMAPI draw_indexed_primitive(D3DPRIMITIVETYPE primitiveType,
                                            U32 vertexFormat, const void * vertices,
                                            int vertexCount, const U16 * indices,
                                            int indexCount, U32 flags) override;

    static NativeVertexBufferView OwnedVertexBufferLookup(
        void * context, IRP_VERTEXBUFFERHANDLE handle) noexcept;

    GENRESULT COMAPI draw_primitive_vb(D3DPRIMITIVETYPE primitiveType,
                                       IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
                                       int startVertex, int vertexCount,
                                       U32 flags) override;

    GENRESULT COMAPI draw_indexed_primitive_vb(
        D3DPRIMITIVETYPE primitiveType, IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
        int startVertex, int vertexCount, const U16 * indices,
        int indexCount, U32 flags) override;

private:
    struct OwnedVertexBuffer
    {
        std::vector<unsigned char> bytes;
        std::uint32_t vertexFormat = 0U;
        U32 vertexCount = 0U;
        bool locked = false;
    };

    NativeVertexBufferView LookupOwnedVertexBuffer(IRP_VERTEXBUFFERHANDLE handle) const noexcept;
    GENRESULT Submit(D3DPRIMITIVETYPE primitiveType,
                     IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
                     int startVertex,
                     int vertexCount,
                     const U16 * indices,
                     int indexCount,
                     U32 flags);

    NativeVertexBufferLookup lookup_ = nullptr;
    void * lookupContext_ = nullptr;
    IRenderPrimitive * drawTarget_ = nullptr;
    std::atomic<U32> referenceCount_{1U};
    std::map<IRP_VERTEXBUFFERHANDLE, OwnedVertexBuffer> ownedBuffers_;
    IRP_VERTEXBUFFERHANDLE nextHandle_ = 1U;
    std::vector<RenderVertexP3DiffuseT2> converted_;
    std::vector<RenderVertexP3DiffuseT4> converted2_;
};
} // namespace FrontierX

#endif
