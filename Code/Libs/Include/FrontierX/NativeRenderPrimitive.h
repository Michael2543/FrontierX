#ifndef FRONTIERX_NATIVE_RENDER_PRIMITIVE_H
#define FRONTIERX_NATIVE_RENDER_PRIMITIVE_H

#include "DACOM.H"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/RenderState.h"
#include "FrontierX/RenderSubmission.h"

struct ICOManager;
class Transform;
class Matrix4;

namespace FrontierX
{
using NativeRenderSubmissionCallback =
    GENRESULT (DACOM_CDECL *)(void * context, const RenderSubmissionView & submission);
using NativeRenderSubmission2Callback =
    GENRESULT (DACOM_CDECL *)(void * context, const RenderSubmissionView2 & submission);
using NativeRenderSubmission3Callback =
    GENRESULT (DACOM_CDECL *)(void * context, const RenderSubmissionView3 & submission);
using NativeRenderSubmission4Callback =
    GENRESULT (DACOM_CDECL *)(void * context, const RenderSubmissionView4 & submission);
using NativeRenderStateCallback =
    GENRESULT (DACOM_CDECL *)(void * context, const RenderStateSnapshot & state);
using NativeTextureStageCallback =
    GENRESULT (DACOM_CDECL *)(void * context, const TextureStageSnapshot & state);
using NativeTextureBindCallback =
    GENRESULT (DACOM_CDECL *)(void * context, U32 stage, U32 texture);
using NativeSamplerStateCallback =
    GENRESULT (DACOM_CDECL *)(void * context, const SamplerStateSnapshot & state);
using NativeModelviewCallback =
    GENRESULT (DACOM_CDECL *)(void * context, const Transform & modelview);
using NativeProjectionCallback =
    GENRESULT (DACOM_CDECL *)(void * context, const Matrix4 & projection);

struct NativeRenderPrimitiveDesc : public DACOMDESC
{
    explicit NativeRenderPrimitiveDesc(
        NativeRenderSubmissionCallback submitCallback = nullptr,
        void * submitContext = nullptr,
        NativeRenderSubmission2Callback submit2Callback = nullptr,
        NativeRenderStateCallback stateCallback = nullptr,
        NativeVertexBufferLookup vertexBufferLookup = nullptr,
        void * vertexBufferContext = nullptr,
        NativeTextureStageCallback textureStageCallback = nullptr,
        NativeTextureBindCallback textureBindCallback = nullptr,
        NativeModelviewCallback modelviewCallback = nullptr,
        NativeProjectionCallback projectionCallback = nullptr,
        NativeRenderSubmission3Callback submit3Callback = nullptr,
        NativeRenderSubmission4Callback submit4Callback = nullptr,
        NativeSamplerStateCallback samplerCallback = nullptr) :
        callback(submitCallback), callback2(submit2Callback), state(stateCallback),
        context(submitContext), lookup(vertexBufferLookup),
        lookupContext(vertexBufferContext), textureStage(textureStageCallback),
        textureBind(textureBindCallback), modelview(modelviewCallback),
        projection(projectionCallback), callback3(submit3Callback),
        callback4(submit4Callback), sampler(samplerCallback)
    {
        interface_name = "IRenderPrimitive";
        size = sizeof(*this);
    }

    NativeRenderSubmissionCallback callback;
    NativeRenderSubmission2Callback callback2;
    NativeRenderStateCallback state;
    void * context;
    NativeVertexBufferLookup lookup;
    void * lookupContext;
    NativeTextureStageCallback textureStage;
    NativeTextureBindCallback textureBind;
    NativeModelviewCallback modelview;
    NativeProjectionCallback projection;
    NativeRenderSubmission3Callback callback3;
    NativeRenderSubmission4Callback callback4;
    NativeSamplerStateCallback sampler;
};

void RegisterNativeRenderPrimitive(ICOManager * manager);
GENRESULT BindNativeRenderPrimitive(IRenderPrimitive * primitive,
                                    const NativeRenderPrimitiveDesc & descriptor);
GENRESULT UnbindNativeRenderPrimitive(IRenderPrimitive * primitive);
}

#endif
