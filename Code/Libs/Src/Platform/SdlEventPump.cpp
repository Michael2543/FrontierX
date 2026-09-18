#include "FrontierX/SdlEventPump.h"

#include "FrontierX/LegacyEventDispatch.h"
#include "FrontierX/LegacyInput.h"
#include "FrontierX/SdlWindow.h"

namespace FrontierX
{
SdlEventPump::SdlEventPump(SdlWindow & window, IEventSystem & event_system)
    : window_(window), event_system_(event_system)
{
}

SdlEventPumpResult SdlEventPump::Pump()
{
    SdlEventPumpResult result;
    WindowEvent event;
    while (window_.PollEvent(event))
    {
        ++result.event_count;
        if (event.type == WindowEventType::CloseRequested) result.close_requested = true;
        if (event.type == WindowEventType::QuitRequested) result.quit_requested = true;
        adapter_.Dispatch(event, DispatchLegacyInputToEventSystem, &event_system_);
    }
    return result;
}
}
