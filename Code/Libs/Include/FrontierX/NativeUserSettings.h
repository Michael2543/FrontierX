#ifndef FRONTIERX_NATIVE_USER_SETTINGS_H
#define FRONTIERX_NATIVE_USER_SETTINGS_H

#include "FrontierX/PreferenceStore.h"

#include <cstdint>
#include <string>
#include <string_view>

namespace FrontierX
{
enum class NativeFogMode : std::int32_t
{
    Normal = -1,
    Explored = 0,
    None = 1,
};

enum class NativeUserSettingFlag : std::uint32_t
{
    WindowMode = 1U << 0U,
    NoAutoSave = 1U << 1U,
    NoTooltips = 1U << 2U,
    NoFrameLimit = 1U << 3U,
    PositionToolWorld = 1U << 4U,
    EditorMode = 1U << 5U,
    DrawHotrects = 1U << 6U,
    NoStatusBar = 1U << 7U,
    VisibilityRulesOff = 1U << 8U,
    ChoseMultiplayer = 1U << 9U,
    ChoseJoin = 1U << 10U,
    PartDialogVisible = 1U << 11U,
    CheatsEnabled = 1U << 12U,
    SectormapRotates = 1U << 13U,
    NoAutoTarget = 1U << 14U,
    NoWinningConditions = 1U << 15U,
    NoHints = 1U << 16U,
    RightClickOption = 1U << 17U,
    InfoHighlights = 1U << 18U,
    NoSupplies = 1U << 19U,
    NoDamage = 1U << 20U,
    ShowGrids = 1U << 21U,
    CheapMovement = 1U << 22U,
    HardwareRender = 1U << 23U,
    ConstantUpdateRate = 1U << 24U,
    HardwareCursor = 1U << 25U,
    LockDiplomacy = 1U << 26U,
    SpectatorModeAllowed = 1U << 27U,
    SpectatorModeOn = 1U << 28U,
    Subtitles = 1U << 29U,
    NetworkBandwidth = 1U << 30U,
};

constexpr std::uint32_t NativeUserSettingFlagMask = 0x7fffffffU;

constexpr std::uint32_t NativeUserSettingFlagValue(NativeUserSettingFlag flag) noexcept
{
    return static_cast<std::uint32_t>(flag);
}

struct NativeSoundSettings
{
    bool muted = false;
    std::int32_t volume = 0;
};

struct NativeUserSettings
{
    std::int32_t mainX = 0;
    std::int32_t mainY = 0;
    std::int32_t mainWidth = 0;
    std::int32_t mainHeight = 0;
    std::uint32_t showState = 5;
    std::int32_t viewerWidth = 100;
    std::int32_t viewerHeight = 100;
    std::uint32_t minLatency = 0;
    std::uint32_t packetLossPercent = 0;
    std::int32_t mouseSpeed = 0;
    float scrollRate = 1.0F;
    NativeSoundSettings music{false, 6};
    NativeSoundSettings effects{false, 9};
    NativeSoundSettings comms{false, 7};
    NativeSoundSettings chat{false, 9};
    std::int32_t gammaCorrection = 0;
    std::int32_t gameSpeed = 0;
    NativeFogMode fogMode = NativeFogMode::Normal;
    std::uint32_t flags =
        NativeUserSettingFlagValue(NativeUserSettingFlag::SectormapRotates) |
        NativeUserSettingFlagValue(NativeUserSettingFlag::RightClickOption) |
        NativeUserSettingFlagValue(NativeUserSettingFlag::HardwareRender) |
        NativeUserSettingFlagValue(NativeUserSettingFlag::HardwareCursor);
    std::uint32_t maxProjectiles = 20;
};

enum class NativeUserSettingsLoadResult
{
    Loaded,
    NoSettings,
    Invalid,
};

bool StoreNativeUserSettings(const NativeUserSettings & settings, PreferenceStore & store,
                             std::string * error = nullptr);
NativeUserSettingsLoadResult LoadNativeUserSettings(
    const PreferenceStore & store, NativeUserSettings & settings, std::string * error = nullptr);
bool StoreNativeUserSettingsAtPrefix(
    const NativeUserSettings & settings, PreferenceStore & store, std::string_view prefix,
    std::string * error = nullptr);
NativeUserSettingsLoadResult LoadNativeUserSettingsAtPrefix(
    const PreferenceStore & store, NativeUserSettings & settings, std::string_view prefix,
    std::string * error = nullptr);
}

#endif
