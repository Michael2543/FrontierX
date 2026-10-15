#ifndef FRONTIERX_NATIVE_RENDER_PIPELINE_H
#define FRONTIERX_NATIVE_RENDER_PIPELINE_H

#include "IRenderPrimitive.h"
#include "FrontierX/NativeMaterialEffect.h"
#include "FrontierX/RenderPipelineAdapter.h"
#include "FrontierX/RenderSubmission.h"
#include "FrontierX/NativeTextureDC.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
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
using NativeTextureHandleLookup = bool (*)(void * context, U32 handle);
using NativeTextureCreate = GENRESULT (*)(void * context, U32 width, U32 height,
                                         U32 & handle);
using NativeTextureUpload = GENRESULT (*)(void * context, U32 handle,
                                         const std::uint8_t * pixels, U32 width,
                                         U32 height, std::size_t byteCount);
struct NativeTextureLevel
{
    const std::uint8_t * pixels;
    U32 width, height;
    std::size_t byteCount;
};
using NativeTextureCreateMips = GENRESULT (*)(void *, U32, U32, U32, U32 &);
using NativeTextureUploadMips = GENRESULT (*)(void *, U32,
                                             const NativeTextureLevel *, U32);
using NativeTextureDestroy = GENRESULT (*)(void * context, U32 handle);
using NativeVertexConstants = GENRESULT (*)(void * context, UINT firstRegister,
                                           const FLOAT * values, UINT registerCount);

// Native-only frame callbacks; no persistent or legacy interface layout changes.
struct NativeFrameOperations
{
    void * context = nullptr;
    GENRESULT (*clear)(void *, U32 color, U32 depth) = nullptr;
    GENRESULT (*present)(void *) = nullptr;
};

struct NativeTextureOperations
{
    void * context = nullptr;
    NativeTextureHandleLookup lookup = nullptr;
    // Created handles must not exceed NativeBatchStateIds.h's limit; the pipeline
    // destroys and rejects higher handles reserved for App material states.
    NativeTextureCreate create = nullptr;
    NativeTextureUpload upload = nullptr;
    NativeTextureDestroy destroy = nullptr;
    NativeTextureCreateMips createMips = nullptr;
    NativeTextureUploadMips uploadMips = nullptr;
};

// Native implementation of the established IRenderPipeline boundary. The
// implemented slots forward to the checked IRenderPrimitive owner or manage
// CPU vertex buffers; every other slot fails in RenderPipelineAdapter.
class NativeRenderPipeline : public RenderPipelineAdapter
{
public:
    NativeRenderPipeline() noexcept = default;
    explicit NativeRenderPipeline(IRenderPrimitive * drawTarget,
                                  NativeTextureOperations textures = {},
                                  NativeVertexConstants vertexConstants = nullptr,
                                  void * vertexConstantsContext = nullptr) noexcept;
    NativeRenderPipeline(NativeVertexBufferLookup lookup,
                         void * lookupContext,
                         IRenderPrimitive * drawTarget,
                         NativeTextureOperations textures = {},
                         NativeVertexConstants vertexConstants = nullptr,
                         void * vertexConstantsContext = nullptr) noexcept;

    GENRESULT BindScene(IRenderPrimitive * drawTarget,
                        NativeTextureOperations textures,
                        NativeVertexConstants vertexConstants,
                        void * vertexConstantsContext) noexcept;
    GENRESULT UnbindScene() noexcept;
    GENRESULT BindFrames(NativeFrameOperations frames) noexcept;
    GENRESULT COMAPI clear_buffers(U32 flags, RECT * rectangle) override;
    GENRESULT COMAPI swap_buffers() override;

    GENRESULT COMAPI QueryInterface(const C8 * interfaceName, void ** instance) override;
    U32 COMAPI AddRef() override;
    U32 COMAPI Release() override;

