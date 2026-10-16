#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
#include "Camera.h"
#include "IRenderPrimitive.h"
#include "MyVertex.h"
#include "CQImage.h"
#include "VFX.H"
#include "rendpipeline.h"
#include "xform.h"

#include <cstdlib>
#include <algorithm>
#include <cstdint>
#include <limits>

namespace
{
void Require2D(GENRESULT result, const char * operation)
{
    if (result == GR_OK) return;
    ICQImage::Bomb("native App 2D draw rejected %s (%d)", operation, result);
    std::abort();
}

void Check2D(const PANE * pane)
{
    if (!PIPE || !BATCH || !SCREENRESX || !SCREENRESY ||
        SCREENRESX > static_cast<DWORD>(std::numeric_limits<int>::max()) ||
        SCREENRESY > static_cast<DWORD>(std::numeric_limits<int>::max()) ||
        (pane && (!CAMERA || SCREEN_HEIGHT <= 0 || pane->x0 < 0 || pane->y0 < 0 ||
                  pane->x1 < pane->x0 || pane->y1 < pane->y0 ||
                  pane->x1 == std::numeric_limits<int>::max() ||
                  pane->y1 == std::numeric_limits<int>::max())))
    {
        ICQImage::Bomb("native App 2D draw has no valid camera/render target");
        std::abort();
    }
    if (CQFLAGS.bFrameLockEnabled)
    {
        ICQImage::Bomb("native App framebuffer locking is unsupported");
        std::abort();
    }
}

bool HiddenUnderToolbar(const PANE * pane, S32 y0, S32 y1)
{
    return pane && pane == CAMERA->GetPane() && !CQFLAGS.bNoToolbar &&
        y0 > pane->y1 - IDEAL2REALY(40) &&
        y1 > pane->y1 - IDEAL2REALY(40);
}

void DrawRectangle(const PANE * pane, S32 x0, S32 y0, S32 x1, S32 y1,
                   COLORREF color, bool hash, S32 filledLength = 0)
{
    Check2D(pane);
    if (HiddenUnderToolbar(pane, y0, y1)) return;
    if (x1 < x0 || y1 < y0 ||
        x1 == std::numeric_limits<S32>::max() ||
        y1 == std::numeric_limits<S32>::max())
    {
        ICQImage::Bomb("native App 2D rectangle bounds invalid");
        std::abort();
    }

    OrthoView(pane);
    if (hash)
    {
        Require2D(PIPE->set_render_state(D3DRS_ZENABLE, FALSE), "depth disable");
        Require2D(PIPE->set_render_state(D3DRS_ALPHABLENDENABLE, TRUE), "alpha enable");
        Require2D(PIPE->set_render_state(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA), "source blend");
        Require2D(PIPE->set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA),
                  "destination blend");
    }
    DisableTextures();
    if (pane)
    {
        const std::int64_t maxX = static_cast<std::int64_t>(x1) + pane->x0;
        const std::int64_t maxY = static_cast<std::int64_t>(y1) + pane->y0;
        const std::int64_t minX = static_cast<std::int64_t>(x0) + pane->x0;
        const std::int64_t minY = static_cast<std::int64_t>(y0) + pane->y0;
        if (maxX >= std::numeric_limits<S32>::max() ||
            maxY >= std::numeric_limits<S32>::max() ||
            minX < std::numeric_limits<S32>::min() ||
            minY < std::numeric_limits<S32>::min())
        {
            ICQImage::Bomb("native App 2D rectangle pane offset overflow");
            std::abort();
        }
        x0 += pane->x0; x1 += pane->x0;
        y0 += pane->y0; y1 += pane->y0;
    }
    const U8 red = static_cast<U8>(color);
    const U8 green = static_cast<U8>(color >> 8U);
    const U8 blue = static_cast<U8>(color >> 16U);
    if (!hash) Require2D(BATCH->set_state(RPR_STATE_ID, 6U), "fill batch state");
    if (hash) PB.Color4ub(red, green, blue, 100U);
    else PB.Color3ub(red, green, blue);
    PB.Begin(PB_QUADS);
    const auto quad = [y0, y1](S32 left, S32 right)
    {
        PB.Vertex3f(left, y0, 0.0F);
        PB.Vertex3f(right + 1, y0, 0.0F);
        PB.Vertex3f(right + 1, y1 + 1, 0.0F);
        PB.Vertex3f(left, y1 + 1, 0.0F);
    };
    if (filledLength > 0)
    {
        // DrawHighlighted's original four-pixel rectangles at five-pixel
        // intervals share all state. Keep their geometry and submission order.
        for (std::int64_t i = 0; i < filledLength; i += 5)
            quad(static_cast<S32>(x0 + i), static_cast<S32>(x0 + i + 3));
    }
    else quad(x0, x1);
    PB.End();
    if (!hash) Require2D(BATCH->set_state(RPR_STATE_ID, 0U), "restore batch state");
}
}

