#include "FrontierX/NativeUserDefaults.h"

#include "FrontierX/LegacyPreferenceValues.h"
#include "TComponent.h"
#include "UserDefaults.h"

#include <cmath>
#include <cstring>
#include <limits>
#include <new>
#include <string>
#include <utility>

USER_DEFAULTS * IUserDefaults::pUserDefaults = nullptr;

namespace FrontierX
{
namespace
{
constexpr U32 DefaultsVersion = 11;
constexpr std::size_t LegacyStringBytes = 256;
char interface_name[] = "IUserDefaults";

void SetFlag(std::uint32_t & flags, NativeUserSettingFlag flag, bool enabled)
{
    const std::uint32_t value = NativeUserSettingFlagValue(flag);
    if (enabled)
        flags |= value;
    else
        flags &= ~value;
}

bool HasFlag(std::uint32_t flags, NativeUserSettingFlag flag)
{
    return (flags & NativeUserSettingFlagValue(flag)) != 0;
}

void CopySoundToLegacy(const NativeSoundSettings & source, SOUNDSTATE::ITEM & destination)
{
    destination.bMute = source.muted;
    destination.volume = source.volume;
}

void CopySoundFromLegacy(const SOUNDSTATE::ITEM & source, NativeSoundSettings & destination)
{
    destination.muted = source.bMute;
    destination.volume = source.volume;
}

void CopyToLegacy(const NativeUserSettings & source, USER_DEFAULTS & destination)
{
    std::memset(&destination, 0, sizeof(destination));
    destination.dwSize = sizeof(destination);
    destination.dwVersion = DefaultsVersion;
    destination.iMainX = source.mainX;
    destination.iMainY = source.mainY;
    destination.iMainWidth = source.mainWidth;
    destination.iMainHeight = source.mainHeight;
    destination.showState = source.showState;
    destination.iViewerWidth = source.viewerWidth;
    destination.iViewerHeight = source.viewerHeight;
    destination.minLatency = source.minLatency;
    destination.packetLossPercent = source.packetLossPercent;
    destination.mouseSpeed = source.mouseSpeed;
    destination.scrollRate = source.scrollRate;
    CopySoundToLegacy(source.music, destination.soundState.music);
    CopySoundToLegacy(source.effects, destination.soundState.effects);
    CopySoundToLegacy(source.comms, destination.soundState.comms);
    CopySoundToLegacy(source.chat, destination.soundState.chat);
    destination.gammaCorrection = source.gammaCorrection;
    destination.gameSpeed = source.gameSpeed;
    destination.fogMode = static_cast<FOGOFWARMODE>(source.fogMode);
    destination.bWindowMode = HasFlag(source.flags, NativeUserSettingFlag::WindowMode);
    destination.bNoAutoSave = HasFlag(source.flags, NativeUserSettingFlag::NoAutoSave);
    destination.bNoTooltips = HasFlag(source.flags, NativeUserSettingFlag::NoTooltips);
    destination.bNoFrameLimit = HasFlag(source.flags, NativeUserSettingFlag::NoFrameLimit);
    destination.bPosToolWorld = HasFlag(source.flags, NativeUserSettingFlag::PositionToolWorld);
    destination.bEditorMode = HasFlag(source.flags, NativeUserSettingFlag::EditorMode);
    destination.bDrawHotrects = HasFlag(source.flags, NativeUserSettingFlag::DrawHotrects);
    destination.bNoStatusBar = HasFlag(source.flags, NativeUserSettingFlag::NoStatusBar);
    destination.bVisibilityRulesOff =
        HasFlag(source.flags, NativeUserSettingFlag::VisibilityRulesOff);
    destination.bChoseMultiplayer = HasFlag(source.flags, NativeUserSettingFlag::ChoseMultiplayer);
    destination.bChoseJoin = HasFlag(source.flags, NativeUserSettingFlag::ChoseJoin);
    destination.bPartDlgVisible = HasFlag(source.flags, NativeUserSettingFlag::PartDialogVisible);
    destination.bCheatsEnabled = HasFlag(source.flags, NativeUserSettingFlag::CheatsEnabled);
    destination.bSectormapRotates = HasFlag(source.flags, NativeUserSettingFlag::SectormapRotates);
    destination.bNoAutoTarget = HasFlag(source.flags, NativeUserSettingFlag::NoAutoTarget);
    destination.bNoWinningConditions =
        HasFlag(source.flags, NativeUserSettingFlag::NoWinningConditions);
    destination.bNoHints = HasFlag(source.flags, NativeUserSettingFlag::NoHints);
    destination.bRightClickOption = HasFlag(source.flags, NativeUserSettingFlag::RightClickOption);
    destination.bInfoHighlights = HasFlag(source.flags, NativeUserSettingFlag::InfoHighlights);
    destination.bNoSupplies = HasFlag(source.flags, NativeUserSettingFlag::NoSupplies);
    destination.bNoDamage = HasFlag(source.flags, NativeUserSettingFlag::NoDamage);
    destination.bShowGrids = HasFlag(source.flags, NativeUserSettingFlag::ShowGrids);
    destination.bCheapMovement = HasFlag(source.flags, NativeUserSettingFlag::CheapMovement);
    destination.bHardwareRender = HasFlag(source.flags, NativeUserSettingFlag::HardwareRender);
    destination.bConstUpdateRate = HasFlag(source.flags, NativeUserSettingFlag::ConstantUpdateRate);
    destination.bHardwareCursor = HasFlag(source.flags, NativeUserSettingFlag::HardwareCursor);
    destination.bLockDiplomacy = HasFlag(source.flags, NativeUserSettingFlag::LockDiplomacy);
    destination.bSpectatorModeAllowed =
        HasFlag(source.flags, NativeUserSettingFlag::SpectatorModeAllowed);
    destination.bSpectatorModeOn = HasFlag(source.flags, NativeUserSettingFlag::SpectatorModeOn);
    destination.bSubtitles = HasFlag(source.flags, NativeUserSettingFlag::Subtitles);
    destination.bNetworkBandwidth = HasFlag(source.flags, NativeUserSettingFlag::NetworkBandwidth);
    destination.maxProjectiles = source.maxProjectiles;
}

bool CopyFromLegacy(const USER_DEFAULTS & source, NativeUserSettings & destination)
{
    if (!std::isfinite(source.scrollRate) || source.fogMode < FOGOWAR_NORMAL ||
        source.fogMode > FOGOWAR_NONE)
        return false;

    NativeUserSettings converted;
    converted.mainX = source.iMainX;
    converted.mainY = source.iMainY;
    converted.mainWidth = source.iMainWidth;
    converted.mainHeight = source.iMainHeight;
    converted.showState = source.showState;
    converted.viewerWidth = source.iViewerWidth;
    converted.viewerHeight = source.iViewerHeight;
    converted.minLatency = source.minLatency;
    converted.packetLossPercent = source.packetLossPercent;
    converted.mouseSpeed = source.mouseSpeed;
    converted.scrollRate = source.scrollRate;
    CopySoundFromLegacy(source.soundState.music, converted.music);
    CopySoundFromLegacy(source.soundState.effects, converted.effects);
    CopySoundFromLegacy(source.soundState.comms, converted.comms);
    CopySoundFromLegacy(source.soundState.chat, converted.chat);
    converted.gammaCorrection = source.gammaCorrection;
    converted.gameSpeed = source.gameSpeed;
    converted.fogMode = static_cast<NativeFogMode>(source.fogMode);
    converted.flags = 0;
    SetFlag(converted.flags, NativeUserSettingFlag::WindowMode, source.bWindowMode);
    SetFlag(converted.flags, NativeUserSettingFlag::NoAutoSave, source.bNoAutoSave);
    SetFlag(converted.flags, NativeUserSettingFlag::NoTooltips, source.bNoTooltips);
    SetFlag(converted.flags, NativeUserSettingFlag::NoFrameLimit, source.bNoFrameLimit);
    SetFlag(converted.flags, NativeUserSettingFlag::PositionToolWorld, source.bPosToolWorld);
    SetFlag(converted.flags, NativeUserSettingFlag::EditorMode, source.bEditorMode);
    SetFlag(converted.flags, NativeUserSettingFlag::DrawHotrects, source.bDrawHotrects);
    SetFlag(converted.flags, NativeUserSettingFlag::NoStatusBar, source.bNoStatusBar);
    SetFlag(converted.flags, NativeUserSettingFlag::VisibilityRulesOff,
        source.bVisibilityRulesOff);
    SetFlag(converted.flags, NativeUserSettingFlag::ChoseMultiplayer, source.bChoseMultiplayer);
    SetFlag(converted.flags, NativeUserSettingFlag::ChoseJoin, source.bChoseJoin);
    SetFlag(converted.flags, NativeUserSettingFlag::PartDialogVisible, source.bPartDlgVisible);
    SetFlag(converted.flags, NativeUserSettingFlag::CheatsEnabled, source.bCheatsEnabled);
    SetFlag(converted.flags, NativeUserSettingFlag::SectormapRotates, source.bSectormapRotates);
    SetFlag(converted.flags, NativeUserSettingFlag::NoAutoTarget, source.bNoAutoTarget);
    SetFlag(converted.flags, NativeUserSettingFlag::NoWinningConditions,
        source.bNoWinningConditions);
    SetFlag(converted.flags, NativeUserSettingFlag::NoHints, source.bNoHints);
    SetFlag(converted.flags, NativeUserSettingFlag::RightClickOption, source.bRightClickOption);
    SetFlag(converted.flags, NativeUserSettingFlag::InfoHighlights, source.bInfoHighlights);
    SetFlag(converted.flags, NativeUserSettingFlag::NoSupplies, source.bNoSupplies);
    SetFlag(converted.flags, NativeUserSettingFlag::NoDamage, source.bNoDamage);
    SetFlag(converted.flags, NativeUserSettingFlag::ShowGrids, source.bShowGrids);
    SetFlag(converted.flags, NativeUserSettingFlag::CheapMovement, source.bCheapMovement);
    SetFlag(converted.flags, NativeUserSettingFlag::HardwareRender, source.bHardwareRender);
    SetFlag(converted.flags, NativeUserSettingFlag::ConstantUpdateRate,
        source.bConstUpdateRate);
    SetFlag(converted.flags, NativeUserSettingFlag::HardwareCursor, source.bHardwareCursor);
    SetFlag(converted.flags, NativeUserSettingFlag::LockDiplomacy, source.bLockDiplomacy);
    SetFlag(converted.flags, NativeUserSettingFlag::SpectatorModeAllowed,
        source.bSpectatorModeAllowed);
    SetFlag(converted.flags, NativeUserSettingFlag::SpectatorModeOn, source.bSpectatorModeOn);
    SetFlag(converted.flags, NativeUserSettingFlag::Subtitles, source.bSubtitles);
    SetFlag(converted.flags, NativeUserSettingFlag::NetworkBandwidth, source.bNetworkBandwidth);
    converted.maxProjectiles = source.maxProjectiles;
    destination = converted;
    return true;
}

class NativeUserDefaults : public IUserDefaults
{
public:
    BEGIN_DACOM_MAP_INBOUND(NativeUserDefaults)
    DACOM_INTERFACE_ENTRY(IUserDefaults)
    END_DACOM_MAP()

