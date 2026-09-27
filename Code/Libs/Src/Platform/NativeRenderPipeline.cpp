#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/NativeBatchStateIds.h"

#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/LegacyFileTypes.h"
#include "FrontierX/NativeGameIndexBuffer.h"
#include "IMaterialManager.h"
#include "Filesys.h"
#include "TComponent.h"

#include <array>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <utility>

namespace FrontierX
{
namespace
{
char profilePipelineName[] = "IRenderPipeline";

class NativeProfileRenderPipeline : public NativeRenderPipeline
{
public:
    BEGIN_DACOM_MAP_INBOUND(NativeProfileRenderPipeline)
    DACOM_INTERFACE_ENTRY(IRenderPipeline)
    DACOM_INTERFACE_ENTRY2(IID_IRenderPipeline, IRenderPipeline)
    END_DACOM_MAP()

    GENRESULT init(AGGDESC * descriptor)
    {
        if (!descriptor || descriptor->size != sizeof(*descriptor) ||
            !descriptor->interface_name ||
            std::strcmp(descriptor->interface_name, profilePipelineName) != 0 ||
            !descriptor->outer || !descriptor->inner)
            return GR_INVALID_PARMS;
        return GR_OK;
    }
};

// Stable IRP_VBF_READ, IRP_VBF_NO_CLIP and IRP_VBF_SYSTEM bits.
constexpr U32 kSupportedVertexBufferFlags = 0x7U;
constexpr std::size_t kMaximumEffectSourceBytes = 65536U;
constexpr std::uint64_t kVertexColorSourceHash = 0xeb95bc07e124d705ULL;
static_assert(D3DFVF_STANDARD == LegacyD3d9::kGameStandardFvf);
static_assert(sizeof(StandardVertex) == 68U);
static_assert(sizeof(StandardVertex) == sizeof(RenderVertexP3N3DiffuseT5));
static_assert(offsetof(StandardVertex, norm) == 12U &&
                  offsetof(StandardVertex, norm) ==
                      offsetof(RenderVertexP3N3DiffuseT5, nx) &&
                  offsetof(StandardVertex, b) == 24U &&
                  offsetof(StandardVertex, b) ==
                      offsetof(RenderVertexP3N3DiffuseT5, diffuseBgra) &&
                  offsetof(StandardVertex, u) == 28U &&
                  offsetof(StandardVertex, u) ==
                      offsetof(RenderVertexP3N3DiffuseT5, u) &&
                  offsetof(StandardVertex, tanX) == 44U &&
                  offsetof(StandardVertex, tanX) ==
                      offsetof(RenderVertexP3N3DiffuseT5, tanX) &&
                  offsetof(StandardVertex, binZ) + sizeof(float) ==
                      sizeof(StandardVertex));

bool IsRgba8(const PixelFormat & format)
{
    const DDPIXELFORMAT & value = format.ddpf;
    return value.dwSize == sizeof(DDPIXELFORMAT) &&
        value.dwFlags == (DDPF_RGB | DDPF_ALPHAPIXELS) &&
        value.dwRGBBitCount == 32U && value.dwRBitMask == 0x000000FFU &&
        value.dwGBitMask == 0x0000FF00U && value.dwBBitMask == 0x00FF0000U &&
        value.dwRGBAlphaBitMask == 0xFF000000U;
}

bool IsRgba8Class(const PixelFormat & format)
{
    const DDPIXELFORMAT & value = format.ddpf;
    return value.dwFlags == DDPF_FOURCC &&
        value.dwFourCC == static_cast<U32>(PF_4CC_DAA8);
}

GENRESULT StatusResult(LegacySubmissionStatus status)
{
    switch (status)
    {
    case LegacySubmissionStatus::Ok:
        return GR_OK;
    case LegacySubmissionStatus::UnsupportedPrimitiveType:
    case LegacySubmissionStatus::UnsupportedVertexFormat:
    case LegacySubmissionStatus::UnsupportedIndexedPointList:
        return GR_NOT_IMPLEMENTED;
    case LegacySubmissionStatus::InvalidInput:
        return GR_INVALID_PARMS;
    case LegacySubmissionStatus::InsufficientDestination:
        return GR_OUT_OF_MEMORY;
    }
    return GR_GENERIC;
}

std::uint64_t NormalizedSourceHash(const std::string & source)
{
    std::uint64_t hash = 0xcbf29ce484222325ULL;
    for (std::size_t index = 0U; index < source.size(); ++index)
    {
        char value = source[index];
        if (value == '\r')
        {
            value = '\n';
            if (index + 1U < source.size() && source[index + 1U] == '\n')
                ++index;
        }
        hash = (hash ^ static_cast<unsigned char>(value)) *
               0x100000001b3ULL;
    }
    return hash;
}

class NativeVertexColorEffect final : public ID3DXEffect
{
public:
    explicit NativeVertexColorEffect(IRenderPrimitive * target) : target_(target) {}

    bool Begin(UINT * passCount, U32 flags) override
    {
        if (!passCount || flags || active_ || poisoned_) return false;
        *passCount = 1U;
        active_ = true;
        return true;
    }

