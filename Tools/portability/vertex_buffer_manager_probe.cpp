#include "DACOM.H"
#include "FDump.h"
#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/VertexBufferManagerRegistration.h"
#include "IVertexBufferManager.h"

#include <iostream>

namespace
{
int DACOM_CDECL IgnoreDiagnostic(ErrorCode, const C8 *, ...)
{
    return 0;
}

bool Expect(bool condition, const char * message)
{
    if (!condition) std::cerr << "vertex-buffer manager probe failed: " << message << '\n';
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
    if (!Expect(components != nullptr, "DACOM bootstrap")) return 1;
    RegisterTheVertexBufferManager(components);

    FrontierX::NativeRenderPipeline pipeline(nullptr);
    AGGDESC descriptor("VertexBufferManager");
    descriptor.outer = &pipeline;
    IDAComponent * inner = nullptr;
    descriptor.inner = &inner;
    void * raw = nullptr;
    if (!Expect(components->CreateInstance(&descriptor, &raw) == GR_OK &&
                raw && inner,
                "production manager aggregate factory")) return 1;
    IVertexBufferManager * manager = nullptr;
    if (!Expect(inner->QueryInterface(IID_IVertexBufferManager,
                    reinterpret_cast<void **>(&manager)) == GR_OK && manager,
                "production manager interface")) return 1;
    if (!Expect(manager->add_vertex_buffer(FrontierX::LegacyD3d9::kRpVertexFvf,
                    64, 0) == GR_INVALID_PARMS,
                "pre-initialization addition rejected")) return 1;
    if (!Expect(manager->initialize(nullptr) == GR_OK, "native default initialization")) return 1;

    constexpr U32 format = FrontierX::LegacyD3d9::kRpVertexFvf;
    if (!Expect(manager->add_vertex_buffer(format, 0, 0) == GR_INVALID_PARMS &&
                manager->add_vertex_buffer(format, 0xffffffffU, 0) == GR_INVALID_PARMS,
                "invalid preallocation counts rejected")) return 1;
    IRP_VERTEXBUFFERHANDLE handle = 0;
    void * vertices = nullptr;
    U32 actualFormat = 0;
    U32 actualCount = 0;
    if (!Expect(manager->acquire_vertex_buffer(format, 64, 0, 0,
                    IVBM_AVBF_EXACT | IVBM_AVBF_NO_CREATE,
                    &handle, &vertices, &actualFormat, &actualCount) != GR_OK &&
                handle == 0 && vertices == nullptr,
                "NO_CREATE must preserve outputs")) return 1;
    if (!Expect(manager->add_vertex_buffer(format, 64, 0) == GR_OK,
                "production preallocation")) return 1;
    if (!Expect(manager->acquire_vertex_buffer(format, 64, 0, 0,
                    IVBM_AVBF_EXACT | IVBM_AVBF_NO_CREATE,
                    &handle, &vertices, &actualFormat, &actualCount) == GR_OK &&
                handle != 0 && vertices && actualFormat == format && actualCount == 64,
                "exact-size preallocated acquire")) return 1;
    if (!Expect(manager->remove_all_vertex_buffers() == GR_GENERIC,
                "locked buffer teardown must fail without losing ownership")) return 1;
    if (!Expect(pipeline.unlock_vertex_buffer(handle) == GR_OK,
                "release of locked backing buffer")) return 1;
    if (!Expect(manager->release_vertex_buffer(handle) == GR_OK,
                "production release")) return 1;
    if (!Expect(manager->release_vertex_buffer(handle) == GR_INVALID_PARMS &&
                manager->release_vertex_buffer(0xffffU) == GR_INVALID_PARMS,
                "duplicate and unknown releases rejected")) return 1;

    IRP_VERTEXBUFFERHANDLE rounded = 0;
    void * roundedVertices = nullptr;
    U32 roundedFormat = 0;
    U32 roundedCount = 0;
    if (!Expect(manager->acquire_vertex_buffer(format, 65, 0, 0, 0,
                    &rounded, &roundedVertices, &roundedFormat, &roundedCount) == GR_OK &&
                rounded != 0 && rounded != handle && roundedVertices &&
                roundedFormat == format && roundedCount == 128,
                "dynamic rounded allocation")) return 1;
    if (!Expect(pipeline.unlock_vertex_buffer(rounded) == GR_OK &&
                manager->release_vertex_buffer(rounded) == GR_OK,
                "dynamic buffer release")) return 1;
    if (!Expect(manager->remove_all_vertex_buffers() == GR_OK,
                "production teardown")) return 1;
    if (!Expect(pipeline.lock_vertex_buffer(handle, 0, &vertices, &actualCount) ==
                    GR_INVALID_PARMS &&
                pipeline.lock_vertex_buffer(rounded, 0, &vertices, &actualCount) ==
                    GR_INVALID_PARMS,
                "backing buffers destroyed")) return 1;
    if (!Expect(manager->cleanup() == GR_OK, "idempotent cleanup")) return 1;
    if (!Expect(manager->add_vertex_buffer(format, 64, 0) == GR_INVALID_PARMS,
                "post-cleanup addition rejected")) return 1;
    manager->Release();
    inner->Release();
    std::cout << "production vertex-buffer manager lifecycle passed\n";
    return 0;
}
