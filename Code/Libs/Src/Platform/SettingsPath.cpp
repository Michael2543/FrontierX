#include "FrontierX/SettingsPath.h"

namespace FrontierX
{
namespace
{
const std::filesystem::path RelativeSettingsPath = "frontierx/preferences.dat";
}

SettingsPathSelection SelectSettingsPath(
    const std::filesystem::path & explicit_path,
    const std::filesystem::path & xdg_config_home,
    const std::filesystem::path & home)
{
    if (!explicit_path.empty()) return {explicit_path, SettingsPathSource::Explicit};
    if (xdg_config_home.is_absolute())
        return {xdg_config_home / RelativeSettingsPath, SettingsPathSource::XdgConfigHome};
    if (home.is_absolute())
        return {home / ".config" / RelativeSettingsPath, SettingsPathSource::HomeDefault};
    return {};
}

const char * SettingsPathSourceName(SettingsPathSource source) noexcept
{
    switch (source)
    {
    case SettingsPathSource::Explicit:
        return "command-line";
    case SettingsPathSource::XdgConfigHome:
        return "xdg-config-home";
    case SettingsPathSource::HomeDefault:
        return "home-default";
    case SettingsPathSource::Unavailable:
        return "unavailable";
    }
    return "unknown";
}
}
