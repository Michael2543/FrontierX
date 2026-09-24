#include <SDL3/SDL.h>

#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "FrontierX/RenderSubmission.h"
#include "FrontierX/RenderSmokeScene.h"
#include "FrontierX/SdlGpuRenderer.h"
#include "FrontierX/StateMaterialRegistration.h"
#include "FrontierX/VertexBufferManagerRegistration.h"
#include "FDump.h"
#include "IMaterial.h"
#include "IRenderPrimitive.h"
#include "IStateMaterial.h"
#include "IVertexBufferManager.h"
#include "RPVertex1.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <limits>
#include <string>

namespace
{
constexpr int kSkipped = 77;
using FrontierX::RenderVertexP3DiffuseT2;

int DACOM_CDECL IgnoreDiagnostic(ErrorCode, const C8 *, ...) { return 0; }

class MaterialServices final : public IDAComponent
{
public:
    MaterialServices(IRenderPrimitive * primitive, IRenderPipeline * pipeline,
                     IVertexBufferManager * manager) :
        primitive_(primitive), pipeline_(pipeline), manager_(manager) {}

    GENRESULT COMAPI QueryInterface(const C8 * name, void ** output) override
    {
        if (!name || !output) return GR_INVALID_PARMS;
        *output = nullptr;
        if (std::strcmp(name, IID_IRenderPrimitive) == 0) *output = primitive_;
        else if (std::strcmp(name, IID_IRenderPipeline) == 0) *output = pipeline_;
        else if (std::strcmp(name, IID_IVertexBufferManager) == 0) *output = manager_;
        else return GR_INTERFACE_UNSUPPORTED;
        static_cast<IDAComponent *>(*output)->AddRef();
        return GR_OK;
    }
    U32 COMAPI AddRef() override { return ++references_; }
    U32 COMAPI Release() override { return --references_; }

private:
    IRenderPrimitive * primitive_;
    IRenderPipeline * pipeline_;
    IVertexBufferManager * manager_;
    U32 references_ = 1U;
};

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

bool UploadAlternateTextureByLock(FrontierX::NativeRenderPipeline & pipeline, U32 handle)
{
    RPLOCKDATA data{};
    if (pipeline.lock_texture(handle, 0, &data) != GR_OK) return false;
    if (data.width != 2U || data.height != 2U || data.pitch != 8U ||
        data.pf.num_bits() != 32 || !data.pixels)
    {
        pipeline.unlock_texture(handle, 0);
        return false;
    }
    std::memcpy(data.pixels, FrontierX::RenderSmokeAlternateTextureRgba.data(),
        FrontierX::RenderSmokeAlternateTextureRgba.size());
    return pipeline.unlock_texture(handle, 0) == GR_OK;
}

bool SubmitProductionFixture(FrontierX::SdlGpuRenderer & renderer,
                             std::uint64_t & extendedChecksumA,
                             std::uint64_t & extendedChecksumB,
                             std::uint64_t & materialChecksum,
                             std::uint64_t & alternateChecksum,
                             std::uint64_t & copiedChecksum,
                             std::string & error)
{
    static_assert(sizeof(RPVertex) == sizeof(RenderVertexP3DiffuseT2));
    std::array<RPVertex, kVertices.size()> legacyVertices{};
    for (std::size_t index = 0U; index < kVertices.size(); ++index)
    {
        legacyVertices[index] = ToRpVertex(kVertices[index]);
        const U32 alpha = index < 3U ? 210U : 160U;
        legacyVertices[index].color =
            (legacyVertices[index].color & 0x00ffffffU) | (alpha << 24U);
    }
    legacyVertices[6].v = 1.0F;

    FrontierX::NativeComponentManager manager;
    FrontierX::RegisterNativeRenderPrimitive(&manager);
    RegisterTheVertexBufferManager(&manager);
    if (!FrontierX::RegisterNativeStateMaterial(&manager))
    {
        error = "production state material registration failed";
        return false;
    }
    FrontierX::NativeRenderPipeline vertexPipeline(nullptr);
    FrontierX::NativeRenderPrimitiveDesc descriptor(
        FrontierX::SdlGpuRenderer::SubmissionCallback, &renderer,
        FrontierX::SdlGpuRenderer::Submission2Callback,
        FrontierX::SdlGpuRenderer::RenderStateCallback,
        FrontierX::NativeRenderPipeline::OwnedVertexBufferLookup, &vertexPipeline,
        FrontierX::SdlGpuRenderer::TextureStageCallback,
        FrontierX::SdlGpuRenderer::TextureBindCallback);
    void * rawPrimitive = nullptr;
    if (manager.CreateInstance(&descriptor, &rawPrimitive) != GR_OK || !rawPrimitive)
    {
        error = "native IRenderPrimitive creation failed";
        return false;
    }
    auto * primitive = static_cast<IRenderPrimitive *>(rawPrimitive);
    FrontierX::NativeRenderPipeline pipeline(primitive,
        {&renderer, FrontierX::SdlGpuRenderer::TextureLookupCallback,
         FrontierX::SdlGpuRenderer::TextureCreateCallback,
         FrontierX::SdlGpuRenderer::TextureUploadCallback,
         FrontierX::SdlGpuRenderer::TextureDestroyCallback});
    AGGDESC managerDescriptor("VertexBufferManager");
    managerDescriptor.outer = &vertexPipeline;
    void * rawManager = nullptr;
    if (manager.CreateInstance(&managerDescriptor, &rawManager) != GR_OK || !rawManager)
    {
        error = "production vertex manager creation failed";
        return false;
    }
    auto * managerComponent = static_cast<IDAComponent *>(rawManager);
    IVertexBufferManager * vertexManager = nullptr;
    if (managerComponent->QueryInterface(IID_IVertexBufferManager,
            reinterpret_cast<void **>(&vertexManager)) != GR_OK || !vertexManager ||
        vertexManager->initialize(nullptr) != GR_OK)
    {
        error = "production vertex manager initialization failed";
        return false;
    }
    MaterialServices services(primitive, &pipeline, vertexManager);
    DACOMDESC materialDescriptor;
    materialDescriptor.interface_name = "StateMaterial";
    void * rawMaterial = nullptr;
    if (manager.CreateInstance(&materialDescriptor, &rawMaterial) != GR_OK || !rawMaterial)
    {
        error = "production state material creation failed";
        return false;
    }
    auto * material = static_cast<IMaterial *>(rawMaterial);
    IStateMaterial * materialStates = nullptr;
    if (material->QueryInterface(IID_IStateMaterial,
            reinterpret_cast<void **>(&materialStates)) != GR_OK || !materialStates ||
        material->initialize(&services) != GR_OK)
    {
        error = "production state material initialization failed";
        return false;
    }
    IRP_VERTEXBUFFERHANDLE materialBuffer = 0U;
    void * materialVertices = nullptr;
    U32 materialVertexCount = 0U;
    if (vertexPipeline.create_vertex_buffer(D3DFVF_RPVERTEX,
            static_cast<int>(legacyVertices.size()), 0U, &materialBuffer) != GR_OK ||
        vertexPipeline.lock_vertex_buffer(materialBuffer, 0U, &materialVertices,
            &materialVertexCount) != GR_OK ||
        materialVertexCount != legacyVertices.size() || !materialVertices)
    {
        error = "production material vertex buffer setup failed";
        return false;
    }
    std::memcpy(materialVertices, legacyVertices.data(), sizeof(legacyVertices));
    if (vertexPipeline.unlock_vertex_buffer(materialBuffer) != GR_OK)
    {
        error = "production material vertex buffer unlock failed";
        return false;
    }
    const GENRESULT stateResult = FrontierX::ConfigureRenderSmokeState(&pipeline);
    const GENRESULT textureResult = pipeline.set_texture_stage_texture(
        0U, renderer.FixtureTextureHandle());
    const bool materialStateReady =
        materialStates->set_render_state(D3DRS_ZENABLE, 1U) == GR_OK &&
        materialStates->set_texture_stage_state(0U, D3DTSS_COLOROP,
            D3DTOP_MODULATE) == GR_OK &&
        materialStates->set_texture_stage_texture(0U,
            renderer.FixtureTextureHandle()) == GR_OK &&
        material->apply() == GR_OK;
    MaterialContext materialContext{};
    materialContext.vertex_buffer = materialBuffer;
    std::array<U16, kIndices.size()> materialIndices{};
    std::copy(kIndices.begin(), kIndices.end(), materialIndices.begin());
    const GENRESULT indexedResult = materialStateReady ?
        material->render(&materialContext, D3DPT_TRIANGLELIST, nullptr, 0U,
            static_cast<U32>(legacyVertices.size()), materialIndices.data(),
            static_cast<U32>(materialIndices.size()), 0U) : GR_GENERIC;
    FrontierX::SdlGpuReadback materialReadback;
    const bool materialReadbackValid = indexedResult == GR_OK &&
        renderer.Readback64(materialReadback, error) && materialReadback.structureValid;
    materialChecksum = materialReadback.checksum;
    const auto renderState = renderer.CurrentRenderState();
    const bool stateValid = stateResult == GR_OK && textureResult == GR_OK &&
        renderer.CurrentTextureHandle() == renderer.FixtureTextureHandle() &&
        renderState.depthEnabled == 1U &&
        renderState.depthWriteEnabled == 1U &&
        renderState.depthFunction == D3DCMP_LESSEQUAL &&
        renderState.alphaBlendEnabled == 1U &&
        renderState.sourceBlend == D3DBLEND_SRCALPHA &&
        renderState.destinationBlend == D3DBLEND_INVSRCALPHA;
    const bool indexedValid = materialStateReady && materialReadbackValid &&
        renderer.VertexCount() == kVertices.size() && renderer.IndexCount() == kIndices.size();
    std::array<FrontierX::RenderVertexP3DiffuseT4, kVertices.size()> extendedVertices{};
    for (std::size_t index = 0U; index < kVertices.size(); ++index)
    {
        const auto & source = kVertices[index];
        extendedVertices[index] = {source.x, source.y, source.z,
            source.diffuseBgra, source.u, source.v, 0.0F, 0.0F};
    }
    const auto drawExtended = [&]() {
        return pipeline.draw_indexed_primitive(D3DPT_TRIANGLELIST,
            FrontierX::LegacyD3d9::kRpVertex2Fvf,
            extendedVertices.data(), static_cast<int>(extendedVertices.size()),
            kIndices.data(), static_cast<int>(kIndices.size()), 0U);
    };
    FrontierX::SdlGpuReadback extendedA;
    FrontierX::SdlGpuReadback extendedB;
    const bool extendedAValid = drawExtended() == GR_OK &&
        renderer.VertexCount() == extendedVertices.size() &&
        renderer.IndexCount() == kIndices.size() &&
        renderer.Readback64(extendedA, error) && extendedA.structureValid;
    for (auto & vertex : extendedVertices)
    {
        vertex.u2 = 1.0F;
        vertex.v2 = 1.0F;
    }
    const bool extendedBValid = extendedAValid && drawExtended() == GR_OK &&
        renderer.Readback64(extendedB, error) && extendedB.structureValid;
    extendedChecksumA = extendedA.checksum;
    extendedChecksumB = extendedB.checksum;
    const bool secondUvObserved = extendedBValid &&
        extendedChecksumA != extendedChecksumB;
    const GENRESULT directResult = FrontierX::SubmitRenderSmokeScene(&pipeline);
    const bool directValid = renderer.VertexCount() == kIndices.size() &&
        renderer.IndexCount() == 0U && directResult == GR_OK;
    FrontierX::SdlGpuReadback modulated;
    FrontierX::SdlGpuReadback diffuseOnly;
    FrontierX::SdlGpuReadback restored;
    FrontierX::SdlGpuReadback textureAlphaOnly;
    FrontierX::SdlGpuReadback alphaRestored;
    U32 colorOperation = 0U;
    const bool textureStageValid = directValid &&
        renderer.Readback64(modulated, error) && modulated.structureValid &&
        materialReadback.changedPixels == modulated.changedPixels &&
        pipeline.set_texture_stage_state(0U, D3DTSS_COLOROP, D3DTOP_SELECTARG1) == GR_OK &&
        renderer.Readback64(diffuseOnly, error) && diffuseOnly.structureValid &&
        diffuseOnly.checksum != modulated.checksum &&
        pipeline.set_texture_stage_state(0U, D3DTSS_COLOROP, D3DTOP_DISABLE) ==
            GR_NOT_IMPLEMENTED &&
        pipeline.get_texture_stage_state(0U, D3DTSS_COLOROP, &colorOperation) == GR_OK &&
        colorOperation == D3DTOP_SELECTARG1 &&
        pipeline.set_texture_stage_state(0U, D3DTSS_COLOROP, D3DTOP_MODULATE) == GR_OK &&
        renderer.Readback64(restored, error) && restored.structureValid &&
        restored.checksum == modulated.checksum &&
        renderer.CurrentTextureStageState().colorOperation == D3DTOP_MODULATE &&
        pipeline.set_texture_stage_state(0U, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2) == GR_OK &&
        renderer.Readback64(textureAlphaOnly, error) && textureAlphaOnly.structureValid &&
        textureAlphaOnly.checksum != modulated.checksum &&
        pipeline.set_texture_stage_state(0U, D3DTSS_ALPHAOP, D3DTOP_MODULATE) == GR_OK &&
        renderer.Readback64(alphaRestored, error) && alphaRestored.structureValid &&
        alphaRestored.checksum == modulated.checksum;
    const PixelFormat rgbaFormat(PF_RGBA8);
    U32 alternateHandle = 0U;
    U32 copiedHandle = 0U;
    U32 boundHandle = 0U;
    FrontierX::SdlGpuReadback alternateReadback;
    FrontierX::SdlGpuReadback copiedReadback;
    FrontierX::SdlGpuReadback reboundReadback;
    const RECT textureRect{0, 0, 2, 2};
    const bool textureBindingValid = textureStageValid &&
        pipeline.create_texture(2, 2, rgbaFormat, 0, 0U, alternateHandle) == GR_OK &&
        UploadAlternateTextureByLock(pipeline, alternateHandle) &&
        alternateHandle != renderer.FixtureTextureHandle() &&
        pipeline.is_texture(alternateHandle) == GR_OK &&
        materialStates->set_texture_stage_texture(0U, alternateHandle) == GR_OK &&
        material->apply() == GR_OK &&
        renderer.Readback64(alternateReadback, error) &&
        alternateReadback.changedPixels == modulated.changedPixels &&
        alternateReadback.checksum != modulated.checksum &&
        pipeline.create_texture(2, 2, rgbaFormat, 0, 0U, copiedHandle) == GR_OK &&
        pipeline.blit_texture(copiedHandle, 0U, textureRect,
                              alternateHandle, 0U, textureRect) == GR_OK &&
        materialStates->set_texture_stage_texture(0U, copiedHandle) == GR_OK &&
        material->apply() == GR_OK &&
        renderer.Readback64(copiedReadback, error) &&
        copiedReadback.changedPixels == alternateReadback.changedPixels &&
        copiedReadback.checksum == alternateReadback.checksum &&
        materialStates->set_texture_stage_texture(0U, alternateHandle) == GR_OK &&
        material->apply() == GR_OK &&
        pipeline.destroy_texture(copiedHandle) == GR_OK &&
        pipeline.set_texture_stage_texture(0U, alternateHandle + 1U) == GR_NOT_IMPLEMENTED &&
        pipeline.get_texture_stage_texture(0U, &boundHandle) == GR_OK &&
        boundHandle == alternateHandle &&
        pipeline.destroy_texture(alternateHandle) == GR_INVALID_PARMS &&
        materialStates->set_texture_stage_texture(0U, renderer.FixtureTextureHandle()) == GR_OK &&
        material->apply() == GR_OK &&
        renderer.Readback64(reboundReadback, error) && reboundReadback.structureValid &&
        reboundReadback.checksum == modulated.checksum &&
        pipeline.destroy_texture(alternateHandle) == GR_OK &&
        pipeline.is_texture(alternateHandle) != GR_OK &&
        pipeline.set_texture_stage_texture(0U, alternateHandle) == GR_NOT_IMPLEMENTED &&
        renderer.CurrentTextureHandle() == renderer.FixtureTextureHandle();
    alternateChecksum = alternateReadback.checksum;
    copiedChecksum = copiedReadback.checksum;
    materialStates->Release();
    material->Release();
    vertexPipeline.destroy_vertex_buffer(materialBuffer);
    vertexManager->cleanup();
    vertexManager->Release();
    managerComponent->Release();
    primitive->Release();
    const GENRESULT shutdownResult = manager.ShutDown();
    if (!stateValid || !indexedValid || !secondUvObserved || !directValid ||
        !textureStageValid || !textureBindingValid ||
        shutdownResult != GR_OK)
    {
        if (!textureBindingValid && textureStageValid)
            error = "native texture handle binding or rollback failed: handle=" +
                std::to_string(alternateHandle) + " bound=" + std::to_string(boundHandle) +
                " current=" + std::to_string(renderer.CurrentTextureHandle()) +
                " baseline=" + std::to_string(modulated.checksum) +
                " alternate=" + std::to_string(alternateReadback.checksum) +
                " copied=" + std::to_string(copiedReadback.checksum) +
                " alternate_valid=" + std::to_string(alternateReadback.structureValid) +
                " rebound=" + std::to_string(reboundReadback.checksum);
        else
            error = renderer.LastError().empty()
                ? "native render submission failed: state=" +
                    std::to_string(stateValid) + " indexed=" +
                    std::to_string(indexedValid) + " material_ready=" +
                    std::to_string(materialStateReady) + " material_readback=" +
                    std::to_string(materialReadbackValid) + " direct=" +
                    std::to_string(directValid) + " texture_stage=" +
                    std::to_string(textureStageValid) + " material_checksum=" +
                    std::to_string(materialReadback.checksum) + " direct_checksum=" +
                    std::to_string(modulated.checksum) + " shutdown=" +
                    std::to_string(shutdownResult)
                : renderer.LastError();
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

extern "C"
{
DA_ERROR_HANDLER FDUMP = IgnoreDiagnostic;
}

int main(int argc, char ** argv)
{
    if (argc < 4 || argc > 6)
    {
        std::cerr << "usage: frontierx_sdl3_gpu_probe VERTEX_SPV FRAGMENT_SPV "
                     "EXTENDED_VERTEX_SPV "
                     "[--require-runtime] [--visible]\n";
        return 2;
    }
    bool requireRuntime = false;
    bool visible = false;
    for (int index = 4; index < argc; ++index)
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
        !std::filesystem::is_regular_file(argv[2]) ||
        !std::filesystem::is_regular_file(argv[3]))
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
    if (!renderer.Initialize(SDL_GetWindowID(windowRuntime.window), argv[1], argv[2],
                             texture, error, argv[3]))
        return RuntimeUnavailable(requireRuntime, "renderer initialization", error);
    std::uint64_t extendedChecksumA = 0U;
    std::uint64_t extendedChecksumB = 0U;
    std::uint64_t materialChecksum = 0U;
    std::uint64_t alternateChecksum = 0U;
    std::uint64_t copiedChecksum = 0U;
    if (!SubmitProductionFixture(renderer, extendedChecksumA, extendedChecksumB,
                                 materialChecksum, alternateChecksum,
                                 copiedChecksum, error))
    {
        std::cerr << "render submission failure: " << error << '\n';
        return 1;
    }
    const FrontierX::RenderSubmissionView oversized{
        FrontierX::PrimitiveTopology::TriangleList, kVertices.data(),
        std::numeric_limits<Uint32>::max() / sizeof(RenderVertexP3DiffuseT2),
        kIndices.data(), 3U};
    FrontierX::SdlGpuReadback afterRejected;
    if (renderer.Submit(oversized) != GR_INVALID_PARMS ||
        renderer.VertexCount() != FrontierX::RenderSmokeVertexCount ||
        renderer.IndexCount() != 0U ||
        !renderer.Readback64(afterRejected, error) ||
        !afterRejected.structureValid)
    {
        std::cerr << "rejected submission lost the prior scene: " << error << '\n';
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
    if (!renderer.Readback64(readback, error) || !readback.structureValid ||
        readback.checksum != afterRejected.checksum ||
        readback.changedPixels != afterRejected.changedPixels)
    {
        std::cerr << "GPU pixel readback validation failed: " << error << '\n';
        return 1;
    }
    const std::uint64_t firstChecksum = readback.checksum;
    const std::size_t firstChangedPixels = readback.changedPixels;
    const std::uint32_t firstDepthRecreations = renderer.DepthRecreations();
    renderer.Shutdown();
    if (!renderer.Initialize(SDL_GetWindowID(windowRuntime.window), argv[1], argv[2],
                             texture, error, argv[3]))
    {
        std::cerr << "repeated GPU initialization failed: " << error << '\n';
        return 1;
    }
    std::uint64_t repeatedExtendedA = 0U;
    std::uint64_t repeatedExtendedB = 0U;
    std::uint64_t repeatedMaterialChecksum = 0U;
    std::uint64_t repeatedAlternateChecksum = 0U;
    std::uint64_t repeatedCopiedChecksum = 0U;
    if (!SubmitProductionFixture(renderer, repeatedExtendedA, repeatedExtendedB,
                                 repeatedMaterialChecksum, repeatedAlternateChecksum,
                                 repeatedCopiedChecksum, error) ||
        repeatedExtendedA != extendedChecksumA ||
        repeatedExtendedB != extendedChecksumB ||
        repeatedMaterialChecksum != materialChecksum ||
        repeatedAlternateChecksum != alternateChecksum ||
        repeatedCopiedChecksum != copiedChecksum)
    {
        std::cerr << "repeated render submission failed: " << error << '\n';
        return 1;
    }
    bool repeatedPresented = false;
    for (int attempt = 0; attempt < 10 && !repeatedPresented; ++attempt)
    {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {}
        if (!renderer.Present(repeatedPresented, error))
        {
            std::cerr << "repeated GPU presentation failed: " << error << '\n';
            return 1;
        }
        if (!repeatedPresented) SDL_Delay(10U);
    }
    FrontierX::SdlGpuReadback repeatedReadback;
    if (!repeatedPresented || !renderer.Readback64(repeatedReadback, error) ||
        !repeatedReadback.structureValid ||
        repeatedReadback.checksum != firstChecksum ||
        repeatedReadback.changedPixels != firstChangedPixels)
    {
        std::cerr << "repeated GPU readback mismatch: " << error << '\n';
        return 1;
    }
    std::cout << "SDL3 GPU probe passed: driver=" << renderer.DriverName()
              << " presented_frames=" << presentedFrames
              << " vertices=" << renderer.VertexCount()
              << " indices=" << renderer.IndexCount()
              << " source=primitivebuilder-native-irenderpipeline"
              << " texture=2x2 resize=" << initialWidth << 'x' << initialHeight << "->"
              << renderer.DepthWidth() << 'x' << renderer.DepthHeight()
              << " depth_recreations=" << firstDepthRecreations
              << " changed_pixels=" << readback.changedPixels
              << " pixel_checksum=" << readback.checksum
              << " material_checksum=" << materialChecksum
              << " extended_uv_checksums=" << extendedChecksumA << ','
              << extendedChecksumB << " alternate_texture_checksum=" << alternateChecksum
              << " copied_texture_checksum=" << copiedChecksum
              << " scene_runs=2 repeated_presented=1\n";
    return 0;
}
