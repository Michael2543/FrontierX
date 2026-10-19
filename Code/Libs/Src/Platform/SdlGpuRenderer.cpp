#include "FrontierX/SdlGpuRenderer.h"
#include "FrontierX/NativeBatchStateIds.h"
#include "FrontierX/NativeRenderPipeline.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif
#include "matrix4.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <new>
#include <utility>
#include <vector>

namespace FrontierX
{
struct SdlGpuRenderer::Impl
{
    struct UploadSet
    {
        SDL_GPUBuffer * vertex = nullptr;
        SDL_GPUBuffer * index = nullptr;
        SDL_GPUTransferBuffer * transfer = nullptr;
        Uint32 vertexBytes = 0, indexBytes = 0, transferBytes = 0;
        Uint64 Bytes() const { return Uint64(vertexBytes) + indexBytes + transferBytes; }
    };
    struct FrameDraw
    {
        SDL_GPUBuffer * vertex = nullptr;
        SDL_GPUBuffer * index = nullptr;
        SDL_GPUTransferBuffer * transfer = nullptr;
        SDL_GPUTexture * uploadedTexture = nullptr;
        SDL_GPUTexture * boundTexture = nullptr;
        SDL_GPUTexture * secondTexture = nullptr;
        SDL_GPUSampler * secondSampler = nullptr;
        SDL_GPUSampler * sampler = nullptr;
        SDL_GPUGraphicsPipeline * pipeline = nullptr;
        Uint32 vertexCount = 0U;
        Uint32 indexCount = 0U;
        SDL_Rect viewport{};
        RenderStateSnapshot renderState{};
        TextureStageSnapshot textureStage{};
        std::array<float, 16> modelview{};
        std::array<float, 16> projection{};
        std::array<float, 4> vertexUv{};
        std::array<float, 4> vertexColor{};
        Uint32 vertexBytes = 0, indexBytes = 0, transferBytes = 0;
    };
    struct PipelineSet
    {
        SDL_GPUGraphicsPipeline * pipeline = nullptr;
        SDL_GPUGraphicsPipeline * line = nullptr;
        SDL_GPUGraphicsPipeline * extended = nullptr;
        SDL_GPUGraphicsPipeline * game = nullptr;
        SDL_GPUGraphicsPipeline * app = nullptr;
    };
    using PipelineKey = std::array<U32, 7>;
    struct CachedPipeline
    {
        PipelineSet set;
        std::uint64_t lastUse = 0U;
    };
    static PipelineKey Key(const RenderStateSnapshot & state)
    {
        return {{state.depthEnabled, state.depthWriteEnabled, state.depthFunction,
                 state.alphaBlendEnabled, state.sourceBlend,
                 state.destinationBlend, state.cullMode}};
    }
    PipelineSet ActivePipelines() const
    {
        return {pipeline, linePipeline, extendedPipeline, gamePipeline, appPipeline};
    }
    void BindPipelines(const PipelineSet & set)
    {
        pipeline = set.pipeline;
        linePipeline = set.line;
        extendedPipeline = set.extended;
        gamePipeline = set.game;
        appPipeline = set.app;
    }
    void ReleasePipelines(const PipelineSet & set)
    {
        SDL_ReleaseGPUGraphicsPipeline(device, set.pipeline);
        SDL_ReleaseGPUGraphicsPipeline(device, set.line);
        SDL_ReleaseGPUGraphicsPipeline(device, set.extended);
        SDL_ReleaseGPUGraphicsPipeline(device, set.game);
        SDL_ReleaseGPUGraphicsPipeline(device, set.app);
    }
    // Call only after GPU idle and after clearing captured draws. During a
    // captured frame all borrowed pipeline pointers must stay live.
    void TrimPipelineCache()
    {
        while (pipelineCache.size() > 64U)
        {
            auto oldest = pipelineCache.end();
            for (auto it = pipelineCache.begin(); it != pipelineCache.end(); ++it)
                if (it->second.set.pipeline != pipeline &&
                    (oldest == pipelineCache.end() ||
                     it->second.lastUse < oldest->second.lastUse))
                    oldest = it;
            if (oldest == pipelineCache.end()) break;
            ReleasePipelines(oldest->second.set);
            pipelineCache.erase(oldest);
        }
    }
    struct RegisteredTexture
    {
        SDL_GPUTexture * texture;
        Uint32 width;
        Uint32 height;
        Uint32 levels = 1U;
    };
    SDL_Window * window = nullptr;
    SDL_GPUDevice * device = nullptr;
    SDL_GPUShader * vertexShader = nullptr;
    SDL_GPUShader * extendedVertexShader = nullptr;
    SDL_GPUShader * gameVertexShader = nullptr;
    SDL_GPUShader * fragmentShader = nullptr;
    SDL_GPUGraphicsPipeline * pipeline = nullptr;
    SDL_GPUGraphicsPipeline * linePipeline = nullptr;
    SDL_GPUGraphicsPipeline * extendedPipeline = nullptr;
    SDL_GPUGraphicsPipeline * gamePipeline = nullptr;
    SDL_GPUGraphicsPipeline * appPipeline = nullptr;
    SDL_GPUBuffer * vertexBuffer = nullptr;
    SDL_GPUBuffer * indexBuffer = nullptr;
    SDL_GPUTransferBuffer * transferBuffer = nullptr;
    SDL_GPUTexture * colorTexture = nullptr;
    std::map<Uint32, RegisteredTexture> registeredTextures;
    SDL_GPUTexture * depthTexture = nullptr;
    SDL_GPUSampler * sampler = nullptr;
    SDL_GPUSampler * secondSampler = nullptr;
    // Five binary sampler properties form a fixed set shared by both stages.
    // Captured draws borrow immutable entries until the idle-waited shutdown.
    std::array<SDL_GPUSampler *, 32> samplerCache{};
    Uint32 samplerCreations = 0U;
    static std::size_t SamplerKey(const SamplerStateSnapshot & state)
    {
        return (state.addressU == D3DTADDRESS_WRAP ? 1U : 0U) |
               (state.addressV == D3DTADDRESS_WRAP ? 2U : 0U) |
               (state.minFilter == D3DTEXF_LINEAR ? 4U : 0U) |
               (state.magFilter == D3DTEXF_LINEAR ? 8U : 0U) |
               (state.mipFilter == D3DTEXF_LINEAR ? 16U : 0U);
    }
    SDL_GPUTextureFormat colorTargetFormat = SDL_GPU_TEXTUREFORMAT_INVALID;
    std::vector<std::uint8_t> texturePixels;
    std::string error;
    Uint32 textureWidth = 0U;
    Uint32 textureHeight = 0U;
    Uint32 depthWidth = 0U;
    Uint32 depthHeight = 0U;
    Uint32 depthRecreations = 0U;
    Uint32 pipelineRebuilds = 0U;
    Uint32 vertexCount = 0U;
    Uint32 indexCount = 0U;
    RenderStateSnapshot renderState{};
    std::array<float, 16> modelview{{1.0F, 0.0F, 0.0F, 0.0F,
                                     0.0F, 1.0F, 0.0F, 0.0F,
                                     0.0F, 0.0F, 1.0F, 0.0F,
                                     0.0F, 0.0F, 0.0F, 1.0F}};
    std::array<float, 16> projection{{1.0F, 0.0F, 0.0F, 0.0F,
                                      0.0F, 1.0F, 0.0F, 0.0F,
                                      0.0F, 0.0F, 1.0F, 0.0F,
                                      0.0F, 0.0F, 0.0F, 1.0F}};
    SDL_Rect viewport{0, 0, 0, 0};
    std::array<float, 4> vertexUv{{0.0F, 0.0F, 1.0F, 1.0F}};
    std::array<float, 4> vertexColor{{1.0F, 1.0F, 1.0F, 1.0F}};
    TextureStageSnapshot textureStage{};
    SamplerStateSnapshot samplerState{};
    SamplerStateSnapshot secondSamplerState{};
    Uint32 currentTextureHandle = 1U;
    Uint32 secondTextureHandle = 0U;
    Uint32 nullTextureHandle = 0U;
    Uint32 nextTextureHandle = 2U;
    unsigned activeVertexFormat = 0U;
    bool windowClaimed = false;
    SDL_FColor clearColor{0.025F, 0.035F, 0.065F, 1.0F};
    float clearDepth = 1.0F;
    bool frameCapture = false;
    std::vector<FrameDraw> frameDraws;
    std::map<PipelineKey, CachedPipeline> pipelineCache;
    std::uint64_t pipelineUse = 0U;
    bool timingsEnabled = false;
    Uint64 submissionTicks = 0, samplerTicks = 0, retirementTicks = 0, presentationTicks = 0;
    Uint64 textureUploadTicks = 0, textureUploadCount = 0;
    Uint64 submissionCount = 0, samplerChanges = 0;
    Uint64 acquisitionTicks = 0, drawingTicks = 0, queueSubmitTicks = 0;
    std::vector<UploadSet> freeUploads;
    Uint64 freeUploadBytes = 0, reusedUploads = 0;
    void ReleaseUpload(const UploadSet & upload)
    {
        SDL_ReleaseGPUBuffer(device, upload.vertex);
        if (upload.index) SDL_ReleaseGPUBuffer(device, upload.index);
        SDL_ReleaseGPUTransferBuffer(device, upload.transfer);
    }
    std::vector<SDL_GPUTexture *> retiredTextures;
};

namespace
{
struct CpuTimer
{
    Uint64 & total;
    bool enabled;
    Uint64 started;
    CpuTimer(Uint64 & value, bool active) : total(value), enabled(active),
        started(active ? SDL_GetPerformanceCounter() : 0U) {}
    ~CpuTimer() { if (enabled) total += SDL_GetPerformanceCounter() - started; }
};
class ReadbackResources
{
public:
    explicit ReadbackResources(SDL_GPUDevice * owner) : device(owner) {}
    ~ReadbackResources()
    {
        if (fence) SDL_ReleaseGPUFence(device, fence);
        if (download) SDL_ReleaseGPUTransferBuffer(device, download);
        if (depth) SDL_ReleaseGPUTexture(device, depth);
        if (color) SDL_ReleaseGPUTexture(device, color);
    }
    ReadbackResources(const ReadbackResources &) = delete;
    ReadbackResources & operator=(const ReadbackResources &) = delete;

    SDL_GPUDevice * device;
    SDL_GPUTexture * color = nullptr;
    SDL_GPUTexture * depth = nullptr;
    SDL_GPUTransferBuffer * download = nullptr;
    SDL_GPUFence * fence = nullptr;
};

class SubmissionResources
{
public:
    explicit SubmissionResources(SDL_GPUDevice * owner) : device(owner) {}
    ~SubmissionResources()
    {
        if (transfer) SDL_ReleaseGPUTransferBuffer(device, transfer);
        if (color) SDL_ReleaseGPUTexture(device, color);
        if (index) SDL_ReleaseGPUBuffer(device, index);
        if (vertex) SDL_ReleaseGPUBuffer(device, vertex);
    }
    SubmissionResources(const SubmissionResources &) = delete;
    SubmissionResources & operator=(const SubmissionResources &) = delete;

