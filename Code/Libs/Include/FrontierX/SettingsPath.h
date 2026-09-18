#ifndef FRONTIERX_SETTINGS_PATH_H
#define FRONTIERX_SETTINGS_PATH_H

#include <filesystem>

namespace FrontierX
{
enum class SettingsPathSource
{
    Explicit,
    XdgConfigHome,
    HomeDefault,
    Unavailable,
};

struct SettingsPathSelection
{
    std::filesystem::path path;
    SettingsPathSource source = SettingsPathSource::Unavailable;

    explicit operator bool() const noexcept
    {
        return source != SettingsPathSource::Unavailable;
    }
};

SettingsPathSelection SelectSettingsPath(
    const std::filesystem::path & explicit_path,
    const std::filesystem::path & xdg_config_home,
    const std::filesystem::path & home);
const char * SettingsPathSourceName(SettingsPathSource source) noexcept;
}

#endif
