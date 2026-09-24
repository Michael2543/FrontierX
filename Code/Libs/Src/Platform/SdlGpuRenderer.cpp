#include "FrontierX/SdlGpuRenderer.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
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
    struct RegisteredTexture
    {
        SDL_GPUTexture * texture;
        Uint32 width;
        Uint32 height;
    };
    SDL_Window * window = nullptr;
    SDL_GPUDevice * device = nullptr;
    SDL_GPUShader * vertexShader = nullptr;
    SDL_GPUShader * extendedVertexShader = nullptr;
    SDL_GPUShader * fragmentShader = nullptr;
    SDL_GPUGraphicsPipeline * pipeline = nullptr;
    SDL_GPUGraphicsPipeline * extendedPipeline = nullptr;
    SDL_GPUBuffer * vertexBuffer = nullptr;
    SDL_GPUBuffer * indexBuffer = nullptr;
    SDL_GPUTransferBuffer * transferBuffer = nullptr;
    SDL_GPUTexture * colorTexture = nullptr;
    std::map<Uint32, RegisteredTexture> registeredTextures;
    SDL_GPUTexture * depthTexture = nullptr;
    SDL_GPUSampler * sampler = nullptr;
    SDL_GPUTextureFormat colorTargetFormat = SDL_GPU_TEXTUREFORMAT_INVALID;
    std::vector<std::uint8_t> texturePixels;
    std::string error;
    Uint32 textureWidth = 0U;
    Uint32 textureHeight = 0U;
    Uint32 depthWidth = 0U;
    Uint32 depthHeight = 0U;
    Uint32 depthRecreations = 0U;
    Uint32 vertexCount = 0U;
    Uint32 indexCount = 0U;
    RenderStateSnapshot renderState{};
    TextureStageSnapshot textureStage{};
    Uint32 currentTextureHandle = 1U;
    Uint32 nextTextureHandle = 2U;
    bool extendedActive = false;
    bool windowClaimed = false;
};

namespace
{
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
};

TextureStageUniform MakeTextureStageUniform(const TextureStageSnapshot & state)
{
    return {{state.colorOperation, state.alphaOperation, 0U, 0U},
            {state.colorArgument1, state.colorArgument2,
             state.alphaArgument1, state.alphaArgument2}};
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
    bool extended)
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
        0U, static_cast<Uint32>(extended ? sizeof(RenderVertexP3DiffuseT4) :
                                  sizeof(RenderVertexP3DiffuseT2)),
        SDL_GPU_VERTEXINPUTRATE_VERTEX, 0U};
    const std::array<SDL_GPUVertexAttribute, 4> attributes = {{
        {0U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0U},
        {1U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
         static_cast<Uint32>(offsetof(RenderVertexP3DiffuseT2, diffuseBgra))},
        {2U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
         static_cast<Uint32>(offsetof(RenderVertexP3DiffuseT2, u))},
        {3U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
         static_cast<Uint32>(offsetof(RenderVertexP3DiffuseT4, u2))},
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
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &bufferDescription;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1U;
    pipelineInfo.vertex_input_state.vertex_attributes = attributes.data();
    pipelineInfo.vertex_input_state.num_vertex_attributes = extended ? 4U : 3U;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipelineInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
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
                                const char * extendedVertexShaderPath)
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
    std::vector<std::uint8_t> fragmentCode;
    if (!ReadShader(vertexShaderPath, vertexCode, error) ||
        !ReadShader(fragmentShaderPath, fragmentCode, error) ||
        (extendedVertexShaderPath &&
         !ReadShader(extendedVertexShaderPath, extendedVertexCode, error)))
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
        impl_->device, vertexCode, SDL_GPU_SHADERSTAGE_VERTEX, 0U);
    if (extendedVertexShaderPath)
        impl_->extendedVertexShader = CreateShader(
            impl_->device, extendedVertexCode, SDL_GPU_SHADERSTAGE_VERTEX, 0U);
    impl_->fragmentShader = CreateShader(
        impl_->device, fragmentCode, SDL_GPU_SHADERSTAGE_FRAGMENT, 1U, 1U);
    if (!impl_->vertexShader || !impl_->fragmentShader ||
        (extendedVertexShaderPath && !impl_->extendedVertexShader))
    {
        error = SDL_GetError();
        impl_->error = error;
        Shutdown();
        return false;
    }

    impl_->pipeline = CreateGraphicsPipeline(
        impl_->device, impl_->vertexShader, impl_->fragmentShader,
        impl_->colorTargetFormat, impl_->renderState, false);
    if (impl_->extendedVertexShader)
        impl_->extendedPipeline = CreateGraphicsPipeline(
            impl_->device, impl_->extendedVertexShader, impl_->fragmentShader,
            impl_->colorTargetFormat, impl_->renderState, true);
    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    impl_->sampler = SDL_CreateGPUSampler(impl_->device, &samplerInfo);
    if (!impl_->pipeline || !impl_->sampler ||
        (impl_->extendedVertexShader && !impl_->extendedPipeline))
    {
        error = SDL_GetError();
        impl_->error = error;
        Shutdown();
        return false;
    }
    impl_->texturePixels.assign(texture.rgba8, texture.rgba8 + texture.byteCount);
    impl_->textureWidth = texture.width;
    impl_->textureHeight = texture.height;
    return true;
}