    SDL_GPUDevice * device;
    Uint32 vertexBytes = 0, indexBytes = 0, transferBytes = 0;
    SDL_GPUBuffer * vertex = nullptr;
    SDL_GPUBuffer * index = nullptr;
    SDL_GPUTransferBuffer * transfer = nullptr;
    SDL_GPUTexture * color = nullptr;
};

bool ReadShader(const char * path, std::vector<std::uint8_t> & bytes, std::string & error)
{
    if (!path || !path[0])
    {
        error = "shader path is empty";
        return false;
    }
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input)
    {
        error = "could not open shader";
        return false;
    }
    const std::streamoff end = input.tellg();
    if (end <= 0 || end > static_cast<std::streamoff>(
            std::numeric_limits<std::streamsize>::max()))
    {
        error = "shader size is invalid";
        return false;
    }
    const auto size = static_cast<std::size_t>(end);
    if ((size % sizeof(std::uint32_t)) != 0U)
    {
        error = "SPIR-V size is not word aligned";
        return false;
    }
    bytes.resize(size);
    input.seekg(0, std::ios::beg);
    input.read(reinterpret_cast<char *>(bytes.data()), static_cast<std::streamsize>(size));
    if (!input)
    {
        error = "could not read complete shader";
        bytes.clear();
        return false;
    }
    return true;
}

SDL_GPUShader * CreateShader(SDL_GPUDevice * device,
                             const std::vector<std::uint8_t> & code,
                             SDL_GPUShaderStage stage,
                             Uint32 samplerCount, Uint32 uniformCount = 0U)
{
    SDL_GPUShaderCreateInfo info{};
    info.code_size = code.size();
    info.code = code.data();
    info.entrypoint = "main";
    info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    info.stage = stage;
    info.num_samplers = samplerCount;
    info.num_uniform_buffers = uniformCount;
    return SDL_CreateGPUShader(device, &info);
}

struct alignas(16) TextureStageUniform
{
    Uint32 operations[4];
    Uint32 arguments[4];
    float factor[4];
    Uint32 alphaTest[4];
    Uint32 secondOperations[4];
    Uint32 secondArguments[4];
};

struct alignas(16) VertexTransformUniform
{
    std::array<float, 16> clip;
    std::array<float, 4> uv;
    std::array<float, 4> color;
};

// RPVERTEX2 and App StandardVertex share a shader, but only the App effect
// consumes c30/c31. Fixed-function draws retain raw UVs and prelit colors.
constexpr std::array<float, 4> FixedFunctionUv{{0.0F, 0.0F, 1.0F, 1.0F}};
constexpr std::array<float, 4> FixedFunctionColor{{1.0F, 1.0F, 1.0F, 1.0F}};

TextureStageUniform MakeTextureStageUniform(const TextureStageSnapshot & state,
                                           const RenderStateSnapshot & renderState)
{
    const U32 color = renderState.textureFactor;
    constexpr float scale = 1.0F / 255.0F;
    return {{state.colorOperation, state.alphaOperation, renderState.specularEnabled,
             state.textureCoordinateIndex},
            {state.colorArgument1, state.colorArgument2,
             state.alphaArgument1, state.alphaArgument2},
            {static_cast<float>((color >> 16U) & 0xFFU) * scale,
             static_cast<float>((color >> 8U) & 0xFFU) * scale,
             static_cast<float>(color & 0xFFU) * scale,
             static_cast<float>((color >> 24U) & 0xFFU) * scale},
            {renderState.alphaTestEnabled, renderState.alphaFunction,
             renderState.alphaReference, 0U},
            {state.second.colorOperation, state.second.alphaOperation, 0U,
             state.second.textureCoordinateIndex},
            {state.second.colorArgument1, state.second.colorArgument2,
             state.second.alphaArgument1, state.second.alphaArgument2}};
}

std::array<float, 16> MakeClipMatrix(const std::array<float, 16> & projection,
                                     const std::array<float, 16> & modelview)
{
    std::array<float, 16> columnMajor{};
    for (std::size_t row = 0U; row < 4U; ++row)
        for (std::size_t column = 0U; column < 4U; ++column)
            for (std::size_t inner = 0U; inner < 4U; ++inner)
                columnMajor[column * 4U + row] +=
                    projection[row * 4U + inner] * modelview[inner * 4U + column];
    return columnMajor;
}

bool ApplyViewport(SDL_GPURenderPass * pass, const SDL_Rect & viewport,
                   Uint32 targetWidth, Uint32 targetHeight,
                   bool offscreenFullTarget = false)
{
    if (!viewport.w && !viewport.h) return true;
    // The 64-pixel diagnostic target represents a full window viewport
    // at reduced resolution; a smaller pane still keeps its own bounds.
    if (offscreenFullTarget && viewport.x == 0 && viewport.y == 0 &&
        static_cast<Uint32>(viewport.w) >= targetWidth &&
        static_cast<Uint32>(viewport.h) >= targetHeight)
        return true;
    if (viewport.w <= 0 || viewport.h <= 0 ||
        static_cast<Uint32>(viewport.x + viewport.w) > targetWidth ||
        static_cast<Uint32>(viewport.y + viewport.h) > targetHeight)
        return false;
    const SDL_GPUViewport gpuViewport{
        static_cast<float>(viewport.x), static_cast<float>(viewport.y),
        static_cast<float>(viewport.w), static_cast<float>(viewport.h), 0.0F, 1.0F};
    SDL_SetGPUViewport(pass, &gpuViewport);
    SDL_SetGPUScissor(pass, &viewport);
    return true;
}

bool MapDepthFunction(U32 value, SDL_GPUCompareOp & result)
{
    switch (value)
    {
    case D3DCMP_NEVER: result = SDL_GPU_COMPAREOP_NEVER; return true;
    case D3DCMP_LESS: result = SDL_GPU_COMPAREOP_LESS; return true;
    case D3DCMP_EQUAL: result = SDL_GPU_COMPAREOP_EQUAL; return true;
    case D3DCMP_LESSEQUAL: result = SDL_GPU_COMPAREOP_LESS_OR_EQUAL; return true;
    case D3DCMP_GREATER: result = SDL_GPU_COMPAREOP_GREATER; return true;
    case D3DCMP_NOTEQUAL: result = SDL_GPU_COMPAREOP_NOT_EQUAL; return true;
    case D3DCMP_GREATEREQUAL: result = SDL_GPU_COMPAREOP_GREATER_OR_EQUAL; return true;
    case D3DCMP_ALWAYS: result = SDL_GPU_COMPAREOP_ALWAYS; return true;
    default: return false;
    }
}

bool MapBlendFactor(U32 value, SDL_GPUBlendFactor & result)
{
    switch (value)
    {
    case D3DBLEND_ZERO: result = SDL_GPU_BLENDFACTOR_ZERO; return true;
    case D3DBLEND_ONE: result = SDL_GPU_BLENDFACTOR_ONE; return true;
    case D3DBLEND_SRCCOLOR: result = SDL_GPU_BLENDFACTOR_SRC_COLOR; return true;
    case D3DBLEND_INVSRCCOLOR: result = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_COLOR; return true;
    case D3DBLEND_SRCALPHA: result = SDL_GPU_BLENDFACTOR_SRC_ALPHA; return true;
    case D3DBLEND_INVSRCALPHA:
        result = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        return true;
    case D3DBLEND_DESTALPHA: result = SDL_GPU_BLENDFACTOR_DST_ALPHA; return true;
    case D3DBLEND_INVDESTALPHA:
        result = SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_ALPHA;
        return true;
    case D3DBLEND_DESTCOLOR: result = SDL_GPU_BLENDFACTOR_DST_COLOR; return true;
    case D3DBLEND_INVDESTCOLOR:
        result = SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_COLOR;
        return true;
    case D3DBLEND_SRCALPHASAT:
        result = SDL_GPU_BLENDFACTOR_SRC_ALPHA_SATURATE;
        return true;
    default: return false;
    }
}

SDL_GPUGraphicsPipeline * CreateGraphicsPipeline(
    SDL_GPUDevice * device,
    SDL_GPUShader * vertexShader,
    SDL_GPUShader * fragmentShader,
    SDL_GPUTextureFormat colorTargetFormat,
    const RenderStateSnapshot & state,
    unsigned vertexFormat,
    SDL_GPUPrimitiveType topology = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST)
{
    SDL_GPUCompareOp compare = SDL_GPU_COMPAREOP_INVALID;
    SDL_GPUBlendFactor source = SDL_GPU_BLENDFACTOR_INVALID;
    SDL_GPUBlendFactor destination = SDL_GPU_BLENDFACTOR_INVALID;
    if (!MapDepthFunction(state.depthFunction, compare) ||
        !MapBlendFactor(state.sourceBlend, source) ||
        !MapBlendFactor(state.destinationBlend, destination))
    {
        return nullptr;
    }
    const SDL_GPUVertexBufferDescription bufferDescription{
        0U, static_cast<Uint32>(vertexFormat == 3U ?
                                  sizeof(RenderVertexP3N3DiffuseT5) :
                                  vertexFormat == 2U ?
                                  sizeof(RenderVertexP3DiffuseSpecularT4) :
                                  vertexFormat == 1U ? sizeof(RenderVertexP3DiffuseT4) :
                                  sizeof(RenderVertexP3DiffuseT2)),
        SDL_GPU_VERTEXINPUTRATE_VERTEX, 0U};
    const std::array<SDL_GPUVertexAttribute, 5> attributes = {{
        {0U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0U},
        {1U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
         static_cast<Uint32>(vertexFormat == 3U ?
             offsetof(RenderVertexP3N3DiffuseT5, diffuseBgra) :
             offsetof(RenderVertexP3DiffuseT2, diffuseBgra))},
        {2U, 0U, vertexFormat == 2U ? SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM :
                                  SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
         static_cast<Uint32>(vertexFormat == 2U ?
             offsetof(RenderVertexP3DiffuseSpecularT4, specularBgra) :
             vertexFormat == 3U ? offsetof(RenderVertexP3N3DiffuseT5, u) :
             offsetof(RenderVertexP3DiffuseT2, u))},
        {3U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
         static_cast<Uint32>(vertexFormat == 2U ?
             offsetof(RenderVertexP3DiffuseSpecularT4, u) :
             vertexFormat == 3U ? offsetof(RenderVertexP3N3DiffuseT5, u2) :
             offsetof(RenderVertexP3DiffuseT4, u2))},
        {4U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
         static_cast<Uint32>(offsetof(RenderVertexP3DiffuseSpecularT4, u2))},
    }};
    SDL_GPUColorTargetDescription colorTarget{};
    colorTarget.format = colorTargetFormat;
    colorTarget.blend_state.src_color_blendfactor = source;
    colorTarget.blend_state.dst_color_blendfactor = destination;
    colorTarget.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.src_alpha_blendfactor = source;
    colorTarget.blend_state.dst_alpha_blendfactor = destination;
    colorTarget.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTarget.blend_state.enable_blend = state.alphaBlendEnabled != 0U;
    // The production D3D9 primary target is X8R8G8B8: source alpha still
    // participates in RGB blending/testing, but the presented destination is
    // opaque. Native render-to-texture targets are not implemented here.
    colorTarget.blend_state.enable_color_write_mask = true;
    colorTarget.blend_state.color_write_mask = SDL_GPU_COLORCOMPONENT_R |
        SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B;
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &bufferDescription;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1U;
    pipelineInfo.vertex_input_state.vertex_attributes = attributes.data();
    pipelineInfo.vertex_input_state.num_vertex_attributes = vertexFormat == 2U ? 5U :
                                                             vertexFormat == 1U || vertexFormat == 3U ? 4U : 3U;
    pipelineInfo.primitive_type = topology;
    pipelineInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipelineInfo.rasterizer_state.cull_mode = state.cullMode == D3DCULL_CW ?
        SDL_GPU_CULLMODE_BACK : state.cullMode == D3DCULL_CCW ?
        SDL_GPU_CULLMODE_FRONT : SDL_GPU_CULLMODE_NONE;
    pipelineInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    pipelineInfo.rasterizer_state.enable_depth_clip = true;
    pipelineInfo.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    pipelineInfo.depth_stencil_state.compare_op = compare;
    pipelineInfo.depth_stencil_state.enable_depth_test = state.depthEnabled != 0U;
    pipelineInfo.depth_stencil_state.enable_depth_write =
        state.depthEnabled != 0U && state.depthWriteEnabled != 0U;
    pipelineInfo.target_info.color_target_descriptions = &colorTarget;
    pipelineInfo.target_info.num_color_targets = 1U;
    pipelineInfo.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    pipelineInfo.target_info.has_depth_stencil_target = true;
    return SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
}

bool PixelsDiffer(const std::uint8_t * left,
                  const std::uint8_t * right,
                  unsigned tolerance)
{
    for (std::size_t channel = 0U; channel < 3U; ++channel)
    {
        const unsigned a = left[channel];
        const unsigned b = right[channel];
        const unsigned difference = a > b ? a - b : b - a;
        if (difference > tolerance) return true;
    }
    return false;
}

template<class Vertex, class Submission>
GENRESULT ExpandTriangleStrip(const Submission &submission, std::vector<Vertex> &vertices)
{
    const std::size_t count = submission.indexCount ? submission.indexCount : submission.vertexCount;
    const std::size_t triangles = count - 2U;
    if (triangles > std::numeric_limits<Uint32>::max() / (3U * sizeof(Vertex)))
        return GR_INVALID_PARMS;
    try
    {
        vertices.reserve(triangles * 3U);
        const auto vertex = [&](std::size_t i) -> const Vertex & {
            return submission.vertices[submission.indices ? submission.indices[i] : i];
        };
        for (std::size_t i = 0U; i < triangles; ++i)
        {
            // D3D strips reverse the first two vertices of every odd triangle.
            vertices.push_back(vertex(i + (i & 1U)));
            vertices.push_back(vertex(i + 1U - (i & 1U)));
            vertices.push_back(vertex(i + 2U));
        }
    }
    catch (const std::bad_alloc &) { return GR_OUT_OF_MEMORY; }
    return GR_OK;
}

SDL_GPUSampler * CreateSampler(SDL_GPUDevice * device,
                               const SamplerStateSnapshot & state)
{
    SDL_GPUSamplerCreateInfo info{};
    info.min_filter = state.minFilter == D3DTEXF_LINEAR ?
        SDL_GPU_FILTER_LINEAR : SDL_GPU_FILTER_NEAREST;
    info.mag_filter = state.magFilter == D3DTEXF_LINEAR ?
        SDL_GPU_FILTER_LINEAR : SDL_GPU_FILTER_NEAREST;
    info.mipmap_mode = state.mipFilter == D3DTEXF_LINEAR ?
        SDL_GPU_SAMPLERMIPMAPMODE_LINEAR : SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    info.address_mode_u = state.addressU == D3DTADDRESS_WRAP ?
        SDL_GPU_SAMPLERADDRESSMODE_REPEAT : SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    info.address_mode_v = state.addressV == D3DTADDRESS_WRAP ?
        SDL_GPU_SAMPLERADDRESSMODE_REPEAT : SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    info.max_lod = 31.0F;
    return SDL_CreateGPUSampler(device, &info);
}
}

