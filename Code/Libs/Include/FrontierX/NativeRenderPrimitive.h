#ifndef FRONTIERX_NATIVE_RENDER_PRIMITIVE_H
#define FRONTIERX_NATIVE_RENDER_PRIMITIVE_H

#include "DACOM.H"
#include "FrontierX/RenderSubmission.h"

struct ICOManager;

namespace FrontierX
{
using NativeRenderSubmissionCallback =
    GENRESULT (DACOM_CDECL *)(void * context, const RenderSubmissionView & submission);

struct NativeRenderPrimitiveDesc : public DACOMDESC
{
    explicit NativeRenderPrimitiveDesc(
        NativeRenderSubmissionCallback submitCallback = nullptr,
        void * submitContext = nullptr) :
        callback(submitCallback), context(submitContext)
    {
        interface_name = "IRenderPrimitive";
        size = sizeof(*this);
    }

    NativeRenderSubmissionCallback callback;
    void * context;
};

void RegisterNativeRenderPrimitive(ICOManager * manager);
}

#endif