GENRESULT SdlGpuRenderer::Submit(const RenderSubmissionView & submission)
{
    if (submission.topology != PrimitiveTopology::TriangleList)
        return GR_NOT_IMPLEMENTED;
    if (!submission.IsValid()) return GR_INVALID_PARMS;
    return SubmitBytes(submission.vertices, submission.vertexCount,
                       sizeof(RenderVertexP3DiffuseT2), submission.indices,
                       submission.indexCount, false);
}

GENRESULT SdlGpuRenderer::Submit(const RenderSubmissionView2 & submission)
{
    if (submission.topology != PrimitiveTopology::TriangleList)
        return GR_NOT_IMPLEMENTED;
    if (!submission.IsValid()) return GR_INVALID_PARMS;
    return SubmitBytes(submission.vertices, submission.vertexCount,
                       sizeof(RenderVertexP3DiffuseT4), submission.indices,
                       submission.indexCount, true);
}

GENRESULT SdlGpuRenderer::SubmitBytes(const void * vertices, std::size_t vertexCount,
                                     std::size_t vertexStride,
                                     const std::uint16_t * indices,
                                     std::size_t indexCount, bool extended)
{
    if (!impl_->device || (extended && !impl_->extendedPipeline))
        return GR_NOT_IMPLEMENTED;
    if (vertexCount > std::numeric_limits<Uint32>::max() / vertexStride ||
        indexCount >
            std::numeric_limits<Uint32>::max() / sizeof(std::uint16_t))
        return GR_INVALID_PARMS;

    const std::size_t vertexBytes =
        vertexCount * vertexStride;
    const std::size_t indexBytes = indexCount * sizeof(std::uint16_t);
    const std::size_t textureOffsetValue = (vertexBytes + indexBytes + 3U) & ~std::size_t{3U};
    if (textureOffsetValue + impl_->texturePixels.size() >
        std::numeric_limits<Uint32>::max()) return GR_INVALID_PARMS;
    const auto vertexByteCount = static_cast<Uint32>(vertexBytes);
    const auto indexByteCount = static_cast<Uint32>(indexBytes);
    const auto textureOffset = static_cast<Uint32>(textureOffsetValue);
    const auto totalByteCount = static_cast<Uint32>(
        textureOffsetValue + impl_->texturePixels.size());

    if (!SDL_WaitForGPUIdle(impl_->device))
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    SubmissionResources candidate(impl_->device);

    const SDL_GPUBufferCreateInfo vertexInfo{
        SDL_GPU_BUFFERUSAGE_VERTEX, vertexByteCount, 0U};
    candidate.vertex = SDL_CreateGPUBuffer(impl_->device, &vertexInfo);
    if (indices)
    {
        const SDL_GPUBufferCreateInfo indexInfo{
            SDL_GPU_BUFFERUSAGE_INDEX, indexByteCount, 0U};
        candidate.index = SDL_CreateGPUBuffer(impl_->device, &indexInfo);
    }
    const SDL_GPUTextureCreateInfo textureInfo{
        SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        SDL_GPU_TEXTUREUSAGE_SAMPLER, impl_->textureWidth, impl_->textureHeight,
        1U, 1U, SDL_GPU_SAMPLECOUNT_1, 0U};
    candidate.color = SDL_CreateGPUTexture(impl_->device, &textureInfo);
    const SDL_GPUTransferBufferCreateInfo transferInfo{
        SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, totalByteCount, 0U};
    candidate.transfer = SDL_CreateGPUTransferBuffer(impl_->device, &transferInfo);
    if (!candidate.vertex || (indices && !candidate.index) ||
        !candidate.color || !candidate.transfer)
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
    std::memcpy(static_cast<std::uint8_t *>(mapped) + textureOffset,
                impl_->texturePixels.data(), impl_->texturePixels.size());
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
    SDL_UploadToGPUTexture(copyPass, &textureSource, &textureDestination, false);
    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    if (impl_->vertexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->vertexBuffer);
    if (impl_->indexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->indexBuffer);
    if (impl_->transferBuffer)
        SDL_ReleaseGPUTransferBuffer(impl_->device, impl_->transferBuffer);
    if (impl_->colorTexture) SDL_ReleaseGPUTexture(impl_->device, impl_->colorTexture);
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
    impl_->extendedActive = extended;
    return GR_OK;
}