SdlGpuRenderer::SdlGpuRenderer() : impl_(std::make_unique<Impl>()) {}

SdlGpuRenderer::~SdlGpuRenderer()
{
    Shutdown();
}

bool SdlGpuRenderer::Initialize(std::uint32_t windowId,
                                const char * vertexShaderPath,
                                const char * fragmentShaderPath,
                                SdlGpuTextureView texture,
                                std::string & error,
                                const char * extendedVertexShaderPath,
                                const char * gameVertexShaderPath)
{
    Shutdown();
    impl_ = std::make_unique<Impl>();
    error.clear();
    if (!windowId || !texture.rgba8 || !texture.width || !texture.height ||
        texture.width > std::numeric_limits<Uint32>::max() / 4U ||
        texture.height > std::numeric_limits<Uint32>::max() / (texture.width * 4U) ||
        texture.byteCount != static_cast<std::size_t>(texture.width) * texture.height * 4U)
    {
        error = "invalid SDL GPU initialization input";
        impl_->error = error;
        return false;
    }
    std::vector<std::uint8_t> vertexCode;
    std::vector<std::uint8_t> extendedVertexCode;
    std::vector<std::uint8_t> gameVertexCode;
    std::vector<std::uint8_t> fragmentCode;
    if (!ReadShader(vertexShaderPath, vertexCode, error) ||
        !ReadShader(fragmentShaderPath, fragmentCode, error) ||
        (extendedVertexShaderPath &&
         !ReadShader(extendedVertexShaderPath, extendedVertexCode, error)) ||
        (gameVertexShaderPath &&
         !ReadShader(gameVertexShaderPath, gameVertexCode, error)))
    {
        impl_->error = error;
        return false;
    }
    impl_->window = SDL_GetWindowFromID(static_cast<SDL_WindowID>(windowId));
    if (!impl_->window)
    {
        error = "SDL window token is not live";
        impl_->error = error;
        return false;
    }
    impl_->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");
    if (!impl_->device || !SDL_ClaimWindowForGPUDevice(impl_->device, impl_->window))
    {
        error = SDL_GetError();
        impl_->error = error;
        Shutdown();
        return false;
    }
    impl_->windowClaimed = true;
    impl_->colorTargetFormat = SDL_GetGPUSwapchainTextureFormat(impl_->device, impl_->window);
    if (impl_->colorTargetFormat == SDL_GPU_TEXTUREFORMAT_INVALID)
    {
        error = "swapchain texture format is invalid";
        impl_->error = error;
        Shutdown();
        return false;
    }
    impl_->vertexShader = CreateShader(
        impl_->device, vertexCode, SDL_GPU_SHADERSTAGE_VERTEX, 0U, 1U);
    if (extendedVertexShaderPath)
        impl_->extendedVertexShader = CreateShader(
            impl_->device, extendedVertexCode, SDL_GPU_SHADERSTAGE_VERTEX, 0U, 1U);
    if (gameVertexShaderPath)
        impl_->gameVertexShader = CreateShader(
            impl_->device, gameVertexCode, SDL_GPU_SHADERSTAGE_VERTEX, 0U, 1U);
    impl_->fragmentShader = CreateShader(
        impl_->device, fragmentCode, SDL_GPU_SHADERSTAGE_FRAGMENT, 2U, 1U);
    if (!impl_->vertexShader || !impl_->fragmentShader ||
        (extendedVertexShaderPath && !impl_->extendedVertexShader) ||
        (gameVertexShaderPath && !impl_->gameVertexShader))
    {
        error = SDL_GetError();
        impl_->error = error;
        Shutdown();
        return false;
    }

    impl_->pipeline = CreateGraphicsPipeline(
        impl_->device, impl_->vertexShader, impl_->fragmentShader,
        impl_->colorTargetFormat, impl_->renderState, 0U);
    impl_->linePipeline = CreateGraphicsPipeline(
        impl_->device, impl_->vertexShader, impl_->fragmentShader,
        impl_->colorTargetFormat, impl_->renderState, 0U,
        SDL_GPU_PRIMITIVETYPE_LINELIST);
    if (impl_->extendedVertexShader)
        impl_->extendedPipeline = CreateGraphicsPipeline(
            impl_->device, impl_->extendedVertexShader, impl_->fragmentShader,
            impl_->colorTargetFormat, impl_->renderState, 1U);
    if (impl_->gameVertexShader)
        impl_->gamePipeline = CreateGraphicsPipeline(
            impl_->device, impl_->gameVertexShader, impl_->fragmentShader,
            impl_->colorTargetFormat, impl_->renderState, 2U);
    if (impl_->extendedVertexShader)
        impl_->appPipeline = CreateGraphicsPipeline(
            impl_->device, impl_->extendedVertexShader, impl_->fragmentShader,
            impl_->colorTargetFormat, impl_->renderState, 3U);
    impl_->sampler = CreateSampler(impl_->device, impl_->samplerState);
    impl_->samplerCache[Impl::SamplerKey(impl_->samplerState)] = impl_->sampler;
    if (impl_->sampler) ++impl_->samplerCreations;
    impl_->secondSampler = impl_->sampler;
    impl_->secondSamplerState.stage = 1U;
    if (!impl_->pipeline || !impl_->linePipeline || !impl_->sampler || !impl_->secondSampler ||
        (impl_->extendedVertexShader && !impl_->extendedPipeline) ||
        (impl_->gameVertexShader && !impl_->gamePipeline) ||
        (impl_->extendedVertexShader && !impl_->appPipeline))
    {
        error = SDL_GetError();
        impl_->error = error;
        Shutdown();
        return false;
    }
    impl_->texturePixels.assign(texture.rgba8, texture.rgba8 + texture.byteCount);
    impl_->textureWidth = texture.width;
    impl_->textureHeight = texture.height;
    const std::array<std::uint8_t, 4> white{{255U, 255U, 255U, 255U}};
    if (RegisterTexture({white.data(), 1U, 1U, white.size()},
                        impl_->nullTextureHandle) != GR_OK)
    {
        error = impl_->error.empty() ? "SDL GPU null texture upload failed" : impl_->error;
        Shutdown();
        return false;
    }
    try
    {
        impl_->pipelineCache.emplace(Impl::Key(impl_->renderState),
            Impl::CachedPipeline{impl_->ActivePipelines(), ++impl_->pipelineUse});
    }
    catch (const std::bad_alloc &)
    {
        error = "SDL GPU initial pipeline cache allocation failed";
        impl_->error = error;
        Shutdown();
        return false;
    }
    ++impl_->pipelineRebuilds;
    return true;
}

GENRESULT SdlGpuRenderer::Submit(const RenderSubmissionView & submission)
{
    if (submission.topology != PrimitiveTopology::TriangleList &&
        submission.topology != PrimitiveTopology::LineList &&
        submission.topology != PrimitiveTopology::TriangleStrip)
        return GR_NOT_IMPLEMENTED;
    if (!submission.IsValid()) return GR_INVALID_PARMS;
    if (submission.topology == PrimitiveTopology::TriangleStrip)
    {
        std::vector<RenderVertexP3DiffuseT2> vertices;
        const GENRESULT result = ExpandTriangleStrip(submission, vertices);
        if (result != GR_OK) return result;
        return SubmitBytes(vertices.data(), vertices.size(), sizeof(RenderVertexP3DiffuseT2),
                           nullptr, 0U, 0U, PrimitiveTopology::TriangleList);
    }
    return SubmitBytes(submission.vertices, submission.vertexCount,
                       sizeof(RenderVertexP3DiffuseT2), submission.indices,
                       submission.indexCount, 0U, submission.topology);
}

GENRESULT SdlGpuRenderer::Submit(const RenderSubmissionView2 & submission)
{
    if (submission.topology != PrimitiveTopology::TriangleList &&
        submission.topology != PrimitiveTopology::TriangleStrip)
        return GR_NOT_IMPLEMENTED;
    if (!submission.IsValid()) return GR_INVALID_PARMS;
    if (submission.topology == PrimitiveTopology::TriangleStrip)
    {
        std::vector<RenderVertexP3DiffuseT4> vertices;
        const GENRESULT result = ExpandTriangleStrip(submission, vertices);
        if (result != GR_OK) return result;
        return SubmitBytes(vertices.data(), vertices.size(), sizeof(RenderVertexP3DiffuseT4),
                           nullptr, 0U, 1U, PrimitiveTopology::TriangleList);
    }
    return SubmitBytes(submission.vertices, submission.vertexCount,
                       sizeof(RenderVertexP3DiffuseT4), submission.indices,
                       submission.indexCount, 1U);
}

GENRESULT SdlGpuRenderer::Submit(const RenderSubmissionView3 & submission)
{
    if (submission.topology != PrimitiveTopology::TriangleList)
        return GR_NOT_IMPLEMENTED;
    if (!submission.IsValid()) return GR_INVALID_PARMS;
    return SubmitBytes(submission.vertices, submission.vertexCount,
                       sizeof(RenderVertexP3DiffuseSpecularT4), submission.indices,
                       submission.indexCount, 2U);
}

GENRESULT SdlGpuRenderer::Submit(const RenderSubmissionView4 & submission)
{
    if (submission.topology != PrimitiveTopology::TriangleList)
        return GR_NOT_IMPLEMENTED;
    if (!submission.IsValid()) return GR_INVALID_PARMS;
    return SubmitBytes(submission.vertices, submission.vertexCount,
                       sizeof(RenderVertexP3N3DiffuseT5), submission.indices,
                       submission.indexCount, 3U);
}

