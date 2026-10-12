#include "Include/NativeAppTextResources.h"
#include "Include/NativeAppGlobals.h"

#include "StatusBar.h"
#include "Hintbox.h"
#include "ScrollingText.h"
#include "TResource.h"
#include "resource.h"
#include "EventSys.h"
#include "TConnContainer.h"

#include <TComponent.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cwchar>
#include <string>

IStatusBarResource *STATUS = nullptr;
IHintResource *HINTBOX = nullptr;
IScrollingText *SCROLLTEXT = nullptr;

namespace
{
void AppendUtf8(std::string &result, std::uint32_t value)
{
    if (value <= 0x7f) result.push_back(static_cast<char>(value));
    else if (value <= 0x7ff)
    {
        result.push_back(static_cast<char>(0xc0 | (value >> 6)));
        result.push_back(static_cast<char>(0x80 | (value & 0x3f)));
    }
    else if (value >= 0xd800 && value <= 0xdfff)
    {
        std::fputs("Native App text contains a surrogate code point\n", stderr);
        std::abort();
    }
    else if (value <= 0xffff)
    {
        result.push_back(static_cast<char>(0xe0 | (value >> 12)));
        result.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3f)));
        result.push_back(static_cast<char>(0x80 | (value & 0x3f)));
    }
    else if (value <= 0x10ffff)
    {
        result.push_back(static_cast<char>(0xf0 | (value >> 18)));
        result.push_back(static_cast<char>(0x80 | ((value >> 12) & 0x3f)));
        result.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3f)));
        result.push_back(static_cast<char>(0x80 | (value & 0x3f)));
    }
    else
    {
        std::fputs("Native App text contains an invalid code point\n", stderr);
        std::abort();
    }
}

void PrintText(const char *kind, const wchar_t *text)
{
    if (!text || !*text) return;
    std::string utf8;
    for (const wchar_t *cursor = text; *cursor; ++cursor)
        AppendUtf8(utf8, static_cast<std::uint32_t>(*cursor));
    std::fprintf(stderr, "Native App %s: %s\n", kind, utf8.c_str());
}

struct NativeStatus : Resource<NativeStatus, IStatusBarResource>,
                            ConnectionPointContainer<NativeStatus>, IEventCallback
{
    virtual ~NativeStatus() = default;

    BEGIN_DACOM_MAP_INBOUND(NativeStatus)
    DACOM_INTERFACE_ENTRY(IResource)
    DACOM_INTERFACE_ENTRY(IStatusBarResource)
    DACOM_INTERFACE_ENTRY(IEventCallback)
    DACOM_INTERFACE_ENTRY(IDAConnectionPointContainer)
    DACOM_INTERFACE_ENTRY2(IID_IDAConnectionPointContainer, IDAConnectionPointContainer)
    END_DACOM_MAP()

    GENRESULT SetText(U32 id, STATUSTEXTMODE mode) override
    {
        textId = id;
        textMode = mode;
        text.clear();
        if (id) PrintText("status", _localLoadStringW(id));
        return GR_OK;
    }

    GENRESULT SetTextString(const wchar_t *value, STATUSTEXTMODE mode) override
    {
        textId = 0;
        textMode = mode;
        text.assign(value ? value : L"", value ? std::min<std::size_t>(std::wcslen(value), 255) : 0);
        PrintText("status", text.c_str());
        return GR_OK;
    }

    GENRESULT SetTextString2(const wchar_t *name, const wchar_t *value) override
    {
        textId = 0;
        textMode = STM_DEFAULT_NAME;
        text = name ? name : L"";
        text += L": ";
        text += value ? value : L"";
        if (text.size() > 255) text.resize(255);
        PrintText("status", text.c_str());
        return GR_OK;
    }

    U32 GetText() override { return textId; }
    U32 GetHeight() override { return 0; }
    void SetToolbarHeight(U32 height) override { toolbarHeight = height; }
    GENRESULT Notify(U32 message, void *) override
    {
        if (message == CQE_UPDATE) updateResource();
        return GR_OK;
    }
    void OnNoOwner() { SetText(0, STM_DEFAULT); }

    U32 textId = 0;
    U32 toolbarHeight = 0;
    STATUSTEXTMODE textMode = STM_DEFAULT;
    std::wstring text;
};

