#ifndef FRONTIERX_LEGACY_EVENT_DISPATCH_H
#define FRONTIERX_LEGACY_EVENT_DISPATCH_H

#include "EventSys.h"

#include <cstdint>

namespace FrontierX
{
inline void DispatchLegacyInputToEventSystem(void * context, std::uint32_t message,
                                             void * parameter) noexcept
{
    if (context)
        static_cast<IEventSystem *>(context)->Send(message, parameter);
}
}

#endif