GENRESULT SdlGpuRenderer::SubmitBytes(const void * vertices, std::size_t vertexCount,
                                     std::size_t vertexStride,
                                     const std::uint16_t * indices,
                                     std::size_t indexCount, unsigned vertexFormat,
                                     PrimitiveTopology topology)
{
    CpuTimer timer(impl_->submissionTicks, impl_->timingsEnabled);
    if (impl_->timingsEnabled) ++impl_->submissionCount;
    if (topology == PrimitiveTopology::LineList && !impl_->frameCapture)
        return GR_NOT_IMPLEMENTED;
    if (!impl_->device || (vertexFormat == 1U && !impl_->extendedPipeline) ||
        (vertexFormat == 2U && !impl_->gamePipeline) ||
        (vertexFormat == 3U && !impl_->appPipeline))
        return GR_NOT_IMPLEMENTED;
    if (vertexCount > std::numeric_limits<Uint32>::max() / vertexStride ||
        indexCount >
            std::numeric_limits<Uint32>::max() / sizeof(std::uint16_t))
        return GR_INVALID_PARMS;

    const std::size_t vertexBytes =
        vertexCount * vertexStride;
    const std::size_t indexBytes = indexCount * sizeof(std::uint16_t);
    const std::size_t textureOffsetValue = (vertexBytes + indexBytes + 3U) & ~std::size_t{3U};
    // Registered textures supply both stages for production draws. Only the
    // explicit fixture handle needs this per-submission fallback upload.
    const bool needsFallback = !impl_->frameCapture || impl_->currentTextureHandle == 1U ||
        impl_->secondTextureHandle == 1U;
    const std::size_t textureBytes = needsFallback ? impl_->texturePixels.size() : 0U;
    if (textureOffsetValue + textureBytes >
        std::numeric_limits<Uint32>::max()) return GR_INVALID_PARMS;
    const auto vertexByteCount = static_cast<Uint32>(vertexBytes);
    const auto indexByteCount = static_cast<Uint32>(indexBytes);
    const auto textureOffset = static_cast<Uint32>(textureOffsetValue);
    const auto totalByteCount = static_cast<Uint32>(
        textureOffsetValue + textureBytes);

    // Captured draws keep distinct upload resources until the idle-waited
    // BeginFrame boundary. Only the immediate path replaces live resources.
    if (!impl_->frameCapture && !SDL_WaitForGPUIdle(impl_->device))
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    SubmissionResources candidate(impl_->device);
    if (impl_->frameCapture)
    {
        auto best = impl_->freeUploads.end();
        for (auto it = impl_->freeUploads.begin(); it != impl_->freeUploads.end(); ++it)
            if ((it->index != nullptr) == (indices != nullptr) &&
                it->vertexBytes >= vertexByteCount && it->indexBytes >= indexByteCount &&
                it->transferBytes >= totalByteCount &&
                (best == impl_->freeUploads.end() || it->Bytes() < best->Bytes()))
                best = it;
        if (best != impl_->freeUploads.end())
        {
            candidate.vertex = best->vertex;
            candidate.index = best->index;
            candidate.transfer = best->transfer;
            candidate.vertexBytes = best->vertexBytes;
            candidate.indexBytes = best->indexBytes;
            candidate.transferBytes = best->transferBytes;
            impl_->freeUploadBytes -= best->Bytes();
            *best = impl_->freeUploads.back();
            impl_->freeUploads.pop_back();
            if (impl_->timingsEnabled) ++impl_->reusedUploads;
        }
    }

    const SDL_GPUBufferCreateInfo vertexInfo{
        SDL_GPU_BUFFERUSAGE_VERTEX, vertexByteCount, 0U};
    if (!candidate.vertex)
    {
        candidate.vertex = SDL_CreateGPUBuffer(impl_->device, &vertexInfo);
        candidate.vertexBytes = vertexByteCount;
    }
    if (indices && !candidate.index)
    {
        const SDL_GPUBufferCreateInfo indexInfo{
            SDL_GPU_BUFFERUSAGE_INDEX, indexByteCount, 0U};
        candidate.index = SDL_CreateGPUBuffer(impl_->device, &indexInfo);
        candidate.indexBytes = indexByteCount;
    }
    const SDL_GPUTextureCreateInfo textureInfo{
        SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        SDL_GPU_TEXTUREUSAGE_SAMPLER, impl_->textureWidth, impl_->textureHeight,
        1U, 1U, SDL_GPU_SAMPLECOUNT_1, 0U};
    if (needsFallback) candidate.color = SDL_CreateGPUTexture(impl_->device, &textureInfo);
    const SDL_GPUTransferBufferCreateInfo transferInfo{
        SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, totalByteCount, 0U};
    if (!candidate.transfer)
    {
        candidate.transfer = SDL_CreateGPUTransferBuffer(impl_->device, &transferInfo);
        candidate.transferBytes = totalByteCount;
    }
    if (!candidate.vertex || (indices && !candidate.index) ||
        (needsFallback && !candidate.color) || !candidate.transfer)
    {
        impl_->error = SDL_GetError();
        return GR_OUT_OF_MEMORY;
    }
    void * mapped = SDL_MapGPUTransferBuffer(impl_->device, candidate.transfer, false);
    if (!mapped)
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    std::memcpy(mapped, vertices, vertexBytes);
    if (indices)
        std::memcpy(static_cast<std::uint8_t *>(mapped) + vertexByteCount,
                    indices, indexBytes);
    if (needsFallback)
        std::memcpy(static_cast<std::uint8_t *>(mapped) + textureOffset,
                    impl_->texturePixels.data(), textureBytes);
    SDL_UnmapGPUTransferBuffer(impl_->device, candidate.transfer);

    SDL_GPUCommandBuffer * commandBuffer = SDL_AcquireGPUCommandBuffer(impl_->device);
    if (!commandBuffer) return GR_GENERIC;
    SDL_GPUCopyPass * copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (!copyPass)
    {
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return GR_GENERIC;
    }
    const SDL_GPUTransferBufferLocation vertexSource{candidate.transfer, 0U};
    const SDL_GPUBufferRegion vertexDestination{
        candidate.vertex, 0U, vertexByteCount};
    SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, false);
    if (indices)
    {
        const SDL_GPUTransferBufferLocation indexSource{
            candidate.transfer, vertexByteCount};
        const SDL_GPUBufferRegion indexDestination{
            candidate.index, 0U, indexByteCount};
        SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, false);
    }
    const SDL_GPUTextureTransferInfo textureSource{
        candidate.transfer, textureOffset, impl_->textureWidth, impl_->textureHeight};
    const SDL_GPUTextureRegion textureDestination{
        candidate.color, 0U, 0U, 0U, 0U, 0U,
        impl_->textureWidth, impl_->textureHeight, 1U};
    if (needsFallback)
        SDL_UploadToGPUTexture(copyPass, &textureSource, &textureDestination, false);
    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    if (impl_->frameCapture)
    {
        SDL_GPUTexture * boundTexture = candidate.color;
        if (impl_->currentTextureHandle != 1U)
        {
            const auto found = impl_->registeredTextures.find(
                impl_->currentTextureHandle == 0U ? impl_->nullTextureHandle :
                                                    impl_->currentTextureHandle);
            boundTexture = found == impl_->registeredTextures.end()
                ? nullptr : found->second.texture;
        }
        SDL_GPUGraphicsPipeline * selectedPipeline =
            topology == PrimitiveTopology::LineList ? impl_->linePipeline :
            vertexFormat == 3U ? impl_->appPipeline :
            vertexFormat == 2U ? impl_->gamePipeline :
            vertexFormat == 1U ? impl_->extendedPipeline : impl_->pipeline;
        const auto secondFound = impl_->registeredTextures.find(
            impl_->secondTextureHandle ? impl_->secondTextureHandle : impl_->nullTextureHandle);
        SDL_GPUTexture * secondTexture = impl_->secondTextureHandle == 1U ?
            candidate.color : secondFound == impl_->registeredTextures.end() ?
                nullptr : secondFound->second.texture;
        if (!boundTexture || !secondTexture || !selectedPipeline) return GR_NOT_IMPLEMENTED;
        try
        {
            impl_->frameDraws.push_back(Impl::FrameDraw{
                candidate.vertex, candidate.index, candidate.transfer, candidate.color,
                boundTexture, secondTexture, impl_->secondSampler, impl_->sampler, selectedPipeline,
                static_cast<Uint32>(vertexCount), static_cast<Uint32>(indexCount),
                impl_->viewport, impl_->renderState, impl_->textureStage,
                impl_->modelview, impl_->projection,
                vertexFormat == 3U ? impl_->vertexUv : FixedFunctionUv,
                vertexFormat == 3U ? impl_->vertexColor : FixedFunctionColor,
                candidate.vertexBytes, candidate.indexBytes, candidate.transferBytes});
        }
        catch (const std::bad_alloc &)
        {
            return GR_OUT_OF_MEMORY;
        }
    }
    else
    {
        if (impl_->vertexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->vertexBuffer);
        if (impl_->indexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->indexBuffer);
        if (impl_->transferBuffer)
            SDL_ReleaseGPUTransferBuffer(impl_->device, impl_->transferBuffer);
        if (impl_->colorTexture) SDL_ReleaseGPUTexture(impl_->device, impl_->colorTexture);
    }
    impl_->vertexBuffer = candidate.vertex;
    impl_->indexBuffer = candidate.index;
    impl_->transferBuffer = candidate.transfer;
    impl_->colorTexture = candidate.color;
    candidate.vertex = nullptr;
    candidate.index = nullptr;
    candidate.transfer = nullptr;
    candidate.color = nullptr;
    impl_->vertexCount = static_cast<Uint32>(vertexCount);
    impl_->indexCount = static_cast<Uint32>(indexCount);
    impl_->activeVertexFormat = vertexFormat;
    return GR_OK;
}

GENRESULT SdlGpuRenderer::SetModelview(const Transform & modelview)
{
    if (!impl_->device) return GR_NOT_IMPLEMENTED;
    std::array<float, 16> candidate{{
        modelview.d[0][0], modelview.d[0][1], modelview.d[0][2], modelview.translation.x,
        modelview.d[1][0], modelview.d[1][1], modelview.d[1][2], modelview.translation.y,
        modelview.d[2][0], modelview.d[2][1], modelview.d[2][2], modelview.translation.z,
        0.0F, 0.0F, 0.0F, 1.0F}};
    for (float value : candidate)
        if (!std::isfinite(value)) return GR_INVALID_PARMS;
    impl_->modelview = candidate;
    return GR_OK;
}

GENRESULT SdlGpuRenderer::SetProjection(const Matrix4 & projection)
{
    if (!impl_->device) return GR_NOT_IMPLEMENTED;
    std::array<float, 16> candidate{};
    for (std::size_t row = 0U; row < 4U; ++row)
        for (std::size_t column = 0U; column < 4U; ++column)
        {
            const float value = projection.d[row][column];
            if (!std::isfinite(value)) return GR_INVALID_PARMS;
            candidate[row * 4U + column] = value;
        }
    impl_->projection = candidate;
    return GR_OK;
}

GENRESULT SdlGpuRenderer::SetViewport(int x, int y, int width, int height)
{
    if (!impl_->device) return GR_NOT_IMPLEMENTED;
    if (x < 0 || y < 0 || width <= 0 || height <= 0 ||
        x > INT32_MAX - width || y > INT32_MAX - height)
        return GR_INVALID_PARMS;
    impl_->viewport = SDL_Rect{x, y, width, height};
    return GR_OK;
}

GENRESULT SdlGpuRenderer::SetVertexConstants(std::uint32_t firstRegister,
                                             const float * values,
                                             std::uint32_t registerCount)
{
    if (!impl_->device) return GR_NOT_IMPLEMENTED;
    if (!values || !registerCount || firstRegister < 30U ||
        firstRegister > 31U || registerCount > 32U - firstRegister)
        return GR_INVALID_PARMS;
    for (std::uint32_t index = 0U; index < registerCount * 4U; ++index)
        if (!std::isfinite(values[index])) return GR_INVALID_PARMS;
    if (firstRegister == 30U)
        std::copy_n(values, 4U, impl_->vertexUv.begin());
    if (firstRegister + registerCount == 32U)
        std::copy_n(values + (31U - firstRegister) * 4U, 4U,
                    impl_->vertexColor.begin());
    return GR_OK;
}

