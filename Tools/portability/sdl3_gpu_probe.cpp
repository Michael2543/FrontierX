#include <SDL3/SDL.h>

#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "FrontierX/RenderSubmission.h"
#include "FrontierX/RenderSmokeScene.h"
#include "FrontierX/SdlGpuRenderer.h"
#include "IRenderPrimitive.h"
#include "RPVertex1.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

namespace
{
constexpr int kSkipped = 77;
using FrontierX::RenderVertexP3DiffuseT2;

constexpr std::array<RenderVertexP3DiffuseT2, 7> kVertices = {{
    {0.0F, -0.52F, 0.20F, FrontierX::PackDiffuseBgra(255U, 87U, 46U, 255U), 0.5F, 1.0F},
    {0.58F, 0.48F, 0.20F, FrontierX::PackDiffuseBgra(41U, 255U, 87U, 255U), 1.0F, 0.0F},
    {-0.58F, 0.48F, 0.20F, FrontierX::PackDiffuseBgra(51U, 107U, 255U, 255U), 0.0F, 0.0F},
    {-0.82F, -0.74F, 0.70F, FrontierX::PackDiffuseBgra(209U, 209U, 209U, 255U), 0.0F, 0.0F},
    {0.82F, -0.74F, 0.70F, FrontierX::PackDiffuseBgra(209U, 209U, 209U, 255U), 0.0F, 0.0F},
    {0.82F, 0.74F, 0.70F, FrontierX::PackDiffuseBgra(209U, 209U, 209U, 255U), 0.0F, 0.0F},
    {-0.82F, 0.74F, 0.70F, FrontierX::PackDiffuseBgra(209U, 209U, 209U, 255U), 0.0F, 0.0F},
}};
constexpr std::array<std::uint16_t, 9> kIndices =
    {{0U, 1U, 2U, 3U, 4U, 5U, 3U, 5U, 6U}};
class WindowRuntime
{
public:
    ~WindowRuntime()
    {
        if (window) SDL_DestroyWindow(window);
        if (initialized) SDL_Quit();
    }
    WindowRuntime(const WindowRuntime &) = delete;
    WindowRuntime & operator=(const WindowRuntime &) = delete;
    WindowRuntime() = default;

    bool initialized = false;
    SDL_Window * window = nullptr;
};

RPVertex ToRpVertex(const RenderVertexP3DiffuseT2 & source)
{
    RPVertex vertex{};
    vertex.pos.set(source.x, source.y, source.z);
    vertex.color = source.diffuseBgra;
    vertex.u = source.u;
    vertex.v = source.v;
    return vertex;
}

bool SubmitProductionFixture(FrontierX::SdlGpuRenderer & renderer, std::string & error)
{
    static_assert(sizeof(RPVertex) == sizeof(RenderVertexP3DiffuseT2));
    std::array<RPVertex, kVertices.size()> legacyVertices{};
    for (std::size_t index = 0U; index < kVertices.size(); ++index)
        legacyVertices[index] = ToRpVertex(kVertices[index]);

    FrontierX::NativeComponentManager manager;
    FrontierX::RegisterNativeRenderPrimitive(&manager);
    FrontierX::NativeRenderPrimitiveDesc descriptor(
        FrontierX::SdlGpuRenderer::SubmissionCallback, &renderer);
    void * rawPrimitive = nullptr;
    if (manager.CreateInstance(&descriptor, &rawPrimitive) != GR_OK || !rawPrimitive)
    {
        error = "native IRenderPrimitive creation failed";
        return false;
    }
    auto * primitive = static_cast<IRenderPrimitive *>(rawPrimitive);
    const GENRESULT indexedResult = primitive->draw_indexed_primitive(
        D3DPT_TRIANGLELIST, D3DFVF_RPVERTEX,
        legacyVertices.data(), static_cast<int>(legacyVertices.size()),
        kIndices.data(), static_cast<int>(kIndices.size()), 0U);
    const bool indexedValid = indexedResult == GR_OK &&
        renderer.VertexCount() == kVertices.size() && renderer.IndexCount() == kIndices.size();
    const GENRESULT directResult = FrontierX::SubmitRenderSmokeScene(primitive);
    const bool directValid = renderer.VertexCount() == kIndices.size() &&
        renderer.IndexCount() == 0U && directResult == GR_OK;
    primitive->Release();
    const GENRESULT shutdownResult = manager.ShutDown();
    if (!indexedValid || !directValid || shutdownResult != GR_OK)
    {
        error = renderer.LastError().empty()
            ? "native render submission failed" : renderer.LastError();
        return false;
    }
    return true;
}

int RuntimeUnavailable(bool required, const char * stage, const std::string & error)
{
    std::cerr << "SDL3 GPU runtime unavailable at " << stage << ": " << error << '\n';
    return required ? 1 : kSkipped;
}
}

