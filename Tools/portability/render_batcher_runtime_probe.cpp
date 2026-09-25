#include "CQImage.h"
#include "CQBatch.h"
#include "DEffectOpts.h"
#include "FDump.h"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "FrontierX/RenderSmokeScene.h"
#include "GlobalFlagTypes.h"
#include "IRenderPrimitive.h"
#include "IVertexBufferManager.h"
#include "IProfileParser_Utility.h"
#include "RenderBatcherRegistration.h"

#include <cstring>
#include <iostream>

class RenderBatcher;
extern RenderBatcher * BATCHER;

CQGlobalFlags CQFLAGS = {};
GlobalRenderFlags CQRENDERFLAGS = {};
GlobalEffectsOptions CQEFFECTS = {};
IRenderPipeline *PIPE = nullptr;
IVertexBufferManager *VB_MANAGER = nullptr;

namespace
{
unsigned assertionCount = 0;
unsigned submissionCount = 0;
unsigned mergedSubmissionCount = 0;
bool submissionShapeValid = true;

GENRESULT DACOM_CDECL AcceptRenderState(
    void *, const FrontierX::RenderStateSnapshot &)
{
    return GR_OK;
}

GENRESULT DACOM_CDECL AcceptTextureStage(
    void *, const FrontierX::TextureStageSnapshot &)
{
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
    if (!Expect(components != nullptr, "DACOM bootstrap")) return 1;
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
    void * nativeRaw = nullptr;
    if (!Expect(components->CreateInstance(&nativeDescriptor, &nativeRaw) == GR_OK &&
                    nativeRaw != nullptr, "native submission owner")) return 1;
    auto * nativePrimitive = static_cast<IRenderPrimitive *>(nativeRaw);
    FrontierX::NativeRenderPipeline pipeline(nativePrimitive);
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
    if (!Expect(assertionCount == 0U, "no production assertions")) return 1;
    nativePrimitive->Release();
    std::cout << "production batcher submitted " << submissionCount << " draws\n";
    return 0;
}
