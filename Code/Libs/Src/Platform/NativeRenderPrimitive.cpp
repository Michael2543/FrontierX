#include "FrontierX/NativeRenderPrimitive.h"

#include "FrontierX/LegacyRenderSubmission.h"
#include "IRenderPrimitive.h"
#include "TComponent.h"
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif
#include "matrix4.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include <vector>

namespace FrontierX
{
namespace
{
char interface_name[] = "IRenderPrimitive";

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
    case LegacySubmissionStatus::InsufficientDestination:
        return GR_OUT_OF_MEMORY;
    case LegacySubmissionStatus::InvalidInput:
        return GR_INVALID_PARMS;
    }
    return GR_GENERIC;
}

class NativeRenderPrimitive : public IRenderPrimitive
{
public:
    BEGIN_DACOM_MAP_INBOUND(NativeRenderPrimitive)
    DACOM_INTERFACE_ENTRY(IRenderPrimitive)
    DACOM_INTERFACE_ENTRY2(IID_IRenderPrimitive, IRenderPrimitive)
    END_DACOM_MAP()

    GENRESULT init(const NativeRenderPrimitiveDesc * descriptor)
    {
        if (!descriptor || descriptor->size != sizeof(*descriptor) ||
            !descriptor->interface_name ||
            std::strcmp(descriptor->interface_name, interface_name) != 0 ||
            !descriptor->callback || callback_)
        {
            return GR_INVALID_PARMS;
        }
        std::unique_ptr<NativeRenderPipeline> vertexPipeline;
        if (descriptor->lookup)
        {
            try
            {
                vertexPipeline = std::make_unique<NativeRenderPipeline>(
                    descriptor->lookup, descriptor->lookupContext, this);
            }
            catch (const std::bad_alloc &)
            {
                return GR_OUT_OF_MEMORY;
            }
        }
        callback_ = descriptor->callback;
        callback2_ = descriptor->callback2;
        callback3_ = descriptor->callback3;
        callback4_ = descriptor->callback4;
        stateCallback_ = descriptor->state;
        textureStageCallback_ = descriptor->textureStage;
        textureBindCallback_ = descriptor->textureBind;
        samplerCallback_ = descriptor->sampler;
        modelviewCallback_ = descriptor->modelview;
        projectionCallback_ = descriptor->projection;
        viewportCallback_ = descriptor->viewport;
        context_ = descriptor->context;
        vertexPipeline_ = std::move(vertexPipeline);
        return GR_OK;
    }

    GENRESULT init(AGGDESC * descriptor)
    {
        if (!descriptor || descriptor->size != sizeof(*descriptor) ||
            !descriptor->interface_name ||
            std::strcmp(descriptor->interface_name, interface_name) != 0 ||
            !descriptor->outer || !descriptor->inner)
            return GR_INVALID_PARMS;
        profileAggregate_ = true;
        return GR_OK;
    }

    GENRESULT Bind(const NativeRenderPrimitiveDesc & descriptor)
    {
        return profileAggregate_ ? init(&descriptor) : GR_INVALID_PARMS;
    }

    GENRESULT Unbind()
    {
        if (!profileAggregate_ || !callback_) return GR_INVALID_PARMS;
        vertexPipeline_.reset();
        callback_ = nullptr;
        callback2_ = nullptr;
        callback3_ = nullptr;
        callback4_ = nullptr;
        stateCallback_ = nullptr;
        textureStageCallback_ = nullptr;
        textureBindCallback_ = nullptr;
        samplerCallback_ = nullptr;
        modelviewCallback_ = nullptr;
        projectionCallback_ = nullptr;
        viewportCallback_ = nullptr;
        context_ = nullptr;
        renderState_ = {};
        stateId_ = 0U;
        textureStage_ = {};
        samplerState_ = {};
        textureHandle_ = 0U;
        modelview_ = {{1.0F, 0.0F, 0.0F, 0.0F,
                       0.0F, 1.0F, 0.0F, 0.0F,
                       0.0F, 0.0F, 1.0F, 0.0F}};
        projection_ = Matrix4(true);
        viewport_ = {{0, 0, 0, 0}};
        converted_.clear();
        converted2_.clear();
        converted3_.clear();
        converted4_.clear();
        return GR_OK;
    }