GENRESULT SdlGpuRenderer::SetRenderState(const RenderStateSnapshot & state)
{
    if (!impl_->device || !IsRenderBoolean(state.depthEnabled) ||
        !IsRenderBoolean(state.depthWriteEnabled) ||
        !IsRenderBoolean(state.alphaBlendEnabled) ||
        !IsRenderDepthFunction(state.depthFunction) ||
        !IsRenderBlendFactor(state.sourceBlend) ||
        !IsRenderBlendFactor(state.destinationBlend))
    {
        return GR_INVALID_PARMS;
    }
    SDL_GPUGraphicsPipeline * pipeline = CreateGraphicsPipeline(
        impl_->device, impl_->vertexShader, impl_->fragmentShader,
        impl_->colorTargetFormat, state, false);
    if (!pipeline)
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    SDL_GPUGraphicsPipeline * extendedPipeline = nullptr;
    if (impl_->extendedVertexShader)
    {
        extendedPipeline = CreateGraphicsPipeline(
            impl_->device, impl_->extendedVertexShader, impl_->fragmentShader,
            impl_->colorTargetFormat, state, true);
        if (!extendedPipeline)
        {
            impl_->error = SDL_GetError();
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, pipeline);
            return GR_GENERIC;
        }
    }
    if (!SDL_WaitForGPUIdle(impl_->device))
    {
        impl_->error = SDL_GetError();
        SDL_ReleaseGPUGraphicsPipeline(impl_->device, pipeline);
        if (extendedPipeline)
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, extendedPipeline);
        return GR_GENERIC;
    }
    if (impl_->pipeline)
        SDL_ReleaseGPUGraphicsPipeline(impl_->device, impl_->pipeline);
    if (impl_->extendedPipeline)
        SDL_ReleaseGPUGraphicsPipeline(impl_->device, impl_->extendedPipeline);
    impl_->pipeline = pipeline;
    impl_->extendedPipeline = extendedPipeline;
    impl_->renderState = state;
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
        !IsTextureStageArgument(state.alphaArgument2))
    {
        return GR_NOT_IMPLEMENTED;
    }
    impl_->textureStage = state;
    return GR_OK;
}

GENRESULT SdlGpuRenderer::RegisterTexture(SdlGpuTextureView texture, std::uint32_t & handle)
{
    handle = 0U;
    if (!impl_->device || !texture.rgba8 || !texture.width || !texture.height ||
        texture.width > std::numeric_limits<Uint32>::max() / 4U ||
        texture.height > std::numeric_limits<Uint32>::max() / (texture.width * 4U) ||
        texture.byteCount != static_cast<std::size_t>(texture.width) * texture.height * 4U ||
        impl_->nextTextureHandle == std::numeric_limits<Uint32>::max())
    {
        return GR_INVALID_PARMS;
    }
    SubmissionResources candidate(impl_->device);
    const SDL_GPUTextureCreateInfo textureInfo{
        SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        SDL_GPU_TEXTUREUSAGE_SAMPLER, texture.width, texture.height,
        1U, 1U, SDL_GPU_SAMPLECOUNT_1, 0U};
    const SDL_GPUTransferBufferCreateInfo transferInfo{
        SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        static_cast<Uint32>(texture.byteCount), 0U};
    candidate.color = SDL_CreateGPUTexture(impl_->device, &textureInfo);
    candidate.transfer = SDL_CreateGPUTransferBuffer(impl_->device, &transferInfo);
    if (!candidate.color || !candidate.transfer)
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
    std::memcpy(mapped, texture.rgba8, texture.byteCount);
    SDL_UnmapGPUTransferBuffer(impl_->device, candidate.transfer);
    SDL_GPUCommandBuffer * commandBuffer = SDL_AcquireGPUCommandBuffer(impl_->device);
    if (!commandBuffer)
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    SDL_GPUCopyPass * copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (!copyPass)
    {
        impl_->error = SDL_GetError();
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return GR_GENERIC;
    }
    const SDL_GPUTextureTransferInfo source{
        candidate.transfer, 0U, texture.width, texture.height};
    const SDL_GPUTextureRegion destination{
        candidate.color, 0U, 0U, 0U, 0U, 0U, texture.width, texture.height, 1U};
    SDL_UploadToGPUTexture(copyPass, &source, &destination, false);
    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer) || !SDL_WaitForGPUIdle(impl_->device))
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    try
    {
        impl_->registeredTextures.emplace(impl_->nextTextureHandle,
            Impl::RegisteredTexture{candidate.color, texture.width, texture.height});
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_MEMORY;
    }
    handle = impl_->nextTextureHandle++;
    candidate.color = nullptr;
    return GR_OK;
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

