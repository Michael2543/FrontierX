#include "FrontierX/StartupPreferences.h"

#include <utility>

namespace FrontierX
{
namespace
{
void SetError(std::string * error, const std::string & message)
{
    if (error) *error = message;
}
}

bool LoadStartupPreferences(
    const std::filesystem::path & explicit_path,
    const std::filesystem::path & xdg_config_home,
    const std::filesystem::path & home,
    StartupPreferences & result,
    std::string * error)
{
    SetError(error, {});
    StartupPreferences loaded;
    loaded.selection = SelectSettingsPath(explicit_path, xdg_config_home, home);
    if (!loaded.selection)
    {
        SetError(error, "no absolute XDG_CONFIG_HOME or HOME is available");
        return false;
    }

    std::string store_error;
    loaded.storeResult = loaded.store.Load(loaded.selection.path, &store_error);
    if ((!explicit_path.empty() && loaded.storeResult == PreferenceLoadResult::NotFound) ||
        loaded.storeResult == PreferenceLoadResult::Invalid ||
        loaded.storeResult == PreferenceLoadResult::IoError)
    {
        SetError(error, store_error.empty() ? "explicit preference file is missing" : store_error);
        return false;
    }

    std::string settings_error;
    loaded.userSettingsResult = loaded.userSettings.Load(loaded.store, &settings_error);
    if (loaded.userSettingsResult == NativeUserSettingsOwnerLoadResult::Invalid)
    {
        SetError(error, settings_error);
        return false;
    }

    result = std::move(loaded);
    return true;
}

const char * PreferenceLoadResultName(PreferenceLoadResult result) noexcept
{
    switch (result)
    {
    case PreferenceLoadResult::Loaded: return "loaded";
    case PreferenceLoadResult::NotFound: return "missing";
    case PreferenceLoadResult::Invalid: return "invalid";
    case PreferenceLoadResult::IoError: return "io-error";
    }
    return "unknown";
}

const char * UserSettingsLoadResultName(
    NativeUserSettingsOwnerLoadResult result) noexcept
{
    switch (result)
    {
    case NativeUserSettingsOwnerLoadResult::Loaded: return "loaded";
    case NativeUserSettingsOwnerLoadResult::Defaults: return "absent";
    case NativeUserSettingsOwnerLoadResult::Invalid: return "invalid";
    }
    return "unknown";
}
}
