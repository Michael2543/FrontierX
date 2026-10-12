#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppFont.h"
#include "DrawAgent.h"
#include "GenData.h"
#include "MyVertex.h"
#include <VFX.H>
#include <TComponent.h>
#include <IRenderPrimitive.h>
#include <rendpipeline.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <map>
#include <string>
#include <vector>
#if defined(FRONTIERX_NATIVE_FREETYPE)
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

namespace
{
std::string fontFile;
std::uint64_t drawCount = 0;
unsigned liveFonts = 0;
[[noreturn]] void FontFailure(const char *reason)
{
    std::fprintf(stderr, "Native font rejected: %s\n", reason);
    std::abort();
}
#if defined(FRONTIERX_NATIVE_FREETYPE)
struct Span { int x0, x1, y; };
struct Glyph { int advance; std::vector<Span> spans; };
struct NativeFont : IFontDrawAgent
{
    BEGIN_DACOM_MAP_INBOUND(NativeFont)
    DACOM_INTERFACE_ENTRY(IFontDrawAgent)
    END_DACOM_MAP()
    FT_Library library = nullptr;
    FT_Face face = nullptr;
    U32 resource = 0;
    COLORREF color = 0;
    unsigned height = 0;
    bool counted = false;
    bool multiline = false;
    bool notScaling = false;
    IGeneralData *archetypeOwner = nullptr;
    PGENTYPE archetype = nullptr;
    int baseline = 0;
    std::map<wchar_t, Glyph> glyphs;
    ~NativeFont()
    {
        if (counted) --liveFonts;
        if (face) FT_Done_Face(face);
        if (library) FT_Done_FreeType(library);
        if (archetypeOwner)
        {
            archetypeOwner->Release(archetype);
            static_cast<IDAComponent *>(archetypeOwner)->Release();
        }
    }
    bool Init(U32 id, COLORREF pen)
    {
        resource = id; color = pen;
        // Match CQCreateLogFont's resource pixel height and IDEAL2REALY scaling.
        const wchar_t *descriptor = _localLoadStringW(id);
        const wchar_t *comma = descriptor ? std::wcschr(descriptor, L',') : nullptr;
        if (!comma || fontFile.empty()) return false;
        wchar_t *end = nullptr;
        const long pixels = std::wcstol(comma + 1, &end, 10);
        if (pixels < 1 || pixels > 64 || !end || *end != L',') return false;
        height = notScaling ? pixels : IDEAL2REALY(pixels);
        if (!height || height > 128 || FT_Init_FreeType(&library) ||
            FT_New_Face(library, fontFile.c_str(), 0, &face) ||
            FT_Select_Charmap(face, FT_ENCODING_UNICODE) ||
            FT_Set_Pixel_Sizes(face, 0, height)) return false;
        baseline = static_cast<int>(face->size->metrics.ascender >> 6);
        ++liveFonts; counted = true;
        return true;
    }
    Glyph &GetGlyph(wchar_t c)
    {
        if (c < 32 || c > 126) FontFailure("selected font path supports printable ASCII only");
        const auto found = glyphs.find(c);
        if (found != glyphs.end()) return found->second;
        if (!FT_Get_Char_Index(face, c) || FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_MONO))
            FontFailure("glyph unavailable");
        const auto &bitmap = face->glyph->bitmap;
        if (bitmap.width > 256 || bitmap.rows > 256 || bitmap.pitch < 0 ||
            (bitmap.width && bitmap.pixel_mode != FT_PIXEL_MODE_MONO)) FontFailure("bitmap format");
        Glyph glyph{static_cast<int>(face->glyph->advance.x >> 6), {}};
        if (glyph.advance < 0 || glyph.advance > 256) FontFailure("advance");
        for (unsigned row = 0; row < bitmap.rows; ++row)
        {
            const auto *bits = bitmap.buffer + row * bitmap.pitch;
            const auto on = [&](unsigned x) { return bits[x / 8] & (0x80U >> (x % 8)); };
            for (unsigned x = 0; x < bitmap.width;)
            {
                if (!on(x)) { ++x; continue; }
                const unsigned start = x++;
                while (x < bitmap.width && on(x)) ++x;
                glyph.spans.push_back({static_cast<int>(start) + face->glyph->bitmap_left,
                    static_cast<int>(x) + face->glyph->bitmap_left,
                    static_cast<int>(row) + baseline - face->glyph->bitmap_top});
            }
        }
        return glyphs.emplace(c, std::move(glyph)).first->second;
    }
    U32 COMAPI GetFontHeight() const override { return height; }
    U32 COMAPI GetStringWidth(const wchar_t *text) override
    {
        if (!text || std::wcslen(text) > 1024) FontFailure("string bounds");
        U32 width = 0;
        for (; *text; ++text)
        {
            if (multiline && (*text == L'\r' || *text == L'\n')) continue;
            width += GetGlyph(*text).advance;
        }
        return width;
    }
    U32 COMAPI GetStringWidth(U32 id) override { return GetStringWidth(_localLoadStringW(id)); }
    int COMAPI GetCharWidth(wchar_t c) override { return GetGlyph(c).advance; }
    void COMAPI SetFontColor(COLORREF fg, COLORREF) override { color = fg; }
    bool COMAPI IsMultiLine() override { return multiline; }
    // Preserve MultiLineFont.cpp's space/newline and 20-character lookback
    // rule. Oversize words still consume one glyph so narrow panes progress.
    std::vector<std::wstring> Lines(const wchar_t *text, int width)
    {
        GetStringWidth(text);
        if (!multiline) return {text};
        if (width < 1 || width > 32768) FontFailure("line width");
        std::vector<std::wstring> lines;
        while (*text)
        {
            int extent = 0, lastSpace = -1, length = 0;
            while (text[length] && extent <= width)
            {
                const wchar_t c = text[length];
                if (c == L'\r' || c == L'\n') break;
                if (c == L' ') lastSpace = length;
                extent += GetGlyph(c).advance;
                ++length;
            }
            if (extent > width)
            {
                if (lastSpace >= 0 && length-lastSpace < 20) length = lastSpace+1;
                else if (length > 1) --length;
            }
            lines.emplace_back(text, length);
            text += length;
            while (*text == L'\r' || *text == L'\n') ++text;
        }
        if (lines.empty()) lines.emplace_back();
        return lines;
    }
    int COMAPI GetNumRows(const wchar_t *text, PANE *pane) override
    {
        if (!multiline) { GetStringWidth(text); return 1; }
        if (!pane || pane->x0 < 0 || pane->x1 < pane->x0 || pane->x1 > 32768)
            FontFailure("row pane");
        return static_cast<int>(Lines(text, pane->x1-pane->x0+1).size());
    }
    GENRESULT COMAPI CreateDuplicate(IFontDrawAgent **out) override
    {
        if (!out) return GR_INVALID_PARMS;
        *out = FrontierX::CreateNativeFontDrawAgent(resource, color, multiline, notScaling);
        return *out ? GR_OK : GR_GENERIC;
    }
    void COMAPI StringDraw(PANE *pane, S32 x, S32 y, U32 id) override
    { StringDraw(pane, x, y, _localLoadStringW(id)); }
    void COMAPI StringDraw(PANE *pane, S32 x, S32 y, const wchar_t *text) override
    {
        GetStringWidth(text);
        if (!PIPE || !BATCH || CQFLAGS.bFrameLockEnabled || !SCREENRESX || !SCREENRESY ||
            SCREENRESX > 32768 || SCREENRESY > 32768 || x < -32768 || x > 32768 ||
            y < -32768 || y > 32768) FontFailure("render owner or coordinate bounds");
        if (pane && (pane->x0 < 0 || pane->y0 < 0 || pane->x1 < pane->x0 || pane->y1 < pane->y0 ||
            pane->x1 > 32768 || pane->y1 > 32768))
            FontFailure("pane bounds");
        const int left = pane ? pane->x0 : 0, top = pane ? pane->y0 : 0;
        const int right = pane ? pane->x1 : static_cast<int>(SCREENRESX)-1;
        const int bottom = pane ? pane->y1 : static_cast<int>(SCREENRESY)-1;
        std::vector<Span> visible;
        const auto lines = Lines(text, right-left+1);
        int rowY = top + y;
        for (const auto &line : lines)
        {
          int cursor = left + x;
          for (const wchar_t c : line)
          {
            const Glyph &glyph = GetGlyph(c);
            for (const auto &span : glyph.spans)
            {
                Span clipped{std::max(left, cursor + span.x0), std::min(right + 1, cursor + span.x1), rowY + span.y};
                if (clipped.x1 > clipped.x0 && clipped.y >= top && clipped.y <= bottom)
                    visible.push_back(clipped);
            }
            cursor += glyph.advance;
          }
          rowY += height;
        }
        if (visible.empty()) return;
        OrthoView(nullptr); // Explicit pixel clipping preserves the source reveal pane.
        DisableTextures();
        if (BATCH->set_state(RPR_STATE_ID, 6U) != GR_OK) FontFailure("batch state");
        PB.Color3ub(static_cast<U8>(color), static_cast<U8>(color >> 8), static_cast<U8>(color >> 16));
        PB.Begin(PB_QUADS);
        for (const auto &span : visible)
        {
            PB.Vertex3f(span.x0, span.y, 0); PB.Vertex3f(span.x1, span.y, 0);
            PB.Vertex3f(span.x1, span.y+1, 0); PB.Vertex3f(span.x0, span.y+1, 0);
        }
        PB.End();
        if (BATCH->set_state(RPR_STATE_ID, 0U) != GR_OK) FontFailure("restore batch state");
        ++drawCount;
    }
};
#endif
}
namespace FrontierX
{
bool SetNativeFontFile(const char *path)
{
#if defined(FRONTIERX_NATIVE_FREETYPE)
    if (!path || !*path) return false;
    FT_Library library = nullptr;
    FT_Face face = nullptr;
    if (FT_Init_FreeType(&library)) return false;
    const bool valid = FT_New_Face(library, path, 0, &face) == 0;
    if (face) FT_Done_Face(face);
    FT_Done_FreeType(library);
    if (valid) fontFile = path;
    return valid;
#else
    (void)path; return false;
#endif
}
IFontDrawAgent *CreateNativeFontDrawAgent(U32 resource, COLORREF color, bool multiline, bool notScaling)
{
#if defined(FRONTIERX_NATIVE_FREETYPE)
    auto *font = new DAComponent<NativeFont>;
    font->multiline = multiline;
    font->notScaling = notScaling;
    if (!font->Init(resource, color)) { font->Release(); return nullptr; }
    return font;
#else
    (void)resource; (void)color; (void)multiline; (void)notScaling; return nullptr;
#endif
}
IFontDrawAgent *CreateNativeArchetypeFont(PGENTYPE archetype, U32 resource, COLORREF color,
                                         bool multiline, bool notScaling)
{
#if defined(FRONTIERX_NATIVE_FREETYPE)
    if (!GENDATA || !archetype) return nullptr;
    auto *font = static_cast<NativeFont *>(CreateNativeFontDrawAgent(resource, color, multiline, notScaling));
    if (!font) return nullptr;
    font->archetype = archetype;
    font->archetypeOwner = GENDATA;
    static_cast<IDAComponent *>(GENDATA)->AddRef();
    // GenData::CreateInstance adds the archetype usage after factory success.
    return font;
#else
    (void)archetype; (void)resource; (void)color; (void)multiline; (void)notScaling;
    return nullptr;
#endif
}
std::uint64_t NativeFontDrawCount() { return drawCount; }
unsigned NativeFontLiveCount() { return liveFonts; }
}