    NativeUserDefaults() = default;
    ~NativeUserDefaults()
    {
        if (active_)
        {
            StoreDefaults();
            active_ = false;
            pUserDefaults = nullptr;
        }
    }

    bool Configure(const std::filesystem::path & path, const StartupPreferences & preferences)
    {
        if (active_ || pUserDefaults || path.empty()) return false;
        path_ = path;
        preferences_ = preferences;
        CopyToLegacy(preferences_.userSettings.Settings(), user_defaults_);
        pUserDefaults = &user_defaults_;
        active_ = true;
        return true;
    }

    BOOL32 LoadDefaults() override
    {
        PreferenceStore store;
        std::string error;
        const PreferenceLoadResult result = store.Load(path_, &error);
        if (result == PreferenceLoadResult::NotFound)
        {
            if (preferences_.selection.source == SettingsPathSource::Explicit) return FALSE;
            store.Clear();
        }
        else if (result != PreferenceLoadResult::Loaded)
            return FALSE;

        NativeUserSettingsOwner owner = preferences_.userSettings;
        const NativeUserSettingsOwnerLoadResult owner_result = owner.Load(store, &error);
        if (owner_result == NativeUserSettingsOwnerLoadResult::Invalid) return FALSE;
        preferences_.store = std::move(store);
        preferences_.storeResult = result;
        preferences_.userSettings = std::move(owner);
        preferences_.userSettingsResult = owner_result;
        CopyToLegacy(preferences_.userSettings.Settings(), user_defaults_);
        return TRUE;
    }

