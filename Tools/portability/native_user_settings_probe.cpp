#include "FrontierX/NativeUserSettings.h"
#include "FrontierX/NativeUserSettingsOwner.h"
#include "FrontierX/StartupPreferences.h"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace
{
struct TemporaryPreferencesFile
{
    std::filesystem::path path = "frontierx_startup_preferences_probe.dat";
    ~TemporaryPreferencesFile()
    {
        std::error_code error;
        std::filesystem::remove(path, error);
    }
};

void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

bool EqualSound(const FrontierX::NativeSoundSettings & left,
                const FrontierX::NativeSoundSettings & right)
{
    return left.muted == right.muted && left.volume == right.volume;
}

bool Equal(const FrontierX::NativeUserSettings & left,
           const FrontierX::NativeUserSettings & right)
{
    return left.mainX == right.mainX && left.mainY == right.mainY &&
        left.mainWidth == right.mainWidth && left.mainHeight == right.mainHeight &&
        left.showState == right.showState && left.viewerWidth == right.viewerWidth &&
        left.viewerHeight == right.viewerHeight && left.minLatency == right.minLatency &&
        left.packetLossPercent == right.packetLossPercent &&
        left.mouseSpeed == right.mouseSpeed && left.scrollRate == right.scrollRate &&
        EqualSound(left.music, right.music) && EqualSound(left.effects, right.effects) &&
        EqualSound(left.comms, right.comms) && EqualSound(left.chat, right.chat) &&
        left.gammaCorrection == right.gammaCorrection && left.gameSpeed == right.gameSpeed &&
        left.fogMode == right.fogMode && left.flags == right.flags &&
        left.maxProjectiles == right.maxProjectiles;
}
}

