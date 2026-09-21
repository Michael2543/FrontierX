#ifndef FRONTIERX_SDL_GPU_RENDERER_H
#define FRONTIERX_SDL_GPU_RENDERER_H

#include "DACOM.H"
#include "FrontierX/RenderSubmission.h"

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
    std::uint64_t checksum = 0U;
    std::size_t changedPixels = 0U;
    bool structureValid = false;
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
                    std::string & error);
    GENRESULT Submit(const RenderSubmissionView & submission);
    bool Present(bool & presented, std::string & error);
    bool Readback64(SdlGpuReadback & result, std::string & error);
    void Shutdown();

    static GENRESULT DACOM_CDECL SubmissionCallback(
        void * context, const RenderSubmissionView & submission);

    const std::string & LastError() const;
    const char * DriverName() const;
    std::uint32_t DepthWidth() const;
    std::uint32_t DepthHeight() const;
    std::uint32_t DepthRecreations() const;
    std::uint32_t VertexCount() const;
    std::uint32_t IndexCount() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}

#endif
