#ifndef FRONTIERX_NATIVE_WINDOW_MANAGER_H
#define FRONTIERX_NATIVE_WINDOW_MANAGER_H

#include "FrontierX/LegacyMessage.h"
#include <vector>
#include <cstdint>
struct IWindowManager;

struct ICOManager;

namespace FrontierX
{
bool SetNativeWindowDiagnosticInput(IWindowManager *manager,bool enabled);
struct NativeDisplayMode { std::uint32_t width, height; };
struct NativeWindowMode
{
    bool fullscreen = false;
    std::uint32_t width = 800, height = 600; // Restored windowed client size.
};
bool GetNativeWindowMode(IWindowManager *manager, NativeWindowMode &mode);
bool SetNativeWindowMode(IWindowManager *manager, const NativeWindowMode &mode);
bool SetNativeWindowMinimumSize(IWindowManager *manager,std::uint32_t width,std::uint32_t height);
std::vector<NativeDisplayMode> GetNativeWindowDisplayModes(IWindowManager *manager);
bool HasNativeWindowMessageTarget(LegacyWindowToken window);
bool DispatchNativeWindowMessage(LegacyWindowToken window, LegacyMessageId message,
                                 LegacyWParam wparam, LegacyLParam lparam);
void RegisterNativeWindowManager(ICOManager * manager);
}

#endif
