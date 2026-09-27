#ifndef FRONTIERX_NATIVE_WINDOW_MANAGER_H
#define FRONTIERX_NATIVE_WINDOW_MANAGER_H

#include "FrontierX/LegacyMessage.h"

struct ICOManager;

namespace FrontierX
{
bool HasNativeWindowMessageTarget(LegacyWindowToken window);
bool DispatchNativeWindowMessage(LegacyWindowToken window, LegacyMessageId message,
                                 LegacyWParam wparam, LegacyLParam lparam);
void RegisterNativeWindowManager(ICOManager * manager);
}

#endif
