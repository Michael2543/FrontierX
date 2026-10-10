#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
#include "CQImage.h"
#include "IRenderPrimitive.h"

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
