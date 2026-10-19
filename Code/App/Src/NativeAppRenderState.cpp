#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
#include "Include/CQTrace.h"
#include "MeshRender.h"
#include "IRenderPrimitive.h"
#include <ICamera.h>
#include <ENGINE.H>
#include <renderer.h>
#include <Mesh.h>
#include <vector>
#include <cstring>
#include <stdexcept>

#include <cstdlib>

namespace
{
void RequireState(GENRESULT result, const char * operation)
{
    if (result == GR_OK) return;
    ICQImage::Bomb("native SetupDiffuseBlend rejected %s (%d)", operation, result);
    std::abort();
}
}

// The direct Engine renderer consumes legacy materials, not CQ2 modifiers.
// Use its existing per-instance mesh owner for both ships and platforms.
void SetNativeLegacyTeamColor(IEngine *engine, IRenderer *renderer,
                              S32 root, COLORREF color, MeshChain *chain)
{
    // Destruction renders the extent owner, rather than the direct Engine mesh.
    // Keep its per-instance markings in sync before it is detached into debris.
    if (chain)
        for (int child = 0; child < chain->numChildren; ++child)
        {
            MeshInfo *info = chain->mi[child];
            if (!info->bHasMesh || !info->mr) continue;
            IRenderMaterial *group = nullptr;
            while ((group = info->mr->GetNextFaceGroup(group)))
            {
                if (!(group->flags & MM_MARKINGS)) continue;
                auto &material = info->fgi[group->fg_idx];
                material.diffuse.r = color & 0xffU;
                material.diffuse.g = (color >> 8) & 0xffU;
                material.diffuse.b = (color >> 16) & 0xffU;
                material.emissive.r = 0.3F * material.diffuse.r;
                material.emissive.g = 0.3F * material.diffuse.g;
                material.emissive.b = 0.3F * material.diffuse.b;
                info->bCacheValid = false;
            }
        }
    std::vector<INSTANCE_INDEX> pending{root};
    while (!pending.empty())
    {
        const INSTANCE_INDEX index = pending.back();
        pending.pop_back();
        Mesh *mesh = renderer->get_instance_mesh(index);
        bool markings = false;
        if (mesh && mesh->material_list)
            for (int material = 0; material < mesh->material_cnt; ++material)
                markings |= std::strstr(mesh->material_list[material].name, "markings") != nullptr;
        if (markings)
        {
            mesh = renderer->get_unique_instance_mesh(index);
            if (!mesh) throw std::runtime_error("Native team-color unique mesh unavailable");
            for (int material = 0; material < mesh->material_cnt; ++material)
            {
                Material &entry = mesh->material_list[material];
                if (!std::strstr(entry.name, "markings")) continue;
                entry.specular.r = entry.diffuse.r = color & 0xffU;
                entry.specular.g = entry.diffuse.g = (color >> 8) & 0xffU;
                entry.specular.b = entry.diffuse.b = (color >> 16) & 0xffU;
                entry.emission.r = 0.3F * entry.diffuse.r;
                entry.emission.g = 0.3F * entry.diffuse.g;
                entry.emission.b = 0.3F * entry.diffuse.b;
            }
        }
        for (INSTANCE_INDEX child = engine->get_instance_child_next(index, EN_DONT_RECURSE,
                INVALID_INSTANCE_INDEX); child != INVALID_INSTANCE_INDEX;
             child = engine->get_instance_child_next(index, EN_DONT_RECURSE, child))
            pending.push_back(child);
    }
}

void __stdcall SetupDiffuseBlend(U32 textureID, bool clamp)
{
    if (!BATCH)
    {
        ICQImage::Bomb("native SetupDiffuseBlend has no render primitive");
        std::abort();
    }
    RequireState(BATCH->set_render_state(D3DRS_COLORVERTEX, 1U), "vertex color");
    RequireState(BATCH->set_render_state(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1),
                 "diffuse source");
    RequireState(BATCH->set_render_state(D3DRS_LIGHTING, 0U), "lighting");
    RequireState(BATCH->set_texture_stage_texture(0U, textureID), "stage-zero texture");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX, 0U),
                 "stage-zero primary UVs");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_COLOROP, D3DTOP_MODULATE),
                 "color operation");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_COLORARG1, D3DTA_TEXTURE),
                 "color argument one");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_COLORARG2, D3DTA_DIFFUSE),
                 "color argument two");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_ALPHAOP, D3DTOP_MODULATE),
                 "alpha operation");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_ALPHAARG1, D3DTA_TEXTURE),
                 "alpha argument one");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE),
                 "alpha argument two");
    RequireState(BATCH->set_texture_stage_state(1U, D3DTSS_COLOROP, D3DTOP_DISABLE),
                 "stage-one color disable");
    RequireState(BATCH->set_texture_stage_state(1U, D3DTSS_ALPHAOP, D3DTOP_DISABLE),
                 "stage-one alpha disable");
    RequireState(BATCH->set_sampler_state(0U, D3DSAMP_MINFILTER, D3DTEXF_LINEAR),
                 "minification filter");
    RequireState(BATCH->set_sampler_state(0U, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR),
                 "magnification filter");
    RequireState(BATCH->set_sampler_state(0U, D3DSAMP_MIPFILTER, D3DTEXF_POINT),
                 "mipmap filter");
    const U32 address = clamp ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    RequireState(BATCH->set_sampler_state(0U, D3DSAMP_ADDRESSU,
                                         address), "U address mode");
    RequireState(BATCH->set_sampler_state(0U, D3DSAMP_ADDRESSV,
                                         address), "V address mode");
    for (U32 stage = 1U; stage < 4U; ++stage)
    {
        RequireState(BATCH->set_texture_stage_state(stage, D3DTSS_COLOROP,
                                                    D3DTOP_DISABLE), "later-stage color disable");
        RequireState(BATCH->set_texture_stage_state(stage, D3DTSS_ALPHAOP,
                                                    D3DTOP_DISABLE), "later-stage alpha disable");
        RequireState(BATCH->set_texture_stage_texture(stage, 0U),
                     "later-stage texture clear");
    }
}

void __stdcall DisableTextures()
{
    if (!BATCH)
    {
        ICQImage::Bomb("native DisableTextures has no render primitive");
        std::abort();
    }
    RequireState(BATCH->set_texture_stage_texture(0U, 0U), "stage-zero texture clear");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_COLOROP, D3DTOP_SELECTARG1),
                 "color operation");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_COLORARG1, D3DTA_DIFFUSE),
                 "color argument one");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1),
                 "alpha operation");
    RequireState(BATCH->set_texture_stage_state(0U, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE),
                 "alpha argument one");
    RequireState(BATCH->set_texture_stage_texture(1U, 0U), "stage-one texture clear");
    RequireState(BATCH->set_texture_stage_state(1U, D3DTSS_COLOROP, D3DTOP_DISABLE),
                 "stage-one color disable");
    RequireState(BATCH->set_texture_stage_state(1U, D3DTSS_ALPHAOP, D3DTOP_DISABLE),
                 "stage-one alpha disable");
}
#endif
