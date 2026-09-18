#ifndef FRONTIERX_NATIVE_USER_SETTINGS_OWNER_H
#define FRONTIERX_NATIVE_USER_SETTINGS_OWNER_H

#include "FrontierX/NativeUserSettings.h"

#include <string>
#include <string_view>

namespace FrontierX
{
enum class NativeUserSettingsSource
{
    Defaults,
    Global,
    Player,
};

enum class NativeUserSettingsOwnerLoadResult
{
    Loaded,
    Defaults,
    Invalid,
};

class NativeUserSettingsOwner
{
public:
    NativeUserSettingsOwner() = default;

    NativeUserSettingsOwnerLoadResult Load(
        PreferenceStore & store, std::string * error = nullptr);
    bool Store(PreferenceStore & store, std::string * error = nullptr) const;

    NativeUserSettings & Settings() noexcept;
    const NativeUserSettings & Settings() const noexcept;
    NativeUserSettingsSource Source() const noexcept;

    static std::string PlayerPrefix(std::string_view playerName);

private:
    NativeUserSettings settings_;
    NativeUserSettingsSource source_ = NativeUserSettingsSource::Defaults;
};

const char * NativeUserSettingsSourceName(NativeUserSettingsSource source) noexcept;
}

#endif