    bool BeginPass(UINT pass) override
    {
        if (!active_ || pass != 0U || passActive_ || !target_ || poisoned_)
            return false;
        std::array<U32, 2U> samplerBefore{};
        std::array<U32, 6U> stageBefore{};
        for (std::size_t index = 0U; index < samplerBefore.size(); ++index)
            if (target_->get_sampler_state(0U, kSamplerStates[index],
                                           &samplerBefore[index]) != GR_OK)
                return false;
        for (std::size_t index = 0U; index < stageBefore.size(); ++index)
            if (target_->get_texture_stage_state(0U, kStageStates[index],
                                                 &stageBefore[index]) != GR_OK)
                return false;

        std::size_t samplerApplied = 0U;
        std::size_t stageApplied = 0U;
        for (; samplerApplied < kSamplerStates.size(); ++samplerApplied)
            if (target_->set_sampler_state(0U, kSamplerStates[samplerApplied],
                                           D3DTADDRESS_WRAP) != GR_OK)
            {
                poisoned_ = !Restore(samplerBefore, stageBefore,
                                     samplerApplied, stageApplied);
                return false;
            }
        for (; stageApplied < kStageStates.size(); ++stageApplied)
            if (target_->set_texture_stage_state(0U, kStageStates[stageApplied],
                                                 kStageValues[stageApplied]) != GR_OK)
            {
                poisoned_ = !Restore(samplerBefore, stageBefore,
                                     samplerApplied, stageApplied);
                return false;
            }
        if (target_->set_texture_stage_state(1U, D3DTSS_COLOROP,
                                             D3DTOP_DISABLE) != GR_OK ||
            target_->set_texture_stage_state(1U, D3DTSS_ALPHAOP,
                                             D3DTOP_DISABLE) != GR_OK)
        {
            poisoned_ = !Restore(samplerBefore, stageBefore,
                                 samplerApplied, stageApplied);
            return false;
        }
        passActive_ = true;
        return true;
    }

    bool EndPass() override
    {
        if (!passActive_) return false;
        passActive_ = false;
        return true;
    }

    bool End() override
    {
        if (!active_ || passActive_) return false;
        active_ = false;
        return true;
    }

private:
    bool Restore(const std::array<U32, 2U> & samplers,
                 const std::array<U32, 6U> & stages,
                 std::size_t samplerCount, std::size_t stageCount)
    {
        bool restored = true;
        while (stageCount)
        {
            --stageCount;
            restored &= target_->set_texture_stage_state(
                0U, kStageStates[stageCount], stages[stageCount]) == GR_OK;
        }
        while (samplerCount)
        {
            --samplerCount;
            restored &= target_->set_sampler_state(
                0U, kSamplerStates[samplerCount], samplers[samplerCount]) == GR_OK;
        }
        return restored;
    }