GENRESULT SdlGpuRenderer::SetRenderState(const RenderStateSnapshot & state)
{
    if (!impl_->device || !IsRenderBoolean(state.depthEnabled) ||
        !IsRenderBoolean(state.depthWriteEnabled) ||
        !IsRenderBoolean(state.alphaBlendEnabled) ||
        !IsRenderBoolean(state.alphaTestEnabled) ||
        !IsRenderDepthFunction(state.alphaFunction) || state.alphaReference > 255U ||
        !IsRenderBoolean(state.specularEnabled) ||
        !IsRenderDepthFunction(state.depthFunction) ||
        !IsRenderBlendFactor(state.sourceBlend) ||
        !IsRenderBlendFactor(state.destinationBlend) ||
        state.cullMode < D3DCULL_NONE || state.cullMode > D3DCULL_CCW)
    {
        return GR_INVALID_PARMS;
    }
    const RenderStateSnapshot & current = impl_->renderState;
    if (state.depthEnabled == current.depthEnabled &&
        state.depthWriteEnabled == current.depthWriteEnabled &&
        state.depthFunction == current.depthFunction &&
        state.alphaBlendEnabled == current.alphaBlendEnabled &&
        state.sourceBlend == current.sourceBlend &&
        state.destinationBlend == current.destinationBlend &&
        state.cullMode == current.cullMode)
    {
        // Specular, texture factor and alpha testing are fragment uniforms only.
        impl_->renderState = state;
        impl_->pipelineCache.at(Impl::Key(state)).lastUse = ++impl_->pipelineUse;
        return GR_OK;
    }
    const auto key = Impl::Key(state);
    const auto cached = impl_->pipelineCache.find(key);
    if (cached != impl_->pipelineCache.end())
    {
        // Immutable pipelines remain owned by the cache; captured draws can
        // keep using the previous set without an idle wait or replacement.
        impl_->BindPipelines(cached->second.set);
        cached->second.lastUse = ++impl_->pipelineUse;
        impl_->renderState = state;
        return GR_OK;
    }
    SDL_GPUGraphicsPipeline * pipeline = CreateGraphicsPipeline(
        impl_->device, impl_->vertexShader, impl_->fragmentShader,
        impl_->colorTargetFormat, state, 0U);
    if (!pipeline)
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    SDL_GPUGraphicsPipeline * linePipeline = CreateGraphicsPipeline(
        impl_->device, impl_->vertexShader, impl_->fragmentShader,
        impl_->colorTargetFormat, state, 0U, SDL_GPU_PRIMITIVETYPE_LINELIST);
    if (!linePipeline)
    {
        impl_->error = SDL_GetError();
        SDL_ReleaseGPUGraphicsPipeline(impl_->device, pipeline);
        return GR_GENERIC;
    }
    SDL_GPUGraphicsPipeline * extendedPipeline = nullptr;
    if (impl_->extendedVertexShader)
    {
        extendedPipeline = CreateGraphicsPipeline(
            impl_->device, impl_->extendedVertexShader, impl_->fragmentShader,
            impl_->colorTargetFormat, state, 1U);
        if (!extendedPipeline)
        {
            impl_->error = SDL_GetError();
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, pipeline);
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, linePipeline);
            return GR_GENERIC;
        }
    }
    SDL_GPUGraphicsPipeline * gamePipeline = nullptr;
    if (impl_->gameVertexShader)
    {
        gamePipeline = CreateGraphicsPipeline(
            impl_->device, impl_->gameVertexShader, impl_->fragmentShader,
            impl_->colorTargetFormat, state, 2U);
        if (!gamePipeline)
        {
            impl_->error = SDL_GetError();
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, pipeline);
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, linePipeline);
            if (extendedPipeline)
                SDL_ReleaseGPUGraphicsPipeline(impl_->device, extendedPipeline);
            return GR_GENERIC;
        }
    }
    SDL_GPUGraphicsPipeline * appPipeline = nullptr;
    if (impl_->extendedVertexShader)
    {
        appPipeline = CreateGraphicsPipeline(
            impl_->device, impl_->extendedVertexShader, impl_->fragmentShader,
            impl_->colorTargetFormat, state, 3U);
        if (!appPipeline)
        {
            impl_->error = SDL_GetError();
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, pipeline);
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, linePipeline);
            if (extendedPipeline)
                SDL_ReleaseGPUGraphicsPipeline(impl_->device, extendedPipeline);
            if (gamePipeline)
                SDL_ReleaseGPUGraphicsPipeline(impl_->device, gamePipeline);
            return GR_GENERIC;
        }
    }
    if (!SDL_WaitForGPUIdle(impl_->device))
    {
        impl_->error = SDL_GetError();
        SDL_ReleaseGPUGraphicsPipeline(impl_->device, pipeline);
        SDL_ReleaseGPUGraphicsPipeline(impl_->device, linePipeline);
        if (extendedPipeline)
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, extendedPipeline);
        if (gamePipeline)
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, gamePipeline);
        if (appPipeline)
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, appPipeline);
        return GR_GENERIC;
    }
    const Impl::PipelineSet candidate{pipeline, linePipeline, extendedPipeline,
                                       gamePipeline, appPipeline};
    try
    {
        const auto inserted = impl_->pipelineCache.emplace(key,
            Impl::CachedPipeline{candidate, impl_->pipelineUse + 1U});
        if (!inserted.second)
        {
            impl_->ReleasePipelines(candidate);
            return GR_GENERIC;
        }
    }
    catch (const std::bad_alloc &)
    {
        impl_->ReleasePipelines(candidate);
        return GR_OUT_OF_MEMORY;
    }
    ++impl_->pipelineUse;
    impl_->BindPipelines(candidate);
    if (!impl_->frameCapture) impl_->TrimPipelineCache();
    impl_->renderState = state;
    ++impl_->pipelineRebuilds;
    return GR_OK;
}

GENRESULT SdlGpuRenderer::SetTextureStageState(const TextureStageSnapshot & state)
{
    if (!impl_->device) return GR_NOT_IMPLEMENTED;
    if (!IsTextureStageOperation(state.colorOperation) ||
        !IsTextureStageOperation(state.alphaOperation) ||
        !IsTextureStageArgument(state.colorArgument1) ||
        !IsTextureStageArgument(state.colorArgument2) ||
        !IsTextureStageArgument(state.alphaArgument1) ||
        !IsTextureStageArgument(state.alphaArgument2) ||
        state.textureCoordinateIndex > 1U ||
        !IsTextureStageOperation(state.second.colorOperation) ||
        !IsTextureStageOperation(state.second.alphaOperation) ||
        !IsTextureStageArgument(state.second.colorArgument1) ||
        !IsTextureStageArgument(state.second.colorArgument2) ||
        !IsTextureStageArgument(state.second.alphaArgument1) ||
        !IsTextureStageArgument(state.second.alphaArgument2) ||
        state.second.textureCoordinateIndex > 1U)
    {
        return GR_NOT_IMPLEMENTED;
    }
    impl_->textureStage = state;
    return GR_OK;
}

GENRESULT SdlGpuRenderer::SetSamplerState(const SamplerStateSnapshot & state)
{
    CpuTimer timer(impl_->samplerTicks, impl_->timingsEnabled);
    if (!impl_->device || state.stage > 1U) return GR_NOT_IMPLEMENTED;
    if ((state.addressU != D3DTADDRESS_WRAP && state.addressU != D3DTADDRESS_CLAMP) ||
        (state.addressV != D3DTADDRESS_WRAP && state.addressV != D3DTADDRESS_CLAMP) ||
        (state.minFilter != D3DTEXF_POINT && state.minFilter != D3DTEXF_LINEAR) ||
        (state.magFilter != D3DTEXF_POINT && state.magFilter != D3DTEXF_LINEAR) ||
        (state.mipFilter != D3DTEXF_POINT && state.mipFilter != D3DTEXF_LINEAR))
        return GR_NOT_IMPLEMENTED;
    SDL_GPUSampler *& selected = state.stage == 0U ? impl_->sampler : impl_->secondSampler;
    const auto & current = state.stage == 0U ? impl_->samplerState : impl_->secondSamplerState;
    if (selected && state.addressU == current.addressU && state.addressV == current.addressV &&
        state.minFilter == current.minFilter && state.magFilter == current.magFilter &&
        state.mipFilter == current.mipFilter)
        return GR_OK;
    SDL_GPUSampler * candidate = impl_->samplerCache[Impl::SamplerKey(state)];
    if (!candidate)
    {
        candidate = CreateSampler(impl_->device, state);
        if (!candidate)
        {
            impl_->error = SDL_GetError();
            return GR_GENERIC;
        }
        impl_->samplerCache[Impl::SamplerKey(state)] = candidate;
        ++impl_->samplerCreations;
    }
    selected = candidate;
    if (impl_->timingsEnabled) ++impl_->samplerChanges;
    (state.stage == 0U ? impl_->samplerState : impl_->secondSamplerState) = state;
    return GR_OK;
}

GENRESULT SdlGpuRenderer::RegisterTexture(SdlGpuTextureView texture, std::uint32_t &handle)
{
    const NativeTextureLevel level{texture.rgba8, texture.width, texture.height, texture.byteCount};
    return RegisterMipTexture(&level, 1U, handle);
}

GENRESULT SdlGpuRenderer::RegisterMipTexture(const NativeTextureLevel *levels,
                                             std::uint32_t count, std::uint32_t &handle)
{
    CpuTimer timer(impl_->textureUploadTicks, impl_->timingsEnabled);
    if (impl_->timingsEnabled) ++impl_->textureUploadCount;
    handle = 0U;
    if (!impl_->device || !levels || !count || count > 31U ||
        impl_->nextTextureHandle > kMaxTextureStateId) return GR_INVALID_PARMS;
    const auto &base = levels[0];
    if (!base.width || !base.height) return GR_INVALID_PARMS;
    U32 maxLevels = 1;
    for (U32 dimension = (std::max)(base.width, base.height); dimension > 1; dimension >>= 1)
        ++maxLevels;
    if (count > maxLevels) return GR_INVALID_PARMS;
    std::size_t bytes = 0;
    for (U32 index = 0; index < count; ++index)
    {
        const auto &level = levels[index];
        if (!level.pixels || level.width != (std::max)(1U, base.width >> index) ||
            level.height != (std::max)(1U, base.height >> index) ||
            level.width > std::numeric_limits<U32>::max() / 4U ||
            level.height > std::numeric_limits<U32>::max() / (level.width * 4U) ||
            level.byteCount != static_cast<std::size_t>(level.width) * level.height * 4U ||
            level.byteCount > std::numeric_limits<U32>::max() - bytes) return GR_INVALID_PARMS;
        bytes += level.byteCount;
    }
    SubmissionResources candidate(impl_->device);
    const SDL_GPUTextureCreateInfo textureInfo{
        SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        SDL_GPU_TEXTUREUSAGE_SAMPLER, base.width, base.height,
        1U, count, SDL_GPU_SAMPLECOUNT_1, 0U};
    const SDL_GPUTransferBufferCreateInfo transferInfo{
        SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, static_cast<U32>(bytes), 0U};
    candidate.color = SDL_CreateGPUTexture(impl_->device, &textureInfo);
    candidate.transfer = SDL_CreateGPUTransferBuffer(impl_->device, &transferInfo);
    if (!candidate.color || !candidate.transfer) { impl_->error = SDL_GetError(); return GR_OUT_OF_MEMORY; }
    auto *mapped = static_cast<std::uint8_t *>(SDL_MapGPUTransferBuffer(impl_->device, candidate.transfer, false));
    if (!mapped) { impl_->error = SDL_GetError(); return GR_GENERIC; }
    std::size_t offset = 0;
    for (U32 index = 0; index < count; ++index)
    {
        std::memcpy(mapped + offset, levels[index].pixels, levels[index].byteCount);
        offset += levels[index].byteCount;
    }
    SDL_UnmapGPUTransferBuffer(impl_->device, candidate.transfer);
    auto *commandBuffer = SDL_AcquireGPUCommandBuffer(impl_->device);
    if (!commandBuffer) { impl_->error = SDL_GetError(); return GR_GENERIC; }
    auto *copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (!copyPass) { SDL_CancelGPUCommandBuffer(commandBuffer); impl_->error = SDL_GetError(); return GR_GENERIC; }
    offset = 0;
    for (U32 index = 0; index < count; ++index)
    {
        const auto &level = levels[index];
        const SDL_GPUTextureTransferInfo source{candidate.transfer, static_cast<U32>(offset), level.width, level.height};
        const SDL_GPUTextureRegion destination{candidate.color, index, 0U, 0U, 0U, 0U, level.width, level.height, 1U};
        SDL_UploadToGPUTexture(copyPass, &source, &destination, false);
        offset += level.byteCount;
    }
    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer) || !SDL_WaitForGPUIdle(impl_->device))
    { impl_->error = SDL_GetError(); return GR_GENERIC; }
    try { impl_->registeredTextures.emplace(impl_->nextTextureHandle,
        Impl::RegisteredTexture{candidate.color, base.width, base.height, count}); }
    catch (const std::bad_alloc &) { return GR_OUT_OF_MEMORY; }
    handle = impl_->nextTextureHandle++;
    candidate.color = nullptr;
    return GR_OK;
}

