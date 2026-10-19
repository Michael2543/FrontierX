#include <SDL3/SDL.h>
#include "SdlGpuHostSetup.h"

#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "FrontierX/RenderSubmission.h"
#include "FrontierX/RenderSmokeScene.h"
#include "FrontierX/SdlGpuRenderer.h"
#include "FrontierX/StateMaterialRegistration.h"
#include "FrontierX/VertexBufferManagerRegistration.h"
#include "MaterialTextureFixture.h"
#include "FDump.h"
#include "IMaterial.h"
#include "IMaterialLibrary.h"
#include "IMaterialProperties.h"
#include "IRenderPrimitive.h"
#include "IStateMaterial.h"
#include "ITextureLibrary.h"
#include "IVertexBufferManager.h"
#include "RPVertex1.h"
#include "matrix4.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

extern bool RegisterNativeTextureLibrary(ICOManager * manager);
extern bool Register_MaterialLibrary(void);
extern bool Register_DcDtOcMaterial(void);

namespace
{
constexpr int kSkipped = 77;
using FrontierX::RenderVertexP3DiffuseT2;

std::pair<std::uint64_t, std::uint64_t> ImageXMoment(
    const FrontierX::SdlGpuReadback & image)
{
    std::uint64_t sum = 0U;
    std::uint64_t count = 0U;
    for (std::size_t y = 0U; y < FrontierX::SdlGpuReadback::Height; ++y)
        for (std::size_t x = 0U; x < FrontierX::SdlGpuReadback::Width; ++x)
        {
            const std::size_t offset =
                (y * FrontierX::SdlGpuReadback::Width + x) * 3U;
            if (image.rgb8[offset] > 24U || image.rgb8[offset + 1U] > 24U ||
                image.rgb8[offset + 2U] > 32U)
            {
                sum += x;
                ++count;
            }
        }
    return {sum, count};
}

int DACOM_CDECL IgnoreDiagnostic(ErrorCode, const C8 *, ...) { return 0; }

// StatisBolt::Render and RepellentCloud::Render require stage1 color AND
// alpha multiplication, with independently wrapped/clamped second coordinates.
// Existing ADD/emissive checks cannot distinguish a missing alpha multiply.
// This protects their actual native receiver contract, not the ability caller.
template<std::size_t Count, class Draw>
bool CheckAbilityModulation(FrontierX::NativeRenderPipeline &pipeline,
                           FrontierX::SdlGpuRenderer &renderer,
                           std::array<FrontierX::RenderVertexP3DiffuseT4, Count> &vertices,
                           Draw draw, std::string &error)
{
    constexpr std::array<D3DRENDERSTATETYPE, 7> renderFields{{D3DRS_ZENABLE, D3DRS_ZWRITEENABLE,
        D3DRS_ALPHABLENDENABLE, D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_CULLMODE, D3DRS_ALPHATESTENABLE}};
    constexpr std::array<D3DTEXTURESTAGESTATETYPE, 7> stageFields{{D3DTSS_COLOROP, D3DTSS_COLORARG1,
        D3DTSS_COLORARG2, D3DTSS_ALPHAOP, D3DTSS_ALPHAARG1, D3DTSS_ALPHAARG2, D3DTSS_TEXCOORDINDEX}};
    constexpr std::array<D3DSAMPLERSTATETYPE, 5> samplerFields{{D3DSAMP_ADDRESSU, D3DSAMP_ADDRESSV,
        D3DSAMP_MINFILTER, D3DSAMP_MAGFILTER, D3DSAMP_MIPFILTER}};
    std::array<U32, 7> savedRender{};
    std::array<std::array<U32, 7>, 2> savedStages{};
    std::array<std::array<U32, 5>, 2> savedSamplers{};
    std::array<U32, 2> savedTextures{};
    const auto savedVertices = vertices;
    bool valid = true;
    for (std::size_t i = 0U; i < renderFields.size(); ++i)
        valid &= pipeline.get_render_state(renderFields[i], &savedRender[i]) == GR_OK;
    for (U32 stage = 0U; stage < 2U; ++stage)
    {
        valid &= pipeline.get_texture_stage_texture(stage, &savedTextures[stage]) == GR_OK;
        for (std::size_t i = 0U; i < stageFields.size(); ++i)
            valid &= pipeline.get_texture_stage_state(stage, stageFields[i], &savedStages[stage][i]) == GR_OK;
        for (std::size_t i = 0U; i < samplerFields.size(); ++i)
            valid &= pipeline.get_sampler_state(stage, samplerFields[i], &savedSamplers[stage][i]) == GR_OK;
    }
    FrontierX::SdlGpuReadback previous, zeroAlpha, wrap, clamp, repel, alphaChanged, colorChanged, restored;
    valid &= draw() == GR_OK && renderer.Readback64(previous, error);
    const std::array<unsigned char, 16> basePixels{{192,160,224,160, 192,160,224,160,
                                                  192,160,224,160, 192,160,224,160}};
    const std::array<unsigned char, 16> movingPixels{{160,96,224,80, 192,64,128,96,
                                                    64,192,128,160, 96,128,64,224}};
    U32 baseTexture = 0U, movingTexture = 0U;
    valid &= renderer.CreateTexture(2U, 2U, baseTexture) == GR_OK &&
        renderer.ReplaceTexture(baseTexture, {basePixels.data(), 2U, 2U, basePixels.size()}) == GR_OK &&
        renderer.CreateTexture(2U, 2U, movingTexture) == GR_OK &&
        renderer.ReplaceTexture(movingTexture, {movingPixels.data(), 2U, 2U, movingPixels.size()}) == GR_OK;
    for (auto &vertex : vertices)
    {
        vertex.diffuseBgra = FrontierX::PackDiffuseBgra(160, 192, 224, 128);
        vertex.u = vertex.v = 0.0F;
        vertex.u2 = vertex.v2 = 2.0F;
    }
    for (U32 stage = 0U; stage < 2U; ++stage)
    {
        const U32 values[7] = {D3DTOP_MODULATE, D3DTA_TEXTURE, stage ? D3DTA_CURRENT : D3DTA_DIFFUSE,
            D3DTOP_MODULATE, D3DTA_TEXTURE, stage ? D3DTA_CURRENT : D3DTA_DIFFUSE, stage};
        for (std::size_t i = 0U; i < stageFields.size(); ++i)
            valid &= pipeline.set_texture_stage_state(stage, stageFields[i], values[i]) == GR_OK;
        valid &= pipeline.set_texture_stage_texture(stage, stage ? movingTexture : baseTexture) == GR_OK;
        for (const auto state : {D3DSAMP_ADDRESSU, D3DSAMP_ADDRESSV})
            valid &= pipeline.set_sampler_state(stage, state, D3DTADDRESS_WRAP) == GR_OK;
        for (const auto state : {D3DSAMP_MINFILTER, D3DSAMP_MAGFILTER, D3DSAMP_MIPFILTER})
            valid &= pipeline.set_sampler_state(stage, state, D3DTEXF_POINT) == GR_OK;
    }
    const U32 renderValues[7] = {TRUE, FALSE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_ONE, D3DCULL_NONE, FALSE};
    for (std::size_t i = 0U; i < renderFields.size(); ++i)
        valid &= pipeline.set_render_state(renderFields[i], renderValues[i]) == GR_OK;
    auto pixels = movingPixels;
    for (std::size_t i = 3U; i < pixels.size(); i += 4U) pixels[i] = 0U;
    valid &= renderer.ReplaceTexture(movingTexture, {pixels.data(), 2U, 2U, pixels.size()}) == GR_OK &&
        draw() == GR_OK && renderer.Readback64(zeroAlpha, error) &&
        renderer.ReplaceTexture(movingTexture, {movingPixels.data(), 2U, 2U, movingPixels.size()}) == GR_OK &&
        draw() == GR_OK && renderer.Readback64(wrap, error);
    // Independent arithmetic oracle: two texture colors * diffuse, all three
    // alpha factors, then the source blend against the actually observed clear.
    const auto matches = [&](const FrontierX::SdlGpuReadback &image, unsigned sample, bool additive) {
        const unsigned diffuse[4] = {160U, 192U, 224U, 128U};
        const double alpha = (160.0 / 255.0) * (128.0 / 255.0) * (movingPixels[sample + 3U] / 255.0);
        unsigned affected = 0U;
        for (std::size_t offset = 0U; offset < image.rgb8.size(); offset += 3U)
        {
            if (std::equal(image.rgb8.begin() + offset, image.rgb8.begin() + offset + 3U,
                           zeroAlpha.rgb8.begin() + offset)) continue;
            ++affected;
            for (unsigned channel = 0U; channel < 3U; ++channel)
            {
                const double color = (basePixels[channel] / 255.0) * (diffuse[channel] / 255.0) *
                    (movingPixels[sample + channel] / 255.0);
                const double expected = std::min(255.0, color * alpha * 255.0 +
                    zeroAlpha.rgb8[offset + channel] * (additive ? 1.0 : 1.0 - alpha));
                if (std::abs(static_cast<int>(image.rgb8[offset + channel]) -
                             static_cast<int>(std::lround(expected))) > 2) return false;
            }
        }
        return affected > 0U;
    };
    const bool wrapArithmetic = matches(wrap, 0U, true);
    if (!wrapArithmetic) std::cerr << "ability wrap arithmetic failed\n";
    valid &= wrapArithmetic;
    for (const auto state : {D3DSAMP_ADDRESSU, D3DSAMP_ADDRESSV})
        valid &= pipeline.set_sampler_state(1U, state, D3DTADDRESS_CLAMP) == GR_OK;
    valid &= draw() == GR_OK && renderer.Readback64(clamp, error) &&
        clamp.checksum != wrap.checksum && matches(clamp, 12U, true);
    for (const auto state : {D3DSAMP_ADDRESSU, D3DSAMP_ADDRESSV})
        valid &= pipeline.set_sampler_state(0U, state, D3DTADDRESS_CLAMP) == GR_OK;
    valid &= pipeline.set_render_state(D3DRS_ZENABLE, FALSE) == GR_OK &&
        pipeline.set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA) == GR_OK &&
        draw() == GR_OK && renderer.Readback64(repel, error) && matches(repel, 12U, false);
    valid &= pipeline.set_render_state(D3DRS_ZENABLE, TRUE) == GR_OK &&
        pipeline.set_render_state(D3DRS_DESTBLEND, D3DBLEND_ONE) == GR_OK;
    for (U32 stage = 0U; stage < 2U; ++stage)
        for (const auto state : {D3DSAMP_ADDRESSU, D3DSAMP_ADDRESSV})
            valid &= pipeline.set_sampler_state(stage, state, D3DTADDRESS_WRAP) == GR_OK;
    pixels = movingPixels;
    for (std::size_t i = 3U; i < pixels.size(); i += 4U) pixels[i] = 255U;
    valid &= renderer.ReplaceTexture(movingTexture, {pixels.data(), 2U, 2U, pixels.size()}) == GR_OK &&
        draw() == GR_OK && renderer.Readback64(alphaChanged, error) && alphaChanged.checksum != wrap.checksum;
    pixels = movingPixels;
    pixels[0] = 224U; pixels[1] = 224U;
    valid &= renderer.ReplaceTexture(movingTexture, {pixels.data(), 2U, 2U, pixels.size()}) == GR_OK &&
        draw() == GR_OK && renderer.Readback64(colorChanged, error) && colorChanged.checksum != wrap.checksum &&
        renderer.ReplaceTexture(movingTexture, {movingPixels.data(), 2U, 2U, movingPixels.size()}) == GR_OK &&
        draw() == GR_OK && renderer.Readback64(restored, error) && restored.checksum == wrap.checksum;
    for (std::size_t i = 0U; i < renderFields.size(); ++i)
        valid &= pipeline.set_render_state(renderFields[i], savedRender[i]) == GR_OK;
    for (U32 stage = 0U; stage < 2U; ++stage)
    {
        for (std::size_t i = 0U; i < stageFields.size(); ++i)
            valid &= pipeline.set_texture_stage_state(stage, stageFields[i], savedStages[stage][i]) == GR_OK;
        for (std::size_t i = 0U; i < samplerFields.size(); ++i)
            valid &= pipeline.set_sampler_state(stage, samplerFields[i], savedSamplers[stage][i]) == GR_OK;
        valid &= pipeline.set_texture_stage_texture(stage, savedTextures[stage]) == GR_OK;
    }
    vertices = savedVertices;
    valid &= draw() == GR_OK && renderer.Readback64(restored, error) && restored.checksum == previous.checksum;
    if (baseTexture) valid &= renderer.DestroyTexture(baseTexture) == GR_OK;
    if (movingTexture) valid &= renderer.DestroyTexture(movingTexture) == GR_OK;
    if (valid) std::cout << "Selected Stasis/Repellent stage1 color and alpha modulation, WRAP/CLAMP and exact restoration checked\n";
    else { error = "selected ability two-stage color/alpha modulation or state restoration failed"; std::cerr << error << "\n"; }
    return valid;
}

