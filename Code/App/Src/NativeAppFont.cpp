#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppFont.h"
#include "DrawAgent.h"
#include "GenData.h"
#include "resource.h"
#include "MyVertex.h"
#include <VFX.H>
#include <TComponent.h>
#include <IRenderPrimitive.h>
#include <rendpipeline.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#if defined(FRONTIERX_NATIVE_FREETYPE)
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SFNT_NAMES_H
#endif

namespace
{
std::string fontFile;
std::map<std::pair<std::wstring, long>, std::string> mappedFonts;
std::uint64_t drawCount = 0;
unsigned liveFonts = 0;
[[noreturn]] void FontFailure(const char *reason)
{
    std::fprintf(stderr, "Native font rejected: %s\n", reason);
    std::abort();
}
#if defined(FRONTIERX_NATIVE_FREETYPE)
std::wstring NormalizeFamily(const std::wstring &family)
{
    std::wstring normalized;
    for (wchar_t c : family)
    {
        if (c >= L'A' && c <= L'Z') c += L'a' - L'A';
        if ((c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9')) normalized += c;
    }
    return normalized;
}
std::wstring NormalizeFamily(const char *family)
{
    std::wstring wide;
    if (family) while (*family) wide += static_cast<unsigned char>(*family++);
    return NormalizeFamily(wide);
}
#if defined(FRONTIERX_NATIVE_FREETYPE)
bool MatchesFontFamily(FT_Face face, const std::wstring &family)
{
    if (NormalizeFamily(face->family_name) == family ||
        NormalizeFamily(FT_Get_Postscript_Name(face)) == family) return true;
    // OpenType can group Arial Narrow under typographic family Arial while
    // preserving its original GDI family in nameID1. Validate that authored
    // family too; do not infer a family from filenames or strip style suffixes.
    const FT_UInt count = FT_Get_Sfnt_Name_Count(face);
    for (FT_UInt i = 0; i < count; ++i)
    {
        FT_SfntName name = {};
        if (FT_Get_Sfnt_Name(face, i, &name) || name.name_id != 1 ||
            (name.platform_id != 0 && !(name.platform_id == 3 &&
                (name.encoding_id == 1 || name.encoding_id == 10))) ||
            !name.string || !name.string_len || name.string_len > 512 || name.string_len % 2)
            continue;
        std::wstring legacyFamily;
        bool valid = true;
        for (FT_UInt offset = 0; offset < name.string_len; offset += 2)
        {
            const unsigned character = (unsigned(name.string[offset]) << 8) | name.string[offset+1];
            if (!character) { valid = false; break; }
            legacyFamily.push_back(static_cast<wchar_t>(character));
        }
        if (valid && NormalizeFamily(legacyFamily) == family) return true;
    }
    return false;
}
#endif
std::string Trim(std::string text)
{
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    return text.substr(first, text.find_last_not_of(" \t\r\n") - first + 1);
}
struct Span { int x0, x1, y; };
struct Glyph { int advance; std::vector<Span> spans; };
bool IsFontTextCharacter(wchar_t c)
{
    return (c >= 32 && c <= 126) || c == 0xa9 || c == 0x2122 || c == 0x2026 || c == L'\r' || c == L'\n';
}
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
    unsigned authoredHeight = 0;
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
        if (!comma || (fontFile.empty() && mappedFonts.empty())) return false;
        wchar_t *end = nullptr;
        const long pixels = std::wcstol(comma + 1, &end, 10);
        if (pixels < 1 || pixels > 128 || !end) return false;
        long weight = 400;
        if (*end == L',')
        {
            const wchar_t *number = end + 1;
            weight = std::wcstol(number, &end, 10);
            if (end == number || weight < 1 || weight > 1000) return false;
        }
        while (*end == L' ' || *end == L'\t') ++end;
        if (*end) return false;
        std::string selectedFile = fontFile;
        if (!mappedFonts.empty())
        {
            const auto key = std::make_pair(NormalizeFamily(std::wstring(descriptor, comma)), weight);
            const auto selected = mappedFonts.find(key);
            if (selected == mappedFonts.end())
            {
                std::fprintf(stderr, "Native font has no strict family/weight mapping for resource %u\n", id);
                return false;
            }
            selectedFile = selected->second;
        }
        authoredHeight = pixels;
        height = notScaling ? pixels : IDEAL2REALY(pixels);
        if (!height || height > 256 || FT_Init_FreeType(&library) ||
            FT_New_Face(library, selectedFile.c_str(), 0, &face) ||
            FT_Select_Charmap(face, FT_ENCODING_UNICODE) ||
            FT_Set_Pixel_Sizes(face, 0, height)) return false;
        baseline = static_cast<int>(face->size->metrics.ascender >> 6);
        ++liveFonts; counted = true;
        return true;
    }
    void EnsureScale()
    {
        const unsigned next=notScaling ? authoredHeight : IDEAL2REALY(authoredHeight);
        if (next==height) return;
        if (!next || next>256 || FT_Set_Pixel_Sizes(face,0,next)) FontFailure("resized font height");
        height=next; baseline=static_cast<int>(face->size->metrics.ascender>>6);
        glyphs.clear();
    }
    Glyph &GetGlyph(wchar_t c)
    {
        EnsureScale();
        if (!IsFontTextCharacter(c)) FontFailure("selected font path supports printable ASCII, copyright, trademark, ellipsis and CR/LF only");
        // Single-line agents keep one baseline. Only the multiline owner below
        // interprets CR/LF as line separators; neither path rasterizes them.
        if (c == L'\r' || c == L'\n')
        {
            static Glyph separator{0, {}};
            return separator;
        }
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
    U32 COMAPI GetFontHeight() const override { const_cast<NativeFont *>(this)->EnsureScale(); return height; }
    U32 COMAPI GetStringWidth(const wchar_t *text) override
    {
        if (!text || std::wcslen(text) > 1024) FontFailure("string bounds");
        U32 width = 0;
        for (; *text; ++text)
        {
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
        if (!pane || pane->x0 < -32768 || pane->x1 < pane->x0 || pane->x1 > 32768)
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
    std::vector<Span> VisibleSpans(PANE *pane, S32 x, S32 y, const wchar_t *text)
    {
        const int left = pane ? pane->x0 : 0, top = pane ? pane->y0 : 0;
        const int right = pane ? pane->x1 : static_cast<int>(SCREENRESX)-1;
        const int bottom = pane ? pane->y1 : static_cast<int>(SCREENRESY)-1;
        // Legacy static controls can center text partly outside the window.
        // Preserve their origin and wrapping, then clip pixels to the viewport.
        const int clipLeft = std::max(0, left), clipTop = std::max(0, top);
        const int clipRight = std::min(right, static_cast<int>(SCREENRESX)-1);
        const int clipBottom = std::min(bottom, static_cast<int>(SCREENRESY)-1);
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
                Span clipped{std::max(clipLeft, cursor + span.x0), std::min(clipRight + 1, cursor + span.x1), rowY + span.y};
                if (clipped.x1 > clipped.x0 && clipped.y >= clipTop && clipped.y <= clipBottom)
                    visible.push_back(clipped);
            }
            cursor += glyph.advance;
          }
          rowY += height;
        }
        return visible;
    }
    void COMAPI StringDraw(PANE *pane, S32 x, S32 y, const wchar_t *text) override
    {
        GetStringWidth(text);
        if (!PIPE || !BATCH || CQFLAGS.bFrameLockEnabled || !SCREENRESX || !SCREENRESY ||
            SCREENRESX > 32768 || SCREENRESY > 32768 || x < -32768 || x > 32768 ||
            y < -32768 || y > 32768) FontFailure("render owner or coordinate bounds");
        if (pane && (pane->x0 < -32768 || pane->y0 < -32768 || pane->x1 < pane->x0 || pane->y1 < pane->y0 ||
            pane->x1 > 32768 || pane->y1 > 32768))
            FontFailure("pane bounds");
        const auto visible = VisibleSpans(pane, x, y, text);
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
    if (valid) { fontFile = path; mappedFonts.clear(); }
    return valid;
#else
    (void)path; return false;
#endif
}
bool SetNativeFontMapping(const char *path, std::string &error)
{
#if defined(FRONTIERX_NATIVE_FREETYPE)
    error.clear();
    if (!path || !*path || liveFonts) { error = "Font mapping requires startup before live fonts"; return false; }
    std::ifstream input(path);
    if (!input) { error = "External font mapping cannot be read"; return false; }
    FT_Library library = nullptr;
    if (FT_Init_FreeType(&library)) { error = "FreeType initialization failed"; return false; }
    struct LibraryGuard { FT_Library library; ~LibraryGuard() { FT_Done_FreeType(library); } } guard{library};
    std::map<std::pair<std::wstring, long>, std::string> pending;
    std::string line;
    while (std::getline(input, line))
    {
        line = Trim(line);
        if (line.empty() || line.front() == '#') continue;
        if (line.size() > 4096 || pending.size() >= 64)
        { error = "External font mapping exceeds bounds"; return false; }
        const auto equals = line.find('=');
        const auto comma = line.find(',');
        if (equals == std::string::npos || comma == std::string::npos || comma >= equals)
        { error = "Expected font mapping Family,weight=filename"; return false; }
        const std::string family = Trim(line.substr(0, comma));
        const std::wstring normalized = NormalizeFamily(family.c_str());
        const std::string number = Trim(line.substr(comma + 1, equals - comma - 1));
        char *end = nullptr;
        const long weight = std::strtol(number.c_str(), &end, 10);
        if (normalized.empty() || end == number.c_str() || *end || weight < 1 || weight > 1000)
        { error = "Invalid font mapping family or weight"; return false; }
        std::filesystem::path facePath = Trim(line.substr(equals + 1));
        if (facePath.empty()) { error = "Empty font mapping filename"; return false; }
        if (facePath.is_relative()) facePath = std::filesystem::path(path).parent_path() / facePath;
        FT_Face face = nullptr;
        if (FT_New_Face(library, facePath.c_str(), 0, &face))
        { error = "Mapped external font cannot be opened"; return false; }
        const bool matches = MatchesFontFamily(face, normalized);
        const bool bold = (face->style_flags & FT_STYLE_FLAG_BOLD) != 0;
        // The retail font installation supplies only this Bauhaus face; GDI
        // resolves its requested weights to that same available family face.
        const bool styleMatches = normalized == L"bauhausmditctt" ||
            bold == (weight >= 600);
        FT_Done_Face(face);
        if (!matches) { error = "Mapped face does not match the original font family"; return false; }
        if (!styleMatches) { error = "Mapped face does not match the requested font weight"; return false; }
        if (!pending.emplace(std::make_pair(normalized, weight), facePath.string()).second)
        { error = "Duplicate font family/weight mapping"; return false; }
    }
    if (!input.eof() || pending.empty()) { error = "Incomplete external font mapping"; return false; }
    mappedFonts.swap(pending);
    fontFile.clear();
    return true;
#else
    (void)path;
    error = "Native fonts were not built";
    return false;
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
bool CheckNativeFontTextLayout(IFontDrawAgent *agent, U32 textResource)
{
#if defined(FRONTIERX_NATIVE_FREETYPE)
    auto *font = dynamic_cast<NativeFont *>(agent);
    if (!font) return false;
    const bool previousMultiline = font->multiline;
    struct RestoreMode
    {
        NativeFont *font;
        bool multiline;
        ~RestoreMode() { font->multiline = multiline; }
    } restore{font, previousMultiline};
    PANE pane{};
    pane.x1 = 1023;
    pane.y1 = 511;
    const auto sameSpans = [](const auto &a, const auto &b)
    {
        if (a.size() != b.size()) return false;
        for (std::size_t i = 0; i < a.size(); ++i)
            if (a[i].x0 != b[i].x0 || a[i].x1 != b[i].x1 || a[i].y != b[i].y) return false;
        return true;
    };
    font->multiline = false;
    bool valid = font->GetCharWidth(L'\r') == 0 && font->GetCharWidth(L'\n') == 0 &&
        !IsFontTextCharacter(L'\t') && !IsFontTextCharacter(1) && !IsFontTextCharacter(127) &&
        !IsFontTextCharacter(0x100);
    struct TextCase { const wchar_t *text; const wchar_t *visible; };
    const TextCase cases[] = {{L"AB\n", L"AB"}, {L"A\r\nB", L"AB"},
        {L"A\rB", L"AB"}, {L"A\nB", L"AB"}, {L"\r\nAB\r\n", L"AB"}, {L"\r\n", L""}};
    for (const auto &item : cases)
        valid = valid && font->GetStringWidth(item.text) == font->GetStringWidth(item.visible) &&
            font->GetNumRows(item.text, nullptr) == 1 &&
            sameSpans(font->VisibleSpans(&pane, 0, 0, item.text),
                font->VisibleSpans(&pane, 0, 0, item.visible));
    // Original in-game options uses APP_NAMETM for its title. Its source
    // resource must retain the trademark, not UTF-8 replacement bytes.
    const wchar_t *title = _localLoadStringW(IDS_APP_NAMETM);
    valid = valid && std::wcscmp(title, L"Conquest: Frontier Wars\u2122") == 0 &&
        font->GetStringWidth(title) > font->GetStringWidth(L"Conquest: Frontier Wars") &&
        !font->VisibleSpans(&pane, 0, 0, L"\u2122").empty();
    valid = valid && _localLoadStringW(IDS_LEGAL_MAINSCREEN)[0] == L'\u00a9' &&
        _localLoadStringW(IDS_LEGAL_ABOUTBOX)[0] == L'\u00a9' &&
        !font->VisibleSpans(&pane, 0, 0, L"\u00a9").empty();
    // The original SOL-entry news subtitle contains an authored ellipsis.
    // Rasterize the actual glyph, preserving its metrics instead of replacing
    // it with three ASCII dots or suppressing the subtitle.
    const auto &ellipsis = font->GetGlyph(L'\u2026');
    valid = valid && ellipsis.advance > 0 && !ellipsis.spans.empty() &&
        font->GetStringWidth(L"A\u2026B") ==
            font->GetStringWidth(L"AB") + static_cast<U32>(ellipsis.advance) &&
        !font->VisibleSpans(&pane, 0, 0, L"A\u2026B").empty();
    // The original StatusBar passes the authored ID straight to the agent.
    // Its Rally Point hint has a trailing LF. Use that resource, not a patched
    // replacement string or a production special case for its numeric ID.
    const wchar_t *authored = _localLoadStringW(textResource);
    const auto length = std::wcslen(authored);
    if (!length || authored[length - 1] != L'\n') return false;
    const std::wstring withoutTerminator(authored, length - 1);
    valid = valid && font->GetStringWidth(textResource) == font->GetStringWidth(withoutTerminator.c_str()) &&
        sameSpans(font->VisibleSpans(&pane, 0, 0, authored),
            font->VisibleSpans(&pane, 0, 0, withoutTerminator.c_str()));
    font->multiline = true;
    valid = valid && font->GetNumRows(L"A\u2026\r\nB", &pane) == 2 &&
        font->GetStringWidth(L"A\u2026\r\nB") == font->GetStringWidth(L"A\u2026B");
    auto rows = font->VisibleSpans(&pane, 0, 0, L"A");
    const auto second = font->VisibleSpans(&pane, 0, font->GetFontHeight(), L"B");
    rows.insert(rows.end(), second.begin(), second.end());
    valid = valid && font->GetStringWidth(L"A\r\nB") == font->GetStringWidth(L"AB") &&
        font->GetNumRows(L"A\r\nB", &pane) == 2 &&
        font->GetNumRows(L"A\r\n\r\nB", &pane) == 2 &&
        font->GetNumRows(L"AB\r\n", &pane) == 1 &&
        sameSpans(font->VisibleSpans(&pane, 0, 0, L"A\r\nB"), rows) &&
        sameSpans(font->VisibleSpans(&pane, 0, 0, L"A\r\n\r\nB"), rows) &&
        sameSpans(font->VisibleSpans(&pane, 0, 0, L"AB\r\n"), font->VisibleSpans(&pane, 0, 0, L"AB"));
    // Regression: Static::draw may supply a negative origin when a resource
    // counter's centered font is taller than its resized control rectangle.
    font->multiline = false;
    auto expectedClipped = font->VisibleSpans(&pane, 0, 0, L"37/45");
    for (auto &span : expectedClipped)
    {
        span.x0 = std::max(0, span.x0-4);
        span.x1 -= 4;
        span.y -= 4;
    }
    expectedClipped.erase(std::remove_if(expectedClipped.begin(), expectedClipped.end(),
        [](const Span &span) { return span.y<0 || span.x1<=span.x0; }), expectedClipped.end());
    PANE clippedPane = pane;
    clippedPane.x0 = clippedPane.y0 = -4;
    valid = valid && !expectedClipped.empty() &&
        sameSpans(font->VisibleSpans(&clippedPane, 0, 0, L"37/45"), expectedClipped) &&
        sameSpans(font->VisibleSpans(&clippedPane, 4, 4, L"37/45"),
                  font->VisibleSpans(&pane, 0, 0, L"37/45"));
    return valid;
#else
    (void)agent; (void)textResource;
    return false;
#endif
}
}
