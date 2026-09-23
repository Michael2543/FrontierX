#ifndef FRONTIERX_RENDER_PIPELINE_ADAPTER_H
#define FRONTIERX_RENDER_PIPELINE_ADAPTER_H

#include "rendpipeline.h"

namespace FrontierX
{

// Native renderer owners derive from this existing-interface adapter and
// override only slots whose semantics have been implemented and validated.
// Every remaining legacy operation fails explicitly.
class RenderPipelineAdapter : public IRenderPipeline
{
public:
    ID3DXEffect ** load_effect(const char *, IComponentFactory *) override { return nullptr; }
    GENRESULT COMAPI set_vs_constants(UINT, CONST FLOAT *, UINT) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_ps_constants(UINT, CONST FLOAT *, UINT) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_vertex_declaration(IDirect3DVertexDeclaration9 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_default_constants(const Transform &, Vector, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI startup(const char *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI shutdown() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_device_info(RPDEVICEINFO *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_num_display_modes(U32 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_display_mode(RPDISPLAYMODEINFO *, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI query_device_ability(RPDEVICEABILITY, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_pipeline_state(RPPIPELINESTATE, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_pipeline_state(RPPIPELINESTATE, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI create_buffers(HWND, int, int) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI destroy_buffers() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_num_device_texture_formats(U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_device_texture_format(PixelFormat *, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_device_stats(RPDEVICESTATS *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI clear_buffers(U32, RECT *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI swap_buffers() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI lock_buffer(RPLOCKDATA *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI unlock_buffer() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_buffer_interface(const char *, void **) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_buffer_dc(HDC *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI release_buffer_dc(HDC) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI create_vertex_buffer(U32, int, U32, IRP_VERTEXBUFFERHANDLE *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI destroy_vertex_buffer(IRP_VERTEXBUFFERHANDLE) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI lock_vertex_buffer(IRP_VERTEXBUFFERHANDLE, U32, void **, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI unlock_vertex_buffer(IRP_VERTEXBUFFERHANDLE) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI optimize_vertex_buffer(IRP_VERTEXBUFFERHANDLE) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI process_vertex_buffer(IRP_VERTEXBUFFERHANDLE, IRP_VERTEXBUFFERHANDLE,
                                           int, int, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_modelview(const Transform &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_modelview(Transform &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_projection(const Matrix4 &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_projection(Matrix4 &) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_viewport(int, int, int, int) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_viewport(int *, int *, int *, int *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_window(HWND, int, int, int, int) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_window(HWND *, int *, int *, int *, int *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_depth_range(float, float) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_depth_range(float *, float *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_lookat(float, float, float, float, float, float,
                               float, float, float) override
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
    GENRESULT COMAPI set_light(U32, D3DLIGHT9 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_light(U32, D3DLIGHT9 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_light_enable(U32, U32) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_light_enable(U32, U32 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_num_lights(U32 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_lights(U32, U32, U32 *) override { return GR_NOT_IMPLEMENTED; }
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
    GENRESULT COMAPI set_sampler_state(U32, D3DSAMPLERSTATETYPE, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_sampler_state(U32, D3DSAMPLERSTATETYPE, U32 *) override
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
    GENRESULT COMAPI set_texture_stage_transform(U32, Matrix4 &) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_texture_stage_transform(U32, Matrix4 &) override
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
    GENRESULT COMAPI create_state_block(D3DSTATEBLOCKTYPE, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI update_state_block(U32) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI begin_state_block() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI end_state_block(U32 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI apply_state_block(U32) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI delete_state_block(U32) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI draw_primitive(D3DPRIMITIVETYPE, U32, const void *, int, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI draw_indexed_primitive(D3DPRIMITIVETYPE, U32, const void *, int,
                                            const U16 *, int, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI draw_primitive_vb(D3DPRIMITIVETYPE, IRP_VERTEXBUFFERHANDLE,
                                       int, int, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI draw_indexed_primitive_vb(D3DPRIMITIVETYPE, IRP_VERTEXBUFFERHANDLE,
                                               int, int, const U16 *, int, U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI draw_indexed_primitive_vb(D3DPRIMITIVETYPE, IRP_VERTEXBUFFERHANDLE,
                                               int, int, IDirect3DIndexBuffer9 *, int,
                                               U32) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI create_index_buffer(UINT, IDirect3DIndexBuffer9 **) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI create_cube_texture_from_file(const char *, IComponentFactory *, U32 &) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI create_texture(int, int, const PixelFormat &, int, U32, U32 &) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI destroy_texture(U32) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI is_texture(U32) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI lock_texture(U32, int, RPLOCKDATA *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI unlock_texture(U32, int) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_texture_format(U32, PixelFormat *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_texture_dim(U32, U32 *, U32 *, U32 *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_texture_interface(U32, const char *, void **) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_texture_dc(U32, HDC *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI release_texture_dc(U32, HDC) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI set_texture_palette(U32, int, int, const RGB *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_texture_palette(U32, int, int, RGB *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI set_texture_level_data(U32, int, int, int, int,
                                            const PixelFormat &, const void *,
                                            const void *, const RGB *) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI blit_texture(U32, U32, RECT, U32, U32, RECT) override
    {
        return GR_NOT_IMPLEMENTED;
    }
    GENRESULT COMAPI get_num_textures(U32 *) override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI get_texture(U32, U32 *) override { return GR_NOT_IMPLEMENTED; }
};

} // namespace FrontierX

#endif