struct NativeHint : Resource<NativeHint, IHintResource>,
                          ConnectionPointContainer<NativeHint>, IEventCallback
{
    virtual ~NativeHint() = default;

    BEGIN_DACOM_MAP_INBOUND(NativeHint)
    DACOM_INTERFACE_ENTRY(IResource)
    DACOM_INTERFACE_ENTRY(IHintResource)
    DACOM_INTERFACE_ENTRY(IEventCallback)
    DACOM_INTERFACE_ENTRY(IDAConnectionPointContainer)
    DACOM_INTERFACE_ENTRY2(IID_IDAConnectionPointContainer, IDAConnectionPointContainer)
    END_DACOM_MAP()

    void SetText(U32 id) override
    {
        textId = id;
        text.clear();
        if (id) PrintText("hint", _localLoadStringW(id));
    }
    void SetTextString(const wchar_t *value) override
    {
        textId = 0;
        text = value ? value : L"";
        if (text.size() > 255) text.resize(255);
        PrintText("hint", text.c_str());
    }
    U32 GetText() override { return textId; }
    U32 GetHeight() override { return 0; }
    void SetToolbarHeight(U32 height) override { toolbarHeight = height; }
    GENRESULT Notify(U32 message, void *) override
    {
        if (message == CQE_UPDATE) updateResource();
        return GR_OK;
    }
    void OnNoOwner() { SetText(0); }

    U32 textId = 0;
    U32 toolbarHeight = 0;
    std::wstring text;
};

struct NativeScrollingText : IScrollingText
{
    virtual ~NativeScrollingText() = default;

    BEGIN_DACOM_MAP_INBOUND(NativeScrollingText)
    DACOM_INTERFACE_ENTRY(IScrollingText)
    END_DACOM_MAP()

    void Redraw() override
    {
        if (!overlayWarningEmitted)
        {
            std::fputs("Native scrolling-text window overlay unavailable; messages are terminal-only\n", stderr);
            overlayWarningEmitted = true;
        }
    }
    GENRESULT SetText(U32 id) override
    {
        if (!id) return GR_INVALID_PARMS;
        RequireCuePolicy();
        PrintText("message", _localLoadStringW(id));
        return GR_OK;
    }
    GENRESULT SetTextString(const wchar_t *value) override
    {
        if (value && *value)
        {
            RequireCuePolicy();
            PrintText("message", value);
        }
        return GR_OK;
    }
    GENRESULT SetTextStringEx(const wchar_t *, U32, U32) override
    {
        std::fputs("Native player-attributed scrolling messages are unsupported\n", stderr);
        std::abort();
    }
    U32 GetText() override { return 0; }
    U32 GetHeight() override { return 0; }
    void SetToolbarHeight(U32 height) override { toolbarHeight = height; }

    void RequireCuePolicy() const
    {
        if (!cueSkipped)
        {
            std::fputs("Native scrolling-text notification cue requires a sound owner or explicit --skip-notification-cue\n", stderr);
            std::abort();
        }
    }

    bool cueSkipped = false;
    bool overlayWarningEmitted = false;
    U32 toolbarHeight = 0;
};
}

