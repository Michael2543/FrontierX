#ifndef FRONTIERX_SDL_GPU_RENDERER_H
#define FRONTIERX_SDL_GPU_RENDERER_H

#include "DACOM.H"
#include "FrontierX/RenderState.h"
#include "FrontierX/RenderSubmission.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

class Transform;
class Matrix4;
struct SDL_GPUCommandBuffer;
struct SDL_GPUColorTargetInfo;
struct SDL_GPUDepthStencilTargetInfo;

namespace FrontierX
{
struct NativeTextureLevel;
struct SdlGpuTextureView
{
    const std::uint8_t * rgba8 = nullptr;
    std::uint32_t width = 0U;
    std::uint32_t height = 0U;
    std::size_t byteCount = 0U;
};

struct SdlGpuReadback
{
    static constexpr std::size_t Width = 64U;
    static constexpr std::size_t Height = 64U;
    std::array<std::uint8_t, Width * Height * 3U> rgb8{};
    std::uint64_t checksum = 0U;
    std::size_t changedPixels = 0U;
    bool structureValid = false;
    bool rgbValid = false;
    bool opaquePixels = false;
};

class SdlGpuRenderer
{
public:
    SdlGpuRenderer();
    ~SdlGpuRenderer();
    SdlGpuRenderer(const SdlGpuRenderer &) = delete;
    SdlGpuRenderer & operator=(const SdlGpuRenderer &) = delete;

    bool Initialize(std::uint32_t windowId,
                    const char * vertexShaderPath,
                    const char * fragmentShaderPath,
                    SdlGpuTextureView texture,
                    std::string & error,
                    const char * extendedVertexShaderPath = nullptr,
                    const char * gameVertexShaderPath = nullptr);
    GENRESULT Submit(const RenderSubmissionView & submission);
    GENRESULT Submit(const RenderSubmissionView2 & submission);
    GENRESULT Submit(const RenderSubmissionView3 & submission);
    GENRESULT Submit(const RenderSubmissionView4 & submission);
    GENRESULT SetRenderState(const RenderStateSnapshot & state);
    GENRESULT SetModelview(const Transform & modelview);
    GENRESULT SetProjection(const Matrix4 & projection);
    GENRESULT SetViewport(int x, int y, int width, int height);
    GENRESULT SetVertexConstants(std::uint32_t firstRegister,
                                 const float * values,
                                 std::uint32_t registerCount);
    GENRESULT SetTextureStageState(const TextureStageSnapshot & state);
    GENRESULT SetSamplerState(const SamplerStateSnapshot & state);
    GENRESULT RegisterTexture(SdlGpuTextureView texture, std::uint32_t & handle);
    GENRESULT RegisterMipTexture(const NativeTextureLevel *levels, std::uint32_t count,
                                  std::uint32_t &handle);
    GENRESULT ReplaceMipTexture(std::uint32_t handle, const NativeTextureLevel *levels,
                                 std::uint32_t count);
    GENRESULT CreateMipTexture(std::uint32_t width, std::uint32_t height,
                                std::uint32_t count, std::uint32_t &handle);
    GENRESULT CreateTexture(std::uint32_t width, std::uint32_t height,
                            std::uint32_t & handle);
    GENRESULT ReplaceTexture(std::uint32_t handle, SdlGpuTextureView texture);
    GENRESULT DestroyTexture(std::uint32_t handle);
    GENRESULT BindTexture(std::uint32_t stage, std::uint32_t handle);
    bool IsTexture(std::uint32_t handle) const;
    static GENRESULT ClearFrameCallback(void * context, U32 color, U32 depth);
    static GENRESULT PresentFrameCallback(void * context);
    bool BeginFrame(std::string & error);
    std::size_t FrameSubmissionCount() const;
    bool Present(bool & presented, std::string & error);
    bool Readback64(SdlGpuReadback & result, std::string & error);
    void Shutdown();