GENRESULT SdlGpuRenderer::ReplaceTexture(std::uint32_t handle,
                                          SdlGpuTextureView texture)
{
    const auto found = impl_->registeredTextures.find(handle);
    if (found == impl_->registeredTextures.end() ||
        found->second.width != texture.width ||
        found->second.height != texture.height)
        return GR_INVALID_PARMS;
    U32 candidateHandle = 0U;
    const GENRESULT result = RegisterTexture(texture, candidateHandle);
    if (result != GR_OK) return result;
    const auto candidate = impl_->registeredTextures.find(candidateHandle);
    SDL_GPUTexture * previous = found->second.texture;
    found->second.texture = candidate->second.texture;
    impl_->registeredTextures.erase(candidate);
    SDL_ReleaseGPUTexture(impl_->device, previous);
    return GR_OK;
}

GENRESULT SdlGpuRenderer::DestroyTexture(std::uint32_t handle)
{
    if (!impl_->device || handle == impl_->currentTextureHandle)
        return GR_INVALID_PARMS;
    const auto found = impl_->registeredTextures.find(handle);
    if (found == impl_->registeredTextures.end()) return GR_INVALID_PARMS;
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
    if (stage == 1U && handle == 0U) return GR_OK;
    if (stage != 0U || !IsTexture(handle)) return GR_NOT_IMPLEMENTED;
    impl_->currentTextureHandle = handle;
    return GR_OK;
}

bool SdlGpuRenderer::IsTexture(std::uint32_t handle) const
{
    return impl_->device &&
        (handle == 1U || impl_->registeredTextures.find(handle) !=
                             impl_->registeredTextures.end());
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

GENRESULT DACOM_CDECL SdlGpuRenderer::RenderStateCallback(
    void * context, const RenderStateSnapshot & state)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->SetRenderState(state)
                   : GR_INVALID_PARMS;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::TextureStageCallback(
    void * context, const TextureStageSnapshot & state)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->SetTextureStageState(state)
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

GENRESULT SdlGpuRenderer::TextureDestroyCallback(void * context, std::uint32_t handle)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->DestroyTexture(handle)
                   : GR_INVALID_PARMS;
}