    BOOL32 StoreDefaults() override
    {
        user_defaults_.dwSize = sizeof(user_defaults_);
        user_defaults_.dwVersion = DefaultsVersion;
        NativeUserSettings settings;
        if (!CopyFromLegacy(user_defaults_, settings)) return FALSE;

        NativeUserSettingsOwner owner = preferences_.userSettings;
        owner.Settings() = settings;
        PreferenceStore store = preferences_.store;
        std::string error;
        if (!owner.Store(store, &error) || !Save(store)) return FALSE;
        preferences_.store = std::move(store);
        preferences_.userSettings = std::move(owner);
        preferences_.storeResult = PreferenceLoadResult::Loaded;
        preferences_.userSettingsResult = NativeUserSettingsOwnerLoadResult::Loaded;
        return TRUE;
    }

    bool RenamePlayerSettings(const char *previous,const char *name)
    {
        if (!previous || !name || !*previous || !*name) return false;
        const auto before=NativeUserSettingsOwner::PlayerPrefix(previous);
        const auto after=NativeUserSettingsOwner::PlayerPrefix(name);
        if (before==after) return true;
        NativeUserSettings settings;
        std::string error;
        const auto loaded=LoadNativeUserSettingsAtPrefix(preferences_.store,settings,before,&error);
        if (loaded==NativeUserSettingsLoadResult::NoSettings) return true;
        if (loaded!=NativeUserSettingsLoadResult::Loaded) return false;
        PreferenceStore updated=preferences_.store;
        if (!StoreNativeUserSettingsAtPrefix(settings,updated,after,&error)) return false;
        updated.RemovePrefix(before);
        if (!Save(updated)) return false;
        preferences_.store=std::move(updated);
        return true;
    }