void OrthoView(const PANE * pane)
{
    Check2D(pane);
    Transform transform;
    transform.translation.x = -0.5F;
    transform.translation.y = -0.5F;
    Require2D(BATCH->set_modelview(transform), "orthographic modelview");
    if (pane)
    {
        Require2D(BATCH->set_ortho(static_cast<float>(pane->x0),
                  static_cast<float>(pane->x1 + 1),
                  static_cast<float>(pane->y1 + 1),
                  static_cast<float>(pane->y0), -1.0F, 1.0F), "pane projection");
        Require2D(BATCH->set_viewport(pane->x0, pane->y0,
                  pane->x1 - pane->x0 + 1, pane->y1 - pane->y0 + 1),
                  "pane viewport");
    }
    else
    {
        Require2D(BATCH->set_ortho(0.0F, static_cast<float>(SCREENRESX),
                  static_cast<float>(SCREENRESY), 0.0F, -1.0F, 1.0F),
                  "full projection");
        Require2D(BATCH->set_viewport(0, 0, static_cast<int>(SCREENRESX),
                  static_cast<int>(SCREENRESY)), "full viewport");
    }
    Require2D(BATCH->set_render_state(D3DRS_CULLMODE, D3DCULL_NONE), "2D culling");
}

void DA::RectangleHash(const PANE * pane, S32 x0, S32 y0, S32 x1, S32 y1,
                       COLORREF color)
{
    DrawRectangle(pane, x0, y0, x1, y1, color, true);
}

void DA::RectangleFill(const PANE * pane, S32 x0, S32 y0, S32 x1, S32 y1,
                       COLORREF color)
{
    DrawRectangle(pane, x0, y0, x1, y1, color, false);
}

void DA::RectangleFillSegments(const PANE * pane, S32 x0, S32 y0,
                               S32 filledLength, COLORREF color)
{
    if (filledLength <= 0) return;
    const std::int64_t lastX = static_cast<std::int64_t>(x0) +
        ((static_cast<std::int64_t>(filledLength) - 1) / 5) * 5 + 3;
    if (lastX >= std::numeric_limits<S32>::max() ||
        y0 > std::numeric_limits<S32>::max() - 3)
    {
        ICQImage::Bomb("native App segmented bar bounds invalid");
        std::abort();
    }
    DrawRectangle(pane, x0, y0, static_cast<S32>(lastX), y0 + 2,
                  color, false, filledLength);
}

void DA::LineDraw(const PANE * pane, S32 x0, S32 y0, S32 x1, S32 y1,
                  COLORREF color, BOOL32 bSmooth)
{
    Check2D(pane);
    if (HiddenUnderToolbar(pane, y0, y1)) return;
    // DrawAgent's unlocked 3D path uses the same line for both smooth modes.
    (void)bSmooth;
    OrthoView(pane);
    DisableTextures();
    Require2D(BATCH->set_state(RPR_STATE_ID, 5U), "line batch state");
    PB.Color3ub(static_cast<U8>(color), static_cast<U8>(color >> 8U),
                static_cast<U8>(color >> 16U));
    const std::int64_t offsetX = pane ? pane->x0 : 0;
    const std::int64_t offsetY = pane ? pane->y0 : 0;
    PB.Begin(PB_LINES);
    PB.Vertex3f(static_cast<float>(x0 + offsetX) + 0.5F,
                static_cast<float>(y0 + offsetY) + 0.5F, 0.0F);
    PB.Vertex3f(static_cast<float>(x1 + offsetX) + 0.5F,
                static_cast<float>(y1 + offsetY) + 0.5F, 0.0F);
    PB.End();
    Require2D(BATCH->set_state(RPR_STATE_ID, 0U), "restore batch state");
}
void DA::DrawTexture(const PANE *pane, S32 x0, S32 y0, S32 x1, S32 y1,
                     U32 textureHandle, SINGLE u0, SINGLE v0, SINGLE u1, SINGLE v1)
{
    Check2D(pane);
    if (HiddenUnderToolbar(pane, y0, y1)) return;
    if (!textureHandle || x0 > x1 || y0 > y1 ||
        x1 == std::numeric_limits<S32>::max() || y1 == std::numeric_limits<S32>::max())
    {
        ICQImage::Bomb("native App texture quad has no valid texture/rectangle");
        std::abort();
    }
    // Original DrawAgent unlocked texture path: identical states, UV and quad order.
    Require2D(BATCH->set_state(RPR_BATCH, false), "texture batch state");
    OrthoView(pane);
    Require2D(BATCH->set_render_state(D3DRS_ALPHABLENDENABLE, FALSE), "texture blending");
    Require2D(BATCH->set_render_state(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA), "texture source blend");
    Require2D(BATCH->set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA), "texture destination blend");
    Require2D(BATCH->set_render_state(D3DRS_ZENABLE, false), "texture depth test");
    Require2D(BATCH->set_render_state(D3DRS_ZWRITEENABLE, false), "texture depth write");
    SetupDiffuseBlend(textureHandle, false);
    const std::int64_t offsetX = pane ? pane->x0 : 0;
    const std::int64_t offsetY = pane ? pane->y0 : 0;
    PB.Color4ub(255, 255, 255, 255);
    PB.Begin(PB_QUADS);
    PB.TexCoord2f(u0, v0); PB.Vertex3f(float(x0 + offsetX), float(y0 + offsetY), 0);
    PB.TexCoord2f(u1, v0); PB.Vertex3f(float(x1 + offsetX + 1), float(y0 + offsetY), 0);
    PB.TexCoord2f(u1, v1); PB.Vertex3f(float(x1 + offsetX + 1), float(y1 + offsetY + 1), 0);
    PB.TexCoord2f(u0, v1); PB.Vertex3f(float(x0 + offsetX), float(y1 + offsetY + 1), 0);
    PB.End();
    Require2D(BATCH->set_state(RPR_STATE_ID, 0), "texture restore batch state");
}
#endif