    static constexpr std::array<D3DSAMPLERSTATETYPE, 2U> kSamplerStates{
        D3DSAMP_ADDRESSU, D3DSAMP_ADDRESSV};
    static constexpr std::array<D3DTEXTURESTAGESTATETYPE, 6U> kStageStates{
        D3DTSS_COLOROP, D3DTSS_COLORARG1, D3DTSS_COLORARG2,
        D3DTSS_ALPHAOP, D3DTSS_ALPHAARG1, D3DTSS_ALPHAARG2};
    static constexpr std::array<U32, 6U> kStageValues{
        D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE,
        D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE};
    IRenderPrimitive * target_ = nullptr;
    bool active_ = false;
    bool passActive_ = false;
    // A failed restoration leaves backend state unknown; reject later passes.
    bool poisoned_ = false;
};
} // namespace

NativeRenderPipeline::NativeRenderPipeline(IRenderPrimitive * drawTarget,
                                           NativeTextureOperations textures,
                                           NativeVertexConstants vertexConstants,
                                           void * vertexConstantsContext) noexcept
    : drawTarget_(drawTarget), textures_(textures),
      vertexConstants_(vertexConstants),
      vertexConstantsContext_(vertexConstantsContext)
{
}

NativeRenderPipeline::NativeRenderPipeline(NativeVertexBufferLookup lookup,
                                           void * lookupContext,
                                           IRenderPrimitive * drawTarget,
                                           NativeTextureOperations textures,
                                           NativeVertexConstants vertexConstants,
                                           void * vertexConstantsContext) noexcept
    : lookup_(lookup), lookupContext_(lookupContext), drawTarget_(drawTarget),
      textures_(textures), vertexConstants_(vertexConstants),
      vertexConstantsContext_(vertexConstantsContext)
{
}

GENRESULT NativeRenderPipeline::BindScene(
    IRenderPrimitive * drawTarget, NativeTextureOperations textures,
    NativeVertexConstants vertexConstants,
    void * vertexConstantsContext) noexcept
{
    if (!drawTarget || drawTarget_ || !ownedTextures_.empty() ||
        !ownedBuffers_.empty() || !effects_.empty()) return GR_INVALID_PARMS;
    drawTarget_ = drawTarget;
    textures_ = textures;
    vertexConstants_ = vertexConstants;
    vertexConstantsContext_ = vertexConstantsContext;
    return GR_OK;
}

GENRESULT NativeRenderPipeline::UnbindScene() noexcept
{
    if (!drawTarget_) return GR_INVALID_PARMS;
    effects_.clear();
    GENRESULT result = GR_OK;
    for (const auto & texture : ownedTextures_)
    {
        if (!textures_.destroy ||
            textures_.destroy(textures_.context, texture.first) != GR_OK)
            result = GR_GENERIC;
    }
    ownedTextures_.clear();
    ownedBuffers_.clear();
    nextHandle_ = 1U;
    converted_.clear();
    converted2_.clear();
    textureEnabled_ = 1U;
    lookup_ = nullptr;
    lookupContext_ = nullptr;
    drawTarget_ = nullptr;
    textures_ = {};
    vertexConstants_ = nullptr;
    vertexConstantsContext_ = nullptr;
    return result;
}

GENRESULT NativeRenderPipeline::QueryInterface(const C8 * interfaceName, void ** instance)
{
    if (!instance)
    {
        return GR_INVALID_PARMS;
    }
    *instance = nullptr;
    if (!interfaceName)
    {
        return GR_INTERFACE_UNSUPPORTED;
    }
    if (std::strcmp(interfaceName, IID_IRenderPipeline) == 0)
    {
        *instance = static_cast<IRenderPipeline *>(this);
    }
    else if (std::strcmp(interfaceName, IID_IDAComponent) == 0)
    {
        *instance = static_cast<IDAComponent *>(this);
    }
    else
    {
        return GR_INTERFACE_UNSUPPORTED;
    }
    AddRef();
    return GR_OK;
}

U32 NativeRenderPipeline::AddRef()
{
    return referenceCount_.fetch_add(1U, std::memory_order_relaxed) + 1U;
}

U32 NativeRenderPipeline::Release()
{
    const U32 remaining = referenceCount_.fetch_sub(1U, std::memory_order_acq_rel) - 1U;
    if (remaining == 0U)
    {
        delete this;
    }
    return remaining;
}

ID3DXEffect ** NativeRenderPipeline::load_effect(
    const char * filename, IComponentFactory * directory)
{
    if (!filename || std::strcmp(filename, "VertexColor.fx") != 0 ||
        !directory || !drawTarget_ || !vertexConstants_)
        return nullptr;
    const auto found = effects_.find(filename);
    if (found != effects_.end()) return &found->second.pointer;

    DAFILEDESC descriptor(filename);
    IFileSystem * file = nullptr;
    if (directory->CreateInstance(&descriptor,
                                  reinterpret_cast<void **>(&file)) != GR_OK ||
        !file)
        return nullptr;
    const U32 size = file->GetFileSize();
    if (!size || size > kMaximumEffectSourceBytes)
    {
        file->Release();
        return nullptr;
    }
    std::string source;
    try
    {
        source.resize(size);
    }
    catch (const std::bad_alloc &)
    {
        file->Release();
        return nullptr;
    }
    U32 bytesRead = 0U;
    const bool complete = file->ReadFile(nullptr, source.data(), size,
                                          &bytesRead, nullptr) && bytesRead == size;
    file->Release();
    if (!complete || NormalizedSourceHash(source) != kVertexColorSourceHash)
        return nullptr;

    try
    {
        OwnedEffect effect;
        effect.owner = std::make_unique<NativeVertexColorEffect>(drawTarget_);
        effect.pointer = effect.owner.get();
        return &effects_.emplace(filename, std::move(effect)).first->second.pointer;
    }
    catch (const std::bad_alloc &)
    {
        return nullptr;
    }
}

GENRESULT NativeRenderPipeline::set_vs_constants(UINT firstRegister,
                                                 CONST FLOAT * values,
                                                 UINT registerCount)
{
    if (!vertexConstants_) return GR_NOT_IMPLEMENTED;
    if (!values || !registerCount || firstRegister >= 256U ||
        registerCount > 256U - firstRegister)
        return GR_INVALID_PARMS;
    if (firstRegister < 30U || firstRegister > 31U ||
        registerCount > 32U - firstRegister)
        return GR_NOT_IMPLEMENTED;
    return vertexConstants_(vertexConstantsContext_, firstRegister,
                            values, registerCount);
}

GENRESULT NativeRenderPipeline::set_pipeline_state(RPPIPELINESTATE state, U32 value)
{
    if (state != RP_TEXTURE) return GR_NOT_IMPLEMENTED;
    if (value > 1U) return GR_INVALID_PARMS;
    textureEnabled_ = value;
    return GR_OK;
}

GENRESULT NativeRenderPipeline::get_pipeline_state(RPPIPELINESTATE state, U32 * value)
{
    if (!value) return GR_INVALID_PARMS;
    if (state != RP_TEXTURE) return GR_NOT_IMPLEMENTED;
    *value = textureEnabled_;
    return GR_OK;
}

GENRESULT NativeRenderPipeline::query_device_ability(RPDEVICEABILITY ability,
                                                      U32 * outAnswer)
{
    if (!outAnswer)
    {
        return GR_INVALID_PARMS;
    }
    *outAnswer = 0U;
    return ability == RP_A_DEPTH_BIAS ? GR_OK : GR_NOT_IMPLEMENTED;
}

GENRESULT NativeRenderPipeline::set_modelview(const Transform & modelview)
{
    return drawTarget_ ? drawTarget_->set_modelview(modelview) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_modelview(Transform & modelview)
{
    return drawTarget_ ? drawTarget_->get_modelview(modelview) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_projection(const Matrix4 & projection)
{
    return drawTarget_ ? drawTarget_->set_projection(projection) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_projection(Matrix4 & projection)
{
    return drawTarget_ ? drawTarget_->get_projection(projection) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_viewport(int x, int y, int width, int height)
{
    return drawTarget_ ? drawTarget_->set_viewport(x, y, width, height) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_viewport(int * x, int * y, int * width, int * height)
{
    return drawTarget_ ? drawTarget_->get_viewport(x, y, width, height) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_ortho(float left, float right, float bottom, float top,
                                          float nearValue, float farValue)
{
    return drawTarget_ ? drawTarget_->set_ortho(
                             left, right, bottom, top, nearValue, farValue)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_perspective(float fieldOfView, float aspect,
                                                float nearValue, float farValue)
{
    return drawTarget_ ? drawTarget_->set_perspective(
                             fieldOfView, aspect, nearValue, farValue)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_material(D3DMATERIAL9 * material)
{
    return drawTarget_ ? drawTarget_->set_material(material) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_material(D3DMATERIAL9 * material)
{
    return drawTarget_ ? drawTarget_->get_material(material) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::begin_scene()
{
    return drawTarget_ ? drawTarget_->begin_scene() : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::end_scene()
{
    return drawTarget_ ? drawTarget_->end_scene() : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_render_state(D3DRENDERSTATETYPE state, U32 value)
{
    return drawTarget_ ? drawTarget_->set_render_state(state, value) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_render_state(D3DRENDERSTATETYPE state, U32 * value)
{
    return drawTarget_ ? drawTarget_->get_render_state(state, value) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_sampler_state(U32 stage, D3DSAMPLERSTATETYPE state,
                                                  U32 value)
{
    return drawTarget_ ? drawTarget_->set_sampler_state(stage, state, value) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_sampler_state(U32 stage, D3DSAMPLERSTATETYPE state,
                                                  U32 * value)
{
    return drawTarget_ ? drawTarget_->get_sampler_state(stage, state, value) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_texture_stage_state(
    U32 stage, D3DTEXTURESTAGESTATETYPE state, U32 value)
{
    return drawTarget_ ? drawTarget_->set_texture_stage_state(stage, state, value)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_texture_stage_state(
    U32 stage, D3DTEXTURESTAGESTATETYPE state, U32 * value)
{
    return drawTarget_ ? drawTarget_->get_texture_stage_state(stage, state, value)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::set_texture_stage_texture(U32 stage, U32 texture)
{
    return drawTarget_ ? drawTarget_->set_texture_stage_texture(stage, texture) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::get_texture_stage_texture(U32 stage, U32 * texture)
{
    return drawTarget_ ? drawTarget_->get_texture_stage_texture(stage, texture) : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::is_texture(U32 texture)
{
    if (!textures_.lookup) return GR_NOT_IMPLEMENTED;
    return texture && textures_.lookup(textures_.context, texture) ? GR_OK : GR_INVALID_PARMS;
}

GENRESULT NativeRenderPipeline::create_texture(int width, int height,
                                                const PixelFormat & format,
                                                int lodCount, U32 flags, U32 & handle)
{
    handle = 0U;
    if (!textures_.create || !textures_.destroy) return GR_NOT_IMPLEMENTED;
    if ((!IsRgba8(format) && !IsRgba8Class(format)) ||
        lodCount < 0 || lodCount > 1 || flags != 0U)
        return GR_NOT_IMPLEMENTED;
    if (width <= 0 || height <= 0 ||
        static_cast<U32>(width) > std::numeric_limits<U32>::max() / 4U ||
        static_cast<U32>(height) >
            std::numeric_limits<U32>::max() / (static_cast<U32>(width) * 4U))
        return GR_INVALID_PARMS;
    OwnedTexture owned;
    owned.width = static_cast<U32>(width);
    owned.height = static_cast<U32>(height);
    try
    {
        owned.pixels.resize(static_cast<std::size_t>(width) * height * 4U);
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_MEMORY;
    }
    U32 candidate = 0U;
    const GENRESULT result = textures_.create(
        textures_.context, static_cast<U32>(width), static_cast<U32>(height), candidate);
    if (result != GR_OK) return result;
    if (!candidate) return GR_GENERIC;
    if (candidate > kMaxTextureStateId)
    {
        textures_.destroy(textures_.context, candidate);
        return GR_INVALID_PARMS;
    }
    try
    {
        const auto inserted = ownedTextures_.emplace(candidate, std::move(owned));
        if (!inserted.second) return GR_GENERIC;
    }
    catch (const std::bad_alloc &)
    {
        textures_.destroy(textures_.context, candidate);
        return GR_OUT_OF_MEMORY;
    }
    handle = candidate;
    return GR_OK;
}

GENRESULT NativeRenderPipeline::destroy_texture(U32 handle)
{
    if (!textures_.destroy) return GR_NOT_IMPLEMENTED;
    const auto found = ownedTextures_.find(handle);
    if (found == ownedTextures_.end()) return GR_INVALID_PARMS;
    if (found->second.locked) return GR_GENERIC;
    const GENRESULT result = textures_.destroy(textures_.context, handle);
    if (result == GR_OK) ownedTextures_.erase(found);
    return result;
}

GENRESULT NativeRenderPipeline::lock_texture(U32 handle, int level, RPLOCKDATA * lockData)
{
    if (!lockData) return GR_INVALID_PARMS;
    const auto found = ownedTextures_.find(handle);
    if (found == ownedTextures_.end() || level != 0) return GR_INVALID_PARMS;
    OwnedTexture & texture = found->second;
    if (texture.locked || !textures_.upload) return GR_GENERIC;
    lockData->width = texture.width;
    lockData->height = texture.height;
    lockData->pitch = texture.width * 4U;
    lockData->pixels = texture.pixels.data();
    lockData->pf = PixelFormat(PF_RGBA8);
    texture.locked = true;
    return GR_OK;
}

GENRESULT NativeRenderPipeline::unlock_texture(U32 handle, int level)
{
    const auto found = ownedTextures_.find(handle);
    if (found == ownedTextures_.end() || level != 0 || !found->second.locked)
        return GR_INVALID_PARMS;
    OwnedTexture & texture = found->second;
    const GENRESULT result = textures_.upload(textures_.context, handle,
        texture.pixels.data(), texture.width, texture.height, texture.pixels.size());
    if (result == GR_OK) texture.locked = false;
    return result;
}

GENRESULT NativeRenderPipeline::set_texture_level_data(
    U32 handle, int level, int width, int height, int stride,
    const PixelFormat & format, const void * pixels, const void * alpha,
    const RGB * palette)
{
    if (!textures_.upload) return GR_NOT_IMPLEMENTED;
    const auto found = ownedTextures_.find(handle);
    if (found == ownedTextures_.end() || level != 0 || width <= 0 || height <= 0 ||
        static_cast<U32>(width) != found->second.width ||
        static_cast<U32>(height) != found->second.height || found->second.locked ||
        !pixels || width > std::numeric_limits<int>::max() / 4)
        return GR_NOT_IMPLEMENTED;
    const bool rgba8 = IsRgba8(format);
    const bool rgb565 = format.ddpf.dwRGBBitCount == 16 &&
        format.ddpf.dwRBitMask == 0xf800U &&
        format.ddpf.dwGBitMask == 0x07e0U &&
        format.ddpf.dwBBitMask == 0x001fU && !alpha;
    if ((!rgba8 && !rgb565) || (palette && !rgb565) ||
        stride < width * (rgba8 ? 4 : 2))
        return GR_NOT_IMPLEMENTED;
    const std::size_t rowBytes = static_cast<std::size_t>(width) * 4U;
    const std::size_t byteCount = rowBytes * static_cast<std::size_t>(height);
    std::vector<std::uint8_t> rgba;
    try
    {
        rgba.resize(byteCount);
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_MEMORY;
    }
    const auto * source = static_cast<const std::uint8_t *>(pixels);
    const auto * alphaSource = static_cast<const std::uint8_t *>(alpha);
    for (int row = 0; row < height; ++row)
    {
        auto * destination = rgba.data() + static_cast<std::size_t>(row) * rowBytes;
        const auto * sourceRow = source +
            static_cast<std::size_t>(row) * static_cast<std::size_t>(stride);
        if (rgba8) std::memcpy(destination, sourceRow, rowBytes);
        else
        {
            for (int column = 0; column < width; ++column)
            {
                const std::size_t offset = static_cast<std::size_t>(column) * 2U;
                const U32 packed = static_cast<U32>(sourceRow[offset]) |
                    (static_cast<U32>(sourceRow[offset + 1U]) << 8U);
                const U32 red = (packed >> 11U) & 31U;
                const U32 green = (packed >> 5U) & 63U;
                const U32 blue = packed & 31U;
                const std::size_t target = static_cast<std::size_t>(column) * 4U;
                destination[target] = static_cast<std::uint8_t>((red << 3U) | (red >> 2U));
                destination[target + 1U] = static_cast<std::uint8_t>((green << 2U) | (green >> 4U));
                destination[target + 2U] = static_cast<std::uint8_t>((blue << 3U) | (blue >> 2U));
                destination[target + 3U] = 255U;
            }
        }
        if (alphaSource)
        {
            for (int column = 0; column < width; ++column)
                destination[static_cast<std::size_t>(column) * 4U + 3U] =
                    alphaSource[static_cast<std::size_t>(row) * width + column];
        }
    }
    const GENRESULT result = textures_.upload(textures_.context, handle, rgba.data(),
        static_cast<U32>(width), static_cast<U32>(height), byteCount);
    if (result == GR_OK) found->second.pixels.swap(rgba);
    return result;
}

GENRESULT NativeRenderPipeline::blit_texture(U32 destination, U32 destinationLevel,
                                              RECT destinationRect, U32 source,
                                              U32 sourceLevel, RECT sourceRect)
{
    if (!textures_.upload) return GR_NOT_IMPLEMENTED;
    const auto destinationIt = ownedTextures_.find(destination);
    const auto sourceIt = ownedTextures_.find(source);
    if (destinationIt == ownedTextures_.end() || sourceIt == ownedTextures_.end() ||
        destinationLevel != 0U || sourceLevel != 0U ||
        destinationIt->second.locked || sourceIt->second.locked)
        return GR_INVALID_PARMS;

    const OwnedTexture & from = sourceIt->second;
    OwnedTexture & to = destinationIt->second;
    // The native owner currently supports only complete, equal-size RGBA8 copies.
    if (destinationRect.left != 0 || destinationRect.top != 0 ||
        sourceRect.left != 0 || sourceRect.top != 0 ||
        destinationRect.right != static_cast<long>(to.width) ||
        destinationRect.bottom != static_cast<long>(to.height) ||
        sourceRect.right != static_cast<long>(from.width) ||
        sourceRect.bottom != static_cast<long>(from.height) ||
        to.width != from.width || to.height != from.height)
        return GR_NOT_IMPLEMENTED;
    if (destination == source) return GR_OK;

    std::vector<std::uint8_t> copy;
    try
    {
        copy = from.pixels;
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_MEMORY;
    }
    const GENRESULT result = textures_.upload(textures_.context, destination,
        copy.data(), from.width, from.height, copy.size());
    if (result == GR_OK) to.pixels.swap(copy);
    return result;
}

GENRESULT NativeRenderPipeline::get_texture_format(U32 handle, PixelFormat * format)
{
    if (!format) return GR_INVALID_PARMS;
    if (ownedTextures_.find(handle) == ownedTextures_.end()) return GR_INVALID_PARMS;
    *format = PixelFormat(PF_RGBA8);
    return GR_OK;
}

GENRESULT NativeRenderPipeline::get_texture_dim(U32 handle, U32 * width, U32 * height,
                                                 U32 * lodCount)
{
    if (!width || !height || !lodCount) return GR_INVALID_PARMS;
    const auto found = ownedTextures_.find(handle);
    if (found == ownedTextures_.end()) return GR_INVALID_PARMS;
    *width = found->second.width;
    *height = found->second.height;
    *lodCount = 1U;
    return GR_OK;
}

GENRESULT NativeRenderPipeline::verify_state()
{
    return drawTarget_ ? drawTarget_->verify_state() : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::draw_primitive(D3DPRIMITIVETYPE primitiveType,
                                               U32 vertexFormat,
                                               const void * vertices,
                                               int vertexCount,
                                               U32 flags)
{
    return drawTarget_ ? drawTarget_->draw_primitive(
                             primitiveType, vertexFormat, vertices, vertexCount, flags)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::draw_indexed_primitive(
    D3DPRIMITIVETYPE primitiveType, U32 vertexFormat, const void * vertices,
    int vertexCount, const U16 * indices, int indexCount, U32 flags)
{
    return drawTarget_ ? drawTarget_->draw_indexed_primitive(
                             primitiveType, vertexFormat, vertices, vertexCount,
                             indices, indexCount, flags)
                       : GR_GENERIC;
}

GENRESULT NativeRenderPipeline::create_vertex_buffer(
    U32 vertexFormat,
    int vertexCount,
    U32 flags,
    IRP_VERTEXBUFFERHANDLE * outHandle)
{
    if (!outHandle)
    {
        return GR_INVALID_PARMS;
    }
    *outHandle = 0U;
    if (lookup_)
    {
        return GR_NOT_IMPLEMENTED;
    }
    if (vertexCount <= 0)
    {
        return GR_INVALID_PARMS;
    }
    if ((flags & ~kSupportedVertexBufferFlags) != 0U)
    {
        return GR_NOT_IMPLEMENTED;
    }
    const bool extended = vertexFormat == LegacyD3d9::kRpVertex2Fvf;
    const bool gameStandard = vertexFormat == LegacyD3d9::kGameStandardFvf;
    if (vertexFormat != LegacyD3d9::kRpVertexFvf && !extended && !gameStandard)
    {
        return GR_NOT_IMPLEMENTED;
    }
    const std::size_t stride = gameStandard ? sizeof(StandardVertex) :
        extended ? sizeof(RenderVertexP3DiffuseT4) : sizeof(RenderVertexP3DiffuseT2);
    const auto count = static_cast<std::size_t>(vertexCount);
    if (count > std::numeric_limits<std::size_t>::max() / stride || nextHandle_ == 0U)
    {
        return GR_OUT_OF_MEMORY;
    }

    OwnedVertexBuffer buffer;
    buffer.vertexFormat = vertexFormat;
    buffer.vertexCount = static_cast<U32>(vertexCount);
    try
    {
        buffer.bytes.resize(count * stride);
        const auto inserted = ownedBuffers_.emplace(nextHandle_, std::move(buffer));
        if (!inserted.second)
        {
            return GR_GENERIC;
        }
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_MEMORY;
    }
    catch (const std::length_error &)
    {
        return GR_OUT_OF_MEMORY;
    }
    *outHandle = nextHandle_++;
    return GR_OK;
}

GENRESULT NativeRenderPipeline::destroy_vertex_buffer(IRP_VERTEXBUFFERHANDLE handle)
{
    const auto found = ownedBuffers_.find(handle);
    if (found == ownedBuffers_.end())
    {
        return GR_INVALID_PARMS;
    }
    if (found->second.locked)
    {
        return GR_GENERIC;
    }
    ownedBuffers_.erase(found);
    return GR_OK;
}

GENRESULT NativeRenderPipeline::lock_vertex_buffer(
    IRP_VERTEXBUFFERHANDLE handle,
    U32,
    void ** outVertices,
    U32 * outVertexCount)
{
    if (!outVertices || !outVertexCount)
    {
        return GR_INVALID_PARMS;
    }
    *outVertices = nullptr;
    *outVertexCount = 0U;
    const auto found = ownedBuffers_.find(handle);
    if (found == ownedBuffers_.end() || found->second.locked)
    {
        return GR_INVALID_PARMS;
    }
    found->second.locked = true;
    *outVertices = found->second.bytes.data();
    *outVertexCount = found->second.vertexCount;
    return GR_OK;
}

GENRESULT NativeRenderPipeline::unlock_vertex_buffer(IRP_VERTEXBUFFERHANDLE handle)
{
    const auto found = ownedBuffers_.find(handle);
    if (found == ownedBuffers_.end() || !found->second.locked)
    {
        return GR_INVALID_PARMS;
    }
    found->second.locked = false;
    return GR_OK;
}

NativeVertexBufferView NativeRenderPipeline::OwnedVertexBufferLookup(
    void * context, IRP_VERTEXBUFFERHANDLE handle) noexcept
{
    return context
        ? static_cast<const NativeRenderPipeline *>(context)->LookupOwnedVertexBuffer(handle)
        : NativeVertexBufferView{};
}

NativeVertexBufferView NativeRenderPipeline::LookupOwnedVertexBuffer(
    IRP_VERTEXBUFFERHANDLE handle) const noexcept
{
    const auto found = ownedBuffers_.find(handle);
    if (found == ownedBuffers_.end() || found->second.locked)
    {
        return {};
    }
    return {found->second.bytes.data(), found->second.bytes.size(),
            found->second.vertexFormat};
}

GENRESULT NativeRenderPipeline::draw_primitive_vb(D3DPRIMITIVETYPE primitiveType,
                                                  IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
                                                  int startVertex,
                                                  int vertexCount,
                                                  U32 flags)
{
    return Submit(primitiveType, vertexBufferHandle, startVertex, vertexCount,
                  nullptr, 0, flags);
}

GENRESULT NativeRenderPipeline::draw_indexed_primitive_vb(
    D3DPRIMITIVETYPE primitiveType,
    IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
    int startVertex,
    int vertexCount,
    const U16 * indices,
    int indexCount,
    U32 flags)
{
    if (!indices || indexCount <= 0)
    {
        return GR_INVALID_PARMS;
    }
    return Submit(primitiveType, vertexBufferHandle, startVertex, vertexCount,
                  indices, indexCount, flags);
}

GENRESULT NativeRenderPipeline::draw_indexed_primitive_vb(
    D3DPRIMITIVETYPE primitiveType,
    IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
    int startVertex,
    int vertexCount,
    IDirect3DIndexBuffer9 * indexBuffer,
    int indexCount,
    U32 flags)
{
    if (!indexBuffer || indexCount <= 0 ||
        static_cast<std::size_t>(indexCount) > indexBuffer->Count())
        return GR_INVALID_PARMS;
    const U16 * indices = indexBuffer->Data();
    if (!indices) return GR_GENERIC;
    return draw_indexed_primitive_vb(primitiveType, vertexBufferHandle,
                                     startVertex, vertexCount, indices,
                                     indexCount, flags);
}

GENRESULT NativeRenderPipeline::create_index_buffer(
    UINT byteLength, IDirect3DIndexBuffer9 ** outIndexBuffer)
{
    if (!outIndexBuffer) return GR_INVALID_PARMS;
    *outIndexBuffer = nullptr;
    if (!byteLength || (byteLength % sizeof(U16)) != 0U)
        return GR_INVALID_PARMS;
    try
    {
        *outIndexBuffer = new IDirect3DIndexBuffer9(byteLength / sizeof(U16));
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_MEMORY;
    }
    catch (const std::length_error &)
    {
        return GR_OUT_OF_MEMORY;
    }
    return GR_OK;
}

GENRESULT NativeRenderPipeline::Submit(D3DPRIMITIVETYPE primitiveType,
                                       IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
                                       int startVertex,
                                       int vertexCount,
                                       const U16 * indices,
                                       int indexCount,
                                       U32 flags)
{
    if (startVertex < 0 || vertexCount <= 0 || vertexBufferHandle == 0U)
    {
        return GR_INVALID_PARMS;
    }
    if (!drawTarget_)
    {
        return GR_GENERIC;
    }
    const NativeVertexBufferView buffer = lookup_
        ? lookup_(lookupContext_, vertexBufferHandle)
        : LookupOwnedVertexBuffer(vertexBufferHandle);
    if (!buffer.vertices)
    {
        return GR_GENERIC;
    }
    if (buffer.vertexFormat != LegacyD3d9::kRpVertexFvf &&
        buffer.vertexFormat != LegacyD3d9::kRpVertex2Fvf &&
        buffer.vertexFormat != LegacyD3d9::kGameStandardFvf)
    {
        return GR_NOT_IMPLEMENTED;
    }
    const std::uint32_t vertexFormat = buffer.vertexFormat;
    const bool extended = vertexFormat == LegacyD3d9::kRpVertex2Fvf;
    const bool gameStandard = vertexFormat == LegacyD3d9::kGameStandardFvf;
    const std::size_t stride = gameStandard ? sizeof(StandardVertex) :
        extended ? sizeof(RenderVertexP3DiffuseT4) : sizeof(RenderVertexP3DiffuseT2);
    const std::size_t capacity = buffer.byteSize / stride;
    const std::size_t base = static_cast<std::size_t>(startVertex);
    const std::size_t count = static_cast<std::size_t>(vertexCount);
    if (base > capacity || count > capacity - base)
    {
        return GR_INVALID_PARMS;
    }

    if (gameStandard)
    {
        if (primitiveType != D3DPT_TRIANGLELIST || flags != 0U)
            return GR_NOT_IMPLEMENTED;
        const std::size_t primitiveVertexCount = indices ?
            static_cast<std::size_t>(indexCount) : count;
        if (primitiveVertexCount % 3U != 0U)
            return GR_INVALID_PARMS;
        for (std::size_t index = 0U; indices && index < primitiveVertexCount; ++index)
            if (indices[index] >= count) return GR_INVALID_PARMS;
        const void * slice = static_cast<const unsigned char *>(buffer.vertices) +
                             base * stride;
        return indices ? drawTarget_->draw_indexed_primitive(
                             primitiveType, vertexFormat, slice, vertexCount,
                             indices, indexCount, flags) :
                         drawTarget_->draw_primitive(
                             primitiveType, vertexFormat, slice, vertexCount, flags);
    }

    // Windows uses start_vert as BaseVertexIndex. Indices remain slice-relative.
    const LegacySubmissionInput input{
        static_cast<std::uint32_t>(primitiveType),
        vertexFormat,
        static_cast<const unsigned char *>(buffer.vertices) + base * stride,
        count,
        indices,
        static_cast<std::size_t>(indexCount),
    };
    LegacySubmissionStatus status;
    const void * vertices = nullptr;
    try
    {
        if (extended)
        {
            converted2_.resize(count);
            RenderSubmissionView2 submission{};
            status = AdaptLegacyRpVertex2Submission(
                input, converted2_.data(), converted2_.size(), submission);
            vertices = submission.vertices;
        }
        else
        {
            converted_.resize(count);
            RenderSubmissionView submission{};
            status = AdaptLegacyRpVertexSubmission(
                input, converted_.data(), converted_.size(), submission);
            vertices = submission.vertices;
        }
    }
    catch (const std::bad_alloc &)
    {
        return GR_OUT_OF_MEMORY;
    }
    if (status != LegacySubmissionStatus::Ok)
    {
        return StatusResult(status);
    }
    return indices ? drawTarget_->draw_indexed_primitive(
                         primitiveType, vertexFormat, vertices, vertexCount,
                         indices, indexCount, flags)
                   : drawTarget_->draw_primitive(
                         primitiveType, vertexFormat, vertices, vertexCount, flags);
}
void RegisterNativeRenderPipeline(ICOManager * manager)
{
    if (!manager) return;
    IComponentFactory * factory = new (std::nothrow)
        DAComponentFactory2<DAComponentAggregate<NativeProfileRenderPipeline>,
                            AGGDESC>(profilePipelineName);
    if (!factory) return;
    manager->RegisterComponent(factory, profilePipelineName);
    factory->Release();
}

GENRESULT BindNativeRenderPipeline(IRenderPipeline * pipeline,
                                   IRenderPrimitive * drawTarget,
                                   NativeTextureOperations textures,
                                   NativeVertexConstants vertexConstants,
                                   void * vertexConstantsContext)
{
    auto * owner = dynamic_cast<NativeProfileRenderPipeline *>(pipeline);
    return owner ? owner->BindScene(drawTarget, textures, vertexConstants,
                                    vertexConstantsContext)
                 : GR_INVALID_PARMS;
}

GENRESULT UnbindNativeRenderPipeline(IRenderPipeline * pipeline)
{
    auto * owner = dynamic_cast<NativeProfileRenderPipeline *>(pipeline);
    return owner ? owner->UnbindScene() : GR_INVALID_PARMS;
}
} // namespace FrontierX