    GENRESULT COMAPI set_state(RPRSTATE state, U32 value) override
    {
        if (state != RPR_STATE_ID) return GR_NOT_IMPLEMENTED;
        // The native primitive submits immediately; retain the game's batching
        // key for callers that query it without changing draw order.
        stateId_ = value;
        return GR_OK;
    }
    GENRESULT COMAPI get_state(RPRSTATE state, U32 * value) override
    {
        if (state != RPR_STATE_ID) return GR_NOT_IMPLEMENTED;
        if (!value) return GR_INVALID_PARMS;
        *value = stateId_;
        return GR_OK;
    }
    GENRESULT COMAPI set_modelview(const Transform & modelview) override
    {
        if (!modelviewCallback_) return GR_NOT_IMPLEMENTED;
        const GENRESULT result = modelviewCallback_(context_, modelview);
        if (result == GR_OK)
        {
            for (unsigned row = 0U; row < 3U; ++row)
                for (unsigned column = 0U; column < 3U; ++column)
                    modelview_[row * 4U + column] = modelview.d[row][column];
            modelview_[3] = modelview.translation.x;
            modelview_[7] = modelview.translation.y;
            modelview_[11] = modelview.translation.z;
        }
        return result;
    }
    GENRESULT COMAPI get_modelview(Transform & modelview) override
    {
        if (!modelviewCallback_) return GR_NOT_IMPLEMENTED;
        for (unsigned row = 0U; row < 3U; ++row)
            for (unsigned column = 0U; column < 3U; ++column)
                modelview.d[row][column] = modelview_[row * 4U + column];
        modelview.translation.x = modelview_[3];
        modelview.translation.y = modelview_[7];
        modelview.translation.z = modelview_[11];
        return GR_OK;
    }
    GENRESULT COMAPI set_viewport(int x, int y, int width, int height) override
    {
        if (!viewportCallback_) return GR_NOT_IMPLEMENTED;
        if (x < 0 || y < 0 || width <= 0 || height <= 0 ||
            x > INT32_MAX - width || y > INT32_MAX - height)
            return GR_INVALID_PARMS;
        const GENRESULT result = viewportCallback_(context_, x, y, width, height);
        if (result == GR_OK) viewport_ = {{x, y, width, height}};
        return result;
    }
    GENRESULT COMAPI get_viewport(int * x, int * y, int * width, int * height) override
    {
        if (!viewportCallback_) return GR_NOT_IMPLEMENTED;
        if (!x || !y || !width || !height) return GR_INVALID_PARMS;
        *x = viewport_[0]; *y = viewport_[1];
        *width = viewport_[2]; *height = viewport_[3];
        return GR_OK;
    }
    GENRESULT COMAPI set_ortho(float left, float right, float bottom, float top,
                               float nearval, float farval) override
    {
        if (!projectionCallback_) return GR_NOT_IMPLEMENTED;
        if (!std::isfinite(left) || !std::isfinite(right) ||
            !std::isfinite(bottom) || !std::isfinite(top) ||
            !std::isfinite(nearval) || !std::isfinite(farval) ||
            left == right || bottom == top || nearval == farval)
            return GR_INVALID_PARMS;
        Matrix4 projection(true);
        projection.d[0][0] = 2.0F / (right - left);
        projection.d[1][1] = 2.0F / (top - bottom);
        projection.d[2][2] = 1.0F / (farval - nearval);
        projection.d[0][3] = -(right + left) / (right - left);
        projection.d[1][3] = -(top + bottom) / (top - bottom);
        projection.d[2][3] = -nearval / (farval - nearval);
        for (unsigned row = 0U; row < 4U; ++row)
            for (unsigned column = 0U; column < 4U; ++column)
                if (!std::isfinite(projection.d[row][column]))
                    return GR_INVALID_PARMS;
        return set_projection(projection);
    }
    GENRESULT COMAPI set_perspective(float fovyHalfAngle, float aspect,
                                    float nearval, float farval) override
    {
        if (!projectionCallback_) return GR_NOT_IMPLEMENTED;
        if (!std::isfinite(fovyHalfAngle) || !std::isfinite(aspect) ||
            !std::isfinite(nearval) || !std::isfinite(farval) ||
            fovyHalfAngle <= 0.0F || fovyHalfAngle >= 90.0F ||
            aspect <= 0.0F || nearval <= 0.0F || farval <= nearval)
            return GR_INVALID_PARMS;
        constexpr float radiansPerDegree = 0.01745329251994329577F;
        const float cotangent = 1.0F /
            std::tan(fovyHalfAngle * radiansPerDegree);
        const float depthRange = farval - nearval;
        Matrix4 projection(0.0F);
        projection.d[0][0] = cotangent / aspect;
        projection.d[1][1] = cotangent;
        // The game camera looks down -Z; the Win32 D3D owner flips Z before
        // applying its left-handed projection.
        projection.d[2][2] = -farval / depthRange;
        projection.d[2][3] = -farval * nearval / depthRange;
        projection.d[3][2] = -1.0F;
        for (unsigned row = 0U; row < 4U; ++row)
            for (unsigned column = 0U; column < 4U; ++column)
                if (!std::isfinite(projection.d[row][column]))
                    return GR_INVALID_PARMS;
        return set_projection(projection);
    }
    GENRESULT COMAPI set_projection(const Matrix4 & projection) override
    {
        if (!projectionCallback_) return GR_NOT_IMPLEMENTED;
        const GENRESULT result = projectionCallback_(context_, projection);
        if (result == GR_OK) projection_ = projection;
        return result;
    }
    GENRESULT COMAPI get_projection(Matrix4 & projection) override
    {
        if (!projectionCallback_) return GR_NOT_IMPLEMENTED;
        projection = projection_;
        return GR_OK;
    }
    GENRESULT COMAPI set_material(D3DMATERIAL9 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_material(D3DMATERIAL9 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI begin_scene() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI end_scene() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_render_state(D3DRENDERSTATETYPE state, U32 value) override
    {
        if (!stateCallback_)
        {
            return GR_NOT_IMPLEMENTED;
        }
        // The native shader always reads vertex color and has no fixed lighting.
        if (state == D3DRS_COLORVERTEX || state == D3DRS_LIGHTING)
        {
            if (!IsRenderBoolean(value)) return GR_INVALID_PARMS;
            return value == (state == D3DRS_COLORVERTEX ? 1U : 0U)
                ? GR_OK : GR_NOT_IMPLEMENTED;
        }
        if (state == D3DRS_DIFFUSEMATERIALSOURCE)
            return value == D3DMCS_COLOR1 ? GR_OK : GR_NOT_IMPLEMENTED;
        if (state == D3DRS_ALPHATESTENABLE)
        {
            if (!IsRenderBoolean(value)) return GR_INVALID_PARMS;
            return value == 0U ? GR_OK : GR_NOT_IMPLEMENTED;
        }
        if (state == D3DRS_ALPHAREF)
            return value <= 255U ? GR_OK : GR_INVALID_PARMS;
        RenderStateSnapshot candidate = renderState_;
        switch (state)
        {
        case D3DRS_ZENABLE:
            if (!IsRenderBoolean(value)) return GR_INVALID_PARMS;
            candidate.depthEnabled = value;
            break;
        case D3DRS_ZWRITEENABLE:
            if (!IsRenderBoolean(value)) return GR_INVALID_PARMS;
            candidate.depthWriteEnabled = value;
            break;
        case D3DRS_ZFUNC:
            if (!IsRenderDepthFunction(value)) return GR_INVALID_PARMS;
            candidate.depthFunction = value;
            break;
        case D3DRS_ALPHABLENDENABLE:
            if (!IsRenderBoolean(value)) return GR_INVALID_PARMS;
            candidate.alphaBlendEnabled = value;
            break;
        case D3DRS_SRCBLEND:
            if (!IsRenderBlendFactor(value))
                return value >= D3DBLEND_BOTHSRCALPHA &&
                               value <= D3DBLEND_INVBLENDFACTOR
                           ? GR_NOT_IMPLEMENTED
                           : GR_INVALID_PARMS;
            candidate.sourceBlend = value;
            break;
        case D3DRS_DESTBLEND:
            if (!IsRenderBlendFactor(value))
                return value >= D3DBLEND_BOTHSRCALPHA &&
                               value <= D3DBLEND_INVBLENDFACTOR
                           ? GR_NOT_IMPLEMENTED
                           : GR_INVALID_PARMS;
            candidate.destinationBlend = value;
            break;
        case D3DRS_SPECULARENABLE:
            if (!IsRenderBoolean(value)) return GR_INVALID_PARMS;
            candidate.specularEnabled = value;
            break;
        case D3DRS_CULLMODE:
            if (value < D3DCULL_NONE || value > D3DCULL_CCW)
                return GR_INVALID_PARMS;
            candidate.cullMode = value;
            break;
        case D3DRS_TEXTUREFACTOR:
            candidate.textureFactor = value;
            break;
        default:
            return GR_NOT_IMPLEMENTED;
        }
        const GENRESULT result = stateCallback_(context_, candidate);
        if (result == GR_OK)
        {
            renderState_ = candidate;
        }
        return result;
    }
    GENRESULT COMAPI get_render_state(D3DRENDERSTATETYPE state, U32 * value) override
    {
        if (!stateCallback_ || !value)
        {
            return stateCallback_ ? GR_INVALID_PARMS : GR_NOT_IMPLEMENTED;
        }
        switch (state)
        {
        case D3DRS_COLORVERTEX: *value = 1U; break;
        case D3DRS_LIGHTING: *value = 0U; break;
        case D3DRS_DIFFUSEMATERIALSOURCE: *value = D3DMCS_COLOR1; break;
        case D3DRS_CULLMODE: *value = renderState_.cullMode; break;
        case D3DRS_TEXTUREFACTOR: *value = renderState_.textureFactor; break;
        case D3DRS_ZENABLE: *value = renderState_.depthEnabled; break;
        case D3DRS_ZWRITEENABLE: *value = renderState_.depthWriteEnabled; break;
        case D3DRS_ZFUNC: *value = renderState_.depthFunction; break;
        case D3DRS_ALPHABLENDENABLE: *value = renderState_.alphaBlendEnabled; break;
        case D3DRS_SRCBLEND: *value = renderState_.sourceBlend; break;
        case D3DRS_DESTBLEND: *value = renderState_.destinationBlend; break;
        case D3DRS_SPECULARENABLE: *value = renderState_.specularEnabled; break;
        default: return GR_NOT_IMPLEMENTED;
        }
        return GR_OK;
    }
    GENRESULT COMAPI set_texture_stage_state(
        U32 stage, D3DTEXTURESTAGESTATETYPE state, U32 value) override
    {
        if (!textureStageCallback_) return GR_NOT_IMPLEMENTED;
        if (stage >= 1U && stage <= 3U)
        {
            return (state == D3DTSS_COLOROP || state == D3DTSS_ALPHAOP) &&
                           value == D3DTOP_DISABLE
                       ? GR_OK : GR_NOT_IMPLEMENTED;
        }
        if (stage != 0U) return GR_NOT_IMPLEMENTED;

        TextureStageSnapshot candidate = textureStage_;
        switch (state)
        {
        case D3DTSS_COLOROP:
            if (!IsTextureStageOperation(value)) return GR_NOT_IMPLEMENTED;
            candidate.colorOperation = value;
            break;
        case D3DTSS_COLORARG1:
            if (!IsTextureStageArgument(value)) return GR_NOT_IMPLEMENTED;
            candidate.colorArgument1 = value;
            break;
        case D3DTSS_COLORARG2:
            if (!IsTextureStageArgument(value)) return GR_NOT_IMPLEMENTED;
            candidate.colorArgument2 = value;
            break;
        case D3DTSS_ALPHAOP:
            if (!IsTextureStageOperation(value)) return GR_NOT_IMPLEMENTED;
            candidate.alphaOperation = value;
            break;
        case D3DTSS_ALPHAARG1:
            if (!IsTextureStageArgument(value)) return GR_NOT_IMPLEMENTED;
            candidate.alphaArgument1 = value;
            break;
        case D3DTSS_ALPHAARG2:
            if (!IsTextureStageArgument(value)) return GR_NOT_IMPLEMENTED;
            candidate.alphaArgument2 = value;
            break;
        case D3DTSS_TEXCOORDINDEX:
            if (value > 1U) return GR_NOT_IMPLEMENTED;
            candidate.textureCoordinateIndex = value;
            break;
        default:
            return GR_NOT_IMPLEMENTED;
        }
        const GENRESULT result = textureStageCallback_(context_, candidate);
        if (result == GR_OK) textureStage_ = candidate;
        return result;
    }
    GENRESULT COMAPI get_texture_stage_state(
        U32 stage, D3DTEXTURESTAGESTATETYPE state, U32 * value) override
    {
        if (!textureStageCallback_) return GR_NOT_IMPLEMENTED;
        if (!value) return GR_INVALID_PARMS;
        if (stage >= 1U && stage <= 3U)
        {
            if (state != D3DTSS_COLOROP && state != D3DTSS_ALPHAOP)
                return GR_NOT_IMPLEMENTED;
            *value = D3DTOP_DISABLE;
            return GR_OK;
        }
        if (stage != 0U) return GR_NOT_IMPLEMENTED;
        switch (state)
        {
        case D3DTSS_COLOROP: *value = textureStage_.colorOperation; break;
        case D3DTSS_COLORARG1: *value = textureStage_.colorArgument1; break;
        case D3DTSS_COLORARG2: *value = textureStage_.colorArgument2; break;
        case D3DTSS_ALPHAOP: *value = textureStage_.alphaOperation; break;
        case D3DTSS_ALPHAARG1: *value = textureStage_.alphaArgument1; break;
        case D3DTSS_ALPHAARG2: *value = textureStage_.alphaArgument2; break;
        case D3DTSS_TEXCOORDINDEX: *value = textureStage_.textureCoordinateIndex; break;
        default: return GR_NOT_IMPLEMENTED;
        }
        return GR_OK;
    }
    GENRESULT COMAPI set_sampler_state(
        U32 stage, D3DSAMPLERSTATETYPE state, U32 value) override
    {
        if (!samplerCallback_ || stage != 0U) return GR_NOT_IMPLEMENTED;
        SamplerStateSnapshot candidate = samplerState_;
        switch (state)
        {
        case D3DSAMP_ADDRESSU:
            if (value != D3DTADDRESS_WRAP && value != D3DTADDRESS_CLAMP)
                return GR_NOT_IMPLEMENTED;
            candidate.addressU = value;
            break;
        case D3DSAMP_ADDRESSV:
            if (value != D3DTADDRESS_WRAP && value != D3DTADDRESS_CLAMP)
                return GR_NOT_IMPLEMENTED;
            candidate.addressV = value;
            break;
        case D3DSAMP_MINFILTER:
            if (value != D3DTEXF_POINT && value != D3DTEXF_LINEAR)
                return GR_NOT_IMPLEMENTED;
            candidate.minFilter = value;
            break;
        case D3DSAMP_MAGFILTER:
            if (value != D3DTEXF_POINT && value != D3DTEXF_LINEAR)
                return GR_NOT_IMPLEMENTED;
            candidate.magFilter = value;
            break;
        case D3DSAMP_MIPFILTER:
            if (value != D3DTEXF_POINT && value != D3DTEXF_LINEAR)
                return GR_NOT_IMPLEMENTED;
            candidate.mipFilter = value;
            break;
        default:
            return GR_NOT_IMPLEMENTED;
        }
        const GENRESULT result = samplerCallback_(context_, candidate);
        if (result == GR_OK) samplerState_ = candidate;
        return result;
    }
    GENRESULT COMAPI get_sampler_state(
        U32 stage, D3DSAMPLERSTATETYPE state, U32 * value) override
    {
        if (!samplerCallback_ || stage != 0U) return GR_NOT_IMPLEMENTED;
        if (!value) return GR_INVALID_PARMS;
        switch (state)
        {
        case D3DSAMP_ADDRESSU: *value = samplerState_.addressU; break;
        case D3DSAMP_ADDRESSV: *value = samplerState_.addressV; break;
        case D3DSAMP_MINFILTER: *value = samplerState_.minFilter; break;
        case D3DSAMP_MAGFILTER: *value = samplerState_.magFilter; break;
        case D3DSAMP_MIPFILTER: *value = samplerState_.mipFilter; break;
        default: return GR_NOT_IMPLEMENTED;
        }
        return GR_OK;
    }
    GENRESULT COMAPI set_texture_stage_texture(U32 stage, U32 texture) override
    {
        if (stage >= 2U && stage <= 3U)
            return texture == 0U ? GR_OK : GR_NOT_IMPLEMENTED;
        if (!textureBindCallback_) return GR_NOT_IMPLEMENTED;
        const GENRESULT result = textureBindCallback_(context_, stage, texture);
        if (result == GR_OK && stage == 0U) textureHandle_ = texture;
        return result;
    }
    GENRESULT COMAPI get_texture_stage_texture(U32 stage, U32 * texture) override
    {
        if (!texture) return GR_INVALID_PARMS;
        if (stage >= 2U && stage <= 3U)
        {
            *texture = 0U;
            return GR_OK;
        }
        if (!textureBindCallback_) return GR_NOT_IMPLEMENTED;
        if (stage > 3U) return GR_NOT_IMPLEMENTED;
        *texture = stage == 0U ? textureHandle_ : 0U;
        return GR_OK;
    }
    GENRESULT COMAPI verify_state() override { return GR_NOT_IMPLEMENTED; }

    GENRESULT COMAPI draw_primitive(D3DPRIMITIVETYPE primitiveType,
                                    U32 vertexFormat,
                                    const void * vertices,
                                    int vertexCount,
                                    U32) override
    {
        return Submit(primitiveType, vertexFormat, vertices, vertexCount, nullptr, 0);
    }

    GENRESULT COMAPI draw_indexed_primitive(D3DPRIMITIVETYPE primitiveType,
                                            U32 vertexFormat,
                                            const void * vertices,
                                            int vertexCount,
                                            const U16 * indices,
                                            int indexCount,
                                            U32) override
    {
        return Submit(
            primitiveType, vertexFormat, vertices, vertexCount, indices, indexCount);
    }

    GENRESULT COMAPI draw_primitive_vb(D3DPRIMITIVETYPE primitiveType,
                                       IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
                                       int startVertex,
                                       int vertexCount,
                                       U32 flags) override
    {
        return vertexPipeline_
                   ? vertexPipeline_->draw_primitive_vb(
                         primitiveType, vertexBufferHandle, startVertex, vertexCount, flags)
                   : GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI draw_indexed_primitive_vb(D3DPRIMITIVETYPE primitiveType,
                                               IRP_VERTEXBUFFERHANDLE vertexBufferHandle,
                                               int startVertex,
                                               int vertexCount,
                                               const U16 * indices,
                                               int indexCount,
                                               U32 flags) override
    {
        return vertexPipeline_
                   ? vertexPipeline_->draw_indexed_primitive_vb(
                         primitiveType, vertexBufferHandle, startVertex, vertexCount,
                         indices, indexCount, flags)
                   : GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI flush(U32) override { return GR_NOT_IMPLEMENTED; }

private:
    GENRESULT Submit(D3DPRIMITIVETYPE primitiveType,
                     U32 vertexFormat,
                     const void * vertices,
                     int vertexCount,
                     const U16 * indices,
                     int indexCount)
    {
        if (vertexCount < 0 || indexCount < 0 || !callback_)
        {
            return GR_INVALID_PARMS;
        }

        if (primitiveType == D3DPT_TRIANGLEFAN)
        {
            const int count = indices ? indexCount : vertexCount;
            if (!vertices || count < 3 || count > (std::numeric_limits<int>::max() / 3) || vertexCount > 65536 ||
                (indices == nullptr && indexCount != 0)) return GR_INVALID_PARMS;
            for (int i = 0; indices && i < count; ++i)
                if (indices[i] >= vertexCount) return GR_INVALID_PARMS;
            try
            {
                fanIndices_.resize(static_cast<std::size_t>(count - 2) * 3);
            }
            catch (const std::bad_alloc &)
            {
                return GR_OUT_OF_MEMORY;
            }
            for (int i = 1; i + 1 < count; ++i)
            {
                const std::size_t offset = static_cast<std::size_t>(i - 1) * 3;
                fanIndices_[offset] = indices ? indices[0] : 0;
                fanIndices_[offset + 1] = indices ? indices[i] : static_cast<U16>(i);
                fanIndices_[offset + 2] = indices ? indices[i + 1] : static_cast<U16>(i + 1);
            }
            primitiveType = D3DPT_TRIANGLELIST;
            indices = fanIndices_.data();
            indexCount = static_cast<int>(fanIndices_.size());
        }

        const LegacySubmissionInput input{
            static_cast<std::uint32_t>(primitiveType),
            vertexFormat,
            vertices,
            static_cast<std::size_t>(vertexCount),
            indices,
            static_cast<std::size_t>(indexCount),
        };
        if (vertexFormat == LegacyD3d9::kGameStandardFvf)
        {
            if (!callback4_) return GR_NOT_IMPLEMENTED;
            try
            {
                converted4_.resize(static_cast<std::size_t>(vertexCount));
            }
            catch (const std::bad_alloc &)
            {
                return GR_OUT_OF_MEMORY;
            }
            RenderSubmissionView4 submission{};
            const LegacySubmissionStatus status = AdaptLegacyGameStandardSubmission(
                input, converted4_.data(), converted4_.size(), submission);
            if (status != LegacySubmissionStatus::Ok) return StatusResult(status);
            return callback4_(context_, submission);
        }
        if (vertexFormat == LegacyD3d9::kMtVertexFvf)
        {
            if (!callback3_) return GR_NOT_IMPLEMENTED;
            try
            {
                converted3_.resize(static_cast<std::size_t>(vertexCount));
            }
            catch (const std::bad_alloc &)
            {
                return GR_OUT_OF_MEMORY;
            }
            RenderSubmissionView3 submission{};
            const LegacySubmissionStatus status = AdaptLegacyMtVertexSubmission(
                input, converted3_.data(), converted3_.size(), submission);
            if (status != LegacySubmissionStatus::Ok) return StatusResult(status);
            return callback3_(context_, submission);
        }
        if (vertexFormat == LegacyD3d9::kRpVertex2Fvf)
        {
            if (!callback2_)
            {
                return GR_NOT_IMPLEMENTED;
            }
            try
            {
                converted2_.resize(static_cast<std::size_t>(vertexCount));
            }
            catch (const std::bad_alloc &)
            {
                return GR_OUT_OF_MEMORY;
            }
            RenderSubmissionView2 submission{};
            const LegacySubmissionStatus status = AdaptLegacyRpVertex2Submission(
                input, converted2_.data(), converted2_.size(), submission);
            if (status != LegacySubmissionStatus::Ok)
            {
                return StatusResult(status);
            }
            return callback2_(context_, submission);
        }
        if (vertexFormat != LegacyD3d9::kRpVertexFvf)
        {
            return GR_NOT_IMPLEMENTED;
        }

        try
        {
            converted_.resize(static_cast<std::size_t>(vertexCount));
        }
        catch (const std::bad_alloc &)
        {
            return GR_OUT_OF_MEMORY;
        }
        RenderSubmissionView submission{};
        const LegacySubmissionStatus status = AdaptLegacyRpVertexSubmission(
            input, converted_.data(), converted_.size(), submission);
        if (status != LegacySubmissionStatus::Ok)
        {
            return StatusResult(status);
        }
        return callback_(context_, submission);
    }

    NativeRenderSubmissionCallback callback_ = nullptr;
    NativeRenderSubmission2Callback callback2_ = nullptr;
    NativeRenderSubmission3Callback callback3_ = nullptr;
    NativeRenderSubmission4Callback callback4_ = nullptr;
    NativeRenderStateCallback stateCallback_ = nullptr;
    NativeTextureStageCallback textureStageCallback_ = nullptr;
    NativeTextureBindCallback textureBindCallback_ = nullptr;
    NativeSamplerStateCallback samplerCallback_ = nullptr;
    NativeModelviewCallback modelviewCallback_ = nullptr;
    NativeProjectionCallback projectionCallback_ = nullptr;
    NativeViewportCallback viewportCallback_ = nullptr;
    std::array<float, 12> modelview_{{1.0F, 0.0F, 0.0F, 0.0F,
                                      0.0F, 1.0F, 0.0F, 0.0F,
                                      0.0F, 0.0F, 1.0F, 0.0F}};
    Matrix4 projection_{true};
    std::array<int, 4> viewport_{{0, 0, 0, 0}};
    void * context_ = nullptr;
    RenderStateSnapshot renderState_{};
    U32 stateId_ = 0U;
    TextureStageSnapshot textureStage_{};
    SamplerStateSnapshot samplerState_{};
    U32 textureHandle_ = 0U;
    std::vector<U16> fanIndices_;
    std::vector<RenderVertexP3DiffuseT2> converted_;
    std::vector<RenderVertexP3DiffuseT4> converted2_;
    std::vector<RenderVertexP3DiffuseSpecularT4> converted3_;
    std::vector<RenderVertexP3N3DiffuseT5> converted4_;
    std::unique_ptr<NativeRenderPipeline> vertexPipeline_;
    bool profileAggregate_ = false;
};
}

void RegisterNativeRenderPrimitive(ICOManager * manager)
{
    if (!manager) return;
    IComponentFactory * factory =
        new (std::nothrow) DAComponentFactory<DAComponent<NativeRenderPrimitive>,
                                                 NativeRenderPrimitiveDesc>(interface_name);
    if (!factory) return;
    manager->RegisterComponent(factory, interface_name);
    factory->Release();
    IComponentFactory * aggregate = new (std::nothrow)
        DAComponentFactory2<DAComponentAggregate<NativeRenderPrimitive>,
                            AGGDESC>(interface_name);
    if (!aggregate) return;
    manager->RegisterComponent(aggregate, interface_name);
    aggregate->Release();
}

GENRESULT BindNativeRenderPrimitive(IRenderPrimitive * primitive,
                                    const NativeRenderPrimitiveDesc & descriptor)
{
    auto * owner = dynamic_cast<NativeRenderPrimitive *>(primitive);
    return owner ? owner->Bind(descriptor) : GR_INVALID_PARMS;
}

GENRESULT UnbindNativeRenderPrimitive(IRenderPrimitive * primitive)
{
    auto * owner = dynamic_cast<NativeRenderPrimitive *>(primitive);
    return owner ? owner->Unbind() : GR_INVALID_PARMS;
}
}
