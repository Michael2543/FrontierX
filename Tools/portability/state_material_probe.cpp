#include "DACOM.H"
#include "FDump.h"
#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "FrontierX/RenderState.h"
#include "FrontierX/StateMaterialRegistration.h"
#include "FrontierX/VertexBufferManagerRegistration.h"
#include "IMaterial.h"
#include "IStateMaterial.h"
#include "IVertexBufferManager.h"

#include <array>
#include <cstring>
#include <iostream>

namespace
{
struct Capture
{
    unsigned draws = 0U;
    U32 texture = 0U;
    FrontierX::TextureStageSnapshot stage{};
    FrontierX::RenderStateSnapshot render{};
};

int DACOM_CDECL IgnoreDiagnostic(ErrorCode, const C8 *, ...) { return 0; }

GENRESULT DACOM_CDECL CaptureDraw(void * context,
                                   const FrontierX::RenderSubmissionView & view)
{
    auto & capture = *static_cast<Capture *>(context);
    if (!view.IsValid() || view.vertexCount != 3U || view.indexCount != 3U ||
        view.PrimitiveCount() != 1U) return GR_INVALID_PARMS;
    ++capture.draws;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureRenderState(
    void * context, const FrontierX::RenderStateSnapshot & state)
{
    static_cast<Capture *>(context)->render = state;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureTextureStage(
    void * context, const FrontierX::TextureStageSnapshot & state)
{
    static_cast<Capture *>(context)->stage = state;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureTexture(void * context, U32 stage, U32 handle)
{
    if (stage != 0U || handle != 42U) return GR_NOT_IMPLEMENTED;
    static_cast<Capture *>(context)->texture = handle;
    return GR_OK;
}

class MaterialServices final : public IDAComponent
{
public:
    MaterialServices(IRenderPrimitive * primitive, IRenderPipeline * pipeline,
                     IVertexBufferManager * manager) :
        primitive_(primitive), pipeline_(pipeline), manager_(manager) {}

    GENRESULT COMAPI QueryInterface(const C8 * name, void ** output) override
    {
        if (!name || !output) return GR_INVALID_PARMS;
        *output = nullptr;
        if (std::strcmp(name, IID_IRenderPrimitive) == 0)
            *output = primitive_;
        else if (std::strcmp(name, IID_IRenderPipeline) == 0)
            *output = pipeline_;
        else if (std::strcmp(name, IID_IVertexBufferManager) == 0)
            *output = manager_;
        else
            return GR_INTERFACE_UNSUPPORTED;
        static_cast<IDAComponent *>(*output)->AddRef();
        return GR_OK;
    }
    U32 COMAPI AddRef() override { return ++references_; }
    U32 COMAPI Release() override { return --references_; }

private:
    IRenderPrimitive * primitive_;
    IRenderPipeline * pipeline_;
    IVertexBufferManager * manager_;
    U32 references_ = 1U;
};

bool Expect(bool condition, const char * message)
{
    if (!condition) std::cerr << "state material probe failed: " << message << '\n';
    return condition;
}
}

extern "C"
{
DA_ERROR_HANDLER FDUMP = IgnoreDiagnostic;
}

int main()
{
    ICOManager * components = DACOM_Acquire();
    if (!Expect(components && FrontierX::RegisterNativeStateMaterial(components),
                "production material registration")) return 1;
    RegisterTheVertexBufferManager(components);
    FrontierX::RegisterNativeRenderPrimitive(components);

    FrontierX::NativeRenderPipeline vertexPipeline(nullptr);
    Capture capture;
    FrontierX::NativeRenderPrimitiveDesc primitiveDescriptor(
        CaptureDraw, &capture, nullptr, CaptureRenderState,
        FrontierX::NativeRenderPipeline::OwnedVertexBufferLookup,
        &vertexPipeline, CaptureTextureStage, CaptureTexture);
    void * rawPrimitive = nullptr;
    if (!Expect(components->CreateInstance(&primitiveDescriptor, &rawPrimitive) == GR_OK &&
                    rawPrimitive, "native primitive creation")) return 1;
    auto * primitive = static_cast<IRenderPrimitive *>(rawPrimitive);
    FrontierX::NativeRenderPipeline pipeline(primitive);

    AGGDESC managerDescriptor("VertexBufferManager");
    managerDescriptor.outer = &vertexPipeline;
    void * rawManager = nullptr;
    if (!Expect(components->CreateInstance(&managerDescriptor, &rawManager) == GR_OK &&
                    rawManager, "production vertex manager creation")) return 1;
    auto * managerComponent = static_cast<IDAComponent *>(rawManager);
    IVertexBufferManager * manager = nullptr;
    if (!Expect(managerComponent->QueryInterface(IID_IVertexBufferManager,
                    reinterpret_cast<void **>(&manager)) == GR_OK && manager &&
                    manager->initialize(nullptr) == GR_OK,
                "production vertex manager initialization")) return 1;

    MaterialServices services(primitive, &pipeline, manager);
    DACOMDESC materialDescriptor;
    materialDescriptor.interface_name = "StateMaterial";
    void * rawMaterial = nullptr;
    if (!Expect(components->CreateInstance(&materialDescriptor, &rawMaterial) == GR_OK &&
                    rawMaterial, "production state material creation")) return 1;
    auto * material = static_cast<IMaterial *>(rawMaterial);
    IStateMaterial * states = nullptr;
    if (!Expect(material->QueryInterface(IID_IStateMaterial,
                    reinterpret_cast<void **>(&states)) == GR_OK && states &&
                    material->initialize(&services) == GR_OK,
                "material interfaces and initialization")) return 1;

    if (!Expect(states->set_render_state(D3DRS_ZENABLE, 0U) == GR_OK &&
                    states->set_texture_stage_state(0U, D3DTSS_COLOROP,
                        D3DTOP_SELECTARG1) == GR_OK &&
                    states->set_texture_stage_texture(0U, 42U) == GR_OK &&
                    material->apply() == GR_OK && capture.render.depthEnabled == 0U &&
                    capture.stage.colorOperation == D3DTOP_SELECTARG1 &&
                    capture.texture == 42U,
                "production material applies saved depth, combine and texture states"))
        return 1;
    if (!Expect(states->set_texture_stage_texture(0U, 43U) == GR_OK &&
                    material->apply() == GR_NOT_IMPLEMENTED &&
                    capture.texture == 42U &&
                    states->set_texture_stage_texture(0U, 42U) == GR_OK &&
                    material->apply() == GR_OK,
                "material reports rejected backend texture and recovers")) return 1;

    constexpr U32 format = FrontierX::LegacyD3d9::kRpVertexFvf;
    IRP_VERTEXBUFFERHANDLE buffer = 0U;
    if (!Expect(vertexPipeline.create_vertex_buffer(format, 3, 0U, &buffer) == GR_OK &&
                    buffer, "cached vertex buffer creation")) return 1;
    void * vertexData = nullptr;
    U32 vertexCount = 0U;
    const std::array<FrontierX::RenderVertexP3DiffuseT2, 3> vertices{{
        {-0.5F, -0.5F, 0.5F, 0xffffffffU, 0.0F, 1.0F},
        {0.5F, -0.5F, 0.5F, 0xffffffffU, 1.0F, 1.0F},
        {0.0F, 0.5F, 0.5F, 0xffffffffU, 0.5F, 0.0F},
    }};
    if (!Expect(vertexPipeline.lock_vertex_buffer(buffer, 0U, &vertexData,
                    &vertexCount) == GR_OK && vertexCount == vertices.size() &&
                    vertexData, "cached buffer lock")) return 1;
    std::memcpy(vertexData, vertices.data(), sizeof(vertices));
    if (!Expect(vertexPipeline.unlock_vertex_buffer(buffer) == GR_OK,
                "cached buffer unlock")) return 1;
    MaterialContext context{};
    context.vertex_buffer = buffer;
    U16 indices[3] = {0U, 1U, 2U};
    if (!Expect(material->render(&context, D3DPT_TRIANGLELIST, nullptr, 0U, 3U,
                    indices, 3U, 0U) == GR_OK && capture.draws == 1U,
                "production material indexed draw reaches native primitive")) return 1;
    indices[0] = 3U;
    if (!Expect(material->render(&context, D3DPT_TRIANGLELIST, nullptr, 0U, 3U,
                    indices, 3U, 0U) == GR_INVALID_PARMS && capture.draws == 1U &&
                    material->render(nullptr, D3DPT_TRIANGLELIST, nullptr, 0U, 3U,
                        indices, 3U, 0U) == GR_INVALID_PARMS,
                "material propagates rejected draw and null context")) return 1;

    if (!Expect(vertexPipeline.destroy_vertex_buffer(buffer) == GR_OK &&
                    manager->cleanup() == GR_OK,
                "material draw resources destroyed")) return 1;
    states->Release();
    material->Release();
    manager->Release();
    managerComponent->Release();
    primitive->Release();
    std::cout << "production state material draw passed\n";
    return 0;
}
