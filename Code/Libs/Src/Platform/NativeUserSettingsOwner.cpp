#include "FrontierX/NativeUserSettingsOwner.h"

#include "FrontierX/LegacyPreferenceValues.h"

#include <array>
#include <utility>

namespace FrontierX
{
namespace
{
constexpr const char * PlayerNameKey = "CQPlayerName";
constexpr std::size_t PlayerNameBytes = 64;

void SetError(std::string * error, const std::string & message)
{
    if (error) *error = message;
}
}

std::string NativeUserSettingsOwner::PlayerPrefix(std::string_view playerName)
{
    static constexpr char Hex[] = "0123456789abcdef";
    std::string result = "Players.";
    result.reserve(result.size() + playerName.size() * 2 + 1);
    for (unsigned char character : playerName)
    {
        result.push_back(Hex[character >> 4U]);
        result.push_back(Hex[character & 0x0fU]);
    }
    result.push_back('.');
    return result;
}

NativeUserSettingsOwnerLoadResult NativeUserSettingsOwner::Load(
    PreferenceStore & store, std::string * error)
{
    SetError(error, {});
    LegacyPreferenceValues values(store);
    std::array<char, PlayerNameBytes> playerName{};
    const std::uint32_t playerNameBytes =
        values.GetString(PlayerNameKey, playerName.data(), playerName.size());
    if (playerNameBytes == LegacyPreferenceValues::BufferTooSmall)
    {
        SetError(error, "native player name exceeds the supported 63-byte limit");
        return NativeUserSettingsOwnerLoadResult::Invalid;
    }

    if (playerNameBytes != 0)
    {
        const std::string prefix = PlayerPrefix(playerName.data());
        NativeUserSettings playerSettings = settings_;
        const NativeUserSettingsLoadResult playerResult =
            LoadNativeUserSettingsAtPrefix(store, playerSettings, prefix, error);
        if (playerResult == NativeUserSettingsLoadResult::Loaded)
        {
            settings_ = playerSettings;
            source_ = NativeUserSettingsSource::Player;
            return NativeUserSettingsOwnerLoadResult::Loaded;
        }
        if (playerResult == NativeUserSettingsLoadResult::NoSettings)
        {
            const std::uint32_t hardwareCursor =
                settings_.flags & NativeUserSettingFlagValue(
                    NativeUserSettingFlag::HardwareCursor);
            settings_ = NativeUserSettings{};
            settings_.flags =
                (settings_.flags & ~NativeUserSettingFlagValue(
                    NativeUserSettingFlag::HardwareCursor)) |
                hardwareCursor;
            source_ = NativeUserSettingsSource::Defaults;
            SetError(error, {});
            return NativeUserSettingsOwnerLoadResult::Defaults;
        }

        // The Windows owner deletes an incompatible player blob before trying
        // the global defaults. Remove only this encoded player namespace.
        store.RemovePrefix(prefix);
    }

    NativeUserSettings globalSettings = settings_;
    const NativeUserSettingsLoadResult globalResult =
        LoadNativeUserSettings(store, globalSettings, error);
    if (globalResult == NativeUserSettingsLoadResult::Loaded)
    {
        settings_ = globalSettings;
        source_ = NativeUserSettingsSource::Global;
        return NativeUserSettingsOwnerLoadResult::Loaded;
    }
    if (globalResult == NativeUserSettingsLoadResult::NoSettings)
    {
        source_ = NativeUserSettingsSource::Defaults;
        SetError(error, {});
        return NativeUserSettingsOwnerLoadResult::Defaults;
    }

    settings_ = NativeUserSettings{};
    source_ = NativeUserSettingsSource::Defaults;
    return NativeUserSettingsOwnerLoadResult::Invalid;
}

bool NativeUserSettingsOwner::Store(PreferenceStore & store, std::string * error) const
{
    SetError(error, {});
    PreferenceStore updated = store;
    if (!StoreNativeUserSettings(settings_, updated, error))
        return false;

    LegacyPreferenceValues values(updated);
    std::array<char, PlayerNameBytes> playerName{};
    const std::uint32_t playerNameBytes =
        values.GetString(PlayerNameKey, playerName.data(), playerName.size());
    if (playerNameBytes == LegacyPreferenceValues::BufferTooSmall)
    {
        SetError(error, "native player name exceeds the supported 63-byte limit");
        return false;
    }
    if (playerNameBytes != 0 &&
        !StoreNativeUserSettingsAtPrefix(
            settings_, updated, PlayerPrefix(playerName.data()), error))
        return false;

    store = std::move(updated);
    return true;
}

NativeUserSettings & NativeUserSettingsOwner::Settings() noexcept
{
    return settings_;
}

const NativeUserSettings & NativeUserSettingsOwner::Settings() const noexcept
{
    return settings_;
}

NativeUserSettingsSource NativeUserSettingsOwner::Source() const noexcept
{
    return source_;
}

const char * NativeUserSettingsSourceName(NativeUserSettingsSource source) noexcept
{
    switch (source)
    {
    case NativeUserSettingsSource::Defaults:
        return "defaults";
    case NativeUserSettingsSource::Global:
        return "global";
    case NativeUserSettingsSource::Player:
        return "player";
    }
    return "unknown";
}
}