GENRESULT SdlGpuRenderer::CreateMipTexture(std::uint32_t width, std::uint32_t height,
                                           std::uint32_t count, std::uint32_t &handle)
{
    handle = 0;
    if (!count || count > 31 || !width || !height || width > std::numeric_limits<U32>::max() / 4 ||
        height > std::numeric_limits<U32>::max() / (width * 4)) return GR_INVALID_PARMS;
    try
    {
        std::vector<std::vector<std::uint8_t>> storage;
        std::vector<NativeTextureLevel> levels;
        for (U32 index = 0; index < count; ++index)
        {
            const U32 w = (std::max)(1U, width >> index), h = (std::max)(1U, height >> index);
            storage.emplace_back(static_cast<std::size_t>(w) * h * 4U);
            levels.push_back({storage.back().data(), w, h, storage.back().size()});
        }
        return RegisterMipTexture(levels.data(), count, handle);
    }
    catch (const std::bad_alloc &) { return GR_OUT_OF_MEMORY; }
}

GENRESULT SdlGpuRenderer::CreateTexture(std::uint32_t width, std::uint32_t height,
                                         std::uint32_t & handle)
{
    handle = 0U;
    if (!width || !height || width > std::numeric_limits<Uint32>::max() / 4U ||
        height > std::numeric_limits<Uint32>::max() / (width * 4U))
        return GR_INVALID_PARMS;
    std::vector<std::uint8_t> empty;
    try
    {
        empty.resize(static_cast<std::size_t>(width) * height * 4U);
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_MEMORY;
    }
    return RegisterTexture({empty.data(), width, height, empty.size()}, handle);
}

GENRESULT SdlGpuRenderer::ReplaceTexture(std::uint32_t handle, SdlGpuTextureView texture)
{
    const NativeTextureLevel level{texture.rgba8, texture.width, texture.height, texture.byteCount};
    return ReplaceMipTexture(handle, &level, 1U);
}

GENRESULT SdlGpuRenderer::ReplaceMipTexture(std::uint32_t handle,
                                            const NativeTextureLevel *levels, std::uint32_t count)
{
    if (!levels || !count || handle == impl_->nullTextureHandle) return GR_INVALID_PARMS;
    const auto found = impl_->registeredTextures.find(handle);
    if (found == impl_->registeredTextures.end() ||
        found->second.width != levels[0].width ||
        found->second.height != levels[0].height || found->second.levels != count)
        return GR_INVALID_PARMS;
    const bool captured = std::any_of(impl_->frameDraws.begin(), impl_->frameDraws.end(),
        [&](const auto &draw) { return (draw.boundTexture == found->second.texture || draw.secondTexture == found->second.texture); });
    if (captured)
    {
        try { impl_->retiredTextures.push_back(found->second.texture); }
        catch (const std::bad_alloc &) { return GR_OUT_OF_MEMORY; }
    }
    U32 candidateHandle = 0U;
    const GENRESULT result = RegisterMipTexture(levels, count, candidateHandle);
    if (result != GR_OK)
    {
        if (captured) impl_->retiredTextures.pop_back();
        return result;
    }
    const auto candidate = impl_->registeredTextures.find(candidateHandle);
    SDL_GPUTexture * previous = found->second.texture;
    found->second.texture = candidate->second.texture;
    impl_->registeredTextures.erase(candidate);
    if (!captured) SDL_ReleaseGPUTexture(impl_->device, previous);
    return GR_OK;
}

GENRESULT SdlGpuRenderer::DestroyTexture(std::uint32_t handle)
{
    if (!impl_->device || (handle == impl_->currentTextureHandle || handle == impl_->secondTextureHandle) ||
        handle == impl_->nullTextureHandle)
        return GR_INVALID_PARMS;
    const auto found = impl_->registeredTextures.find(handle);
    if (found == impl_->registeredTextures.end()) return GR_INVALID_PARMS;
    const bool captured = std::any_of(impl_->frameDraws.begin(), impl_->frameDraws.end(),
        [&](const auto &draw) { return draw.boundTexture == found->second.texture ||
                                      draw.secondTexture == found->second.texture; });
    if (captured)
    {
        // The source MovieScreen can release its last frame during CQE_UPDATE,
        // before BeginFrame retires the preceding presentation's captured draws.
        // Invalidate the logical handle now, retaining its GPU storage through
        // the existing idle-waited frame retirement boundary.
        try { impl_->retiredTextures.push_back(found->second.texture); }
        catch (const std::bad_alloc &) { return GR_OUT_OF_MEMORY; }
        impl_->registeredTextures.erase(found);
        return GR_OK;
    }
    if (!SDL_WaitForGPUIdle(impl_->device))
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    SDL_ReleaseGPUTexture(impl_->device, found->second.texture);
    impl_->registeredTextures.erase(found);
    return GR_OK;
}

GENRESULT SdlGpuRenderer::BindTexture(std::uint32_t stage, std::uint32_t handle)
{
    if (stage > 1U || (handle != 0U && !IsTexture(handle))) return GR_NOT_IMPLEMENTED;
    (stage == 0U ? impl_->currentTextureHandle : impl_->secondTextureHandle) = handle;
    return GR_OK;
}

bool SdlGpuRenderer::IsTexture(std::uint32_t handle) const
{
    return impl_->device &&
        (handle == 1U || (handle != impl_->nullTextureHandle &&
                          impl_->registeredTextures.find(handle) !=
                             impl_->registeredTextures.end()));
}

GENRESULT DACOM_CDECL SdlGpuRenderer::SubmissionCallback(
    void * context, const RenderSubmissionView & submission)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->Submit(submission) : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::Submission2Callback(
    void * context, const RenderSubmissionView2 & submission)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->Submit(submission) : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::Submission3Callback(
    void * context, const RenderSubmissionView3 & submission)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->Submit(submission) : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::Submission4Callback(
    void * context, const RenderSubmissionView4 & submission)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->Submit(submission) : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::RenderStateCallback(
    void * context, const RenderStateSnapshot & state)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->SetRenderState(state)
                   : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::ModelviewCallback(
    void * context, const Transform & modelview)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->SetModelview(modelview)
                   : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::ProjectionCallback(
    void * context, const Matrix4 & projection)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->SetProjection(projection)
                   : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::ViewportCallback(
    void * context, int x, int y, int width, int height)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->SetViewport(
        x, y, width, height) : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::VertexConstantsCallback(
    void * context, UINT firstRegister, const FLOAT * values,
    UINT registerCount)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->SetVertexConstants(
                         firstRegister, values, registerCount)
                   : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::TextureStageCallback(
    void * context, const TextureStageSnapshot & state)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->SetTextureStageState(state)
                   : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::SamplerStateCallback(
    void * context, const SamplerStateSnapshot & state)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->SetSamplerState(state)
                   : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::TextureBindCallback(
    void * context, std::uint32_t stage, std::uint32_t handle)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->BindTexture(stage, handle)
                   : GR_INVALID_PARMS;
}

bool SdlGpuRenderer::TextureLookupCallback(void * context, std::uint32_t handle)
{
    return context && static_cast<SdlGpuRenderer *>(context)->IsTexture(handle);
}

GENRESULT SdlGpuRenderer::TextureCreateCallback(
    void * context, std::uint32_t width, std::uint32_t height,
    std::uint32_t & handle)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->CreateTexture(
                         width, height, handle) : GR_INVALID_PARMS;
}

GENRESULT SdlGpuRenderer::TextureUploadCallback(
    void * context, std::uint32_t handle, const std::uint8_t * pixels,
    std::uint32_t width, std::uint32_t height, std::size_t byteCount)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->ReplaceTexture(
                         handle, {pixels, width, height, byteCount}) : GR_INVALID_PARMS;
}

GENRESULT SdlGpuRenderer::TextureCreateMipsCallback(void *context, U32 width, U32 height,
    U32 count, U32 &handle)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->CreateMipTexture(width, height, count, handle) : GR_INVALID_PARMS;
}
GENRESULT SdlGpuRenderer::TextureUploadMipsCallback(void *context, U32 handle,
    const NativeTextureLevel *levels, U32 count)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->ReplaceMipTexture(handle, levels, count) : GR_INVALID_PARMS;
}

GENRESULT SdlGpuRenderer::TextureDestroyCallback(void * context, std::uint32_t handle)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->DestroyTexture(handle)
                   : GR_INVALID_PARMS;
}

GENRESULT SdlGpuRenderer::ClearFrameCallback(void * context, U32 color, U32 depth)
{
    if (!context) return GR_INVALID_PARMS;
    auto & renderer = *static_cast<SdlGpuRenderer *>(context);
    int width = 0, height = 0;
    const auto & viewport = renderer.impl_->viewport;
    if (!renderer.impl_->window ||
        !SDL_GetWindowSizeInPixels(renderer.impl_->window, &width, &height)) return GR_GENERIC;
    if ((viewport.w || viewport.h) &&
        (viewport.x || viewport.y || viewport.w != width || viewport.h != height))
    {
        renderer.impl_->error = "Native full-target clear viewport differs from window pixel size";
        return GR_NOT_IMPLEMENTED;
    }
    std::string error;
    if (!renderer.BeginFrame(error)) { renderer.impl_->error = error; return GR_GENERIC; }
    renderer.impl_->clearColor = SDL_FColor{
        ((color >> 16U) & 255U) / 255.0F, ((color >> 8U) & 255U) / 255.0F,
        (color & 255U) / 255.0F, 1.0F};
    renderer.impl_->clearDepth = static_cast<float>(depth / 4294967295.0);
    return GR_OK;
}

GENRESULT SdlGpuRenderer::PresentFrameCallback(void * context)
{
    if (!context) return GR_INVALID_PARMS;
    auto & renderer = *static_cast<SdlGpuRenderer *>(context);
    bool presented = false;
    std::string error;
    if (!renderer.Present(presented, error))
    { renderer.impl_->error = error; return GR_GENERIC; }
    return GR_OK;
}

bool SdlGpuRenderer::BeginFrame(std::string & error)
{
    CpuTimer timer(impl_->retirementTicks, impl_->timingsEnabled);
    error.clear();
    if (!impl_->device || !SDL_WaitForGPUIdle(impl_->device))
    {
        error = impl_->device ? SDL_GetError() : "SDL GPU device is not initialized";
        return false;
    }
    if (impl_->frameCapture)
    {
        // Captured buffers are exclusively reusable only after the idle wait
        // above. Reserve before transferring ownership so allocation failure
        // preserves the current captured frame and its resources.
        constexpr std::size_t uploadLimit = 512U;
        constexpr Uint64 budget = 64U * 1024U * 1024U;
        try { impl_->freeUploads.reserve(uploadLimit); }
        catch (const std::bad_alloc &)
        { error = "captured upload pool allocation failed"; return false; }
        // Reserve room for this frame before adding anything: trimming the
        // mixed pool's back would discard the just-retired active draws while
        // preserving stale entries that could not satisfy this frame's draws.
        std::size_t retainedCount = 0;
        Uint64 retainedBytes = 0;
        for (const auto &draw : impl_->frameDraws)
        {
            const Uint64 bytes = Uint64(draw.vertexBytes) + draw.indexBytes + draw.transferBytes;
            if (retainedCount < uploadLimit && bytes <= budget - retainedBytes)
            { ++retainedCount; retainedBytes += bytes; }
        }
        while (!impl_->freeUploads.empty() &&
            (impl_->freeUploads.size() + retainedCount > uploadLimit ||
             impl_->freeUploadBytes > budget - retainedBytes))
        {
            impl_->freeUploadBytes -= impl_->freeUploads.back().Bytes();
            impl_->ReleaseUpload(impl_->freeUploads.back());
            impl_->freeUploads.pop_back();
        }
        retainedCount = 0;
        retainedBytes = 0;
        for (const auto & draw : impl_->frameDraws)
        {
            const Impl::UploadSet upload{draw.vertex, draw.index, draw.transfer,
                draw.vertexBytes, draw.indexBytes, draw.transferBytes};
            if (retainedCount < uploadLimit && upload.Bytes() <= budget - retainedBytes)
            {
                impl_->freeUploads.push_back(upload);
                impl_->freeUploadBytes += upload.Bytes();
                ++retainedCount;
                retainedBytes += upload.Bytes();
            }
            else impl_->ReleaseUpload(upload);
            SDL_ReleaseGPUTexture(impl_->device, draw.uploadedTexture);
        }
        impl_->frameDraws.clear();
        for (SDL_GPUTexture * texture : impl_->retiredTextures)
            SDL_ReleaseGPUTexture(impl_->device, texture);
        impl_->retiredTextures.clear();
    }
    else
    {
        if (impl_->vertexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->vertexBuffer);
        if (impl_->indexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->indexBuffer);
        if (impl_->transferBuffer)
            SDL_ReleaseGPUTransferBuffer(impl_->device, impl_->transferBuffer);
        if (impl_->colorTexture) SDL_ReleaseGPUTexture(impl_->device, impl_->colorTexture);
    }
    impl_->vertexBuffer = nullptr;
    impl_->indexBuffer = nullptr;
    impl_->transferBuffer = nullptr;
    impl_->colorTexture = nullptr;
    impl_->vertexCount = 0U;
    impl_->indexCount = 0U;
    impl_->TrimPipelineCache();
    impl_->frameCapture = true;
    return true;
}

