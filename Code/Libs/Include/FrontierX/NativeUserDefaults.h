#ifndef FRONTIERX_NATIVE_USER_DEFAULTS_H
#define FRONTIERX_NATIVE_USER_DEFAULTS_H

#include "FrontierX/StartupPreferences.h"

#include <filesystem>

struct ICOManager;

namespace FrontierX
{
void RegisterNativeUserDefaults(
    ICOManager * manager,
    const std::filesystem::path & path,
    const StartupPreferences & preferences);
}

#endif