    GENRESULT COMAPI query_device_ability(RPDEVICEABILITY ability, U32 * outAnswer) override;
    ID3DXEffect ** load_effect(const char * filename,
                              IComponentFactory * directory) override;
    GENRESULT COMAPI set_vs_constants(UINT firstRegister, CONST FLOAT * values,
                                     UINT registerCount) override;
    GENRESULT COMAPI set_pipeline_state(RPPIPELINESTATE state, U32 value) override;
    GENRESULT COMAPI get_pipeline_state(RPPIPELINESTATE state, U32 * value) override;
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
    GENRESULT COMAPI is_texture(U32 texture) override;
    GENRESULT COMAPI create_texture(int width, int height, const PixelFormat & format,
                                    int lodCount, U32 flags, U32 & handle) override;
    GENRESULT COMAPI destroy_texture(U32 handle) override;
    GENRESULT COMAPI get_texture_dc(U32 handle, HDC *dc) override;
    GENRESULT COMAPI release_texture_dc(U32 handle, HDC dc) override;
    GENRESULT COMAPI lock_texture(U32 handle, int level, RPLOCKDATA * lockData) override;
    GENRESULT COMAPI unlock_texture(U32 handle, int level) override;
    GENRESULT COMAPI set_texture_level_data(
        U32 handle, int level, int width, int height, int stride,
        const PixelFormat & format, const void * pixels, const void * alpha,
        const RGB * palette) override;
    GENRESULT COMAPI blit_texture(U32 destination, U32 destinationLevel,
                                  RECT destinationRect, U32 source, U32 sourceLevel,
                                  RECT sourceRect) override;
    GENRESULT COMAPI get_texture_format(U32 handle, PixelFormat * format) override;
    GENRESULT COMAPI get_texture_dim(U32 handle, U32 * width, U32 * height,
                                    U32 * lodCount) override;
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
    GENRESULT COMAPI draw_indexed_primitive_vb(
        D3DPRIMITIVETYPE primitiveType, IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
        int startVertex, int vertexCount, IDirect3DIndexBuffer9 * indexBuffer,
        int indexCount, U32 flags) override;
    GENRESULT COMAPI create_index_buffer(
        UINT byteLength, IDirect3DIndexBuffer9 ** outIndexBuffer) override;

private:
    struct OwnedEffect
    {
        std::unique_ptr<ID3DXEffect> owner;
        ID3DXEffect * pointer = nullptr;
    };

    struct TextureMip
    {
        U32 width, height;
        std::vector<std::uint8_t> pixels;
    };

    struct OwnedTexture
    {
        U32 width = 0U;
        U32 height = 0U;
        std::vector<std::uint8_t> pixels;
        std::vector<TextureMip> mips;
        int lockedLevel = -1;
        bool locked = false;
        bool opaque = false;
        bool rgb5a1 = false;
        std::unique_ptr<TextureDC::Context> dc;
    };

    GENRESULT UploadTexture(U32 handle, const OwnedTexture &texture,
                            int overrideLevel = -1,
                            const std::vector<std::uint8_t> *overridePixels = nullptr);
    static void ApplyTexturePrecision(const OwnedTexture &texture,
                                      std::vector<std::uint8_t> &pixels);

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

    NativeFrameOperations frames_{};
    enum class FramePhase { Idle, Cleared, Drawing, Ended };
    FramePhase framePhase_ = FramePhase::Idle;
    U32 clearColor_ = 0U;
    U32 clearDepth_ = 0xffffffffU;
    NativeVertexBufferLookup lookup_ = nullptr;
    void * lookupContext_ = nullptr;
    IRenderPrimitive * drawTarget_ = nullptr;
    NativeTextureOperations textures_{};
    NativeVertexConstants vertexConstants_ = nullptr;
    void * vertexConstantsContext_ = nullptr;
    std::map<std::string, OwnedEffect> effects_;
    std::map<U32, OwnedTexture> ownedTextures_;
    std::atomic<U32> referenceCount_{1U};
    U32 textureEnabled_ = 1U;
    std::map<IRP_VERTEXBUFFERHANDLE, OwnedVertexBuffer> ownedBuffers_;
    IRP_VERTEXBUFFERHANDLE nextHandle_ = 1U;
    std::vector<RenderVertexP3DiffuseT2> converted_;
    std::vector<RenderVertexP3DiffuseT4> converted2_;
};

GENRESULT BindNativeRenderFrames(IRenderPipeline * pipeline, NativeFrameOperations frames);

void RegisterNativeRenderPipeline(ICOManager * manager);
GENRESULT BindNativeRenderPipeline(IRenderPipeline * pipeline,
                                   IRenderPrimitive * drawTarget,
                                   NativeTextureOperations textures,
                                   NativeVertexConstants vertexConstants,
                                   void * vertexConstantsContext);
GENRESULT UnbindNativeRenderPipeline(IRenderPipeline * pipeline);
} // namespace FrontierX

#endif
