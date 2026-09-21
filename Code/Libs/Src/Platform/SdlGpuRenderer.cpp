#include "FrontierX/SdlGpuRenderer.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <utility>
#include <vector>

namespace FrontierX
{
struct SdlGpuRenderer::Impl
{
    SDL_Window * window = nullptr;
    SDL_GPUDevice * device = nullptr;
    SDL_GPUShader * vertexShader = nullptr;
    SDL_GPUShader * fragmentShader = nullptr;
    SDL_GPUGraphicsPipeline * pipeline = nullptr;
    SDL_GPUBuffer * vertexBuffer = nullptr;
    SDL_GPUBuffer * indexBuffer = nullptr;
    SDL_GPUTransferBuffer * transferBuffer = nullptr;
    SDL_GPUTexture * colorTexture = nullptr;
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
                             Uint32 samplerCount)
{
    SDL_GPUShaderCreateInfo info{};
    info.code_size = code.size();
    info.code = code.data();
    info.entrypoint = "main";
    info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    info.stage = stage;
    info.num_samplers = samplerCount;
    return SDL_CreateGPUShader(device, &info);
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
                                std::string & error)
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
    std::vector<std::uint8_t> fragmentCode;
    if (!ReadShader(vertexShaderPath, vertexCode, error) ||
        !ReadShader(fragmentShaderPath, fragmentCode, error))
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
    impl_->fragmentShader = CreateShader(
        impl_->device, fragmentCode, SDL_GPU_SHADERSTAGE_FRAGMENT, 1U);
    if (!impl_->vertexShader || !impl_->fragmentShader)
    {
        error = SDL_GetError();
        impl_->error = error;
        Shutdown();
        return false;
    }

    const SDL_GPUVertexBufferDescription bufferDescription{
        0U, static_cast<Uint32>(sizeof(RenderVertexP3DiffuseT2)),
        SDL_GPU_VERTEXINPUTRATE_VERTEX, 0U};
    const std::array<SDL_GPUVertexAttribute, 3> attributes = {{
        {0U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, 0U},
        {1U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
         static_cast<Uint32>(offsetof(RenderVertexP3DiffuseT2, diffuseBgra))},
        {2U, 0U, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
         static_cast<Uint32>(offsetof(RenderVertexP3DiffuseT2, u))},
    }};
    const SDL_GPUColorTargetDescription colorTarget{impl_->colorTargetFormat, {}};
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = impl_->vertexShader;
    pipelineInfo.fragment_shader = impl_->fragmentShader;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &bufferDescription;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1U;
    pipelineInfo.vertex_input_state.vertex_attributes = attributes.data();
    pipelineInfo.vertex_input_state.num_vertex_attributes =
        static_cast<Uint32>(attributes.size());
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipelineInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pipelineInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    pipelineInfo.rasterizer_state.enable_depth_clip = true;
    pipelineInfo.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    pipelineInfo.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL;
    pipelineInfo.depth_stencil_state.enable_depth_test = true;
    pipelineInfo.depth_stencil_state.enable_depth_write = true;
    pipelineInfo.target_info.color_target_descriptions = &colorTarget;
    pipelineInfo.target_info.num_color_targets = 1U;
    pipelineInfo.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    pipelineInfo.target_info.has_depth_stencil_target = true;
    impl_->pipeline = SDL_CreateGPUGraphicsPipeline(impl_->device, &pipelineInfo);
    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    impl_->sampler = SDL_CreateGPUSampler(impl_->device, &samplerInfo);
    if (!impl_->pipeline || !impl_->sampler)
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
    if (!impl_->device || submission.topology != PrimitiveTopology::TriangleList)
        return GR_NOT_IMPLEMENTED;
    if (!submission.IsValid()) return GR_INVALID_PARMS;
    if (submission.vertexCount >
            std::numeric_limits<Uint32>::max() / sizeof(RenderVertexP3DiffuseT2) ||
        submission.indexCount >
            std::numeric_limits<Uint32>::max() / sizeof(std::uint16_t))
        return GR_INVALID_PARMS;

    if (!SDL_WaitForGPUIdle(impl_->device))
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    if (impl_->vertexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->vertexBuffer);
    if (impl_->indexBuffer) SDL_ReleaseGPUBuffer(impl_->device, impl_->indexBuffer);
    if (impl_->transferBuffer)
        SDL_ReleaseGPUTransferBuffer(impl_->device, impl_->transferBuffer);
    if (impl_->colorTexture) SDL_ReleaseGPUTexture(impl_->device, impl_->colorTexture);
    impl_->vertexBuffer = nullptr;
    impl_->indexBuffer = nullptr;
    impl_->transferBuffer = nullptr;
    impl_->colorTexture = nullptr;

    const std::size_t vertexBytes =
        submission.vertexCount * sizeof(RenderVertexP3DiffuseT2);
    const std::size_t indexBytes = submission.indexCount * sizeof(std::uint16_t);
    const std::size_t textureOffsetValue = (vertexBytes + indexBytes + 3U) & ~std::size_t{3U};
    if (textureOffsetValue + impl_->texturePixels.size() >
        std::numeric_limits<Uint32>::max()) return GR_INVALID_PARMS;
    const auto vertexByteCount = static_cast<Uint32>(vertexBytes);
    const auto indexByteCount = static_cast<Uint32>(indexBytes);
    const auto textureOffset = static_cast<Uint32>(textureOffsetValue);
    const auto totalByteCount = static_cast<Uint32>(
        textureOffsetValue + impl_->texturePixels.size());

