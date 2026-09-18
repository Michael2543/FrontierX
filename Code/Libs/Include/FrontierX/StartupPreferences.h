#ifndef FRONTIERX_STARTUP_PREFERENCES_H
#define FRONTIERX_STARTUP_PREFERENCES_H

#include "FrontierX/NativeUserSettingsOwner.h"
#include "FrontierX/SettingsPath.h"

#include <filesystem>
#include <string>

namespace FrontierX
{
struct StartupPreferences
{
    SettingsPathSelection selection;
    PreferenceStore store;
    NativeUserSettingsOwner userSettings;
    PreferenceLoadResult storeResult = PreferenceLoadResult::NotFound;
    NativeUserSettingsOwnerLoadResult userSettingsResult =
        NativeUserSettingsOwnerLoadResult::Defaults;
};

bool LoadStartupPreferences(
    const std::filesystem::path & explicit_path,
    const std::filesystem::path & xdg_config_home,
    const std::filesystem::path & home,
    StartupPreferences & result,
    std::string * error = nullptr);

const char * PreferenceLoadResultName(PreferenceLoadResult result) noexcept;
const char * UserSettingsLoadResultName(
    NativeUserSettingsOwnerLoadResult result) noexcept;
}

#endif
