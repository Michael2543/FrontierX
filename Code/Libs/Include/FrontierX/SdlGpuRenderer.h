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

namespace FrontierX
{
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
                    const char * extendedVertexShaderPath = nullptr);
    GENRESULT Submit(const RenderSubmissionView & submission);
    GENRESULT Submit(const RenderSubmissionView2 & submission);
    GENRESULT SetRenderState(const RenderStateSnapshot & state);
    GENRESULT SetTextureStageState(const TextureStageSnapshot & state);
    GENRESULT RegisterTexture(SdlGpuTextureView texture, std::uint32_t & handle);
    GENRESULT CreateTexture(std::uint32_t width, std::uint32_t height,
                            std::uint32_t & handle);
    GENRESULT ReplaceTexture(std::uint32_t handle, SdlGpuTextureView texture);
    GENRESULT DestroyTexture(std::uint32_t handle);
    GENRESULT BindTexture(std::uint32_t stage, std::uint32_t handle);
    bool IsTexture(std::uint32_t handle) const;
    bool Present(bool & presented, std::string & error);
    bool Readback64(SdlGpuReadback & result, std::string & error);
    void Shutdown();

    static GENRESULT DACOM_CDECL SubmissionCallback(
        void * context, const RenderSubmissionView & submission);
    static GENRESULT DACOM_CDECL Submission2Callback(
        void * context, const RenderSubmissionView2 & submission);
    static GENRESULT DACOM_CDECL RenderStateCallback(
        void * context, const RenderStateSnapshot & state);
    static GENRESULT DACOM_CDECL TextureStageCallback(
        void * context, const TextureStageSnapshot & state);
    static GENRESULT DACOM_CDECL TextureBindCallback(
        void * context, std::uint32_t stage, std::uint32_t handle);
    static bool TextureLookupCallback(void * context, std::uint32_t handle);
    static GENRESULT TextureCreateCallback(void * context, std::uint32_t width,
                                            std::uint32_t height, std::uint32_t & handle);
    static GENRESULT TextureUploadCallback(void * context, std::uint32_t handle,
                                            const std::uint8_t * pixels,
                                            std::uint32_t width, std::uint32_t height,
                                            std::size_t byteCount);
    static GENRESULT TextureDestroyCallback(void * context, std::uint32_t handle);

    const std::string & LastError() const;
    const char * DriverName() const;
    std::uint32_t DepthWidth() const;
    std::uint32_t DepthHeight() const;
    std::uint32_t DepthRecreations() const;
    std::uint32_t VertexCount() const;
    std::uint32_t IndexCount() const;
    RenderStateSnapshot CurrentRenderState() const;
    TextureStageSnapshot CurrentTextureStageState() const;
    std::uint32_t CurrentTextureHandle() const;
    std::uint32_t FixtureTextureHandle() const;

private:
    GENRESULT SubmitBytes(const void * vertices, std::size_t vertexCount,
                          std::size_t vertexStride, const std::uint16_t * indices,
                          std::size_t indexCount, bool extended);
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}

#endif