std::size_t SdlGpuRenderer::FrameSubmissionCount() const
{
    return impl_->frameDraws.size();
}

bool SdlGpuRenderer::DrawCapturedFrame(SDL_GPUCommandBuffer * commandBuffer,
                                       SDL_GPUColorTargetInfo * colorTarget,
                                       SDL_GPUDepthStencilTargetInfo * depthTarget,
                                       std::uint32_t width, std::uint32_t height,
                                       bool readback, std::string & error)
{
    CpuTimer timer(impl_->drawingTicks, impl_->timingsEnabled);
    // All captured draws use these same targets. Preserve source draw order
    // and per-draw state inside one pass instead of repeatedly storing/loading
    // the full color and depth targets between consecutive draws.
    auto * pass = SDL_BeginGPURenderPass(commandBuffer, colorTarget, 1U, depthTarget);
    if (!pass)
    {
        error = SDL_GetError();
        return false;
    }
    for (const auto & draw : impl_->frameDraws)
    {
        if (!draw.vertex || !draw.boundTexture || !draw.secondTexture ||
            !draw.pipeline || !draw.sampler || !draw.secondSampler)
        {
            SDL_EndGPURenderPass(pass);
            error = "captured GPU draw has an incomplete resource binding";
            return false;
        }
        const TextureStageUniform stage =
            MakeTextureStageUniform(draw.textureStage, draw.renderState);
        const VertexTransformUniform vertex{
            MakeClipMatrix(draw.projection, draw.modelview),
            draw.vertexUv, draw.vertexColor};
        SDL_PushGPUFragmentUniformData(commandBuffer, 0U, &stage, sizeof(stage));
        SDL_PushGPUVertexUniformData(commandBuffer, 0U, &vertex, sizeof(vertex));
        if (!ApplyViewport(pass, draw.viewport, width, height, readback))
        {
            SDL_EndGPURenderPass(pass);
            error = "captured render viewport exceeds target";
            return false;
        }
        SDL_BindGPUGraphicsPipeline(pass, draw.pipeline);
        const SDL_GPUBufferBinding vertexBinding{draw.vertex, 0U};
        SDL_BindGPUVertexBuffers(pass, 0U, &vertexBinding, 1U);
        const SDL_GPUTextureSamplerBinding textureBindings[2]{
            {draw.boundTexture, draw.sampler}, {draw.secondTexture, draw.secondSampler}};
        SDL_BindGPUFragmentSamplers(pass, 0U, textureBindings, 2U);
        if (draw.index)
        {
            const SDL_GPUBufferBinding indexBinding{draw.index, 0U};
            SDL_BindGPUIndexBuffer(pass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
            SDL_DrawGPUIndexedPrimitives(pass, draw.indexCount, 1U, 0U, 0, 0U);
        }
        else SDL_DrawGPUPrimitives(pass, draw.vertexCount, 1U, 0U, 0U);
    }
    SDL_EndGPURenderPass(pass);
    return true;
}

bool SdlGpuRenderer::Present(bool & presented, std::string & error)
{
    CpuTimer timer(impl_->presentationTicks, impl_->timingsEnabled);
    presented = false;
    error.clear();
    SDL_GPUTexture * boundTexture = impl_->colorTexture;
    if (impl_->currentTextureHandle != 1U)
    {
        const auto found = impl_->registeredTextures.find(
            impl_->currentTextureHandle == 0U ? impl_->nullTextureHandle :
                                                impl_->currentTextureHandle);
        boundTexture = found == impl_->registeredTextures.end() ? nullptr : found->second.texture;
    }
    if (!impl_->device || !impl_->pipeline || (!impl_->frameCapture && (!impl_->vertexBuffer || !boundTexture)))
    {
        error = "SDL GPU renderer has no submitted scene";
        return false;
    }
    SDL_GPUCommandBuffer * commandBuffer = nullptr;
    SDL_GPUTexture * swapchainTexture = nullptr;
    Uint32 width = 0U;
    Uint32 height = 0U;
    for (int attempt = 0; attempt < 2; ++attempt)
    {
        CpuTimer timer(impl_->acquisitionTicks, impl_->timingsEnabled);
        commandBuffer = SDL_AcquireGPUCommandBuffer(impl_->device);
        if (!commandBuffer) break;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(
                commandBuffer, impl_->window, &swapchainTexture, &width, &height))
        {
            SDL_CancelGPUCommandBuffer(commandBuffer);
            commandBuffer = nullptr;
            break;
        }
        if (!swapchainTexture || !width || !height)
            return SDL_SubmitGPUCommandBuffer(commandBuffer);
        if (impl_->depthTexture && impl_->depthWidth == width && impl_->depthHeight == height)
            break;
        if (!SDL_SubmitGPUCommandBuffer(commandBuffer) || !SDL_WaitForGPUIdle(impl_->device))
            return false;
        commandBuffer = nullptr;
        swapchainTexture = nullptr;
        if (impl_->depthTexture)
            SDL_ReleaseGPUTexture(impl_->device, impl_->depthTexture);
        const SDL_GPUTextureCreateInfo depthInfo{
            SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET, width, height, 1U, 1U,
            SDL_GPU_SAMPLECOUNT_1, 0U};
        impl_->depthTexture = SDL_CreateGPUTexture(impl_->device, &depthInfo);
        if (!impl_->depthTexture) break;
        impl_->depthWidth = width;
        impl_->depthHeight = height;
        ++impl_->depthRecreations;
    }
    if (!commandBuffer || !swapchainTexture || !impl_->depthTexture ||
        impl_->depthWidth != width || impl_->depthHeight != height)
    {
        error = SDL_GetError();
        impl_->error = error;
        return false;
    }
    SDL_GPUColorTargetInfo colorTarget{};
    colorTarget.texture = swapchainTexture;
    colorTarget.clear_color = impl_->clearColor;
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    SDL_GPUDepthStencilTargetInfo depthTarget{};
    depthTarget.texture = impl_->depthTexture;
    depthTarget.clear_depth = impl_->clearDepth;
    depthTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    depthTarget.store_op = impl_->frameCapture ? SDL_GPU_STOREOP_STORE :
                                                  SDL_GPU_STOREOP_DONT_CARE;
    depthTarget.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depthTarget.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
    if (impl_->frameCapture)
    {
        if (!DrawCapturedFrame(commandBuffer, &colorTarget, &depthTarget,
                               width, height, false, error))
        {
            // SDL forbids cancellation after acquiring a swapchain image.
            SDL_SubmitGPUCommandBuffer(commandBuffer);
            impl_->error = error;
            return false;
        }
        bool submitted;
        {
            CpuTimer timer(impl_->queueSubmitTicks, impl_->timingsEnabled);
            submitted = SDL_SubmitGPUCommandBuffer(commandBuffer);
        }
        if (!submitted)
        {
            error = SDL_GetError();
            impl_->error = error;
            return false;
        }
        presented = true;
        return true;
    }
    const TextureStageUniform textureStageUniform =
        MakeTextureStageUniform(impl_->textureStage, impl_->renderState);
    SDL_PushGPUFragmentUniformData(commandBuffer, 0U, &textureStageUniform,
                                   sizeof(textureStageUniform));
    const VertexTransformUniform vertexUniform{
        MakeClipMatrix(impl_->projection, impl_->modelview),
        impl_->activeVertexFormat == 3U ? impl_->vertexUv : FixedFunctionUv,
        impl_->activeVertexFormat == 3U ? impl_->vertexColor : FixedFunctionColor};
    SDL_PushGPUVertexUniformData(commandBuffer, 0U, &vertexUniform,
                                 sizeof(vertexUniform));
    SDL_GPURenderPass * renderPass =
        SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1U, &depthTarget);
    if (!renderPass)
    {
        // SDL forbids cancellation after acquiring a swapchain image.
        SDL_SubmitGPUCommandBuffer(commandBuffer);
        return false;
    }
    if (!ApplyViewport(renderPass, impl_->viewport, width, height))
    {
        SDL_EndGPURenderPass(renderPass);
        // SDL forbids cancellation after acquiring a swapchain image.
        SDL_SubmitGPUCommandBuffer(commandBuffer);
        error = "render viewport exceeds swapchain target";
        impl_->error = error;
        return false;
    }
    SDL_BindGPUGraphicsPipeline(renderPass,
        impl_->activeVertexFormat == 3U ? impl_->appPipeline :
        impl_->activeVertexFormat == 2U ? impl_->gamePipeline :
        impl_->activeVertexFormat == 1U ? impl_->extendedPipeline : impl_->pipeline);
    const SDL_GPUBufferBinding vertexBinding{impl_->vertexBuffer, 0U};
    SDL_BindGPUVertexBuffers(renderPass, 0U, &vertexBinding, 1U);
    const auto secondFound = impl_->registeredTextures.find(
        impl_->secondTextureHandle ? impl_->secondTextureHandle : impl_->nullTextureHandle);
    SDL_GPUTexture * secondTexture = impl_->secondTextureHandle == 1U ? impl_->colorTexture :
        secondFound == impl_->registeredTextures.end() ? nullptr : secondFound->second.texture;
    if (!secondTexture) { error = "missing second-stage texture"; return false; }
    const SDL_GPUTextureSamplerBinding textureBindings[2]{
        {boundTexture, impl_->sampler}, {secondTexture, impl_->secondSampler}};
    SDL_BindGPUFragmentSamplers(renderPass, 0U, textureBindings, 2U);
    if (impl_->indexBuffer)
    {
        const SDL_GPUBufferBinding indexBinding{impl_->indexBuffer, 0U};
        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_DrawGPUIndexedPrimitives(renderPass, impl_->indexCount, 1U, 0U, 0, 0U);
    }
    else
        SDL_DrawGPUPrimitives(renderPass, impl_->vertexCount, 1U, 0U, 0U);
    SDL_EndGPURenderPass(renderPass);
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
    {
        error = SDL_GetError();
        impl_->error = error;
        return false;
    }
    presented = true;
    return true;
}

