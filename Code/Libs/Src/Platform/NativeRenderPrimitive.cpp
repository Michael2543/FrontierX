#include "FrontierX/NativeRenderPrimitive.h"

#include "FrontierX/LegacyRenderSubmission.h"
#include "IRenderPrimitive.h"
#include "TComponent.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
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

    GENRESULT init(NativeRenderPrimitiveDesc * descriptor)
    {
        if (!descriptor || descriptor->size != sizeof(*descriptor) ||
            !descriptor->interface_name ||
            std::strcmp(descriptor->interface_name, interface_name) != 0 ||
            !descriptor->callback)
        {
            return GR_INVALID_PARMS;
        }
        callback_ = descriptor->callback;
        context_ = descriptor->context;
        return GR_OK;
    }

    GENRESULT COMAPI set_state(RPRSTATE, U32) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_state(RPRSTATE, U32 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_modelview(const Transform &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_modelview(Transform &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_viewport(int, int, int, int) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_viewport(int *, int *, int *, int *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_ortho(float, float, float, float, float, float) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_perspective(float, float, float, float) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_projection(const Matrix4 &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_projection(Matrix4 &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_material(D3DMATERIAL9 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_material(D3DMATERIAL9 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI begin_scene() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI end_scene() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_render_state(D3DRENDERSTATETYPE, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_render_state(D3DRENDERSTATETYPE, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_texture_stage_state(U32, D3DTEXTURESTAGESTATETYPE, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_texture_stage_state(U32, D3DTEXTURESTAGESTATETYPE, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_sampler_state(U32, D3DSAMPLERSTATETYPE, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_sampler_state(U32, D3DSAMPLERSTATETYPE, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_texture_stage_texture(U32, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_texture_stage_texture(U32, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
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

    GENRESULT COMAPI draw_primitive_vb(D3DPRIMITIVETYPE,
                                       IRP_VERTEXBUFFERHANDLE,
                                       int,
                                       int,
                                       U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI draw_indexed_primitive_vb(D3DPRIMITIVETYPE,
                                               IRP_VERTEXBUFFERHANDLE,
                                               int,
                                               int,
                                               const U16 *,
                                               int,
                                               U32) override
    {
        return GR_NOT_IMPLEMENTED;
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

        try
        {
            converted_.resize(static_cast<std::size_t>(vertexCount));
        }
        catch (const std::bad_alloc &)
        {
            return GR_OUT_OF_MEMORY;
        }

        RenderSubmissionView submission{};
        const LegacySubmissionInput input{
            static_cast<std::uint32_t>(primitiveType),
            vertexFormat,
            vertices,
            static_cast<std::size_t>(vertexCount),
            indices,
            static_cast<std::size_t>(indexCount),
        };
        const LegacySubmissionStatus status = AdaptLegacyRpVertexSubmission(
            input, converted_.data(), converted_.size(), submission);
        if (status != LegacySubmissionStatus::Ok)
        {
            return StatusResult(status);
        }
        return callback_(context_, submission);
    }

    NativeRenderSubmissionCallback callback_ = nullptr;
    void * context_ = nullptr;
    std::vector<RenderVertexP3DiffuseT2> converted_;
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
}
}