namespace FrontierX
{
bool FormatNativeUnitSubtitle(U32 id, const wchar_t *name, wchar_t *output, std::size_t capacity)
{
    if (!name || !output || !capacity) return false;
    const wchar_t *format = _localLoadStringW(id);
    const wchar_t *slot = format ? std::wcsstr(format, L"%s") : nullptr;
    if (!slot || std::wcschr(slot + 2, L'%')) return false;
    for (auto *cursor = format; cursor < slot; ++cursor)
        if (*cursor == L'%') return false;
    std::wstring value(format, slot);
    value += name;
    value += slot + 2;
    if (value.size() >= capacity) return false;
    std::wmemcpy(output, value.c_str(), value.size() + 1);
    return true;
}
#if defined(FRONTIERX_NATIVE_SOURCE_TEXT)
bool CheckNativeSourceTextResources()
{
    wchar_t buffer[256];
    wchar_t unchanged[2] = {L'X', 0};
    const bool status = CheckNativeSourceStatus();
    const bool hint = CheckNativeSourceHint();
    const bool scrolling = CheckNativeSourceScrollingText();
    const bool formatting = FormatNativeUnitSubtitle(IDS_SUB_MOVE, L"Forger", buffer, 256) &&
        std::wcscmp(buffer, L"Forger: Yes Sir.") == 0 &&
        !FormatNativeUnitSubtitle(IDS_SUB_MOVE, L"Forger", unchanged, 2) && unchanged[0] == L'X';
    if (!status || !hint || !scrolling || !formatting)
        std::fprintf(stderr, "Native text lifecycle: status=%d hint=%d scrolling=%d formatting=%d\n",
            status, hint, scrolling, formatting);
    return status && hint && scrolling && formatting;
}
#endif
NativeAppTextBinding::~NativeAppTextBinding()
{
    const bool ownScroll = SCROLLTEXT == scroll_;
    if (scroll_) scroll_->Release();
    if (ownScroll) SCROLLTEXT = nullptr;
    if (events_)
    {
        if (hintToken_) events_->Unadvise(hintToken_);
        if (statusToken_) events_->Unadvise(statusToken_);
        events_->Release();
    }
    const bool ownHint = HINTBOX == hint_;
    const bool ownStatus = STATUS == status_;
    if (hint_) hint_->Release();
    if (status_) status_->Release();
    if (ownHint) HINTBOX = nullptr;
    if (ownStatus) STATUS = nullptr;
}

bool NativeAppTextBinding::BindScrollingText(bool skipNotificationCue)
{
    if (SCROLLTEXT || scroll_ || !status_ || !hint_)
        return false;
#if defined(FRONTIERX_NATIVE_SOURCE_TEXT)
    if (sourceOwners_) scroll_ = CreateNativeSourceScrollingText(skipNotificationCue);
    else
#endif
    {
        auto *scroll = new DAComponent<NativeScrollingText>;
        scroll->cueSkipped = skipNotificationCue;
        scroll_ = scroll;
    }
    if (!scroll_) return false;
    SCROLLTEXT = scroll_;
    return true;
}

bool NativeAppTextBinding::Bind(IEventSystem *events, bool sourceOwners)
{
    if (!events || STATUS || HINTBOX || status_ || hint_) return false;
    IStatusBarResource *status = nullptr;
    IHintResource *hint = nullptr;
#if defined(FRONTIERX_NATIVE_SOURCE_TEXT)
    if (sourceOwners)
    {
        status = CreateNativeSourceStatus();
        hint = CreateNativeSourceHint();
    }
    else
#else
    if (sourceOwners) return false;
#endif
    {
        status = new DAComponent<NativeStatus>;
        hint = new DAComponent<NativeHint>;
    }
    IEventCallback *statusCallback = nullptr;
    IEventCallback *hintCallback = nullptr;
    IDAConnectionPoint *point = nullptr;
    U32 statusToken = 0;
    U32 hintToken = 0;
    const bool okay = events->QueryOutgoingInterface("IEventCallback", &point) == GR_OK &&
        point && status && hint &&
        status->QueryInterface("IEventCallback", reinterpret_cast<void **>(&statusCallback)) == GR_OK && statusCallback &&
        hint->QueryInterface("IEventCallback", reinterpret_cast<void **>(&hintCallback)) == GR_OK && hintCallback &&
        point->Advise(statusCallback, &statusToken) == GR_OK && statusToken &&
        point->Advise(hintCallback, &hintToken) == GR_OK && hintToken;
    if (statusCallback) statusCallback->Release();
    if (hintCallback) hintCallback->Release();
    if (!okay)
    {
        if (point)
        {
            if (hintToken) point->Unadvise(hintToken);
            if (statusToken) point->Unadvise(statusToken);
            point->Release();
        }
        if (hint) hint->Release();
        if (status) status->Release();
        return false;
    }
    sourceOwners_ = sourceOwners;
    status_ = status;
    hint_ = hint;
    events_ = point;
    statusToken_ = statusToken;
    hintToken_ = hintToken;
    STATUS = status;
    HINTBOX = hint;
    return true;
}
}
