#include "CQImage.h"
#include "CQBatch.h"
#include "DEffectOpts.h"
#include "FDump.h"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/NativeMathEngine.h"
#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "FrontierX/RenderSmokeScene.h"
#include "GlobalFlagTypes.h"
#include "IRenderPrimitive.h"
#include "IVertexBufferManager.h"
#include "IProfileParser_Utility.h"
#include "RenderBatcherRegistration.h"
#include "RenderBatcher.h"
#include "xform.h"
#include "matrix4.h"

#include <cstring>
#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#include <cmath>

class RenderBatcher;
extern RenderBatcher * BATCHER;

CQGlobalFlags CQFLAGS = {};
GlobalRenderFlags CQRENDERFLAGS = {};
GlobalEffectsOptions CQEFFECTS = {};
IRenderPipeline *PIPE = nullptr;
IVertexBufferManager *VB_MANAGER = nullptr;

namespace
{
struct CpuTexture { U32 handle; U32 width; U32 height; std::vector<std::uint8_t> pixels; };
std::vector<CpuTexture> cpuTextures;
U32 nextTextureHandle = 1U;
bool TextureExists(void *, U32 handle)
{ return std::any_of(cpuTextures.begin(), cpuTextures.end(), [=](const CpuTexture &t) { return t.handle == handle; }); }
GENRESULT CreateTexture(void *, U32 width, U32 height, U32 &handle)
{
    if (!width || !height || width > 16U || height > 16U) return GR_INVALID_PARMS;
    handle = nextTextureHandle++;
    cpuTextures.push_back({handle, width, height, std::vector<std::uint8_t>(width * height * 4U)});
    return GR_OK;
}
GENRESULT UploadTexture(void *, U32 handle, const std::uint8_t *pixels, U32 width, U32 height, std::size_t bytes)
{
    for (auto &t : cpuTextures) if (t.handle == handle)
    {
        if (!pixels || t.width != width || t.height != height || bytes != t.pixels.size()) return GR_INVALID_PARMS;
        std::copy_n(pixels, bytes, t.pixels.begin()); return GR_OK;
    }
    return GR_INVALID_PARMS;
}
GENRESULT DestroyTexture(void *, U32 handle)
{
    const auto i = std::find_if(cpuTextures.begin(), cpuTextures.end(), [=](const CpuTexture &t) { return t.handle == handle; });
    if (i == cpuTextures.end()) return GR_INVALID_PARMS;
    cpuTextures.erase(i); return GR_OK;
}
bool rejectTextureStage = false;
unsigned assertionCount = 0;
unsigned submissionCount = 0;
unsigned mergedSubmissionCount = 0;
bool submissionShapeValid = true;
FrontierX::TextureStageSnapshot recordedStages{};
std::array<U32, 2> recordedTextures{};
FrontierX::NativeRenderPipeline *textureOwner = nullptr;
struct PooledTwoUvDraw
{
    FrontierX::TextureStageSnapshot stages;
    std::array<U32, 2> textures;
    std::array<FrontierX::RenderVertexP3DiffuseT4, 3> vertices;
};
std::vector<PooledTwoUvDraw> pooledTwoUvDraws;
Transform recordedModelview;
Matrix4 recordedProjection(true);
std::array<int, 4> recordedViewport{{0, 0, 64, 64}};
bool rejectModelview = false;
bool rejectProjection = false;
struct DrawTransform { Transform modelview; Matrix4 projection; std::array<int, 4> viewport; std::array<float, 3> firstPosition; };
std::vector<DrawTransform> drawTransforms;
GENRESULT DACOM_CDECL RecordModelview(void *, const Transform &transform)
{
    if (rejectModelview) return GR_GENERIC;
    for (unsigned row = 0; row < 3; ++row)
        for (unsigned col = 0; col < 3; ++col)
            if (!std::isfinite(transform.d[row][col])) return GR_INVALID_PARMS;
    if (!std::isfinite(transform.translation.x) || !std::isfinite(transform.translation.y) ||
        !std::isfinite(transform.translation.z)) return GR_INVALID_PARMS;
    recordedModelview = transform; return GR_OK;
}
GENRESULT DACOM_CDECL RecordProjection(void *, const Matrix4 &projection)
{
    if (rejectProjection) return GR_GENERIC;
    for (unsigned row = 0; row < 4; ++row)
        for (unsigned col = 0; col < 4; ++col)
            if (!std::isfinite(projection.d[row][col])) return GR_INVALID_PARMS;
    recordedProjection = projection; return GR_OK;
}
GENRESULT DACOM_CDECL RecordViewport(void *, int x, int y, int width, int height)
{
    if (x < 0 || y < 0 || width <= 0 || height <= 0) return GR_INVALID_PARMS;
    recordedViewport = {{x, y, width, height}}; return GR_OK;
}
bool SameModelview(const Transform &a, const Transform &b)
{
    for (unsigned row = 0; row < 3; ++row)
        for (unsigned col = 0; col < 3; ++col)
            if (a.d[row][col] != b.d[row][col]) return false;
    return a.translation.x == b.translation.x && a.translation.y == b.translation.y &&
           a.translation.z == b.translation.z;
}
bool SameProjection(const Matrix4 &a, const Matrix4 &b)
{
    for (unsigned row = 0; row < 4; ++row)
        for (unsigned col = 0; col < 4; ++col)
            if (a.d[row][col] != b.d[row][col]) return false;
    return true;
}

GENRESULT DACOM_CDECL AcceptRenderState(
    void *, const FrontierX::RenderStateSnapshot &)
{
    return GR_OK;
}

GENRESULT DACOM_CDECL AcceptTextureStage(
    void *, const FrontierX::TextureStageSnapshot &stages)
{
    if (rejectTextureStage) return GR_GENERIC;
    recordedStages = stages;
    return GR_OK;
}

GENRESULT DACOM_CDECL RecordSampler(void *, const FrontierX::SamplerStateSnapshot &)
{ return GR_OK; }

GENRESULT DACOM_CDECL RecordTextureBinding(void *, U32 stage, U32 texture)
{
    if (stage > 1U || !textureOwner || (texture && textureOwner->is_texture(texture) != GR_OK))
        return GR_INVALID_PARMS;
    recordedTextures[stage] = texture;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureTwoUvSubmission(void *, const FrontierX::RenderSubmissionView2 &submission)
{
    if (!submission.IsValid() || submission.vertexCount != 3U || submission.indexCount != 3U ||
        !submission.indices || submission.indices[0] != 0U || submission.indices[1] != 1U || submission.indices[2] != 2U)
        return GR_INVALID_PARMS;
    PooledTwoUvDraw draw{recordedStages, recordedTextures, {}};
    std::copy_n(submission.vertices, 3U, draw.vertices.begin());
    pooledTwoUvDraws.push_back(draw);
    return GR_OK;
}

int DACOM_CDECL IgnoreDiagnostic(ErrorCode, const C8 *, ...)
{
    return 0;
}

GENRESULT DACOM_CDECL CaptureSubmission(
    void *, const FrontierX::RenderSubmissionView & submission)
{
    if (!submission.IsValid())
        return GR_INVALID_PARMS;
    drawTransforms.push_back({recordedModelview, recordedProjection, recordedViewport,
        {{submission.vertices[0].x, submission.vertices[0].y, submission.vertices[0].z}}});
    const bool directSmoke =
        submission.vertexCount == FrontierX::RenderSmokeVertexCount &&
        submission.indices == nullptr && submission.indexCount == 0U &&
        submission.PrimitiveCount() == 3U;
    const bool indexedSmoke =
        submission.vertexCount == FrontierX::RenderSmokeIndexedVertexCount &&
        submission.indices != nullptr &&
        submission.indexCount == FrontierX::RenderSmokeIndexCount &&
        submission.PrimitiveCount() == 3U &&
        submission.indices[0] == 0U && submission.indices[1] == 1U &&
        submission.indices[2] == 2U && submission.indices[6] == 3U &&
        submission.indices[7] == 5U && submission.indices[8] == 6U;
    const bool mergedIndexedSmoke =
        submission.vertexCount == 2U * FrontierX::RenderSmokeIndexedVertexCount &&
        submission.indices != nullptr &&
        submission.indexCount == 2U * FrontierX::RenderSmokeIndexCount &&
        submission.PrimitiveCount() == 6U &&
        submission.indices[0] == 0U && submission.indices[8] == 6U &&
        submission.indices[9] == 7U && submission.indices[10] == 8U &&
        submission.indices[11] == 9U && submission.indices[15] == 10U &&
        submission.indices[16] == 12U && submission.indices[17] == 13U;
    submissionShapeValid = submissionShapeValid &&
        submission.topology == FrontierX::PrimitiveTopology::TriangleList &&
        (directSmoke || indexedSmoke || mergedIndexedSmoke);
    if (mergedIndexedSmoke) ++mergedSubmissionCount;
    ++submissionCount;
    return GR_OK;
}

bool Expect(bool condition, const char * message)
{
    if (!condition) std::cerr << "production batcher probe failed: " << message << '\n';
    return condition;
}
}

extern "C"
{
DA_ERROR_HANDLER FDUMP = IgnoreDiagnostic;
}

bool ICQImage::Assert(const char *, void *, unsigned)
{
    ++assertionCount;
    return false;
}

bool DACOM_CDECL ICQImage::Bomb(const char *, ...)
{
    ++assertionCount;
    return false;
}

int main()
{
    ICOManager * components = DACOM_Acquire();
    if (!Expect(components != nullptr && FrontierX::RegisterNativeMathEngine(components) && MATH_ENGINE(),
                "DACOM and genuine scalar pretransform provider bootstrap")) return 1;
    U32 integerDefault = 0U;
    float floatDefault = 0.0F;
    char stringDefault[16] = {};
    Vector vectorDefault(0.0F, 0.0F, 0.0F);
    Vector expectedVector(1.0F, 2.0F, 3.0F);
    if (!Expect(opt_get_u32(components, nullptr, "RenderBatcher", "Absent", 17U,
                    &integerDefault) == 17U && integerDefault == 17U &&
                    opt_get_float(components, nullptr, "RenderBatcher", "Absent", 2.5F,
                    &floatDefault) == 2.5F && floatDefault == 2.5F &&
                    std::strcmp(opt_get_string(components, nullptr, "RenderBatcher", "Absent",
                        "default", stringDefault, sizeof(stringDefault)), "default") == 0 &&
                    opt_get_vector(components, nullptr, "RenderBatcher", "Absent",
                        expectedVector, vectorDefault).x == 1.0F &&
                    vectorDefault.y == 2.0F && vectorDefault.z == 3.0F,
                "profile helper defaults without parser interface")) return 1;
    FrontierX::RegisterNativeRenderPrimitive(components);
    FrontierX::NativeRenderPrimitiveDesc nativeDescriptor(
        CaptureSubmission, nullptr, nullptr, AcceptRenderState,
        nullptr, nullptr, AcceptTextureStage);
    nativeDescriptor.callback2 = CaptureTwoUvSubmission;
    nativeDescriptor.textureBind = RecordTextureBinding;
    nativeDescriptor.sampler = RecordSampler;
    nativeDescriptor.modelview = RecordModelview;
    nativeDescriptor.projection = RecordProjection;
    nativeDescriptor.viewport = RecordViewport;
    void * nativeRaw = nullptr;
    if (!Expect(components->CreateInstance(&nativeDescriptor, &nativeRaw) == GR_OK &&
                    nativeRaw != nullptr, "native submission owner")) return 1;
    auto * nativePrimitive = static_cast<IRenderPrimitive *>(nativeRaw);
    FrontierX::NativeTextureOperations textures{};
    textures.lookup = TextureExists; textures.create = CreateTexture;
    textures.upload = UploadTexture; textures.destroy = DestroyTexture;
    FrontierX::NativeRenderPipeline pipeline(nativePrimitive, textures);
    textureOwner = &pipeline;
    CQFLAGS.b3DEnabled = -1;

    RegisterTheRenderBatcher(components);
    AGGDESC batchDescriptor("IRenderPrimitive");
    for (unsigned run = 0U; run < 2U; ++run)
    {
        PIPE = &pipeline;
        void * batchRaw = nullptr;
        if (!Expect(components->CreateInstance(&batchDescriptor, &batchRaw) == GR_OK &&
                        batchRaw != nullptr, "production aggregate factory")) return 1;
        auto * batcher = static_cast<IRenderPrimitive *>(batchRaw);
        IAggregateComponent * aggregate = nullptr;
        if (!Expect(batcher->QueryInterface(IID_IAggregateComponent,
                        reinterpret_cast<void **>(&aggregate)) == GR_OK && aggregate,
                    "production aggregate interface")) return 1;
        if (!Expect(aggregate->Initialize() == GR_OK, "production initialization")) return 1;
        aggregate->Release();

        U32 batchState = U32(-1);
        if (!Expect(batcher->get_state(RPR_BATCH, &batchState) == GR_OK && batchState == 0U,
                    "production default keeps batching disabled")) return 1;
        if (!Expect(batcher->set_state(static_cast<RPRSTATE>(RPR_MAX_STATE +
                            EXTRA_STATES), 1U) == GR_INVALID_PARMS &&
                        batcher->get_state(static_cast<RPRSTATE>(RPR_MAX_STATE +
                            EXTRA_STATES), &batchState) == GR_INVALID_PARMS &&
                        batcher->get_state(RPR_BATCH, nullptr) == GR_INVALID_PARMS &&
                        batcher->get_state(RPR_BATCH, &batchState) == GR_OK &&
                        batchState == 0U,
                    "state bounds reject invalid input without changing batch state"))
            return 1;
        if (!Expect(batcher->set_state(RPR_BATCH, 1U) == GR_OK &&
                        batcher->get_state(RPR_BATCH, &batchState) == GR_OK &&
                        batchState == 1U,
                    "supported texture-stage snapshot enables native batching"))
            return 1;
        if (!Expect(batcher->set_texture_stage_state(D3DTSS_NUM_STAGES,
                        D3DTSS_COLOROP, D3DTOP_DISABLE) == GR_INVALID_PARMS &&
                        batcher->set_texture_stage_state(0U,
                            static_cast<D3DTEXTURESTAGESTATETYPE>(0),
                            D3DTOP_DISABLE) == GR_INVALID_PARMS &&
                        batcher->set_texture_stage_state(0U, D3DTSS_COLOROP,
                            D3DTOP_DISABLE) == GR_OK &&
                        batcher->set_texture_stage_state(0U, D3DTSS_COLOROP,
                            D3DTOP_MODULATE) == GR_OK &&
                        batcher->set_texture_stage_texture(D3DTSS_NUM_STAGES, 0U) ==
                            GR_INVALID_PARMS,
                    "texture-stage setters reject invalid or unsupported state")) return 1;

        if (!Expect(batcher->set_state(RPR_BATCH, 0U) == GR_OK &&
                        FrontierX::SubmitRenderSmokeScene(batcher) == GR_OK &&
                        submissionCount == 3U * run + 1U && submissionShapeValid,
                    "production draw reaches native owner with fixture geometry")) return 1;
        if (!Expect(FrontierX::SubmitIndexedRenderSmokeScene(batcher) == GR_OK &&
                        submissionCount == 3U * run + 2U && submissionShapeValid,
                    "production indexed draw reaches native owner")) return 1;
        ICQBatch * batch = nullptr;
        if (!Expect(batcher->QueryInterface(IID_ICQBatch,
                        reinterpret_cast<void **>(&batch)) == GR_OK && batch,
                    "production pool interface")) return 1;
        batch->SetIdentityModelview();
        batch->Release();
        if (run == 1U && !Expect(
                pipeline.set_render_state(D3DRS_ALPHABLENDENABLE, 1U) == GR_OK,
                "translucent pool state")) return 1;
        if (!Expect(batcher->set_state(RPR_STATE_FLAGS, 0U) == GR_OK &&
                        batcher->set_state(RPR_STATE_ID, 1U) == GR_OK &&
                        batcher->set_state(RPR_BATCH, 1U) == GR_OK &&
                        FrontierX::SubmitIndexedRenderSmokeScene(batcher) == GR_OK &&
                        submissionCount == 3U * run + 2U,
                    "indexed draw remains queued until flush")) return 1;
        if (run == 1U && !Expect(
                FrontierX::SubmitIndexedRenderSmokeScene(batcher) == GR_OK &&
                    submissionCount == 3U * run + 2U,
                "second indexed draw merges without early submission")) return 1;
        if (!Expect(batcher->flush(U32(-1)) == GR_OK &&
                        submissionCount == 3U * run + 3U &&
                        mergedSubmissionCount == run && submissionShapeValid,
                    "production pool flush submits indexed draw with rebased indices")) return 1;
        batcher->Release();
        if (!Expect(BATCHER == nullptr && PIPE == nullptr,
                    "destroyed batcher clears its global ownership")) return 1;
    }
    // Stasis/Repellent use pooled two-texture color+alpha states. Their source
    // arguments and UV selectors were accepted into cache but omitted at flush.
    {
        U32 firstTexture = 0U, secondTexture = 0U;
        const PixelFormat format(PF_RGBA8);
        if (!Expect(pipeline.create_texture(2U, 2U, format, 1U, 0U, firstTexture) == GR_OK &&
                    pipeline.create_texture(2U, 2U, format, 1U, 0U, secondTexture) == GR_OK &&
                    pipeline.set_render_state(D3DRS_ALPHABLENDENABLE, FALSE) == GR_OK,
                    "pooled ability regression uses actual native-owned texture handles")) return 1;
        PIPE = &pipeline;
        void *rawBatcher = nullptr;
        if (!Expect(components->CreateInstance(&batchDescriptor, &rawBatcher) == GR_OK,
                    "pooled two-stage production batcher created")) return 1;
        auto *batcher = static_cast<IRenderPrimitive *>(rawBatcher);
        IAggregateComponent *aggregate = nullptr;
        if (!Expect(batcher->QueryInterface(IID_IAggregateComponent,
                        reinterpret_cast<void **>(&aggregate)) == GR_OK && aggregate->Initialize() == GR_OK,
                    "pooled two-stage production batcher initialized")) return 1;
        aggregate->Release();
        ICQBatch *batch = nullptr;
        if (!Expect(batcher->QueryInterface(IID_ICQBatch, reinterpret_cast<void **>(&batch)) == GR_OK,
                    "pooled two-stage original pool interface")) return 1;
        batch->SetIdentityModelview();
        batch->Release();
        if (!Expect(batcher->set_state(RPR_STATE_FLAGS, BM_M_F_HAS_TEXTURE) == GR_OK &&
                    batcher->set_state(RPR_BATCH, TRUE) == GR_OK,
                    "pooled two-stage texture snapshot enabled")) return 1;
        constexpr std::array<D3DTEXTURESTAGESTATETYPE, 7> fields{{D3DTSS_COLOROP, D3DTSS_COLORARG1,
            D3DTSS_COLORARG2, D3DTSS_ALPHAOP, D3DTSS_ALPHAARG1, D3DTSS_ALPHAARG2, D3DTSS_TEXCOORDINDEX}};
        const std::array<FrontierX::RenderVertexP3DiffuseT4, 3> vertices{{
            {-1,-1,0.5F,0x806080a0U,0,0,0.25F,0.75F},
            {1,-1,0.5F,0x806080a0U,1,0,0.75F,0.75F},
            {0,1,0.5F,0x806080a0U,0.5F,1,0.5F,0.25F}}};
        const U16 indices[3] = {0U,1U,2U};
        const auto queue = [&](bool first, U32 stateID) {
            const U32 stage0[7] = {D3DTOP_MODULATE,D3DTA_TEXTURE,D3DTA_DIFFUSE,
                D3DTOP_MODULATE,D3DTA_TEXTURE,D3DTA_DIFFUSE,first ? 1U : 0U};
            const U32 stage1[7] = {D3DTOP_MODULATE,first ? D3DTA_CURRENT : D3DTA_TEXTURE,
                first ? D3DTA_TEXTURE : D3DTA_CURRENT,D3DTOP_MODULATE,
                first ? D3DTA_CURRENT : D3DTA_TEXTURE,first ? D3DTA_TEXTURE : D3DTA_CURRENT,first ? 0U : 1U};
            bool valid = batcher->set_state(RPR_STATE_ID, stateID) == GR_OK;
            for (U32 stage = 2U; stage < BM_NUM_STAGES; ++stage)
                valid &= batcher->set_texture_stage_state(stage, D3DTSS_COLOROP, D3DTOP_DISABLE) == GR_OK;
            for (std::size_t i = 0U; i < fields.size(); ++i)
            {
                valid &= batcher->set_texture_stage_state(0U, fields[i], stage0[i]) == GR_OK;
                valid &= batcher->set_texture_stage_state(1U, fields[i], stage1[i]) == GR_OK;
            }
            valid &= batcher->set_texture_stage_texture(0U, first ? firstTexture : secondTexture) == GR_OK;
            valid &= batcher->set_texture_stage_texture(1U, first ? secondTexture : firstTexture) == GR_OK;
            // The public getter observes PIPE, not the deferred cache. Exact
            // subsequent draw snapshots below prove rejected writes preserve it.
            valid &= batcher->set_texture_stage_state(1U,D3DTSS_TEXCOORDINDEX,2U) == GR_NOT_IMPLEMENTED &&
                batcher->set_texture_stage_state(1U,D3DTSS_COLOROP,10U /* D3DTOP_SUBTRACT */) == GR_NOT_IMPLEMENTED;
            const GENRESULT drawResult = batcher->draw_indexed_primitive(D3DPT_TRIANGLELIST,
                FrontierX::LegacyD3d9::kRpVertex2Fvf, vertices.data(), 3, indices, 3, 0U);
            if (!valid || drawResult != GR_OK) std::cerr << "pooled queue " << first << " states=" << valid << " draw=" << drawResult << " captures=" << pooledTwoUvDraws.size() << '\n';
            return valid && drawResult == GR_OK;
        };
        const bool firstQueued = queue(true, 11U);
        const bool secondQueued = queue(false, 12U);
        const bool noEarlyDraw = pooledTwoUvDraws.empty();
        const GENRESULT flushed = batcher->flush(U32(-1));
        if (!firstQueued || !secondQueued || !noEarlyDraw || flushed != GR_OK || pooledTwoUvDraws.size() != 2U)
            std::cerr << "pooled flush first=" << firstQueued << " second=" << secondQueued << " noearly=" << noEarlyDraw << " result=" << flushed << " captures=" << pooledTwoUvDraws.size() << '\n';
        if (!Expect(firstQueued && secondQueued && noEarlyDraw && flushed == GR_OK && pooledTwoUvDraws.size() == 2U,
                    "original pooled draws replay separate two-stage snapshots after rejected cache updates")) return 1;
        const auto matches = [&](const PooledTwoUvDraw &draw, bool first) {
            return draw.textures == std::array<U32,2>{{first ? firstTexture : secondTexture, first ? secondTexture : firstTexture}} &&
                draw.stages.colorOperation == D3DTOP_MODULATE && draw.stages.alphaOperation == D3DTOP_MODULATE &&
                draw.stages.colorArgument1 == D3DTA_TEXTURE && draw.stages.colorArgument2 == D3DTA_DIFFUSE &&
                draw.stages.alphaArgument1 == D3DTA_TEXTURE && draw.stages.alphaArgument2 == D3DTA_DIFFUSE &&
                draw.stages.textureCoordinateIndex == (first ? 1U : 0U) &&
                draw.stages.second.colorOperation == D3DTOP_MODULATE && draw.stages.second.alphaOperation == D3DTOP_MODULATE &&
                draw.stages.second.colorArgument1 == (first ? D3DTA_CURRENT : D3DTA_TEXTURE) &&
                draw.stages.second.colorArgument2 == (first ? D3DTA_TEXTURE : D3DTA_CURRENT) &&
                draw.stages.second.alphaArgument1 == (first ? D3DTA_CURRENT : D3DTA_TEXTURE) &&
                draw.stages.second.alphaArgument2 == (first ? D3DTA_TEXTURE : D3DTA_CURRENT) &&
                draw.stages.second.textureCoordinateIndex == (first ? 0U : 1U) &&
                std::memcmp(draw.vertices.data(), vertices.data(), sizeof(vertices)) == 0;
        };
        if (!Expect((matches(pooledTwoUvDraws[0], true) && matches(pooledTwoUvDraws[1], false)) ||
                    (matches(pooledTwoUvDraws[1], true) && matches(pooledTwoUvDraws[0], false)),
                    "original pooled receiver gets all color/alpha arguments, independent UV selectors and exact geometry")) return 1;
        if (!Expect(queue(true, 13U) && batcher->flush(U32(-1)) == GR_OK && pooledTwoUvDraws.size() == 3U &&
                    matches(pooledTwoUvDraws.back(), true),
                    "original pooled state and bindings restore the exact prior two-stage contract")) return 1;
        rejectTextureStage = true;
        if (!Expect(batcher->verify_state() == GR_GENERIC && pooledTwoUvDraws.size() == 3U,
                    "pooled verify propagates actual stage receiver failure without submitting")) return 1;
        rejectTextureStage = false;
        const GENRESULT verified = batcher->verify_state();
        if (verified != GR_OK) std::cerr << "pooled real verify result=" << verified << '\n';
        if (!Expect(verified == GR_OK,
                    "pooled verify succeeds only after real state and binding receivers accept")) return 1;
        // Invalidate the backend handle while the original batcher retains it.
        if (!Expect(pipeline.set_texture_stage_texture(0U, 0U) == GR_OK &&
                    pipeline.destroy_texture(firstTexture) == GR_OK &&
                    batcher->verify_state() != GR_OK && pooledTwoUvDraws.size() == 3U,
                    "pooled verify rejects stale committed texture ownership without submitting")) return 1;
        batcher->Release();
        if (!Expect(pipeline.set_texture_stage_texture(0U, 0U) == GR_OK &&
                    pipeline.set_texture_stage_texture(1U, 0U) == GR_OK &&
                    pipeline.destroy_texture(secondTexture) == GR_OK && cpuTextures.empty() &&
                    BATCHER == nullptr && PIPE == nullptr,
                    "pooled two-texture owners release without retained batcher globals")) return 1;
        std::cout << "Original pooled two-stage color/alpha arguments, UV selectors and rejected cache rollback checked\n";
    }
    // World beams/drive effects can remain pooled while HUD OrthoView changes
    // PIPE projection/viewport. The real receiver must see their prior world
    // state; existing texture-only snapshots cannot detect this regression.
    {
        PIPE = &pipeline;
        void *raw = nullptr;
        if (!Expect(components->CreateInstance(&batchDescriptor, &raw) == GR_OK && raw,
                    "world/HUD transform production batcher created")) return 1;
        auto *batcher = static_cast<IRenderPrimitive *>(raw);
        IAggregateComponent *aggregate = nullptr;
        if (!Expect(batcher->QueryInterface(IID_IAggregateComponent,
                    reinterpret_cast<void **>(&aggregate)) == GR_OK && aggregate->Initialize() == GR_OK,
                    "world/HUD transform batcher initialized")) return 1;
        aggregate->Release();
        Transform world;
        world.translation = Vector(4.0F, -2.0F, 8.0F);
        Matrix4 worldProjection(true);
        worldProjection.d[0][0] = 0.42F; worldProjection.d[1][1] = -0.51F;
        worldProjection.d[2][2] = 1.13F; worldProjection.d[0][3] = 0.1F;
        const std::array<int,4> worldViewport{{7,11,640,480}};
        if (!Expect(batcher->set_state(RPR_BATCH, TRUE) == GR_OK && batcher->set_modelview(world) == GR_OK,
                    "world cached transform prepared")) return 1;
        Transform output; output.translation = Vector(101.0F,102.0F,103.0F);
        const bool getterCorrect = batcher->get_modelview(output) == GR_OK && SameModelview(output, world);
        Transform secondOutput; secondOutput.translation = Vector(-101.0F,-102.0F,-103.0F);
        const bool getterNonMutating = batcher->get_modelview(secondOutput) == GR_OK && SameModelview(secondOutput, world);
        bool transitionsCorrect = true;
        for (unsigned operation = 0; operation < 4; ++operation)
        {
            if (!Expect(batcher->set_projection(worldProjection) == GR_OK &&
                        batcher->set_viewport(7,11,640,480) == GR_OK &&
                        batcher->set_modelview(world) == GR_OK &&
                        batcher->set_state(RPR_STATE_FLAGS,0U) == GR_OK &&
                        batcher->set_state(RPR_STATE_ID,41U + operation) == GR_OK,
                        "queued world transform and viewport prepared")) return 1;
            const auto before = drawTransforms.size();
            if (!Expect(FrontierX::SubmitIndexedRenderSmokeScene(batcher) == GR_OK &&
                        drawTransforms.size() == before, "world draw remains genuinely pooled")) return 1;
            GENRESULT result = GR_GENERIC;
            Matrix4 changedProjection(true); changedProjection.d[0][0] = 0.75F;
            if (operation == 0) result = batcher->set_projection(changedProjection);
            if (operation == 1) result = batcher->set_ortho(0,1024,768,0,-1,1);
            if (operation == 2) result = batcher->set_perspective(0.5F,1.2F,5,100);
            if (operation == 3) result = batcher->set_viewport(3,5,320,240);
            const bool submittedBeforeTransition = drawTransforms.size() == before + 1U;
            if (!submittedBeforeTransition)
            {
                const auto lateFlush = batcher->flush(U32(-1));
                if (!Expect(lateFlush == GR_OK, "diagnostic late world flush must still execute original queued draw")) return 1;
            }
            const bool originalState = drawTransforms.size() == before + 1U &&
                SameProjection(drawTransforms.back().projection, worldProjection) &&
                drawTransforms.back().viewport == worldViewport &&
                std::fabs(drawTransforms.back().firstPosition[0] - 4.0F) < 1e-5F &&
                std::fabs(drawTransforms.back().firstPosition[1] + 2.52F) < 1e-5F &&
                std::fabs(drawTransforms.back().firstPosition[2] - 8.20F) < 1e-5F;
            if (result != GR_OK || !submittedBeforeTransition || !originalState)
                std::cerr << "world/HUD transition " << operation << " result=" << result
                          << " submitted-before=" << submittedBeforeTransition
                          << " original-state=" << originalState << '\n';
            transitionsCorrect &= result == GR_OK && submittedBeforeTransition && originalState;
        }
        if (!Expect(batcher->set_projection(worldProjection) == GR_OK &&
                    batcher->set_viewport(7,11,640,480) == GR_OK &&
                    batcher->set_modelview(world) == GR_OK &&
                    batcher->set_state(RPR_STATE_ID,51U) == GR_OK &&
                    FrontierX::SubmitIndexedRenderSmokeScene(batcher) == GR_OK,
                    "queued world draw before actual projection receiver rejection")) return 1;
        const auto beforeRejectedView = drawTransforms.size();
        rejectProjection = true;
        const auto rejectedProjection = batcher->set_projection(Matrix4(true));
        rejectProjection = false;
        Matrix4 queriedProjection(true);
        const bool rejectedViewPreserved = rejectedProjection == GR_GENERIC &&
            drawTransforms.size() == beforeRejectedView + 1U &&
            SameProjection(drawTransforms.back().projection, worldProjection) &&
            drawTransforms.back().viewport == worldViewport &&
            batcher->get_projection(queriedProjection) == GR_OK &&
            SameProjection(queriedProjection, worldProjection);
        if (!Expect(batcher->set_state(RPR_BATCH,FALSE) == GR_OK && batcher->set_modelview(world) == GR_OK,
                    "direct transform rejection fixture prepared")) return 1;
        Transform identity;
        rejectModelview = true;
        const bool rejectedOnce = batcher->set_modelview(identity) == GR_GENERIC;
        const bool rejectedTwice = batcher->set_modelview(identity) == GR_GENERIC;
        rejectModelview = false;
        if (!Expect(batcher->set_modelview(world) == GR_OK && batcher->set_modelview(identity) == GR_OK,
                    "direct identity transform applied")) return 1;
        Transform external; external.translation = Vector(-0.5F,-0.5F,0.0F);
        if (!Expect(pipeline.set_modelview(external) == GR_OK, "real direct PIPE owner changes transform")) return 1;
        const bool identityRestored = batcher->set_modelview(identity) == GR_OK && SameModelview(recordedModelview, identity);
        batcher->Release();
        const bool passed = getterCorrect && getterNonMutating && transitionsCorrect &&
                            rejectedViewPreserved && rejectedOnce && rejectedTwice && identityRestored;
        if (!passed) std::cerr << "transform predicates getter/nonmutating/transitions/reject1/reject2/identity: "
            << getterCorrect << '/' << getterNonMutating << '/' << transitionsCorrect << '/'
            << rejectedOnce << '/' << rejectedTwice << '/' << identityRestored
            << "; rejected-view=" << rejectedViewPreserved << '\n';
        if (!Expect(passed, "cached getters and world/HUD boundaries preserve real transform ownership and failure")) return 1;
        std::cout << "Original pooled world projection/viewport barriers and transactional modelview cache checked\n";
    }
    if (!Expect(assertionCount == 0U, "no production assertions")) return 1;
    nativePrimitive->Release();
    std::cout << "production batcher submitted " << submissionCount << " draws\n";
    return 0;
}