bool SdlGpuRenderer::Present(bool & presented, std::string & error)
{
    presented = false;
    error.clear();
    SDL_GPUTexture * boundTexture = impl_->colorTexture;
    if (impl_->currentTextureHandle != 1U)
    {
        const auto found = impl_->registeredTextures.find(impl_->currentTextureHandle);
        boundTexture = found == impl_->registeredTextures.end() ? nullptr : found->second.texture;
    }
    if (!impl_->device || !impl_->pipeline || !impl_->vertexBuffer || !boundTexture)
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
    colorTarget.clear_color = SDL_FColor{0.025F, 0.035F, 0.065F, 1.0F};
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    SDL_GPUDepthStencilTargetInfo depthTarget{};
    depthTarget.texture = impl_->depthTexture;
    depthTarget.clear_depth = 1.0F;
    depthTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    depthTarget.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthTarget.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depthTarget.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
    const TextureStageUniform textureStageUniform =
        MakeTextureStageUniform(impl_->textureStage);
    SDL_PushGPUFragmentUniformData(commandBuffer, 0U, &textureStageUniform,
                                   sizeof(textureStageUniform));
    SDL_GPURenderPass * renderPass =
        SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1U, &depthTarget);
    if (!renderPass)
    {
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return false;
    }
    SDL_BindGPUGraphicsPipeline(renderPass,
        impl_->extendedActive ? impl_->extendedPipeline : impl_->pipeline);
    const SDL_GPUBufferBinding vertexBinding{impl_->vertexBuffer, 0U};
    SDL_BindGPUVertexBuffers(renderPass, 0U, &vertexBinding, 1U);
    const SDL_GPUTextureSamplerBinding textureBinding{boundTexture, impl_->sampler};
    SDL_BindGPUFragmentSamplers(renderPass, 0U, &textureBinding, 1U);
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
        const auto found = impl_->registeredTextures.find(impl_->currentTextureHandle);
        boundTexture = found == impl_->registeredTextures.end() ? nullptr : found->second.texture;
    }
    if (!impl_->device || !impl_->vertexBuffer || !boundTexture) return false;
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
    colorTarget.clear_color = SDL_FColor{0.025F, 0.035F, 0.065F, 1.0F};
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    SDL_GPUDepthStencilTargetInfo depthTarget{};
    depthTarget.texture = resources.depth;
    depthTarget.clear_depth = 1.0F;
    depthTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    depthTarget.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthTarget.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depthTarget.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
    const TextureStageUniform textureStageUniform =
        MakeTextureStageUniform(impl_->textureStage);
    SDL_PushGPUFragmentUniformData(commandBuffer, 0U, &textureStageUniform,
                                   sizeof(textureStageUniform));
    SDL_GPURenderPass * renderPass =
        SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1U, &depthTarget);
    if (!renderPass)
    {
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return false;
    }
    SDL_BindGPUGraphicsPipeline(renderPass,
        impl_->extendedActive ? impl_->extendedPipeline : impl_->pipeline);
    const SDL_GPUBufferBinding vertexBinding{impl_->vertexBuffer, 0U};
    SDL_BindGPUVertexBuffers(renderPass, 0U, &vertexBinding, 1U);
    const SDL_GPUTextureSamplerBinding textureBinding{boundTexture, impl_->sampler};
    SDL_BindGPUFragmentSamplers(renderPass, 0U, &textureBinding, 1U);
    if (impl_->indexBuffer)
    {
        const SDL_GPUBufferBinding indexBinding{impl_->indexBuffer, 0U};
        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_DrawGPUIndexedPrimitives(renderPass, impl_->indexCount, 1U, 0U, 0, 0U);
    }
    else
        SDL_DrawGPUPrimitives(renderPass, impl_->vertexCount, 1U, 0U, 0U);
    SDL_EndGPURenderPass(renderPass);
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
    SDL_UnmapGPUTransferBuffer(impl_->device, resources.download);
    return true;
}

void SdlGpuRenderer::Shutdown()
{
    if (!impl_) return;
    if (impl_->device)
    {
        SDL_WaitForGPUIdle(impl_->device);
        if (impl_->vertexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->vertexBuffer);
        if (impl_->indexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->indexBuffer);
        if (impl_->transferBuffer)
            SDL_ReleaseGPUTransferBuffer(impl_->device, impl_->transferBuffer);
        if (impl_->depthTexture) SDL_ReleaseGPUTexture(impl_->device, impl_->depthTexture);
        if (impl_->colorTexture) SDL_ReleaseGPUTexture(impl_->device, impl_->colorTexture);
        for (const auto & texture : impl_->registeredTextures)
            SDL_ReleaseGPUTexture(impl_->device, texture.second.texture);
        if (impl_->sampler) SDL_ReleaseGPUSampler(impl_->device, impl_->sampler);
        if (impl_->pipeline) SDL_ReleaseGPUGraphicsPipeline(impl_->device, impl_->pipeline);
        if (impl_->extendedPipeline)
            SDL_ReleaseGPUGraphicsPipeline(impl_->device, impl_->extendedPipeline);
        if (impl_->vertexShader) SDL_ReleaseGPUShader(impl_->device, impl_->vertexShader);
        if (impl_->extendedVertexShader)
            SDL_ReleaseGPUShader(impl_->device, impl_->extendedVertexShader);
        if (impl_->fragmentShader) SDL_ReleaseGPUShader(impl_->device, impl_->fragmentShader);
        if (impl_->windowClaimed && impl_->window)
            SDL_ReleaseWindowFromGPUDevice(impl_->device, impl_->window);
        SDL_DestroyGPUDevice(impl_->device);
    }
    impl_ = std::make_unique<Impl>();
}

const std::string & SdlGpuRenderer::LastError() const { return impl_->error; }
const char * SdlGpuRenderer::DriverName() const
{
    return impl_->device ? SDL_GetGPUDeviceDriver(impl_->device) : "";
}
std::uint32_t SdlGpuRenderer::DepthWidth() const { return impl_->depthWidth; }
std::uint32_t SdlGpuRenderer::DepthHeight() const { return impl_->depthHeight; }
std::uint32_t SdlGpuRenderer::DepthRecreations() const { return impl_->depthRecreations; }
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
std::uint32_t SdlGpuRenderer::CurrentTextureHandle() const
{
    return impl_->currentTextureHandle;
}
std::uint32_t SdlGpuRenderer::FixtureTextureHandle() const
{
    return impl_->device ? 1U : 0U;
}
}