    const SDL_GPUBufferCreateInfo vertexInfo{
        SDL_GPU_BUFFERUSAGE_VERTEX, vertexByteCount, 0U};
    impl_->vertexBuffer = SDL_CreateGPUBuffer(impl_->device, &vertexInfo);
    if (submission.indices)
    {
        const SDL_GPUBufferCreateInfo indexInfo{
            SDL_GPU_BUFFERUSAGE_INDEX, indexByteCount, 0U};
        impl_->indexBuffer = SDL_CreateGPUBuffer(impl_->device, &indexInfo);
    }
    const SDL_GPUTextureCreateInfo textureInfo{
        SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        SDL_GPU_TEXTUREUSAGE_SAMPLER, impl_->textureWidth, impl_->textureHeight,
        1U, 1U, SDL_GPU_SAMPLECOUNT_1, 0U};
    impl_->colorTexture = SDL_CreateGPUTexture(impl_->device, &textureInfo);
    const SDL_GPUTransferBufferCreateInfo transferInfo{
        SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, totalByteCount, 0U};
    impl_->transferBuffer = SDL_CreateGPUTransferBuffer(impl_->device, &transferInfo);
    if (!impl_->vertexBuffer || (submission.indices && !impl_->indexBuffer) ||
        !impl_->colorTexture || !impl_->transferBuffer)
    {
        impl_->error = SDL_GetError();
        return GR_OUT_OF_MEMORY;
    }
    void * mapped = SDL_MapGPUTransferBuffer(impl_->device, impl_->transferBuffer, false);
    if (!mapped)
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    std::memcpy(mapped, submission.vertices, vertexBytes);
    if (submission.indices)
        std::memcpy(static_cast<std::uint8_t *>(mapped) + vertexByteCount,
                    submission.indices, indexBytes);
    std::memcpy(static_cast<std::uint8_t *>(mapped) + textureOffset,
                impl_->texturePixels.data(), impl_->texturePixels.size());
    SDL_UnmapGPUTransferBuffer(impl_->device, impl_->transferBuffer);

    SDL_GPUCommandBuffer * commandBuffer = SDL_AcquireGPUCommandBuffer(impl_->device);
    if (!commandBuffer) return GR_GENERIC;
    SDL_GPUCopyPass * copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (!copyPass)
    {
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return GR_GENERIC;
    }
    const SDL_GPUTransferBufferLocation vertexSource{impl_->transferBuffer, 0U};
    const SDL_GPUBufferRegion vertexDestination{
        impl_->vertexBuffer, 0U, vertexByteCount};
    SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, false);
    if (submission.indices)
    {
        const SDL_GPUTransferBufferLocation indexSource{
            impl_->transferBuffer, vertexByteCount};
        const SDL_GPUBufferRegion indexDestination{
            impl_->indexBuffer, 0U, indexByteCount};
        SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, false);
    }
    const SDL_GPUTextureTransferInfo textureSource{
        impl_->transferBuffer, textureOffset, impl_->textureWidth, impl_->textureHeight};
    const SDL_GPUTextureRegion textureDestination{
        impl_->colorTexture, 0U, 0U, 0U, 0U, 0U,
        impl_->textureWidth, impl_->textureHeight, 1U};
    SDL_UploadToGPUTexture(copyPass, &textureSource, &textureDestination, false);
    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
    {
        impl_->error = SDL_GetError();
        return GR_GENERIC;
    }
    impl_->vertexCount = static_cast<Uint32>(submission.vertexCount);
    impl_->indexCount = static_cast<Uint32>(submission.indexCount);
    return GR_OK;
}

GENRESULT DACOM_CDECL SdlGpuRenderer::SubmissionCallback(
    void * context, const RenderSubmissionView & submission)
{
    return context ? static_cast<SdlGpuRenderer *>(context)->Submit(submission) : GR_INVALID_PARMS;
}

bool SdlGpuRenderer::Present(bool & presented, std::string & error)
{
    presented = false;
    error.clear();
    if (!impl_->device || !impl_->pipeline || !impl_->vertexBuffer || !impl_->colorTexture)
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
    SDL_GPURenderPass * renderPass =
        SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1U, &depthTarget);
    if (!renderPass)
    {
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return false;
    }
    SDL_BindGPUGraphicsPipeline(renderPass, impl_->pipeline);
    const SDL_GPUBufferBinding vertexBinding{impl_->vertexBuffer, 0U};
    SDL_BindGPUVertexBuffers(renderPass, 0U, &vertexBinding, 1U);
    const SDL_GPUTextureSamplerBinding textureBinding{impl_->colorTexture, impl_->sampler};
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
    if (!impl_->device || !impl_->vertexBuffer || !impl_->colorTexture) return false;
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
    SDL_GPURenderPass * renderPass =
        SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1U, &depthTarget);
    if (!renderPass)
    {
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return false;
    }
    SDL_BindGPUGraphicsPipeline(renderPass, impl_->pipeline);
    const SDL_GPUBufferBinding vertexBinding{impl_->vertexBuffer, 0U};
    SDL_BindGPUVertexBuffers(renderPass, 0U, &vertexBinding, 1U);
    const SDL_GPUTextureSamplerBinding textureBinding{impl_->colorTexture, impl_->sampler};
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
        if (impl_->sampler) SDL_ReleaseGPUSampler(impl_->device, impl_->sampler);
        if (impl_->pipeline) SDL_ReleaseGPUGraphicsPipeline(impl_->device, impl_->pipeline);
        if (impl_->vertexShader) SDL_ReleaseGPUShader(impl_->device, impl_->vertexShader);
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
}