class MaterialServices final : public IDAComponent
{
public:
    MaterialServices(IRenderPrimitive * primitive, IRenderPipeline * pipeline,
                     IVertexBufferManager * manager) :
        primitive_(primitive), pipeline_(pipeline), manager_(manager) {}

    void SetTextureLibrary(ITextureLibrary * textures) { textures_ = textures; }

    GENRESULT COMAPI QueryInterface(const C8 * name, void ** output) override
    {
        if (!name || !output) return GR_INVALID_PARMS;
        *output = nullptr;
        if (std::strcmp(name, IID_IRenderPrimitive) == 0) *output = primitive_;
        else if (std::strcmp(name, IID_IRenderPipeline) == 0) *output = pipeline_;
        else if (std::strcmp(name, IID_IVertexBufferManager) == 0) *output = manager_;
        else if (std::strcmp(name, IID_ITextureLibrary) == 0) *output = textures_;
        else return GR_INTERFACE_UNSUPPORTED;
        if (!*output) return GR_INTERFACE_UNSUPPORTED;
        static_cast<IDAComponent *>(*output)->AddRef();
        return GR_OK;
    }
    U32 COMAPI AddRef() override { return ++references_; }
    U32 COMAPI Release() override { return --references_; }

private:
    IRenderPrimitive * primitive_;
    IRenderPipeline * pipeline_;
    IVertexBufferManager * manager_;
    ITextureLibrary * textures_ = nullptr;
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

bool SameForegroundMask(const FrontierX::SdlGpuReadback & left,
                        const FrontierX::SdlGpuReadback & right)
{
    if (!left.rgbValid || !right.rgbValid) return false;
    for (std::size_t pixel = 0U;
         pixel < FrontierX::SdlGpuReadback::Width * FrontierX::SdlGpuReadback::Height;
         ++pixel)
    {
        const bool leftForeground =
            std::memcmp(left.rgb8.data() + pixel * 3U, left.rgb8.data(), 3U) != 0;
        const bool rightForeground =
            std::memcmp(right.rgb8.data() + pixel * 3U, right.rgb8.data(), 3U) != 0;
        if (leftForeground != rightForeground) return false;
    }
    return true;
}

bool SubmitProductionFixture(FrontierX::SdlGpuRenderer & renderer, SDL_Window * window,
                             const std::filesystem::path & utfFixturePath,
                             std::uint64_t & extendedChecksumA,
                             std::uint64_t & extendedChecksumB,
                             std::uint64_t & materialChecksum,
                             std::uint64_t & alternateChecksum,
                             std::uint64_t & copiedChecksum,
                             std::uint64_t & loadedChecksum,
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
    if (!RegisterNativeTextureLibrary(&manager))
    {
        error = "production texture library registration failed";
        return false;
    }
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
        FrontierX::SdlGpuRenderer::TextureBindCallback,
        FrontierX::SdlGpuRenderer::ModelviewCallback,
        FrontierX::SdlGpuRenderer::ProjectionCallback,
        FrontierX::SdlGpuRenderer::Submission3Callback,
        nullptr, FrontierX::SdlGpuRenderer::SamplerStateCallback,
        FrontierX::SdlGpuRenderer::ViewportCallback);
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
         FrontierX::SdlGpuRenderer::TextureDestroyCallback,
             FrontierX::SdlGpuRenderer::TextureCreateMipsCallback, FrontierX::SdlGpuRenderer::TextureUploadMipsCallback});
    AGGDESC managerDescriptor("VertexBufferManager");
    managerDescriptor.outer = &vertexPipeline;
    IDAComponent * managerComponent = nullptr;
    managerDescriptor.inner = &managerComponent;
    void * rawManager = nullptr;
    if (manager.CreateInstance(&managerDescriptor, &rawManager) != GR_OK ||
        !rawManager || !managerComponent)
    {
        error = "production vertex manager creation failed";
        return false;
    }
    IVertexBufferManager * vertexManager = nullptr;
    if (managerComponent->QueryInterface(IID_IVertexBufferManager,
            reinterpret_cast<void **>(&vertexManager)) != GR_OK || !vertexManager ||
        vertexManager->initialize(nullptr) != GR_OK)
    {
        error = "production vertex manager initialization failed";
        return false;
    }
    MaterialServices services(primitive, &pipeline, vertexManager);
    AGGDESC textureDescriptor("TextureLibrary");
    textureDescriptor.outer = &services;
    IDAComponent * textureInner = nullptr;
    textureDescriptor.inner = &textureInner;
    void * rawTextures = nullptr;
    if (manager.CreateInstance(&textureDescriptor, &rawTextures) != GR_OK ||
        !rawTextures || !textureInner)
    {
        error = "production texture library creation failed";
        return false;
    }
    IAggregateComponent * textureAggregate = nullptr;
    ITextureLibrary * textures = nullptr;
    if (textureInner->QueryInterface(IID_IAggregateComponent,
            reinterpret_cast<void **>(&textureAggregate)) != GR_OK ||
        !textureAggregate || textureAggregate->Initialize() != GR_OK ||
        textureInner->QueryInterface(IID_ITextureLibrary,
            reinterpret_cast<void **>(&textures)) != GR_OK || !textures)
    {
        error = "production texture library initialization failed";
        return false;
    }
    services.SetTextureLibrary(textures);
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
    // SpaceEnv requires real alpha-test discard, not a successful ignored state.
    const auto drawAlphaFixture = [&]() {
        return pipeline.draw_indexed_primitive(D3DPT_TRIANGLELIST,
            FrontierX::LegacyD3d9::kRpVertexFvf,
            legacyVertices.data(), static_cast<int>(legacyVertices.size()),
            kIndices.data(), static_cast<int>(kIndices.size()), 0U);
    };
    FrontierX::SdlGpuReadback alphaRejected, alphaTestRestored;
    U32 alphaReference = 0;
    const bool alphaTestValid =
        pipeline.set_render_state(D3DRS_ALPHAREF, 8U) == GR_OK &&
        pipeline.get_render_state(D3DRS_ALPHAREF, &alphaReference) == GR_OK &&
        alphaReference == 8U &&
        pipeline.set_render_state(D3DRS_ALPHAREF, 256U) == GR_INVALID_PARMS &&
        renderer.CurrentRenderState().alphaReference == 8U &&
        pipeline.set_render_state(D3DRS_ALPHAFUNC, D3DCMP_NEVER) == GR_OK &&
        pipeline.set_render_state(D3DRS_ALPHATESTENABLE, 1U) == GR_OK &&
        drawAlphaFixture() == GR_OK && renderer.Readback64(alphaRejected, error) &&
        alphaRejected.changedPixels == 0U && alphaRejected.opaquePixels &&
        pipeline.set_render_state(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL) == GR_OK &&
        drawAlphaFixture() == GR_OK && renderer.Readback64(alphaTestRestored, error) &&
        alphaTestRestored.checksum == materialReadback.checksum && alphaTestRestored.opaquePixels &&
        pipeline.set_render_state(D3DRS_ALPHATESTENABLE, 0U) == GR_OK;
    if (alphaTestValid) std::cout << "Native alpha test rejects all fragments and restores source threshold output\n";
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
    const bool extendedAValid =
        pipeline.set_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX, 1U) == GR_OK &&
        drawExtended() == GR_OK &&
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
    // Match AreaEffectBolt's 20-segment shared RPVERTEX2 buffer and real PIPE
    // lock/update/unlock/draw_primitive_vb caller, not just a backend submission.
    std::array<FrontierX::RenderVertexP3DiffuseT4, 42> ring{};
    std::array<U16, 120> ringTriangles{};
    for (std::size_t i = 0; i < ring.size(); ++i)
    {
        const float angle = float(i / 2) * 6.283185307F / 20.0F;
        const float radius = i % 2 ? 0.35F : 0.7F;
        ring[i] = {radius * std::cos(angle), radius * std::sin(angle), 0.25F,
            0x80ffffffU, float(i / 2), float(i % 2), float(i / 2) * 0.05F, float(i % 2)};
    }
    for (U16 i = 0; i < 40; ++i)
    {
        ringTriangles[i * 3] = i + (i & 1U);
        ringTriangles[i * 3 + 1] = i + 1U - (i & 1U);
        ringTriangles[i * 3 + 2] = i + 2;
    }
    IRP_VERTEXBUFFERHANDLE ringBuffer = 0;
    void *ringData = nullptr;
    U32 ringCount = 0;
    FrontierX::SdlGpuReadback ringExpected, ringActual;
    if (pipeline.draw_indexed_primitive(D3DPT_TRIANGLELIST, FrontierX::LegacyD3d9::kRpVertex2Fvf,
            ring.data(), int(ring.size()), ringTriangles.data(), int(ringTriangles.size()), 0) != GR_OK ||
        !renderer.Readback64(ringExpected, error) || ringExpected.changedPixels == 0 ||
        pipeline.create_vertex_buffer(FrontierX::LegacyD3d9::kRpVertex2Fvf,
            int(ring.size()), 0, &ringBuffer) != GR_OK ||
        pipeline.lock_vertex_buffer(ringBuffer, 0, &ringData, &ringCount) != GR_OK ||
        ringCount != ring.size())
    {
        error = "area-effect shared two-UV ring buffer setup failed";
        return false;
    }
    std::memcpy(ringData, ring.data(), sizeof(ring));
    if (pipeline.unlock_vertex_buffer(ringBuffer) != GR_OK ||
        pipeline.draw_primitive_vb(D3DPT_TRIANGLESTRIP, ringBuffer, 0, int(ring.size()), 0) != GR_OK ||
        !renderer.Readback64(ringActual, error) || ringActual.checksum != ringExpected.checksum ||
        pipeline.destroy_vertex_buffer(ringBuffer) != GR_OK)
    {
        error = "area-effect PIPE two-UV ring strip differs from explicit triangles";
        return false;
    }
    std::cout << "AreaEffectBolt 42-vertex PIPE buffer strip matches 40 explicit triangles\n";
    if (pipeline.set_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX, 0U) != GR_OK)
    {
        error = "texture coordinate stage reset failed";
        return false;
    }
    FrontierX::SdlGpuReadback stage1First, stage1Second, stage1Wrap, stage1Clamp, stage1Reset;
    U32 stage1Bound = 0U, stage1Address = 0U;
    const bool stage1Valid =
        pipeline.set_texture_stage_texture(1U, renderer.FixtureTextureHandle()) == GR_OK &&
        pipeline.get_texture_stage_texture(1U, &stage1Bound) == GR_OK &&
        stage1Bound == renderer.FixtureTextureHandle() &&
        pipeline.set_sampler_state(1U, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP) == GR_OK &&
        pipeline.get_sampler_state(1U, D3DSAMP_ADDRESSU, &stage1Address) == GR_OK &&
        stage1Address == D3DTADDRESS_WRAP &&
        pipeline.set_sampler_state(1U, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP) == GR_OK &&
        pipeline.set_texture_stage_state(1U, D3DTSS_COLOROP, D3DTOP_ADD) == GR_OK &&
        pipeline.set_texture_stage_state(1U, D3DTSS_COLORARG1, D3DTA_TEXTURE) == GR_OK &&
        pipeline.set_texture_stage_state(1U, D3DTSS_COLORARG2, D3DTA_CURRENT) == GR_OK &&
        pipeline.set_texture_stage_state(1U, D3DTSS_ALPHAOP, D3DTOP_DISABLE) == GR_OK &&
        pipeline.set_texture_stage_state(1U, D3DTSS_TEXCOORDINDEX, 1U) == GR_OK &&
        drawExtended() == GR_OK && renderer.Readback64(stage1First, error);
    for (auto & vertex : extendedVertices) { vertex.u2 = 0.0F; vertex.v2 = 0.0F; }
    const bool stage1SecondValid = stage1Valid &&
        drawExtended() == GR_OK && renderer.Readback64(stage1Second, error) &&
        stage1Second.checksum != stage1First.checksum;
    for (auto & vertex : extendedVertices) { vertex.u2 = 2.0F; vertex.v2 = 2.0F; }
    const bool stage1WrapValid = stage1SecondValid &&
        pipeline.set_sampler_state(1U, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP) == GR_OK &&
        pipeline.set_sampler_state(1U, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP) == GR_OK &&
        drawExtended() == GR_OK && renderer.Readback64(stage1Wrap, error) &&
        stage1Wrap.checksum == stage1Second.checksum &&
        pipeline.set_sampler_state(1U, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP) == GR_OK &&
        pipeline.set_sampler_state(1U, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP) == GR_OK &&
        drawExtended() == GR_OK && renderer.Readback64(stage1Clamp, error) &&
        stage1Clamp.checksum == stage1First.checksum;
    const bool stage1Observed = stage1WrapValid &&
        pipeline.set_texture_stage_state(1U, D3DTSS_COLOROP, D3DTOP_DISABLE) == GR_OK &&
        pipeline.set_texture_stage_texture(1U, 0U) == GR_OK &&
        drawExtended() == GR_OK && renderer.Readback64(stage1Reset, error) &&
        stage1Reset.checksum != stage1Second.checksum;
    if (stage1Observed) std::cout << "Native stage1 ADD uses independent texture, sampler and second UV\n";
    const auto drawAbilityTriangle = [&]() {
        return pipeline.draw_indexed_primitive(D3DPT_TRIANGLELIST,
            FrontierX::LegacyD3d9::kRpVertex2Fvf, extendedVertices.data(),
            static_cast<int>(extendedVertices.size()), kIndices.data(), 3, 0U);
    };
    const bool abilityModulationValid = CheckAbilityModulation(pipeline, renderer, extendedVertices, drawAbilityTriangle, error);
    std::array<FrontierX::RenderVertexP3DiffuseSpecularT4, kVertices.size()>
        gameVertices{};
    for (std::size_t index = 0U; index < kVertices.size(); ++index)
    {
        const auto & source = kVertices[index];
        gameVertices[index] = {source.x, source.y, source.z, source.diffuseBgra,
                               0x00000000U, source.u, source.v, 1.0F, 1.0F};
    }
    const auto drawGame = [&]() {
        return pipeline.draw_indexed_primitive(D3DPT_TRIANGLELIST,
            FrontierX::LegacyD3d9::kMtVertexFvf,
            gameVertices.data(), static_cast<int>(gameVertices.size()),
            kIndices.data(), static_cast<int>(kIndices.size()), 0U);
    };
    FrontierX::SdlGpuReadback gameBase;
    FrontierX::SdlGpuReadback gameSecondUv;
    FrontierX::SdlGpuReadback gameSpecular;
    const bool gameBaseValid = drawGame() == GR_OK &&
        renderer.VertexCount() == gameVertices.size() &&
        renderer.IndexCount() == kIndices.size() &&
        renderer.Readback64(gameBase, error) && gameBase.structureValid;
    const bool gameSecondUvValid = gameBaseValid &&
        pipeline.set_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX, 1U) == GR_OK &&
        renderer.Readback64(gameSecondUv, error) && gameSecondUv.structureValid &&
        gameSecondUv.checksum != gameBase.checksum;
    for (auto & vertex : gameVertices) vertex.specularBgra = 0x00202020U;
    const bool gameSpecularValid = gameSecondUvValid &&
        pipeline.set_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX, 0U) == GR_OK &&
        pipeline.set_render_state(D3DRS_SPECULARENABLE, 1U) == GR_OK &&
        drawGame() == GR_OK &&
        renderer.Readback64(gameSpecular, error) && gameSpecular.structureValid &&
        gameSpecular.checksum != gameBase.checksum;
    if (!gameSpecularValid ||
        pipeline.set_render_state(D3DRS_SPECULARENABLE, 0U) != GR_OK)
    {
        error = "game vertex UV/specular GPU readback failed: " + error;
        return false;
    }
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
        pipeline.set_texture_stage_state(0U, D3DTSS_COLOROP, 10U /* D3DTOP_SUBTRACT */) ==
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
    FrontierX::SdlGpuReadback linearSampled;
    FrontierX::SdlGpuReadback pointRestored;
    U32 minFilter = 0U;
    const bool samplerValid = textureStageValid &&
        pipeline.set_sampler_state(0U, D3DSAMP_MINFILTER, D3DTEXF_LINEAR) == GR_OK &&
        pipeline.set_sampler_state(0U, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR) == GR_OK &&
        renderer.Readback64(linearSampled, error) && linearSampled.structureValid &&
        linearSampled.checksum != modulated.checksum &&
        pipeline.get_sampler_state(0U, D3DSAMP_MINFILTER, &minFilter) == GR_OK &&
        minFilter == D3DTEXF_LINEAR &&
        pipeline.set_sampler_state(0U, D3DSAMP_MINFILTER, D3DTEXF_POINT) == GR_OK &&
        pipeline.set_sampler_state(0U, D3DSAMP_MAGFILTER, D3DTEXF_POINT) == GR_OK &&
        renderer.Readback64(pointRestored, error) && pointRestored.structureValid &&
        pointRestored.checksum == modulated.checksum &&
        renderer.CurrentSamplerState().minFilter == D3DTEXF_POINT;
    // Combat and large selections alternate two-stage filter/address states.
    // All 32 supported immutable samplers must be shared across stages and
    // reused without losing captured bindings or accepting invalid requests.
    const auto savedSampler = renderer.CurrentSamplerState();
    FrontierX::SamplerStateSnapshot savedSecondSampler{};
    savedSecondSampler.stage = 1U;
    bool samplerCacheValid = samplerValid &&
        pipeline.get_sampler_state(1U, D3DSAMP_ADDRESSU, &savedSecondSampler.addressU) == GR_OK &&
        pipeline.get_sampler_state(1U, D3DSAMP_ADDRESSV, &savedSecondSampler.addressV) == GR_OK &&
        pipeline.get_sampler_state(1U, D3DSAMP_MINFILTER, &savedSecondSampler.minFilter) == GR_OK &&
        pipeline.get_sampler_state(1U, D3DSAMP_MAGFILTER, &savedSecondSampler.magFilter) == GR_OK &&
        pipeline.get_sampler_state(1U, D3DSAMP_MIPFILTER, &savedSecondSampler.mipFilter) == GR_OK;
    auto exerciseSamplerCache = [&]() {
        for (unsigned mask = 0; mask < 32; ++mask)
        {
            FrontierX::SamplerStateSnapshot state{};
            state.addressU = mask & 1U ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
            state.addressV = mask & 2U ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
            state.minFilter = mask & 4U ? D3DTEXF_LINEAR : D3DTEXF_POINT;
            state.magFilter = mask & 8U ? D3DTEXF_LINEAR : D3DTEXF_POINT;
            state.mipFilter = mask & 16U ? D3DTEXF_LINEAR : D3DTEXF_POINT;
            if (renderer.SetSamplerState(state) != GR_OK) return false;
            const U32 count = renderer.SamplerCreations();
            state.stage = 1U;
            if (renderer.SetSamplerState(state) != GR_OK ||
                renderer.SamplerCreations() != count) return false;
        }
        return renderer.SetSamplerState(savedSampler) == GR_OK &&
            renderer.SetSamplerState(savedSecondSampler) == GR_OK;
    };
    samplerCacheValid = samplerCacheValid && exerciseSamplerCache() &&
        renderer.SamplerCreations() == 32U && exerciseSamplerCache() &&
        renderer.SamplerCreations() == 32U;
    for (unsigned field = 0; samplerCacheValid && field < 6; ++field)
    {
        auto invalidSampler = savedSampler;
        if (field == 0) invalidSampler.stage = 2U;
        if (field == 1) invalidSampler.addressU = 0U;
        if (field == 2) invalidSampler.addressV = 0U;
        if (field == 3) invalidSampler.minFilter = 0U;
        if (field == 4) invalidSampler.magFilter = 0U;
        if (field == 5) invalidSampler.mipFilter = 0U;
        samplerCacheValid = renderer.SetSamplerState(invalidSampler) == GR_NOT_IMPLEMENTED &&
            renderer.SamplerCreations() == 32U;
        const auto current = renderer.CurrentSamplerState();
        samplerCacheValid = samplerCacheValid && current.stage == savedSampler.stage &&
            current.addressU == savedSampler.addressU && current.addressV == savedSampler.addressV &&
            current.minFilter == savedSampler.minFilter && current.magFilter == savedSampler.magFilter &&
            current.mipFilter == savedSampler.mipFilter;
    }
    FrontierX::SdlGpuReadback samplerCacheRestored;
    samplerCacheValid = samplerCacheValid && renderer.Readback64(samplerCacheRestored, error) &&
        samplerCacheRestored.checksum == pointRestored.checksum;
    Transform shifted(false);
    for (unsigned row = 0U; row < 3U; ++row)
        for (unsigned column = 0U; column < 3U; ++column)
            shifted.d[row][column] = row == column ? 1.0F : 0.0F;
    shifted.translation.x = 0.25F;
    shifted.translation.y = 0.0F;
    shifted.translation.z = 0.0F;
    FrontierX::SdlGpuReadback shiftedReadback;
    FrontierX::SdlGpuReadback transformRestored;
    Matrix4 reducedProjection(true);
    reducedProjection.d[0][0] = 0.75F;
    FrontierX::SdlGpuReadback projectedReadback;
    const bool shiftedValid = samplerValid &&
        pipeline.set_modelview(shifted) == GR_OK &&
        renderer.Readback64(shiftedReadback, error) &&
        shiftedReadback.rgbValid && shiftedReadback.changedPixels > 0U;
    const auto originalMoment = ImageXMoment(modulated);
    const auto shiftedMoment = ImageXMoment(shiftedReadback);
    const bool displacementObserved = shiftedValid && originalMoment.second != 0U &&
        shiftedMoment.second != 0U &&
        shiftedMoment.first * originalMoment.second >
            originalMoment.first * shiftedMoment.second;
    const bool projectionObserved = displacementObserved &&
        pipeline.set_projection(reducedProjection) == GR_OK &&
        renderer.Readback64(projectedReadback, error) &&
        projectedReadback.rgbValid && projectedReadback.changedPixels > 0U &&
        projectedReadback.checksum != shiftedReadback.checksum;
    shifted.translation.x = 0.0F;
    reducedProjection.d[0][0] = 1.0F;
    const bool resetModelview = pipeline.set_modelview(shifted) == GR_OK;
    const bool resetProjection = pipeline.set_projection(reducedProjection) == GR_OK;
    const bool transformValid = projectionObserved && resetModelview &&
        resetProjection &&
        renderer.Readback64(transformRestored, error) &&
        transformRestored.structureValid &&
        transformRestored.checksum == modulated.checksum;
    FrontierX::SdlGpuReadback clippedReadback;
    FrontierX::SdlGpuReadback viewportRestored;
    const bool viewportValid = transformValid &&
        pipeline.set_viewport(0, 0, 16, 16) == GR_OK &&
        renderer.Readback64(clippedReadback, error) && clippedReadback.rgbValid &&
        clippedReadback.changedPixels > 0U &&
        clippedReadback.changedPixels < modulated.changedPixels &&
        pipeline.set_viewport(0, 0, 64, 64) == GR_OK &&
        renderer.Readback64(viewportRestored, error) &&
        viewportRestored.checksum == modulated.checksum;
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
    IFileSystem * utfFixture = nullptr;
    std::string utfError;
    AGGDESC libraryDescriptor("MaterialLibrary");
    libraryDescriptor.outer = &services;
    IDAComponent * libraryInner = nullptr;
    libraryDescriptor.inner = &libraryInner;
    void * rawLibrary = nullptr;
    IAggregateComponent * libraryAggregate = nullptr;
    IMaterialLibrary * library = nullptr;
    IMaterial * simple = nullptr;
    IMaterialProperties * properties = nullptr;
    U32 materialCount = 0U;
    ITL_TEXTURE_REF_ID materialTexture = ITL_INVALID_REF_ID;
    ITL_TEXTUREFRAME_IRP loadedFrame{};
    FrontierX::SdlGpuReadback loadedReadback;
    FrontierX::SdlGpuReadback loadedRestored;
    const bool productionTextureValid = textureBindingValid &&
        FrontierX::OpenNativeUtfFileSystem(utfFixturePath, &utfFixture, utfError) &&
        utfFixture && textures->load_library(utfFixture, nullptr) == GR_OK &&
        textures->has_texture_id("Tile") == GR_OK &&
        DACOM_Acquire()->CreateInstance(&libraryDescriptor, &rawLibrary) == GR_OK &&
        rawLibrary && libraryInner &&
        libraryInner->QueryInterface(IID_IAggregateComponent,
            reinterpret_cast<void **>(&libraryAggregate)) == GR_OK &&
        libraryAggregate && libraryAggregate->Initialize() == GR_OK &&
        libraryInner->QueryInterface(IID_IMaterialLibrary,
            reinterpret_cast<void **>(&library)) == GR_OK && library &&
        library->load_library(utfFixture) == GR_OK &&
        library->get_material_count(&materialCount) == GR_OK &&
        materialCount == 2U &&
        library->find_material("Other", &simple) == GR_OK && simple &&
        simple->QueryInterface(IID_IMaterialProperties,
            reinterpret_cast<void **>(&properties)) == GR_OK && properties &&
        simple->verify(1U, 1.0F) == GR_OK && simple->update(0.0F) == GR_OK &&
        properties->get_texture(0U, &materialTexture) == GR_OK &&
        materialTexture != ITL_INVALID_REF_ID &&
        textures->get_texture_ref_frame(materialTexture,
            ITL_FRAME_CURRENT, &loadedFrame) == GR_OK &&
        pipeline.is_texture(loadedFrame.rp_texture_id) == GR_OK &&
        materialStates->set_texture_stage_texture(0U,
            loadedFrame.rp_texture_id) == GR_OK &&
        material->apply() == GR_OK &&
        renderer.Readback64(loadedReadback, error) && loadedReadback.structureValid &&
        loadedReadback.changedPixels == modulated.changedPixels &&
        SameForegroundMask(loadedReadback, modulated) &&
        loadedReadback.checksum != modulated.checksum &&
        materialStates->set_texture_stage_texture(0U,
            renderer.FixtureTextureHandle()) == GR_OK &&
        material->apply() == GR_OK &&
        renderer.Readback64(loadedRestored, error) &&
        loadedRestored.checksum == modulated.checksum;
    loadedChecksum = loadedReadback.checksum;
    if (materialTexture != ITL_INVALID_REF_ID)
        textures->release_texture_ref(materialTexture);
    if (properties) properties->Release();
    if (simple) simple->Release();
    const bool materialReleased = !library || library->free_library() == GR_OK;
    if (library) library->Release();
    if (libraryAggregate) libraryAggregate->Release();
    if (libraryInner) libraryInner->Release();
    const bool loadedReleased = materialReleased &&
        textures->free_library(TRUE) == GR_OK &&
        (!loadedFrame.rp_texture_id ||
         pipeline.is_texture(loadedFrame.rp_texture_id) != GR_OK);
    if (utfFixture) utfFixture->Release();
    // Terranshp01 uses nine authored palette levels; exercise the real loader
    // and prove minification samples a lower level rather than base-only data.
    const auto mipPath = std::filesystem::path(utfFixturePath.string() + ".mips");
    IFileSystem *mipFixture = nullptr;
    ITL_TEXTURE_ID mipID = ITL_INVALID_ID;
    ITL_TEXTUREFRAME_IRP mipFrame{};
    U32 mipWidth = 0, mipHeight = 0, mipCount = 0;
    PixelFormat opaqueMipFormat;
    RPLOCKDATA mipLock{};
    FrontierX::SdlGpuReadback mipBefore, mipChanged, mipRestored;
    const bool mipLoaded = FrontierXTest::WriteMipTextureFixture(mipPath) &&
        FrontierX::OpenNativeUtfFileSystem(mipPath, &mipFixture, utfError) &&
        textures->load_library(mipFixture, nullptr) == GR_OK &&
        textures->has_texture_id("MipTile") == GR_OK &&
        textures->get_texture_id("MipTile", &mipID) == GR_OK &&
        textures->get_texture_frame(mipID, 0, &mipFrame) == GR_OK &&
        pipeline.get_texture_dim(mipFrame.rp_texture_id, &mipWidth, &mipHeight, &mipCount) == GR_OK &&
        mipWidth == 256 && mipHeight == 256 && mipCount == 9 &&
        pipeline.get_texture_format(mipFrame.rp_texture_id, &opaqueMipFormat) == GR_OK &&
        !opaqueMipFormat.has_alpha_channel() &&
        materialStates->set_texture_stage_texture(0U, mipFrame.rp_texture_id) == GR_OK &&
        material->apply() == GR_OK && renderer.Readback64(mipBefore, error) &&
        mipBefore.changedPixels > 0 &&
        pipeline.lock_texture(mipFrame.rp_texture_id, 2, &mipLock) == GR_OK &&
        mipLock.width == 64 && mipLock.height == 64 && mipLock.pitch == 256 &&
        static_cast<const unsigned char *>(mipLock.pixels)[0] == 40 &&
        static_cast<const unsigned char *>(mipLock.pixels)[1] == 80 &&
        static_cast<const unsigned char *>(mipLock.pixels)[2] == 240;
    bool mipSampled = false;
    if (mipLoaded)
    {
        auto *pixels = static_cast<unsigned char *>(mipLock.pixels);
        for (U32 row = 0; row < mipLock.height; ++row)
            for (U32 column = 0; column < mipLock.width; ++column)
            {
                auto *pixel = pixels + row * mipLock.pitch + column * 4;
                pixel[0] = 240; pixel[1] = 240; pixel[2] = 40; pixel[3] = 255;
            }
        mipSampled = pipeline.unlock_texture(mipFrame.rp_texture_id, 2) == GR_OK &&
            material->apply() == GR_OK && renderer.Readback64(mipChanged, error) &&
            mipChanged.checksum != mipBefore.checksum &&
            pipeline.lock_texture(mipFrame.rp_texture_id, 2, &mipLock) == GR_OK;
        if (mipSampled)
        {
            pixels = static_cast<unsigned char *>(mipLock.pixels);
            for (U32 row = 0; row < mipLock.height; ++row)
                for (U32 column = 0; column < mipLock.width; ++column)
                {
                    auto *pixel = pixels + row * mipLock.pitch + column * 4;
                    pixel[0] = 40; pixel[1] = 80; pixel[2] = 240; pixel[3] = 255;
                }
            mipSampled = pipeline.unlock_texture(mipFrame.rp_texture_id, 2) == GR_OK &&
                material->apply() == GR_OK && renderer.Readback64(mipRestored, error) &&
                mipRestored.checksum == mipBefore.checksum &&
                pipeline.lock_texture(mipFrame.rp_texture_id, 9, &mipLock) == GR_INVALID_PARMS;
        }
    }
    const bool mipUnbound = materialStates->set_texture_stage_texture(0U,
        renderer.FixtureTextureHandle()) == GR_OK && material->apply() == GR_OK;
    const bool mipReferenceReleased = mipID == ITL_INVALID_ID ||
        textures->release_texture_id(mipID) == GR_OK;
    const bool mipReleased = mipUnbound && mipReferenceReleased &&
        textures->free_library(TRUE) == GR_OK &&
        (!mipFrame.rp_texture_id || pipeline.is_texture(mipFrame.rp_texture_id) != GR_OK);
    if (mipFixture) mipFixture->Release();
    const bool mipValid = mipLoaded && mipSampled && mipReleased;
    if (mipValid) std::cout << "Production palette texture loads nine authored mips and samples/restores lower level\n";
    // Initial Terran platforms use independent Alpha 8 bit planes at every
    // palette mip. Asset loading alone cannot prove their visible blending.
    const auto alphaMipPath = std::filesystem::path(utfFixturePath.string() + ".alpha-mips");
    IFileSystem *alphaMipFixture = nullptr;
    ITL_TEXTURE_ID alphaMipID = ITL_INVALID_ID;
    ITL_TEXTUREFRAME_IRP alphaMipFrame{};
    PixelFormat alphaMipFormat;
    bool alphaMipValid = FrontierXTest::WriteMipTextureFixture(alphaMipPath, true) &&
        FrontierX::OpenNativeUtfFileSystem(alphaMipPath, &alphaMipFixture, utfError) &&
        textures->load_library(alphaMipFixture, nullptr) == GR_OK &&
        textures->get_texture_id("AlphaMipTile", &alphaMipID) == GR_OK &&
        textures->get_texture_frame(alphaMipID, 0, &alphaMipFrame) == GR_OK &&
        pipeline.get_texture_format(alphaMipFrame.rp_texture_id, &alphaMipFormat) == GR_OK &&
        alphaMipFormat.has_alpha_channel();
    constexpr unsigned char mipColors[9][3] = {{240,40,80},{40,240,80},{40,80,240},
        {200,160,40},{160,40,200},{40,200,160},{120,80,40},{80,40,120},{40,120,80}};
    for (U32 level = 0U; alphaMipValid && level < 9U; ++level)
    {
        RPLOCKDATA lock{};
        alphaMipValid = pipeline.lock_texture(alphaMipFrame.rp_texture_id, level, &lock) == GR_OK &&
            lock.width == (256U >> level) && lock.height == lock.width;
        if (alphaMipValid)
        {
            const auto *pixels = static_cast<const unsigned char *>(lock.pixels);
            for (U32 index = 0U; index < std::min(3U, lock.width * lock.height); ++index)
                alphaMipValid &= pixels[index * 4U] == mipColors[level][0] &&
                    pixels[index * 4U + 1U] == mipColors[level][1] &&
                    pixels[index * 4U + 2U] == mipColors[level][2] &&
                    pixels[index * 4U + 3U] == (index == 0U ? 0U : index == 1U ? 96U : 255U);
            alphaMipValid &= pipeline.unlock_texture(alphaMipFrame.rp_texture_id, level) == GR_OK;
        }
    }
    U32 savedBlend = 0U, savedSourceBlend = 0U, savedDestBlend = 0U;
    FrontierX::SdlGpuReadback alphaMipBefore, alphaMipOpaque, alphaMipRestored;
    alphaMipValid = alphaMipValid &&
        pipeline.get_render_state(D3DRS_ALPHABLENDENABLE, &savedBlend) == GR_OK &&
        pipeline.get_render_state(D3DRS_SRCBLEND, &savedSourceBlend) == GR_OK &&
        pipeline.get_render_state(D3DRS_DESTBLEND, &savedDestBlend) == GR_OK &&
        materialStates->set_render_state(D3DRS_ALPHABLENDENABLE, TRUE) == GR_OK &&
        materialStates->set_render_state(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA) == GR_OK &&
        materialStates->set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA) == GR_OK &&
        materialStates->set_texture_stage_texture(0U, alphaMipFrame.rp_texture_id) == GR_OK &&
        material->apply() == GR_OK && renderer.Readback64(alphaMipBefore, error);
    std::vector<unsigned char> savedAlphaPixels;
    RPLOCKDATA alphaLock{};
    if (alphaMipValid && pipeline.lock_texture(alphaMipFrame.rp_texture_id, 2U, &alphaLock) == GR_OK)
    {
        auto *pixels = static_cast<unsigned char *>(alphaLock.pixels);
        savedAlphaPixels.assign(pixels, pixels + alphaLock.pitch * alphaLock.height);
        for (U32 row = 0U; row < alphaLock.height; ++row)
            for (U32 column = 0U; column < alphaLock.width; ++column)
                pixels[row * alphaLock.pitch + column * 4U + 3U] = 255U;
        alphaMipValid = pipeline.unlock_texture(alphaMipFrame.rp_texture_id, 2U) == GR_OK &&
            material->apply() == GR_OK && renderer.Readback64(alphaMipOpaque, error) &&
            alphaMipOpaque.checksum != alphaMipBefore.checksum &&
            pipeline.lock_texture(alphaMipFrame.rp_texture_id, 2U, &alphaLock) == GR_OK;
        if (alphaMipValid)
        {
            std::memcpy(alphaLock.pixels, savedAlphaPixels.data(), savedAlphaPixels.size());
            alphaMipValid = pipeline.unlock_texture(alphaMipFrame.rp_texture_id, 2U) == GR_OK &&
                material->apply() == GR_OK && renderer.Readback64(alphaMipRestored, error) &&
                alphaMipRestored.checksum == alphaMipBefore.checksum;
        }
    }
    else alphaMipValid = false;
    alphaMipValid = materialStates->set_render_state(D3DRS_ALPHABLENDENABLE, savedBlend) == GR_OK &&
        materialStates->set_render_state(D3DRS_SRCBLEND, savedSourceBlend) == GR_OK &&
        materialStates->set_render_state(D3DRS_DESTBLEND, savedDestBlend) == GR_OK &&
        materialStates->set_texture_stage_texture(0U, renderer.FixtureTextureHandle()) == GR_OK &&
        material->apply() == GR_OK && alphaMipValid;
    if (alphaMipID != ITL_INVALID_ID) alphaMipValid &= textures->release_texture_id(alphaMipID) == GR_OK;
    alphaMipValid &= textures->free_library(TRUE) == GR_OK;
    if (alphaMipFixture) alphaMipFixture->Release();
    for (const int delta : {-4096, -1, 1})
    {
        const auto badPath = std::filesystem::path(alphaMipPath.string() + ".bad" + std::to_string(delta));
        IFileSystem *badFixture = nullptr;
        const bool opened = FrontierXTest::WriteMipTextureFixture(badPath, true, delta) &&
            FrontierX::OpenNativeUtfFileSystem(badPath, &badFixture, utfError);
        const GENRESULT loaded = opened ? textures->load_library(badFixture, nullptr) : GR_GENERIC;
        // get_texture_id intentionally creates an unresolved name placeholder;
        // inspect publication before invoking that original API side effect.
        const GENRESULT exists = textures->has_texture_id("AlphaMipTile");
        const GENRESULT released = textures->free_library(TRUE);
        // load_library reports discovery of its directory; individual failed
        // textures must remain unpublished despite that historical return value.
        const bool rejected = opened && loaded == GR_OK &&
            exists != GR_OK && released == GR_OK;
        if (!rejected) std::cerr << "malformed mip alpha delta=" << delta << " opened=" << opened
            << " load=" << loaded << " exists=" << exists
            << " release=" << released << '\n';
        if (badFixture) badFixture->Release();
        alphaMipValid &= rejected;
    }
    if (alphaMipValid) std::cout << "Production palette alpha planes preserve nine mip RGBA levels and visible blending/restoration\n";
    // TM4's BlackHole ring uses a single authored RGB888 mip and an independent
    // alpha plane. Exercise that original UTF format through visible blending.
    const auto rgb888Path = std::filesystem::path(utfFixturePath.string() + ".rgb888");
    IFileSystem *rgb888Fixture = nullptr;
    ITL_TEXTURE_ID rgb888ID = ITL_INVALID_ID;
    ITL_TEXTUREFRAME_IRP rgb888Frame{};
    PixelFormat rgb888Format;
    RPLOCKDATA rgb888Lock{};
    FrontierX::SdlGpuReadback rgb888Before, rgb888Reference, rgb888Opaque, rgb888Restored;
    U32 rgb888ReferenceHandle = 0;
    const std::array<unsigned char, 16> rgb888Expected{{17,34,51,0, 68,85,102,96,
                                                     119,136,153,255, 170,187,204,0}};
    bool rgb888Valid = FrontierXTest::WriteMipTextureFixture(rgb888Path, true, 0, true) &&
        FrontierX::OpenNativeUtfFileSystem(rgb888Path, &rgb888Fixture, utfError) &&
        textures->load_library(rgb888Fixture, nullptr) == GR_OK &&
        textures->get_texture_id("Rgb888Tile", &rgb888ID) == GR_OK &&
        textures->get_texture_frame(rgb888ID, 0, &rgb888Frame) == GR_OK &&
        pipeline.get_texture_format(rgb888Frame.rp_texture_id, &rgb888Format) == GR_OK &&
        rgb888Format.has_alpha_channel() &&
        pipeline.lock_texture(rgb888Frame.rp_texture_id, 0, &rgb888Lock) == GR_OK;
    const bool rgb888Loaded = rgb888Valid;
    if (rgb888Valid)
    {
        rgb888Valid = rgb888Lock.width == 2 && rgb888Lock.height == 2 && rgb888Lock.pitch == 8 &&
            std::memcmp(rgb888Lock.pixels, rgb888Expected.data(), rgb888Expected.size()) == 0;
        rgb888Valid = pipeline.unlock_texture(rgb888Frame.rp_texture_id, 0) == GR_OK && rgb888Valid;
    }
    const bool rgb888Uploaded = rgb888Valid;
    rgb888Valid = rgb888Valid &&
        materialStates->set_render_state(D3DRS_ALPHABLENDENABLE, TRUE) == GR_OK &&
        materialStates->set_render_state(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA) == GR_OK &&
        materialStates->set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA) == GR_OK &&
        materialStates->set_texture_stage_texture(0, rgb888Frame.rp_texture_id) == GR_OK &&
        material->apply() == GR_OK && renderer.Readback64(rgb888Before, error) &&
        rgb888Before.rgbValid && rgb888Before.changedPixels > 0;
    // structureValid describes the original opaque smoke fixture's mask. The
    // authored transparent ring instead must match an independent RGBA8 image
    // at every readback pixel under exactly the same geometry and states.
    rgb888Valid = rgb888Valid &&
        pipeline.create_texture(2, 2, rgbaFormat, 0, 0, rgb888ReferenceHandle) == GR_OK &&
        pipeline.set_texture_level_data(rgb888ReferenceHandle, 0, 2, 2, 8, rgbaFormat,
            rgb888Expected.data(), nullptr, nullptr) == GR_OK &&
        materialStates->set_texture_stage_texture(0, rgb888ReferenceHandle) == GR_OK &&
        material->apply() == GR_OK && renderer.Readback64(rgb888Reference, error) &&
        rgb888Reference.rgbValid && rgb888Reference.rgb8 == rgb888Before.rgb8 &&
        materialStates->set_texture_stage_texture(0, rgb888Frame.rp_texture_id) == GR_OK &&
        pipeline.lock_texture(rgb888Frame.rp_texture_id, 0, &rgb888Lock) == GR_OK;
    const bool rgb888Drawn = rgb888Valid;
    if (rgb888Valid)
    {
        auto *pixels = static_cast<unsigned char *>(rgb888Lock.pixels);
        for (unsigned i = 3; i < 16; i += 4) pixels[i] = 255;
        rgb888Valid = pipeline.unlock_texture(rgb888Frame.rp_texture_id, 0) == GR_OK &&
            material->apply() == GR_OK && renderer.Readback64(rgb888Opaque, error) &&
            rgb888Opaque.checksum != rgb888Before.checksum &&
            pipeline.lock_texture(rgb888Frame.rp_texture_id, 0, &rgb888Lock) == GR_OK;
        if (rgb888Valid)
        {
            std::memcpy(rgb888Lock.pixels, rgb888Expected.data(), rgb888Expected.size());
            rgb888Valid = pipeline.unlock_texture(rgb888Frame.rp_texture_id, 0) == GR_OK &&
                material->apply() == GR_OK && renderer.Readback64(rgb888Restored, error) &&
                rgb888Restored.rgbValid && rgb888Restored.rgb8 == rgb888Before.rgb8 &&
                rgb888Restored.checksum == rgb888Before.checksum;
        }
    }
    rgb888Valid = materialStates->set_render_state(D3DRS_ALPHABLENDENABLE, savedBlend) == GR_OK &&
        materialStates->set_render_state(D3DRS_SRCBLEND, savedSourceBlend) == GR_OK &&
        materialStates->set_render_state(D3DRS_DESTBLEND, savedDestBlend) == GR_OK &&
        materialStates->set_texture_stage_texture(0, renderer.FixtureTextureHandle()) == GR_OK &&
        material->apply() == GR_OK && rgb888Valid;
    if (rgb888ReferenceHandle)
        rgb888Valid &= pipeline.destroy_texture(rgb888ReferenceHandle) == GR_OK;
    if (rgb888ID != ITL_INVALID_ID) rgb888Valid &= textures->release_texture_id(rgb888ID) == GR_OK;
    rgb888Valid &= textures->free_library(TRUE) == GR_OK &&
        (!rgb888Frame.rp_texture_id || pipeline.is_texture(rgb888Frame.rp_texture_id) != GR_OK);
    if (rgb888Fixture) rgb888Fixture->Release();
    if (rgb888Valid) std::cout << "Production RGB888 ring texture preserves RGB/alpha and visible blending/restoration\n";
    else std::cerr << "RGB888 diagnostic: loaded=" << rgb888Loaded << " uploaded=" << rgb888Uploaded
        << " drawn=" << rgb888Drawn << " handle=" << rgb888Frame.rp_texture_id
        << " dims=" << rgb888Lock.width << 'x' << rgb888Lock.height << " pitch=" << rgb888Lock.pitch
        << " alpha=" << rgb888Format.has_alpha_channel()
        << " before=" << rgb888Before.checksum << " reference=" << rgb888Reference.checksum
        << " opaque=" << rgb888Opaque.checksum
        << " restored=" << rgb888Restored.checksum << " pixels=" << rgb888Before.changedPixels
        << " structure=" << rgb888Before.structureValid << " error=" << error << '\n';
    FrontierX::SdlGpuReadback firstFrameDraw;
    FrontierX::SdlGpuReadback twoFrameDraws;
    Transform left(false);
    for (unsigned row = 0U; row < 3U; ++row)
        for (unsigned column = 0U; column < 3U; ++column)
            left.d[row][column] = row == column ? 1.0F : 0.0F;
    left.translation.x = -0.35F;
    left.translation.y = 0.0F;
    left.translation.z = 0.0F;
    Transform right = left;
    right.translation.x = 0.35F;
    const bool frameDrawsValid = loadedReleased &&
        FrontierX::ConfigureRenderSmokeState(&pipeline) == GR_OK &&
        pipeline.set_texture_stage_texture(0U,
            renderer.FixtureTextureHandle()) == GR_OK &&
        pipeline.set_projection(Matrix4(true)) == GR_OK &&
        pipeline.set_viewport(0, 0, 64, 64) == GR_OK &&
        renderer.BeginFrame(error) &&
        pipeline.set_modelview(left) == GR_OK &&
        FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
        renderer.FrameSubmissionCount() == 1U &&
        renderer.Readback64(firstFrameDraw, error) &&
        pipeline.set_modelview(right) == GR_OK &&
        FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
        renderer.FrameSubmissionCount() == 2U &&
        renderer.Readback64(twoFrameDraws, error) &&
        twoFrameDraws.changedPixels > firstFrameDraw.changedPixels &&
        twoFrameDraws.checksum != firstFrameDraw.checksum;
    // FogOfWar updates its sampled map texture after the original frame draws.
    const std::array<std::uint8_t, 16> mapRed{{255,0,0,255,255,0,0,255,
                                             255,0,0,255,255,0,0,255}};
    const std::array<std::uint8_t, 16> mapGreen{{0,255,0,255,0,255,0,255,
                                               0,255,0,255,0,255,0,255}};
    U32 mapHandle = 0U;
    FrontierX::SdlGpuReadback mapBefore, mapCaptured, mapNext;
    const bool mapReplacementValid = frameDrawsValid &&
        renderer.CreateTexture(2U, 2U, mapHandle) == GR_OK &&
        renderer.ReplaceTexture(mapHandle, {mapRed.data(), 2U, 2U, mapRed.size()}) == GR_OK &&
        renderer.BeginFrame(error) &&
        pipeline.set_texture_stage_texture(0U, mapHandle) == GR_OK &&
        pipeline.set_modelview(left) == GR_OK &&
        FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
        renderer.Readback64(mapBefore, error) && mapBefore.changedPixels > 0U &&
        renderer.ReplaceTexture(mapHandle, {mapGreen.data(), 2U, 2U, mapGreen.size()}) == GR_OK &&
        renderer.Readback64(mapCaptured, error) &&
        mapCaptured.checksum == mapBefore.checksum &&
        renderer.BeginFrame(error) &&
        FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
        renderer.Readback64(mapNext, error) && mapNext.changedPixels > 0U &&
        mapNext.checksum != mapBefore.checksum;
    // Repeated minimap/movie updates must preserve captured storage. Exercise
    // transfer release and replacement before any readback, while the current
    // captured draw borrows the original texture.
    bool mapQueuedReplacementValid = mapReplacementValid;
    for (unsigned update = 0; mapQueuedReplacementValid && update < 23U; ++update)
    {
        const auto &pixels = (update & 1U) ? mapGreen : mapRed;
        mapQueuedReplacementValid = renderer.ReplaceTexture(mapHandle,
            {pixels.data(), 2U, 2U, pixels.size()}) == GR_OK;
    }
    FrontierX::SdlGpuReadback mapQueuedCaptured, mapQueuedNext;
    mapQueuedReplacementValid = mapQueuedReplacementValid &&
        renderer.Readback64(mapQueuedCaptured, error) &&
        mapQueuedCaptured.checksum == mapNext.checksum &&
        renderer.BeginFrame(error) &&
        FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
        renderer.Readback64(mapQueuedNext, error) &&
        mapQueuedNext.checksum == mapBefore.checksum;
    const bool mapReplacementReleased = mapQueuedReplacementValid && renderer.BeginFrame(error) &&
        pipeline.set_texture_stage_texture(0U, renderer.FixtureTextureHandle()) == GR_OK &&
        (!mapHandle || renderer.DestroyTexture(mapHandle) == GR_OK);
    if (mapReplacementValid && mapReplacementReleased)
        std::cout << "Captured map texture replacement preserves current frame and applies queued uploads in order\n";
    // MovieScreen releases its last sampled texture after unbinding, while
    // presentation/readback can still replay that captured frame. Keep bound
    // and stale rejection, and require the real GPU image to survive retirement.
    U32 capturedHandle = 0U;
    FrontierX::SdlGpuReadback capturedBeforeDestroy, capturedAfterDestroy, capturedAfterPresent, afterRetirement;
    bool capturedDestroyValid = mapReplacementReleased &&
        renderer.CreateTexture(2U, 2U, capturedHandle) == GR_OK &&
        renderer.ReplaceTexture(capturedHandle, {mapGreen.data(), 2U, 2U, mapGreen.size()}) == GR_OK &&
        renderer.BeginFrame(error) && pipeline.set_texture_stage_texture(0U, capturedHandle) == GR_OK &&
        pipeline.set_modelview(left) == GR_OK && FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
        renderer.Readback64(capturedBeforeDestroy, error) && capturedBeforeDestroy.changedPixels > 0U &&
        renderer.DestroyTexture(capturedHandle) == GR_INVALID_PARMS &&
        pipeline.set_texture_stage_texture(0U, renderer.FixtureTextureHandle()) == GR_OK &&
        renderer.DestroyTexture(capturedHandle) == GR_OK &&
        pipeline.set_texture_stage_texture(0U, capturedHandle) == GR_NOT_IMPLEMENTED &&
        renderer.DestroyTexture(capturedHandle) == GR_INVALID_PARMS &&
        renderer.Readback64(capturedAfterDestroy, error) &&
        capturedAfterDestroy.checksum == capturedBeforeDestroy.checksum;
    bool capturedPresented = false;
    for (int attempt = 0; capturedDestroyValid && !capturedPresented && attempt < 10; ++attempt)
    {
        capturedDestroyValid = renderer.Present(capturedPresented, error);
        if (!capturedPresented) SDL_Delay(10U);
    }
    capturedDestroyValid = capturedDestroyValid && capturedPresented &&
        renderer.Readback64(capturedAfterPresent, error) &&
        capturedAfterPresent.checksum == capturedBeforeDestroy.checksum &&
        renderer.BeginFrame(error) && FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
        renderer.Readback64(afterRetirement, error) && afterRetirement.checksum == firstFrameDraw.checksum;
    if (capturedDestroyValid)
        std::cout << "Captured movie texture release preserves GPU image/presentation and retires on next frame\n";
    int outputWidth = 0;
    int outputHeight = 0;
    const bool windowViewportRestored =
        SDL_GetWindowSizeInPixels(window, &outputWidth, &outputHeight) &&
        outputWidth > 0 && outputHeight > 0 &&
        pipeline.set_viewport(0, 0, outputWidth, outputHeight) == GR_OK;
    // Exercise the production modal frame contract, including clear-only frames
    // and recovery from unsupported partial clears, in this existing GPU probe.
    FrontierX::SdlGpuReadback emptyBlack, emptyRed, clearedDepth, lifecycleDraw;
    const U32 clearBoth = RP_CLEAR_COLOR_BIT | RP_CLEAR_DEPTH_BIT;
    const bool lifecycleValid = windowViewportRestored &&
        pipeline.BindFrames({&renderer, FrontierX::SdlGpuRenderer::ClearFrameCallback,
                             FrontierX::SdlGpuRenderer::PresentFrameCallback}) == GR_OK &&
        pipeline.clear_buffers(clearBoth, nullptr) == GR_OK &&
        pipeline.begin_scene() == GR_OK && pipeline.end_scene() == GR_OK &&
        renderer.Readback64(emptyBlack, error) && pipeline.swap_buffers() == GR_OK &&
        pipeline.set_pipeline_state(RP_CLEAR_COLOR, 0xffff0000U) == GR_OK &&
        pipeline.clear_buffers(clearBoth, nullptr) == GR_OK &&
        pipeline.begin_scene() == GR_OK && pipeline.end_scene() == GR_OK &&
        renderer.Readback64(emptyRed, error) && pipeline.swap_buffers() == GR_OK &&
        emptyBlack.checksum != emptyRed.checksum &&
        emptyBlack.rgbValid && emptyRed.rgbValid &&
        emptyBlack.rgb8[0] == 0U && emptyBlack.rgb8[1] == 0U && emptyBlack.rgb8[2] == 0U &&
        emptyRed.rgb8[0] == 255U && emptyRed.rgb8[1] == 0U && emptyRed.rgb8[2] == 0U &&
        pipeline.set_pipeline_state(RP_CLEAR_DEPTH, 0U) == GR_OK &&
        pipeline.clear_buffers(clearBoth, nullptr) == GR_OK &&
        pipeline.begin_scene() == GR_OK &&
        pipeline.set_viewport(0, 0, 64, 64) == GR_OK &&
        FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
        pipeline.end_scene() == GR_OK && renderer.Readback64(clearedDepth, error) &&
        clearedDepth.checksum == emptyRed.checksum && pipeline.swap_buffers() == GR_OK &&
        pipeline.set_viewport(0, 0, outputWidth, outputHeight) == GR_OK &&
        pipeline.set_pipeline_state(RP_CLEAR_DEPTH, 0xffffffffU) == GR_OK &&
        pipeline.clear_buffers(RP_CLEAR_DEPTH_BIT, nullptr) == GR_NOT_IMPLEMENTED &&
        pipeline.set_pipeline_state(RP_CLEAR_COLOR, 0xff060911U) == GR_OK &&
        pipeline.clear_buffers(clearBoth, nullptr) == GR_OK &&
        pipeline.begin_scene() == GR_OK &&
        pipeline.set_viewport(0, 0, 64, 64) == GR_OK &&
        pipeline.set_modelview(left) == GR_OK &&
        FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
        pipeline.set_modelview(right) == GR_OK &&
        FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
        renderer.FrameSubmissionCount() == 2U &&
        pipeline.end_scene() == GR_OK &&
        renderer.Readback64(lifecycleDraw, error) && lifecycleDraw.changedPixels > 0 &&
        pipeline.swap_buffers() == GR_OK;
    // Regression: mission material changes repeatedly revisit fixed GPU states.
    // This existing production submission also checks that cache pressure never
    // destroys a pipeline borrowed by the current captured frame.
    FrontierX::SdlGpuReadback cacheBefore, cacheAfter, cacheNext;
    auto cacheStep = [&](const char * name, GENRESULT result)
    {
        if (result != GR_OK)
            std::cerr << "GPU cache baseline " << name << " failed: " << result
                      << " lifecycle=" << lifecycleValid << " error=" << error
                      << " renderer=" << renderer.LastError() << '\n';
        return result == GR_OK;
    };
    bool pipelineCacheValid =
        cacheStep("full-target viewport", pipeline.set_viewport(0, 0, outputWidth, outputHeight)) &&
        cacheStep("clear", pipeline.clear_buffers(clearBoth, nullptr)) &&
        cacheStep("begin", pipeline.begin_scene()) &&
        cacheStep("viewport", pipeline.set_viewport(0, 0, 64, 64)) &&
        cacheStep("modelview", pipeline.set_modelview(left)) &&
        cacheStep("submit", FrontierX::SubmitRenderSmokeScene(&pipeline)) &&
        renderer.Readback64(cacheBefore, error);
    const auto cacheState = renderer.CurrentRenderState();
    auto alternateState = cacheState;
    alternateState.cullMode = cacheState.cullMode == D3DCULL_NONE ? D3DCULL_CW : D3DCULL_NONE;
    alternateState.textureFactor = 0xff123456U;
    alternateState.alphaTestEnabled = !cacheState.alphaTestEnabled;
    pipelineCacheValid = pipelineCacheValid &&
        renderer.SetRenderState(alternateState) == GR_OK;
    const U32 warmedRebuilds = renderer.PipelineRebuilds();
    pipelineCacheValid = pipelineCacheValid && renderer.SetRenderState(cacheState) == GR_OK &&
        renderer.PipelineRebuilds() == warmedRebuilds &&
        renderer.CurrentRenderState().textureFactor == cacheState.textureFactor &&
        renderer.CurrentRenderState().alphaTestEnabled == cacheState.alphaTestEnabled;
    if (!pipelineCacheValid)
        std::cerr << "GPU cache warm/reuse failed: rebuilds=" << renderer.PipelineRebuilds()
                  << " expected=" << warmedRebuilds << '\n';
    auto pressureState = [&](U32 index)
    {
        auto state = cacheState;
        state.depthEnabled = index & 1U;
        state.depthWriteEnabled = (index >> 1U) & 1U;
        state.depthFunction = D3DCMP_NEVER + ((index >> 2U) & 7U);
        state.alphaBlendEnabled = (index >> 5U) & 1U;
        state.cullMode = D3DCULL_NONE + (index >> 6U);
        return state;
    };
    for (U32 index = 0U; pipelineCacheValid && index < 80U; ++index)
        pipelineCacheValid = renderer.SetRenderState(pressureState(index)) == GR_OK;
    pipelineCacheValid = pipelineCacheValid && renderer.SetRenderState(cacheState) == GR_OK;
    const U32 pressureRebuilds = renderer.PipelineRebuilds();
    auto rejectedState = cacheState;
    rejectedState.depthFunction = 0U;
    pipelineCacheValid = pipelineCacheValid &&
        renderer.SetRenderState(rejectedState) == GR_INVALID_PARMS &&
        renderer.PipelineRebuilds() == pressureRebuilds &&
        renderer.CurrentRenderState().depthFunction == cacheState.depthFunction &&
        renderer.Readback64(cacheAfter, error) && cacheAfter.checksum == cacheBefore.checksum &&
        pipeline.end_scene() == GR_OK && pipeline.swap_buffers() == GR_OK &&
        pipeline.set_viewport(0, 0, outputWidth, outputHeight) == GR_OK &&
        pipeline.clear_buffers(clearBoth, nullptr) == GR_OK && pipeline.begin_scene() == GR_OK;
    if (!pipelineCacheValid)
        std::cerr << "GPU cache pressure/captured readback failed: before=" << cacheBefore.checksum
                  << " after=" << cacheAfter.checksum << '\n';
    const U32 afterTrimRebuilds = renderer.PipelineRebuilds();
    pipelineCacheValid = pipelineCacheValid &&
        renderer.SetRenderState(pressureState(0U)) == GR_OK &&
        renderer.PipelineRebuilds() == afterTrimRebuilds + 1U &&
        renderer.SetRenderState(cacheState) == GR_OK &&
        renderer.PipelineRebuilds() == afterTrimRebuilds + 1U &&
        pipeline.set_viewport(0, 0, 64, 64) == GR_OK && pipeline.set_modelview(left) == GR_OK &&
        FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK && pipeline.end_scene() == GR_OK &&
        renderer.Readback64(cacheNext, error) && cacheNext.checksum == cacheBefore.checksum &&
        pipeline.swap_buffers() == GR_OK;
    if (pipelineCacheValid)
        std::cout << "GPU pipeline cache: state reuse, uniform restore, 80-state captured pressure, idle eviction/regrowth and equal readback\n";
    else
        std::cerr << "GPU cache eviction/regrowth/next frame failed: rebuilds=" << renderer.PipelineRebuilds()
                  << " before-trim=" << pressureRebuilds << " after-trim=" << afterTrimRebuilds
                  << " before=" << cacheBefore.checksum << " next=" << cacheNext.checksum
                  << " active depth=" << cacheState.depthEnabled << ',' << cacheState.depthWriteEnabled
                  << ',' << cacheState.depthFunction << " blend=" << cacheState.alphaBlendEnabled
                  << " cull=" << cacheState.cullMode << '\n';
    // The mission repeatedly uploads changing meshes and HUD geometry. Reuse
    // must preserve mixed indexed/direct draws, growth and shrink after idle;
    // the existing map/movie checks above also protect captured texture lifetime.
    renderer.EnableTimings(true);
    const auto reuseBefore = renderer.GetTimings().reusedUploads;
    std::vector<RPVertex> grownVertices(legacyVertices.begin(), legacyVertices.end());
    grownVertices.insert(grownVertices.end(), legacyVertices.begin(), legacyVertices.end());
    FrontierX::SdlGpuReadback pooledImages[3];
    bool uploadPoolValid = pipelineCacheValid;
    // A large selection changes the mix of draws. Obsolete indexed entries
    // must not evict the active direct buffers just retired into the pool.
    // Four hundred draws also exceed the former 256-entry bring-up limit.
    FrontierX::SdlGpuReadback selectionImages[2];
    for (unsigned frame = 0; uploadPoolValid && frame < 3U; ++frame)
    {
        uploadPoolValid = renderer.BeginFrame(error) &&
            FrontierX::ConfigureRenderSmokeState(&pipeline) == GR_OK &&
            pipeline.set_texture_stage_texture(0U, renderer.FixtureTextureHandle()) == GR_OK;
        const auto reused = renderer.GetTimings().reusedUploads;
        for (unsigned draw = 0; uploadPoolValid && draw < 400U; ++draw)
            uploadPoolValid = (frame == 0U ?
                pipeline.draw_indexed_primitive(D3DPT_TRIANGLELIST, FrontierX::LegacyD3d9::kRpVertexFvf,
                    legacyVertices.data(), static_cast<int>(legacyVertices.size()),
                    kIndices.data(), static_cast<int>(kIndices.size()), 0U) :
                FrontierX::SubmitRenderSmokeScene(&pipeline)) == GR_OK;
        uploadPoolValid = uploadPoolValid && renderer.FrameSubmissionCount() == 400U;
        if (frame)
            uploadPoolValid = uploadPoolValid && renderer.Readback64(selectionImages[frame - 1U], error);
        if (frame == 2U)
            uploadPoolValid = uploadPoolValid &&
                renderer.GetTimings().reusedUploads - reused == 400U &&
                selectionImages[0].checksum == selectionImages[1].checksum;
    }
    for (unsigned frame = 0; uploadPoolValid && frame < 3; ++frame)
    {
        const auto * vertices = frame == 1 ? grownVertices.data() : legacyVertices.data();
        const int count = static_cast<int>(frame == 1 ? grownVertices.size() : legacyVertices.size());
        uploadPoolValid = pipeline.set_viewport(0, 0, outputWidth, outputHeight) == GR_OK &&
            pipeline.clear_buffers(clearBoth, nullptr) == GR_OK && pipeline.begin_scene() == GR_OK &&
            pipeline.set_viewport(0, 0, 64, 64) == GR_OK && pipeline.set_modelview(left) == GR_OK &&
            pipeline.draw_indexed_primitive(D3DPT_TRIANGLELIST, FrontierX::LegacyD3d9::kRpVertexFvf,
                vertices, count, kIndices.data(), static_cast<int>(kIndices.size()), 0U) == GR_OK &&
            pipeline.set_modelview(right) == GR_OK && FrontierX::SubmitRenderSmokeScene(&pipeline) == GR_OK &&
            renderer.FrameSubmissionCount() == 2U && exerciseSamplerCache() &&
            renderer.SamplerCreations() == 32U && pipeline.end_scene() == GR_OK &&
            renderer.Readback64(pooledImages[frame], error) && pooledImages[frame].changedPixels > 0U &&
            pipeline.swap_buffers() == GR_OK;
    }
    uploadPoolValid = uploadPoolValid &&
        pooledImages[0].checksum == pooledImages[1].checksum &&
        pooledImages[0].checksum == pooledImages[2].checksum &&
        renderer.GetTimings().reusedUploads >= reuseBefore + 2U;
    if (uploadPoolValid)
        std::cout << "Captured upload pool: growth/shrink, stale-format pressure, all 400 active draws reused and identical output\n";
    if (samplerCacheValid && uploadPoolValid)
        std::cout << "GPU sampler cache: 32 shared two-stage states, reuse, rejected rollback and captured snapshot readback\n";
    materialStates->Release();
    material->Release();
    vertexPipeline.destroy_vertex_buffer(materialBuffer);
    vertexManager->cleanup();
    vertexManager->Release();
    managerComponent->Release();
    textures->Release();
    textureAggregate->Release();
    textureInner->Release();
    primitive->Release();
    const GENRESULT shutdownResult = manager.ShutDown();
    if (!abilityModulationValid || !rgb888Valid || !alphaMipValid || !mipValid || !stage1Observed || !alphaTestValid || !stateValid || !indexedValid || !secondUvObserved || !directValid ||
        !textureStageValid || !samplerValid || !samplerCacheValid || !transformValid || !viewportValid ||
        !windowViewportRestored || !lifecycleValid || !pipelineCacheValid || !uploadPoolValid ||
        !frameDrawsValid || !mapReplacementValid || !mapReplacementReleased || !capturedDestroyValid ||
        !textureBindingValid ||
        !productionTextureValid || !loadedReleased ||
        shutdownResult != GR_OK)
    {
        if (!samplerCacheValid)
            error = "immutable GPU sampler cache sharing/rollback/readback failed";
        else if (!pipelineCacheValid)
            error = "fixed-state GPU pipeline reuse/pressure/rollback/readback failed";
        else if (!uploadPoolValid)
            error = "captured upload reuse/growth/shrink/readback failed";
        else if (!capturedDestroyValid)
            error = "captured movie texture destruction/presentation/retirement failed";
        else if (!mapReplacementValid || !mapReplacementReleased)
            error = "captured map texture replacement/lifetime failed";
        else if (!textureBindingValid && textureStageValid)
            error = "native texture handle binding or rollback failed: handle=" +
                std::to_string(alternateHandle) + " bound=" + std::to_string(boundHandle) +
                " current=" + std::to_string(renderer.CurrentTextureHandle()) +
                " baseline=" + std::to_string(modulated.checksum) +
                " alternate=" + std::to_string(alternateReadback.checksum) +
                " copied=" + std::to_string(copiedReadback.checksum) +
                " alternate_valid=" + std::to_string(alternateReadback.structureValid) +
                " rebound=" + std::to_string(reboundReadback.checksum);
        else if (!productionTextureValid || !loadedReleased)
            error = "production UTF texture GPU path failed: " + utfError +
                " loaded=" + std::to_string(productionTextureValid) +
                " released=" + std::to_string(loadedReleased) +
                " baseline=" + std::to_string(modulated.checksum) +
                " loaded_checksum=" + std::to_string(loadedChecksum);
        else
            error = renderer.LastError().empty()
                ? "native render submission failed: state=" +
                    std::to_string(stateValid) + " mips=" + std::to_string(mipValid) + " stage1=" + std::to_string(stage1Observed) + " indexed=" +
                    std::to_string(indexedValid) + " material_ready=" +
                    std::to_string(materialStateReady) + " material_readback=" +
                    std::to_string(materialReadbackValid) + " direct=" +
                    std::to_string(directValid) + " texture_stage=" +
                    std::to_string(textureStageValid) + " sampler=" +
                    std::to_string(samplerValid) + " transform=" +
                    std::to_string(transformValid) + " viewport=" +
                    std::to_string(viewportValid) + " clipped_pixels=" +
                    std::to_string(clippedReadback.changedPixels) + " shifted=" +
                    std::to_string(shiftedValid) + " displaced=" +
                    std::to_string(displacementObserved) + " projected=" +
                    std::to_string(projectionObserved) + " reset_modelview=" +
                    std::to_string(resetModelview) + " reset_projection=" +
                    std::to_string(resetProjection) + " shifted_checksum=" +
                    std::to_string(shiftedReadback.checksum) + " projected_checksum=" +
                    std::to_string(projectedReadback.checksum) + " restored_checksum=" +
                    std::to_string(transformRestored.checksum) + " material_checksum=" +
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
    if (argc < 6 || argc > 8)
    {
        std::cerr << "usage: frontierx_sdl3_gpu_probe VERTEX_SPV FRAGMENT_SPV "
                     "EXTENDED_VERTEX_SPV UTF_FIXTURE_PATH GAME_VERTEX_SPV "
                     "[--require-runtime] [--visible]\n";
        return 2;
    }
    bool requireRuntime = false;
    bool visible = false;
    for (int index = 6; index < argc; ++index)
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
        !std::filesystem::is_regular_file(argv[3]) ||
        !std::filesystem::is_regular_file(argv[5]))
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
                             texture, error, argv[3], argv[5]))
        return RuntimeUnavailable(requireRuntime, "renderer initialization", error);
    if (!FrontierX::RegisterNativeStateMaterial(DACOM_Acquire()) ||
        !Register_MaterialLibrary() || !Register_DcDtOcMaterial())
    {
        std::cerr << "production material library registration failed\n";
        return 1;
    }
    const std::filesystem::path utfFixturePath(argv[4]);
    if (!FrontierXTest::WriteMaterialFixture(utfFixturePath))
    {
        std::cerr << "UTF texture fixture creation failed\n";
        return 1;
    }
    std::uint64_t extendedChecksumA = 0U;
    std::uint64_t extendedChecksumB = 0U;
    std::uint64_t materialChecksum = 0U;
    std::uint64_t alternateChecksum = 0U;
    std::uint64_t copiedChecksum = 0U;
    std::uint64_t loadedChecksum = 0U;
    // Original Beam blur submits RPVERTEX strips, including odd-triangle winding.
    const std::array<RenderVertexP3DiffuseT2, 4> stripVertices{{
        kVertices[3], kVertices[4], kVertices[6], kVertices[5]}};
    const std::array<std::uint16_t, 6> stripTriangles{{0U, 1U, 2U, 2U, 1U, 3U}};
    const std::array<std::uint16_t, 4> stripIndices{{0U, 1U, 2U, 3U}};
    FrontierX::SdlGpuReadback stripExpected, stripDirect, stripIndexed;
    if (renderer.Submit({FrontierX::PrimitiveTopology::TriangleList,
            stripVertices.data(), stripVertices.size(), stripTriangles.data(), stripTriangles.size()}) != GR_OK ||
        !renderer.Readback64(stripExpected, error) ||
        renderer.Submit({FrontierX::PrimitiveTopology::TriangleStrip,
            stripVertices.data(), stripVertices.size(), nullptr, 0U}) != GR_OK ||
        renderer.VertexCount() != 6U || !renderer.Readback64(stripDirect, error) ||
        renderer.Submit({FrontierX::PrimitiveTopology::TriangleStrip,
            stripVertices.data(), stripVertices.size(), stripIndices.data(), stripIndices.size()}) != GR_OK ||
        !renderer.Readback64(stripIndexed, error) || stripExpected.changedPixels == 0U ||
        stripDirect.checksum != stripExpected.checksum || stripIndexed.checksum != stripExpected.checksum)
    {
        std::cerr << "beam strip differs from explicit triangles: " << error << '\n';
        return 1;
    }
    // The Battlecruiser AreaEffectBolt caller draws RPVERTEX2 triangle strips.
    // Beam's one-UV fixture above cannot detect rejection or UV loss here.
    std::array<FrontierX::RenderVertexP3DiffuseT4, 4> stripVertices2{};
    for (std::size_t index = 0; index < stripVertices.size(); ++index)
    {
        const auto &v = stripVertices[index];
        stripVertices2[index] = {v.x, v.y, v.z, v.diffuseBgra,
            v.u, v.v, v.v, 1.0F - v.u};
    }
    FrontierX::SdlGpuReadback strip2Expected, strip2Direct, strip2Indexed;
    const auto stagesBeforeStrip2 = renderer.CurrentTextureStageState();
    auto strip2Stages = stagesBeforeStrip2;
    strip2Stages.textureCoordinateIndex = 1;
    if (renderer.SetTextureStageState(strip2Stages) != GR_OK ||
        renderer.Submit(FrontierX::RenderSubmissionView2{FrontierX::PrimitiveTopology::TriangleList,
            stripVertices2.data(), stripVertices2.size(), stripTriangles.data(), stripTriangles.size()}) != GR_OK ||
        !renderer.Readback64(strip2Expected, error) ||
        renderer.Submit(FrontierX::RenderSubmissionView2{FrontierX::PrimitiveTopology::TriangleStrip,
            stripVertices2.data(), stripVertices2.size(), nullptr, 0U}) != GR_OK ||
        renderer.VertexCount() != 6U || !renderer.Readback64(strip2Direct, error) ||
        renderer.Submit(FrontierX::RenderSubmissionView2{FrontierX::PrimitiveTopology::TriangleStrip,
            stripVertices2.data(), stripVertices2.size(), stripIndices.data(), stripIndices.size()}) != GR_OK ||
        !renderer.Readback64(strip2Indexed, error) || strip2Expected.changedPixels == 0U ||
        strip2Direct.checksum != strip2Expected.checksum || strip2Indexed.checksum != strip2Expected.checksum ||
        renderer.SetTextureStageState(stagesBeforeStrip2) != GR_OK)
    {
        std::cerr << "area-effect two-UV strip differs from explicit triangles: " << error << '\n';
        return 1;
    }
    std::cout << "AreaEffectBolt two-UV direct/indexed strips match explicit triangle winding and UV1 readback\n";
    if (!SubmitProductionFixture(renderer, windowRuntime.window, utfFixturePath,
                                 extendedChecksumA, extendedChecksumB,
                                 materialChecksum, alternateChecksum,
                                 copiedChecksum, loadedChecksum, error))
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
                // The compositor can only map the GPU window after its first
                // real presentation. Prepare it before the unchanged resize.
                if (!WaitForSdlGpuHostSetup())
                {
                    std::cerr << "GPU host window preparation failed\n";
                    return 1;
                }
                initialWidth = renderer.DepthWidth();
                initialHeight = renderer.DepthHeight();
                if (!SDL_SetWindowSize(windowRuntime.window, 800, 450) ||
                    !SDL_SyncWindow(windowRuntime.window))
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
        int w = 0, h = 0;
        SDL_GetWindowSizeInPixels(windowRuntime.window, &w, &h);
        std::cerr << "window pixels: " << w << 'x' << h << '\n';
        std::cerr << "GPU resize was not observed by the swapchain: "
                  << initialWidth << 'x' << initialHeight << " -> "
                  << renderer.DepthWidth() << 'x' << renderer.DepthHeight() << '\n';
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
                             texture, error, argv[3], argv[5]))
    {
        std::cerr << "repeated GPU initialization failed: " << error << '\n';
        return 1;
    }
    std::uint64_t repeatedExtendedA = 0U;
    std::uint64_t repeatedExtendedB = 0U;
    std::uint64_t repeatedMaterialChecksum = 0U;
    std::uint64_t repeatedAlternateChecksum = 0U;
    std::uint64_t repeatedCopiedChecksum = 0U;
    std::uint64_t repeatedLoadedChecksum = 0U;
    if (!SubmitProductionFixture(renderer, windowRuntime.window, utfFixturePath,
                                 repeatedExtendedA, repeatedExtendedB,
                                 repeatedMaterialChecksum, repeatedAlternateChecksum,
                                 repeatedCopiedChecksum, repeatedLoadedChecksum, error) ||
        repeatedExtendedA != extendedChecksumA ||
        repeatedExtendedB != extendedChecksumB ||
        repeatedMaterialChecksum != materialChecksum ||
        repeatedAlternateChecksum != alternateChecksum ||
        repeatedCopiedChecksum != copiedChecksum ||
        repeatedLoadedChecksum != loadedChecksum)
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
              << " utf_texture_checksum=" << loadedChecksum
              << " scene_runs=2 repeated_presented=1\n";
    return 0;
}