int main(int argc, char ** argv)
{
    if (argc < 3 || argc > 5)
    {
        std::cerr << "usage: frontierx_sdl3_gpu_probe VERTEX_SPV FRAGMENT_SPV "
                     "[--require-runtime] [--visible]\n";
        return 2;
    }
    bool requireRuntime = false;
    bool visible = false;
    for (int index = 3; index < argc; ++index)
    {
        const std::string option(argv[index]);
        if (option == "--require-runtime") requireRuntime = true;
        else if (option == "--visible") visible = true;
        else
        {
            std::cerr << "unknown option: " << option << '\n';
            return 2;
        }
    }
    if (!std::filesystem::is_regular_file(argv[1]) ||
        !std::filesystem::is_regular_file(argv[2]))
    {
        std::cerr << "shader input failure: shader file is missing\n";
        return 1;
    }

    WindowRuntime windowRuntime;
    if (!SDL_Init(SDL_INIT_VIDEO))
        return RuntimeUnavailable(requireRuntime, "SDL video initialization", SDL_GetError());
    windowRuntime.initialized = true;
    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_NOT_FOCUSABLE;
    if (!visible) flags |= SDL_WINDOW_HIDDEN;
    windowRuntime.window = SDL_CreateWindow("FrontierX SDL3 GPU probe", 640, 360, flags);
    if (!windowRuntime.window)
        return RuntimeUnavailable(requireRuntime, "window creation", SDL_GetError());

    FrontierX::SdlGpuRenderer renderer;
    std::string error;
    const FrontierX::SdlGpuTextureView texture{
        FrontierX::RenderSmokeTextureRgba.data(),
        FrontierX::RenderSmokeTextureWidth,
        FrontierX::RenderSmokeTextureHeight,
        FrontierX::RenderSmokeTextureRgba.size()};
    if (!renderer.Initialize(SDL_GetWindowID(windowRuntime.window), argv[1], argv[2], texture, error))
        return RuntimeUnavailable(requireRuntime, "renderer initialization", error);
    if (!SubmitProductionFixture(renderer, error))
    {
        std::cerr << "render submission failure: " << error << '\n';
        return 1;
    }

    constexpr int frameCount = 12;
    int presentedFrames = 0;
    std::uint32_t initialWidth = 0U;
    std::uint32_t initialHeight = 0U;
    for (int frame = 0; frame < frameCount; ++frame)
    {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {}
        bool presented = false;
        if (!renderer.Present(presented, error))
        {
            std::cerr << "GPU frame submission failed: " << error << '\n';
            return 1;
        }
        if (presented)
        {
            ++presentedFrames;
            if (!initialWidth)
            {
                initialWidth = renderer.DepthWidth();
                initialHeight = renderer.DepthHeight();
                if (!SDL_SetWindowSize(windowRuntime.window, 800, 450))
                {
                    std::cerr << "window resize request failed: " << SDL_GetError() << '\n';
                    return 1;
                }
            }
        }
        SDL_Delay(10U);
    }
    if (!presentedFrames)
        return RuntimeUnavailable(requireRuntime, "swapchain image acquisition", SDL_GetError());
    if (renderer.DepthWidth() == initialWidth && renderer.DepthHeight() == initialHeight)
    {
        std::cerr << "GPU resize was not observed by the swapchain\n";
        return 1;
    }

    FrontierX::SdlGpuReadback readback;
    if (!renderer.Readback64(readback, error) || !readback.structureValid)
    {
        std::cerr << "GPU pixel readback validation failed: " << error << '\n';
        return 1;
    }
    std::cout << "SDL3 GPU probe passed: driver=" << renderer.DriverName()
              << " presented_frames=" << presentedFrames
              << " vertices=" << renderer.VertexCount()
              << " indices=" << renderer.IndexCount()
              << " source=primitivebuilder-native-irenderprimitive"
              << " texture=2x2 resize=" << initialWidth << 'x' << initialHeight << "->"
              << renderer.DepthWidth() << 'x' << renderer.DepthHeight()
              << " depth_recreations=" << renderer.DepthRecreations()
              << " changed_pixels=" << readback.changedPixels
              << " pixel_checksum=" << readback.checksum << '\n';
    return 0;
}
