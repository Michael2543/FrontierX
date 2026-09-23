#ifndef FRONTIERX_NATIVE_RENDER_PRIMITIVE_H
#define FRONTIERX_NATIVE_RENDER_PRIMITIVE_H

#include "DACOM.H"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/RenderState.h"
#include "FrontierX/RenderSubmission.h"

struct ICOManager;

namespace FrontierX
{
using NativeRenderSubmissionCallback =
    GENRESULT (DACOM_CDECL *)(void * context, const RenderSubmissionView & submission);
using NativeRenderSubmission2Callback =
    GENRESULT (DACOM_CDECL *)(void * context, const RenderSubmissionView2 & submission);
using NativeRenderStateCallback =
    GENRESULT (DACOM_CDECL *)(void * context, const RenderStateSnapshot & state);

struct NativeRenderPrimitiveDesc : public DACOMDESC
{
    explicit NativeRenderPrimitiveDesc(
        NativeRenderSubmissionCallback submitCallback = nullptr,
        void * submitContext = nullptr,
        NativeRenderSubmission2Callback submit2Callback = nullptr,
        NativeRenderStateCallback stateCallback = nullptr,
        NativeVertexBufferLookup vertexBufferLookup = nullptr,
        void * vertexBufferContext = nullptr) :
        callback(submitCallback), callback2(submit2Callback), state(stateCallback),
        context(submitContext), lookup(vertexBufferLookup), lookupContext(vertexBufferContext)
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
};

void RegisterNativeRenderPrimitive(ICOManager * manager);
}

#endif