bool SdlGpuRenderer::Readback64(SdlGpuReadback & result, std::string & error)
{
    constexpr Uint32 width = 64U;
    constexpr Uint32 height = 64U;
    constexpr Uint32 bytesPerPixel = 4U;
    constexpr Uint32 byteCount = width * height * bytesPerPixel;
    result = {};
    error.clear();
    SDL_GPUTexture * boundTexture = impl_->colorTexture;
    if (impl_->currentTextureHandle != 1U)
    {
        const auto found = impl_->registeredTextures.find(
            impl_->currentTextureHandle == 0U ? impl_->nullTextureHandle :
                                                impl_->currentTextureHandle);
        boundTexture = found == impl_->registeredTextures.end() ? nullptr : found->second.texture;
    }
    if (!impl_->device || (!impl_->frameCapture && (!impl_->vertexBuffer || !boundTexture))) return false;
    ReadbackResources resources(impl_->device);
    const SDL_GPUTextureCreateInfo colorInfo{
        SDL_GPU_TEXTURETYPE_2D, impl_->colorTargetFormat,
        SDL_GPU_TEXTUREUSAGE_COLOR_TARGET, width, height, 1U, 1U,
        SDL_GPU_SAMPLECOUNT_1, 0U};
    const SDL_GPUTextureCreateInfo depthInfo{
        SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
        SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET, width, height, 1U, 1U,
        SDL_GPU_SAMPLECOUNT_1, 0U};
    const SDL_GPUTransferBufferCreateInfo downloadInfo{
        SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD, byteCount, 0U};
    resources.color = SDL_CreateGPUTexture(impl_->device, &colorInfo);
    resources.depth = SDL_CreateGPUTexture(impl_->device, &depthInfo);
    resources.download = SDL_CreateGPUTransferBuffer(impl_->device, &downloadInfo);
    if (!resources.color || !resources.depth || !resources.download) return false;
    SDL_GPUCommandBuffer * commandBuffer = SDL_AcquireGPUCommandBuffer(impl_->device);
    if (!commandBuffer) return false;
    SDL_GPUColorTargetInfo colorTarget{};
    colorTarget.texture = resources.color;
    colorTarget.clear_color = impl_->clearColor;
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    SDL_GPUDepthStencilTargetInfo depthTarget{};
    depthTarget.texture = resources.depth;
    depthTarget.clear_depth = impl_->clearDepth;
    depthTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    depthTarget.store_op = impl_->frameCapture ? SDL_GPU_STOREOP_STORE :
                                                  SDL_GPU_STOREOP_DONT_CARE;
    depthTarget.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depthTarget.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
    if (impl_->frameCapture)
    {
        if (!DrawCapturedFrame(commandBuffer, &colorTarget, &depthTarget,
                               width, height, true, error))
        {
            SDL_CancelGPUCommandBuffer(commandBuffer);
            impl_->error = error;
            return false;
        }
    }
    else
    {
    const TextureStageUniform textureStageUniform =
        MakeTextureStageUniform(impl_->textureStage, impl_->renderState);
    SDL_PushGPUFragmentUniformData(commandBuffer, 0U, &textureStageUniform,
                                   sizeof(textureStageUniform));
    const VertexTransformUniform vertexUniform{
        MakeClipMatrix(impl_->projection, impl_->modelview),
        impl_->activeVertexFormat == 3U ? impl_->vertexUv : FixedFunctionUv,
        impl_->activeVertexFormat == 3U ? impl_->vertexColor : FixedFunctionColor};
    SDL_PushGPUVertexUniformData(commandBuffer, 0U, &vertexUniform,
                                 sizeof(vertexUniform));
    SDL_GPURenderPass * renderPass =
        SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1U, &depthTarget);
    if (!renderPass)
    {
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return false;
    }
    if (!ApplyViewport(renderPass, impl_->viewport, width, height, true))
    {
        SDL_EndGPURenderPass(renderPass);
        SDL_CancelGPUCommandBuffer(commandBuffer);
        error = "render viewport exceeds readback target";
        impl_->error = error;
        return false;
    }
    SDL_BindGPUGraphicsPipeline(renderPass,
        impl_->activeVertexFormat == 3U ? impl_->appPipeline :
        impl_->activeVertexFormat == 2U ? impl_->gamePipeline :
        impl_->activeVertexFormat == 1U ? impl_->extendedPipeline : impl_->pipeline);
    const SDL_GPUBufferBinding vertexBinding{impl_->vertexBuffer, 0U};
    SDL_BindGPUVertexBuffers(renderPass, 0U, &vertexBinding, 1U);
    const auto secondFound = impl_->registeredTextures.find(
        impl_->secondTextureHandle ? impl_->secondTextureHandle : impl_->nullTextureHandle);
    SDL_GPUTexture * secondTexture = impl_->secondTextureHandle == 1U ? impl_->colorTexture :
        secondFound == impl_->registeredTextures.end() ? nullptr : secondFound->second.texture;
    if (!secondTexture) { error = "missing second-stage texture"; return false; }
    const SDL_GPUTextureSamplerBinding textureBindings[2]{
        {boundTexture, impl_->sampler}, {secondTexture, impl_->secondSampler}};
    SDL_BindGPUFragmentSamplers(renderPass, 0U, textureBindings, 2U);
    if (impl_->indexBuffer)
    {
        const SDL_GPUBufferBinding indexBinding{impl_->indexBuffer, 0U};
        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_DrawGPUIndexedPrimitives(renderPass, impl_->indexCount, 1U, 0U, 0, 0U);
    }
    else
        SDL_DrawGPUPrimitives(renderPass, impl_->vertexCount, 1U, 0U, 0U);
    SDL_EndGPURenderPass(renderPass);
    }
    SDL_GPUCopyPass * copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (!copyPass)
    {
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return false;
    }
    const SDL_GPUTextureRegion source{
        resources.color, 0U, 0U, 0U, 0U, 0U, width, height, 1U};
    const SDL_GPUTextureTransferInfo destination{
        resources.download, 0U, width, height};
    SDL_DownloadFromGPUTexture(copyPass, &source, &destination);
    SDL_EndGPUCopyPass(copyPass);
    resources.fence = SDL_SubmitGPUCommandBufferAndAcquireFence(commandBuffer);
    if (!resources.fence) return false;
    SDL_GPUFence * fences[] = {resources.fence};
    if (!SDL_WaitForGPUFences(impl_->device, true, fences, 1U)) return false;
    const auto * pixels = static_cast<const std::uint8_t *>(
        SDL_MapGPUTransferBuffer(impl_->device, resources.download, false));
    if (!pixels) return false;
    const std::uint8_t * corner = pixels;
    const std::uint8_t * center =
        pixels + ((height / 2U) * width + (width / 2U)) * bytesPerPixel;
    const std::uint8_t * background =
        pixels + ((height / 2U) * width + (width / 6U)) * bytesPerPixel;
    result.checksum = 1469598103934665603ULL;
    for (Uint32 pixel = 0U; pixel < width * height; ++pixel)
    {
        const std::uint8_t * value = pixels + pixel * bytesPerPixel;
        for (Uint32 byte = 0U; byte < bytesPerPixel; ++byte)
        {
            result.checksum ^= value[byte];
            result.checksum *= 1099511628211ULL;
        }
        if (PixelsDiffer(value, corner, 3U)) ++result.changedPixels;
    }
    result.structureValid = result.changedPixels > 1800U && result.changedPixels < 3400U &&
        PixelsDiffer(center, corner, 12U) && PixelsDiffer(background, corner, 12U) &&
        PixelsDiffer(center, background, 12U);
    const bool rgba = impl_->colorTargetFormat == SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM ||
        impl_->colorTargetFormat == SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB;
    const bool bgra = impl_->colorTargetFormat == SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM ||
        impl_->colorTargetFormat == SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM_SRGB;
    if (rgba || bgra)
    {
        result.opaquePixels = true;
        for (Uint32 pixel = 0U; pixel < width * height; ++pixel)
        {
            const std::uint8_t * source = pixels + pixel * bytesPerPixel;
            result.opaquePixels = result.opaquePixels && source[3U] == 255U;
            std::uint8_t * destination = result.rgb8.data() + pixel * 3U;
            destination[0] = source[bgra ? 2U : 0U];
            destination[1] = source[1U];
            destination[2] = source[bgra ? 0U : 2U];
        }
        result.rgbValid = true;
    }
    SDL_UnmapGPUTransferBuffer(impl_->device, resources.download);
    return true;
}

void SdlGpuRenderer::Shutdown()
{
    if (!impl_) return;
    if (impl_->device)
    {
        SDL_WaitForGPUIdle(impl_->device);
        for (const auto & upload : impl_->freeUploads)
            impl_->ReleaseUpload(upload);
        for (const auto & draw : impl_->frameDraws)
        {
            SDL_ReleaseGPUBuffer(impl_->device, draw.vertex);
            if (draw.index) SDL_ReleaseGPUBuffer(impl_->device, draw.index);
            SDL_ReleaseGPUTransferBuffer(impl_->device, draw.transfer);
            SDL_ReleaseGPUTexture(impl_->device, draw.uploadedTexture);
        }
        if (!impl_->frameCapture)
        {
            if (impl_->vertexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->vertexBuffer);
            if (impl_->indexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->indexBuffer);
            if (impl_->transferBuffer)
                SDL_ReleaseGPUTransferBuffer(impl_->device, impl_->transferBuffer);
            if (impl_->colorTexture) SDL_ReleaseGPUTexture(impl_->device, impl_->colorTexture);
        }
        if (impl_->depthTexture) SDL_ReleaseGPUTexture(impl_->device, impl_->depthTexture);
        for (const auto & texture : impl_->registeredTextures)
            SDL_ReleaseGPUTexture(impl_->device, texture.second.texture);
        for (SDL_GPUSampler * sampler : impl_->samplerCache)
            if (sampler) SDL_ReleaseGPUSampler(impl_->device, sampler);
        if (impl_->pipelineCache.empty())
            impl_->ReleasePipelines(impl_->ActivePipelines());
        else
            for (const auto & entry : impl_->pipelineCache)
                impl_->ReleasePipelines(entry.second.set);
        for (SDL_GPUTexture * texture : impl_->retiredTextures)
            SDL_ReleaseGPUTexture(impl_->device, texture);
        if (impl_->vertexShader) SDL_ReleaseGPUShader(impl_->device, impl_->vertexShader);
        if (impl_->extendedVertexShader)
            SDL_ReleaseGPUShader(impl_->device, impl_->extendedVertexShader);
        if (impl_->gameVertexShader)
            SDL_ReleaseGPUShader(impl_->device, impl_->gameVertexShader);
        if (impl_->fragmentShader) SDL_ReleaseGPUShader(impl_->device, impl_->fragmentShader);
        if (impl_->windowClaimed && impl_->window)
            SDL_ReleaseWindowFromGPUDevice(impl_->device, impl_->window);
        SDL_DestroyGPUDevice(impl_->device);
    }
    impl_ = std::make_unique<Impl>();
}

void SdlGpuRenderer::EnableTimings(bool enabled) { impl_->timingsEnabled = enabled; }
SdlGpuRenderer::Timings SdlGpuRenderer::GetTimings() const
{
    const double scale = 1000.0 / SDL_GetPerformanceFrequency();
    return {impl_->submissionTicks * scale, impl_->samplerTicks * scale,
            impl_->retirementTicks * scale, impl_->presentationTicks * scale,
            impl_->submissionCount, impl_->samplerChanges,
            impl_->acquisitionTicks * scale, impl_->drawingTicks * scale,
            impl_->queueSubmitTicks * scale, impl_->reusedUploads,
            impl_->textureUploadTicks * scale, impl_->textureUploadCount};
}
const std::string & SdlGpuRenderer::LastError() const { return impl_->error; }
const char * SdlGpuRenderer::PresentModeName() const { return "vsync"; }
const char * SdlGpuRenderer::DriverName() const
{
    return impl_->device ? SDL_GetGPUDeviceDriver(impl_->device) : "";
}
std::uint32_t SdlGpuRenderer::DepthWidth() const { return impl_->depthWidth; }
std::uint32_t SdlGpuRenderer::DepthHeight() const { return impl_->depthHeight; }
std::uint32_t SdlGpuRenderer::DepthRecreations() const { return impl_->depthRecreations; }
std::uint32_t SdlGpuRenderer::SamplerCreations() const
{
    return impl_->samplerCreations;
}

std::uint32_t SdlGpuRenderer::PipelineRebuilds() const { return impl_->pipelineRebuilds; }
std::uint32_t SdlGpuRenderer::VertexCount() const { return impl_->vertexCount; }
std::uint32_t SdlGpuRenderer::IndexCount() const { return impl_->indexCount; }
RenderStateSnapshot SdlGpuRenderer::CurrentRenderState() const
{
    return impl_->renderState;
}
TextureStageSnapshot SdlGpuRenderer::CurrentTextureStageState() const
{
    return impl_->textureStage;
}

SamplerStateSnapshot SdlGpuRenderer::CurrentSamplerState() const
{
    return impl_->samplerState;
}
std::uint32_t SdlGpuRenderer::CurrentTextureHandle() const
{
    return impl_->currentTextureHandle;
}
std::uint32_t SdlGpuRenderer::FixtureTextureHandle() const
{
    return impl_->device ? 1U : 0U;
}
}