int main()
{
    try
    {
        static_assert(FrontierX::NativeUserSettingFlagValue(
            FrontierX::NativeUserSettingFlag::WindowMode) == (1U << 0U));
        static_assert(FrontierX::NativeUserSettingFlagValue(
            FrontierX::NativeUserSettingFlag::NetworkBandwidth) == (1U << 30U));

        const FrontierX::NativeUserSettings defaults;
        const std::uint32_t expected_default_flags =
            FrontierX::NativeUserSettingFlagValue(
                FrontierX::NativeUserSettingFlag::SectormapRotates) |
            FrontierX::NativeUserSettingFlagValue(
                FrontierX::NativeUserSettingFlag::RightClickOption) |
            FrontierX::NativeUserSettingFlagValue(
                FrontierX::NativeUserSettingFlag::HardwareRender) |
            FrontierX::NativeUserSettingFlagValue(
                FrontierX::NativeUserSettingFlag::HardwareCursor);
        Require(defaults.showState == 5 && defaults.viewerWidth == 100 &&
            defaults.viewerHeight == 100 && defaults.scrollRate == 1.0F &&
            defaults.music.volume == 6 && defaults.effects.volume == 9 &&
            defaults.comms.volume == 7 && defaults.chat.volume == 9 &&
            defaults.flags == expected_default_flags && defaults.maxProjectiles == 20,
            "native defaults differ from source-reviewed UserDefaults initialization");

        FrontierX::PreferenceStore empty;
        FrontierX::NativeUserSettings unchanged;
        unchanged.mainX = 1234;
        std::string error = "stale";
        Require(FrontierX::LoadNativeUserSettings(empty, unchanged, &error) ==
                    FrontierX::NativeUserSettingsLoadResult::NoSettings,
            "missing native schema did not report NoSettings");
        Require(unchanged.mainX == 1234 && error.empty(),
            "missing native schema modified output or reported an error");

        FrontierX::NativeUserSettings input;
        input.mainX = -101;
        input.mainY = 202;
        input.mainWidth = 1920;
        input.mainHeight = 1080;
        input.showState = 3;
        input.viewerWidth = 640;
        input.viewerHeight = 480;
        input.minLatency = 17;
        input.packetLossPercent = 4;
        input.mouseSpeed = -7;
        input.scrollRate = 2.5F;
        input.music = {true, 1};
        input.effects = {false, 2};
        input.comms = {true, 3};
        input.chat = {false, 4};
        input.gammaCorrection = -5;
        input.gameSpeed = 5;
        input.fogMode = FrontierX::NativeFogMode::Explored;
        input.flags = FrontierX::NativeUserSettingFlagMask;
        input.maxProjectiles = 123;

        FrontierX::PreferenceStore store;
        Require(store.SetString("Unrelated", "preserve me"), "could not seed unrelated setting");
        Require(FrontierX::StoreNativeUserSettings(input, store, &error), error.c_str());
        const FrontierX::PreferenceValue * main_x = store.Find("Window.MainX");
        Require(main_x != nullptr && main_x->bytes.size() == 4 &&
            main_x->bytes[0] == 0x9b && main_x->bytes[1] == 0xff &&
            main_x->bytes[2] == 0xff && main_x->bytes[3] == 0xff,
            "signed setting is not explicit little-endian two's-complement bytes");
        Require(store.Find("Unrelated") != nullptr,
            "storing native defaults discarded an unrelated preference");

        FrontierX::NativeUserSettings output;
        Require(FrontierX::LoadNativeUserSettings(store, output, &error) ==
                    FrontierX::NativeUserSettingsLoadResult::Loaded,
            error.c_str());
        Require(Equal(input, output), "native user settings did not round trip exactly");

        FrontierX::NativeUserSettings preserved;
        preserved.mainX = 777;
        FrontierX::PreferenceStore missing_field = store;
        Require(missing_field.Remove("Sound.Chat.Volume"), "could not remove required field");
        Require(FrontierX::LoadNativeUserSettings(missing_field, preserved, &error) ==
                    FrontierX::NativeUserSettingsLoadResult::Invalid,
            "missing required native setting was accepted");
        Require(preserved.mainX == 777, "invalid native settings modified the output");

        FrontierX::PreferenceStore wrong_type = store;
        Require(wrong_type.SetString("Window.Width", "1920"),
            "could not construct wrong-type setting");
        Require(FrontierX::LoadNativeUserSettings(wrong_type, preserved, &error) ==
                    FrontierX::NativeUserSettingsLoadResult::Invalid,
            "wrong-type native setting was accepted");

        FrontierX::PreferenceStore wrong_version = store;
        const std::uint8_t version[] = {2, 0, 0, 0};
        Require(wrong_version.SetBinary("UserDefaults.Schema", version, sizeof(version)),
            "could not construct unsupported schema");
        Require(FrontierX::LoadNativeUserSettings(wrong_version, preserved, &error) ==
                    FrontierX::NativeUserSettingsLoadResult::Invalid,
            "unsupported native settings schema was accepted");

        FrontierX::NativeUserSettings invalid = input;
        invalid.scrollRate = std::numeric_limits<float>::quiet_NaN();
        const std::size_t prior_size = store.Size();
        Require(!FrontierX::StoreNativeUserSettings(invalid, store, &error),
            "non-finite scroll rate was stored");
        Require(store.Size() == prior_size && store.Find("Unrelated") != nullptr,
            "failed native settings store modified the preference store");
        invalid = input;
        invalid.flags = 0x80000000U;
        Require(!FrontierX::StoreNativeUserSettings(invalid, store, &error),
            "unknown native settings flag was stored");
        invalid = input;
        invalid.fogMode = static_cast<FrontierX::NativeFogMode>(2);
        Require(!FrontierX::StoreNativeUserSettings(invalid, store, &error),
            "invalid native fog mode was stored");

        FrontierX::PreferenceStore globalStore;
        Require(FrontierX::StoreNativeUserSettings(input, globalStore, &error), error.c_str());
        FrontierX::NativeUserSettingsOwner globalOwner;
        Require(globalOwner.Load(globalStore, &error) ==
                    FrontierX::NativeUserSettingsOwnerLoadResult::Loaded &&
            globalOwner.Source() == FrontierX::NativeUserSettingsSource::Global &&
            Equal(globalOwner.Settings(), input),
            "native settings owner did not load global defaults");

        FrontierX::PreferenceStore missingPlayerStore = globalStore;
        Require(missingPlayerStore.SetString("CQPlayerName", "Blackwell"),
            "could not seed player name");
        FrontierX::NativeUserSettingsOwner missingPlayerOwner;
        missingPlayerOwner.Settings().flags &=
            ~FrontierX::NativeUserSettingFlagValue(
                FrontierX::NativeUserSettingFlag::HardwareCursor);
        Require(missingPlayerOwner.Load(missingPlayerStore, &error) ==
                    FrontierX::NativeUserSettingsOwnerLoadResult::Defaults &&
            missingPlayerOwner.Source() == FrontierX::NativeUserSettingsSource::Defaults &&
            !Equal(missingPlayerOwner.Settings(), input) &&
            (missingPlayerOwner.Settings().flags &
                FrontierX::NativeUserSettingFlagValue(
                    FrontierX::NativeUserSettingFlag::HardwareCursor)) == 0,
            "missing player defaults did not reset while preserving hardware cursor");

        FrontierX::NativeUserSettings playerSettings = input;
        playerSettings.mainX = 909;
        FrontierX::PreferenceStore playerStore = globalStore;
        Require(playerStore.SetString("CQPlayerName", "Blackwell"),
            "could not seed player name");
        const std::string playerPrefix =
            FrontierX::NativeUserSettingsOwner::PlayerPrefix("Blackwell");
        Require(FrontierX::StoreNativeUserSettingsAtPrefix(
                    playerSettings, playerStore, playerPrefix, &error),
            error.c_str());
        FrontierX::NativeUserSettingsOwner playerOwner;
        Require(playerOwner.Load(playerStore, &error) ==
                    FrontierX::NativeUserSettingsOwnerLoadResult::Loaded &&
            playerOwner.Source() == FrontierX::NativeUserSettingsSource::Player &&
            Equal(playerOwner.Settings(), playerSettings),
            "native settings owner did not prefer player defaults");

        FrontierX::PreferenceStore invalidPlayerStore = playerStore;
        Require(invalidPlayerStore.Remove(playerPrefix + "Window.Width"),
            "could not corrupt player defaults");
        Require(invalidPlayerStore.SetString("Players.other.Unrelated", "preserve"),
            "could not seed unrelated player namespace");
        FrontierX::NativeUserSettingsOwner fallbackOwner;
        Require(fallbackOwner.Load(invalidPlayerStore, &error) ==
                    FrontierX::NativeUserSettingsOwnerLoadResult::Loaded &&
            fallbackOwner.Source() == FrontierX::NativeUserSettingsSource::Global &&
            Equal(fallbackOwner.Settings(), input),
            "invalid player defaults did not fall back to global defaults");
        Require(invalidPlayerStore.Find(playerPrefix + "UserDefaults.Schema") == nullptr &&
            invalidPlayerStore.Find("Players.other.Unrelated") != nullptr,
            "invalid player cleanup crossed its encoded namespace");

        FrontierX::PreferenceStore invalidGlobalStore = globalStore;
        Require(invalidGlobalStore.Remove("Window.Width"),
            "could not corrupt global defaults");
        FrontierX::NativeUserSettingsOwner invalidGlobalOwner;
        Require(invalidGlobalOwner.Load(invalidGlobalStore, &error) ==
                    FrontierX::NativeUserSettingsOwnerLoadResult::Invalid &&
            invalidGlobalOwner.Source() == FrontierX::NativeUserSettingsSource::Defaults,
            "invalid global defaults did not fail transactionally");

        FrontierX::PreferenceStore ownerStore;
        Require(ownerStore.SetString("CQPlayerName", "Hawkes"),
            "could not seed owner player name");
        FrontierX::NativeUserSettingsOwner storingOwner;
        storingOwner.Settings() = playerSettings;
        Require(storingOwner.Store(ownerStore, &error), error.c_str());
        FrontierX::NativeUserSettings globalStored;
        FrontierX::NativeUserSettings playerStored;
        Require(FrontierX::LoadNativeUserSettings(ownerStore, globalStored, &error) ==
                    FrontierX::NativeUserSettingsLoadResult::Loaded &&
            FrontierX::LoadNativeUserSettingsAtPrefix(
                ownerStore, playerStored,
                FrontierX::NativeUserSettingsOwner::PlayerPrefix("Hawkes"), &error) ==
                    FrontierX::NativeUserSettingsLoadResult::Loaded &&
            Equal(globalStored, playerSettings) && Equal(playerStored, playerSettings),
            "native settings owner did not store global and player copies");

        FrontierX::PreferenceStore longNameStore;
        Require(longNameStore.SetString("CQPlayerName", std::string(64, 'x')),
            "could not seed oversized player name");
        const std::size_t longNameSize = longNameStore.Size();
        FrontierX::NativeUserSettingsOwner longNameOwner;
        Require(longNameOwner.Load(longNameStore, &error) ==
                    FrontierX::NativeUserSettingsOwnerLoadResult::Invalid &&
            !longNameOwner.Store(longNameStore, &error) &&
            longNameStore.Size() == longNameSize,
            "oversized player name was accepted or modified storage");

        TemporaryPreferencesFile startupFile;
        std::error_code removeError;
        std::filesystem::remove(startupFile.path, removeError);
        FrontierX::StartupPreferences startupPreferences;
        Require(FrontierX::LoadStartupPreferences(
                    {}, std::filesystem::current_path(), {}, startupPreferences, &error) &&
            startupPreferences.selection.source ==
                FrontierX::SettingsPathSource::XdgConfigHome &&
            startupPreferences.storeResult == FrontierX::PreferenceLoadResult::NotFound &&
            startupPreferences.userSettingsResult ==
                FrontierX::NativeUserSettingsOwnerLoadResult::Defaults,
            "missing default startup preferences did not select defaults");
        Require(!FrontierX::LoadStartupPreferences(
                    "missing-explicit-preferences.dat", {}, {}, startupPreferences, &error) &&
            error == "explicit preference file is missing" &&
            startupPreferences.selection.source ==
                FrontierX::SettingsPathSource::XdgConfigHome,
            "explicit missing startup preferences were accepted or modified output");
        Require(!FrontierX::LoadStartupPreferences(
                    {}, "relative-xdg", "", startupPreferences, &error) &&
            error == "no absolute XDG_CONFIG_HOME or HOME is available",
            "unavailable startup preference base was accepted");
        Require(globalStore.Save(startupFile.path, &error), error.c_str());
        Require(FrontierX::LoadStartupPreferences(
                    startupFile.path, {}, {}, startupPreferences, &error) &&
            startupPreferences.selection.source == FrontierX::SettingsPathSource::Explicit &&
            startupPreferences.storeResult == FrontierX::PreferenceLoadResult::Loaded &&
            startupPreferences.userSettings.Source() ==
                FrontierX::NativeUserSettingsSource::Global &&
            Equal(startupPreferences.userSettings.Settings(), input),
            "explicit startup preferences did not load global native settings");
        Require(std::string(FrontierX::PreferenceLoadResultName(
                    FrontierX::PreferenceLoadResult::Loaded)) == "loaded" &&
            std::string(FrontierX::UserSettingsLoadResultName(
                    FrontierX::NativeUserSettingsOwnerLoadResult::Defaults)) == "absent",
            "startup preference diagnostic names changed");

        std::cout << "fixed-width native user settings semantics passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "native user settings probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
