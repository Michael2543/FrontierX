#ifndef FRONTIERX_SDL_EVENT_PUMP_H
#define FRONTIERX_SDL_EVENT_PUMP_H

#include "FrontierX/LegacyInput.h"

#include <cstddef>

struct IEventSystem;

namespace FrontierX
{
class SdlWindow;

struct SdlEventPumpResult
{
    std::size_t event_count = 0;
    bool close_requested = false;
    bool quit_requested = false;
};

class SdlEventPump
{
public:
    SdlEventPump(SdlWindow & window, IEventSystem & event_system);
    SdlEventPump(const SdlEventPump &) = delete;
    SdlEventPump & operator=(const SdlEventPump &) = delete;

    SdlEventPumpResult Pump();

private:
    SdlWindow & window_;
    IEventSystem & event_system_;
    SdlLegacyInputAdapter adapter_;
};
}

#endif