    BOOL32 DeletePlayerDefaults(const char * player_name) override
    {
        if (!player_name || !player_name[0] || std::strlen(player_name) >= 64) return FALSE;
        PreferenceStore updated = preferences_.store;
        if (updated.RemovePrefix(NativeUserSettingsOwner::PlayerPrefix(player_name)) == 0)
            return FALSE;
        if (!Save(updated)) return FALSE;
        preferences_.store = std::move(updated);
        return TRUE;
    }

    U32 GetDataFromRegistry(const char * key, void * buffer, U32 buffer_size) override
    {
        if (!key) return 0;
        LegacyPreferenceValues values(preferences_.store);
        return values.GetBinary(key, buffer, buffer_size);
    }

    BOOL32 SetDataInRegistry(const char * key, void * buffer, U32 buffer_size) override
    {
        if (!key) return FALSE;
        PreferenceStore updated = preferences_.store;
        LegacyPreferenceValues values(updated);
        if (!values.SetBinary(key, buffer, buffer_size) || !Save(updated)) return FALSE;
        preferences_.store = std::move(updated);
        return TRUE;
    }

    BOOL32 GetStringFromRegistry(char * directory, U32 id) override
    {
        if (!directory) return FALSE;
        LegacyPreferenceValues values(preferences_.store);
        return values.GetString(std::to_string(id), directory, LegacyStringBytes) != 0;
    }

    BOOL32 SetStringInRegistry(const char * directory, U32 id) override
    {
        return SetNamedString(std::to_string(id), directory) ? TRUE : FALSE;
    }

    BOOL32 GetInputFilename(char * destination, U32) override
    {
        if (destination) destination[0] = 0;
        return FALSE;
    }

    BOOL32 GetOutputFilename(char * destination, U32) override
    {
        if (destination) destination[0] = 0;
        return FALSE;
    }

    BOOL32 SetNameInMRU(const char * file_name, U32 id, U32 position = 0) override
    {
        if (!file_name || std::strlen(file_name) >= LegacyStringBytes) return FALSE;
        PreferenceStore updated = preferences_.store;
        LegacyPreferenceValues values(updated);
        if (!values.SetMru(id, position, file_name) || !Save(updated)) return FALSE;
        preferences_.store = std::move(updated);
        return TRUE;
    }

    BOOL32 GetNameInMRU(char * file_name, U32 id, U32 position = 0) override
    {
        if (!file_name) return FALSE;
        file_name[0] = 0;
        LegacyPreferenceValues values(preferences_.store);
        std::string value;
        if (!values.GetMru(id, position, value) || value.size() >= LegacyStringBytes) return FALSE;
        std::memcpy(file_name, value.c_str(), value.size() + 1);
        return TRUE;
    }

    S32 FindNameInMRU(const char * file_name, U32 id) override
    {
        if (!file_name) return -1;
        LegacyPreferenceValues values(preferences_.store);
        return values.FindMru(id, file_name);
    }

    BOOL32 RemoveNameFromMRU(const char * file_name, U32 id) override
    {
        if (!file_name) return FALSE;
        PreferenceStore updated = preferences_.store;
        LegacyPreferenceValues values(updated);
        values.RemoveMruValue(id, file_name);
        if (!Save(updated)) return FALSE;
        preferences_.store = std::move(updated);
        return FALSE;
    }

    BOOL32 RemoveNameFromMRU(U32 id, U32 position) override
    {
        PreferenceStore updated = preferences_.store;
        LegacyPreferenceValues values(updated);
        if (!values.RemoveMruAt(id, position) || !Save(updated)) return FALSE;
        preferences_.store = std::move(updated);
        return TRUE;
    }

