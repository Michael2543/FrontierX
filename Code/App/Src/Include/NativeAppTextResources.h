#ifndef FRONTIERX_NATIVE_APP_TEXT_RESOURCES_H
#define FRONTIERX_NATIVE_APP_TEXT_RESOURCES_H

#if defined(_WIN32)
#error NativeAppTextResources.h is only for the Linux App shell
#endif

#include "DACOM.H"
#include <cstdint>
#include <cstddef>

struct IEventSystem;
struct IStatusBarResource;
struct IHintResource;
struct IScrollingText;
struct IDAConnectionPoint;

namespace FrontierX
{
IStatusBarResource *CreateNativeSourceStatus();
IHintResource *CreateNativeSourceHint();
IScrollingText *CreateNativeSourceScrollingText(bool skipNotificationCue);
bool CheckNativeSourceTextResources();
bool CheckNativeSourceStatus();
bool CheckNativeSourceHint();
bool CheckNativeFieldHint();
bool CheckNativeMouseScroll();
bool CheckNativeHotRect();
bool CheckNativeModalClose();
bool CheckNativeHotButtonCommands();
bool NativeSourceHintIdle();
bool NativeFieldHintIdle();
bool CheckNativeSourceScrollingText();
bool FormatNativeUnitSubtitle(U32 id, const wchar_t *name, wchar_t *output, std::size_t capacity);
std::uint64_t NativeScrollingTextDrawCount();
std::uint64_t NativeStatusDrawCount();
std::uint64_t NativeHintDrawCount();
U32 NativeHintDrawResource();

class NativeAppTextBinding
{
public:
    NativeAppTextBinding() = default;
    ~NativeAppTextBinding();
    NativeAppTextBinding(const NativeAppTextBinding &) = delete;
    NativeAppTextBinding &operator=(const NativeAppTextBinding &) = delete;

    bool Bind(IEventSystem *events, bool sourceOwners = false);
    bool BindScrollingText(bool skipNotificationCue);

private:
    bool sourceOwners_ = false;
    IStatusBarResource *status_ = nullptr;
    IHintResource *hint_ = nullptr;
    IDAConnectionPoint *events_ = nullptr;
    U32 statusToken_ = 0;
    U32 hintToken_ = 0;
    IScrollingText *scroll_ = nullptr;
};
}

#endif