    static GENRESULT DACOM_CDECL SubmissionCallback(
        void * context, const RenderSubmissionView & submission);
    static GENRESULT DACOM_CDECL Submission2Callback(
        void * context, const RenderSubmissionView2 & submission);
    static GENRESULT DACOM_CDECL Submission3Callback(
        void * context, const RenderSubmissionView3 & submission);
    static GENRESULT DACOM_CDECL Submission4Callback(
        void * context, const RenderSubmissionView4 & submission);
    static GENRESULT DACOM_CDECL RenderStateCallback(
        void * context, const RenderStateSnapshot & state);
    static GENRESULT DACOM_CDECL ModelviewCallback(
        void * context, const Transform & modelview);
    static GENRESULT DACOM_CDECL ProjectionCallback(
        void * context, const Matrix4 & projection);
    static GENRESULT DACOM_CDECL ViewportCallback(
        void * context, int x, int y, int width, int height);
    static GENRESULT DACOM_CDECL VertexConstantsCallback(
        void * context, UINT firstRegister, const FLOAT * values,
        UINT registerCount);
    static GENRESULT DACOM_CDECL TextureStageCallback(
        void * context, const TextureStageSnapshot & state);
    static GENRESULT DACOM_CDECL SamplerStateCallback(
        void * context, const SamplerStateSnapshot & state);
    static GENRESULT DACOM_CDECL TextureBindCallback(
        void * context, std::uint32_t stage, std::uint32_t handle);
    static bool TextureLookupCallback(void * context, std::uint32_t handle);
    static GENRESULT TextureCreateCallback(void * context, std::uint32_t width,
                                            std::uint32_t height, std::uint32_t & handle);
    static GENRESULT TextureUploadCallback(void * context, std::uint32_t handle,
                                            const std::uint8_t * pixels,
                                            std::uint32_t width, std::uint32_t height,
                                            std::size_t byteCount);
    static GENRESULT TextureCreateMipsCallback(void *, std::uint32_t, std::uint32_t,
                                                std::uint32_t, std::uint32_t &);
    static GENRESULT TextureUploadMipsCallback(void *, std::uint32_t,
                                                const NativeTextureLevel *, std::uint32_t);
    static GENRESULT TextureDestroyCallback(void * context, std::uint32_t handle);

    // Optional CPU wall time, including driver waits; not GPU timestamp time.
    struct Timings
    {
        double submissions = 0, samplers = 0, retirement = 0, presentation = 0;
        std::uint64_t submissionCount = 0, samplerChanges = 0;
        double acquisition = 0, drawing = 0, queueSubmit = 0;
        std::uint64_t reusedUploads = 0;
    };
    void EnableTimings(bool enabled);
    Timings GetTimings() const;
    const std::string & LastError() const;
    const char * DriverName() const;
    const char * PresentModeName() const;
    std::uint32_t DepthWidth() const;
    std::uint32_t DepthHeight() const;
    std::uint32_t DepthRecreations() const;
    std::uint32_t PipelineRebuilds() const;
    std::uint32_t VertexCount() const;
    std::uint32_t IndexCount() const;
    RenderStateSnapshot CurrentRenderState() const;
    TextureStageSnapshot CurrentTextureStageState() const;
    SamplerStateSnapshot CurrentSamplerState() const;
    std::uint32_t CurrentTextureHandle() const;
    std::uint32_t FixtureTextureHandle() const;

private:
    bool DrawCapturedFrame(SDL_GPUCommandBuffer * commandBuffer,
                           SDL_GPUColorTargetInfo * colorTarget,
                           SDL_GPUDepthStencilTargetInfo * depthTarget, std::uint32_t width,
                           std::uint32_t height, bool readback,
                           std::string & error);
    GENRESULT SubmitBytes(const void * vertices, std::size_t vertexCount,
                          std::size_t vertexStride, const std::uint16_t * indices,
                          std::size_t indexCount, unsigned vertexFormat,
                          PrimitiveTopology topology = PrimitiveTopology::TriangleList);
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}

#endif