    BOOL32 InsertNameIntoMRU(const char * file_name, U32 id) override
    {
        if (!file_name || std::strlen(file_name) >= LegacyStringBytes) return FALSE;
        PreferenceStore updated = preferences_.store;
        LegacyPreferenceValues values(updated);
        if (!values.InsertMru(id, file_name) || !Save(updated)) return FALSE;
        preferences_.store = std::move(updated);
        return TRUE;
    }

    BOOL32 GetUserData(const C8 *, const C8 *, void *, U32) override { return FALSE; }
    BOOL32 GetScriptData(void *, const C8 *, void *, U32) override { return FALSE; }

    U32 GetStringFromRegistry(const char * key, char * buffer, U32 buffer_size) override
    {
        if (!key) return 0;
        LegacyPreferenceValues values(preferences_.store);
        return values.GetString(key, buffer, buffer_size);
    }

    U32 SetStringInRegistry(const char * key, const char * value) override
    {
        return key && SetNamedString(key, value) ? 1U : 0U;
    }

    U32 GetInstallStringFromRegistry(const char *, char * buffer, U32 buffer_size) override
    {
        if (buffer && buffer_size) buffer[0] = 0;
        return 0;
    }

    void ResetToDefaultSettings() override
    {
        preferences_.userSettings.Settings() = NativeUserSettings{};
        CopyToLegacy(preferences_.userSettings.Settings(), user_defaults_);
    }

    BOOL32 UbiEula() override { return FALSE; }

private:
    bool SetNamedString(const std::string & key, const char * value)
    {
        PreferenceStore updated = preferences_.store;
        LegacyPreferenceValues values(updated);
        if (!values.SetString(key, value) || !Save(updated)) return false;
        preferences_.store = std::move(updated);
        return true;
    }

    bool Save(const PreferenceStore & store) const
    {
        std::error_code filesystem_error;
        const std::filesystem::path parent = path_.parent_path();
        if (!parent.empty())
        {
            std::filesystem::create_directories(parent, filesystem_error);
            if (filesystem_error) return false;
        }
        std::string error;
        return store.Save(path_, &error);
    }

    std::filesystem::path path_;
    StartupPreferences preferences_;
    USER_DEFAULTS user_defaults_{};
    bool active_ = false;
};

class NativeUserDefaultsFactory : public IComponentFactory
{
public:
    NativeUserDefaultsFactory(
        std::filesystem::path path, const StartupPreferences & preferences) :
        path_(std::move(path)), preferences_(preferences)
    {
    }

    GENRESULT QueryInterface(const C8 * requested, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (!requested || std::strcmp(requested, IID_IComponentFactory) != 0)
            return GR_INTERFACE_UNSUPPORTED;
        *instance = this;
        AddRef();
        return GR_OK;
    }

    U32 AddRef() override { return ++references_; }
    U32 Release() override
    {
        if (references_) --references_;
        const U32 remaining = references_;
        if (!remaining) delete this;
        return remaining;
    }

    GENRESULT CreateInstance(DACOMDESC * descriptor, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (!descriptor || descriptor->size != sizeof(DACOMDESC) ||
            !descriptor->interface_name ||
            std::strcmp(descriptor->interface_name, interface_name) != 0)
            return GR_INTERFACE_UNSUPPORTED;

        auto * component = new (std::nothrow) DAComponent<NativeUserDefaults>;
        if (!component) return GR_OUT_OF_MEMORY;
        if (!component->Configure(path_, preferences_))
        {
            component->Release();
            return GR_GENERIC;
        }
        *instance = static_cast<IUserDefaults *>(component);
        return GR_OK;
    }

private:
    U32 references_ = 1;
    std::filesystem::path path_;
    StartupPreferences preferences_;
};
}

bool RenameNativePlayerDefaults(IUserDefaults *defaults,const char *previous,const char *name)
{
    auto *native=dynamic_cast<NativeUserDefaults *>(defaults);
    return native && native->RenamePlayerSettings(previous,name);
}

void RegisterNativeUserDefaults(
    ICOManager * manager,
    const std::filesystem::path & path,
    const StartupPreferences & preferences)
{
    if (!manager || path.empty()) return;
    IComponentFactory * factory =
        new (std::nothrow) NativeUserDefaultsFactory(path, preferences);
    if (!factory) return;
    manager->RegisterComponent(factory, interface_name);
    factory->Release();
}
}
