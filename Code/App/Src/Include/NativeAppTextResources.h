#ifndef FRONTIERX_NATIVE_APP_TEXT_RESOURCES_H
#define FRONTIERX_NATIVE_APP_TEXT_RESOURCES_H

#if defined(_WIN32)
#error NativeAppTextResources.h is only for the Linux App shell
#endif

#include "DACOM.H"

struct IEventSystem;
struct IStatusBarResource;
struct IHintResource;
struct IScrollingText;
struct IDAConnectionPoint;

namespace FrontierX
{
class NativeAppTextBinding
{
public:
    NativeAppTextBinding() = default;
    ~NativeAppTextBinding();
    NativeAppTextBinding(const NativeAppTextBinding &) = delete;
    NativeAppTextBinding &operator=(const NativeAppTextBinding &) = delete;

    bool Bind(IEventSystem *events);
    bool BindScrollingText(bool skipNotificationCue);

private:
    IStatusBarResource *status_ = nullptr;
    IHintResource *hint_ = nullptr;
    IDAConnectionPoint *events_ = nullptr;
    U32 statusToken_ = 0;
    U32 hintToken_ = 0;
    IScrollingText *scroll_ = nullptr;
};
}

#endif
