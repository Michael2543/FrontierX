#include "FrontierX/NativeUserSettings.h"

#include <cmath>
#include <cstring>
#include <limits>
#include <utility>

namespace FrontierX
{
namespace
{
constexpr std::uint32_t SchemaVersion = 1;
constexpr const char * SchemaKey = "UserDefaults.Schema";

void SetError(std::string * error, const std::string & message)
{
    if (error) *error = message;
}

std::uint32_t FloatBits(float value) noexcept
{
    static_assert(sizeof(float) == sizeof(std::uint32_t), "settings require 32-bit float");
    static_assert(std::numeric_limits<float>::is_iec559,
                  "settings require IEC 60559 single-precision float");
    std::uint32_t result = 0;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

float BitsFloat(std::uint32_t value) noexcept
{
    float result = 0.0F;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

std::string Key(std::string_view prefix, std::string_view name)
{
    std::string result(prefix);
    result.append(name);
    return result;
}

bool PutU32(PreferenceStore & store, std::string_view prefix, const char * key,
            std::uint32_t value)
{
    const std::uint8_t bytes[] = {
        static_cast<std::uint8_t>(value),
        static_cast<std::uint8_t>(value >> 8U),
        static_cast<std::uint8_t>(value >> 16U),
        static_cast<std::uint8_t>(value >> 24U),
    };
    return store.SetBinary(Key(prefix, key), bytes, sizeof(bytes));
}

bool ReadU32(const PreferenceStore & store, std::string_view prefix, const char * key,
             std::uint32_t & value, std::string * error)
{
    const std::string fullKey = Key(prefix, key);
    const PreferenceValue * stored = store.Find(fullKey);
    if (stored == nullptr || stored->type != PreferenceType::Binary || stored->bytes.size() != 4)
    {
        SetError(error, std::string("missing or invalid native user setting: ") + fullKey);
        return false;
    }
    value = static_cast<std::uint32_t>(stored->bytes[0]) |
        (static_cast<std::uint32_t>(stored->bytes[1]) << 8U) |
        (static_cast<std::uint32_t>(stored->bytes[2]) << 16U) |
        (static_cast<std::uint32_t>(stored->bytes[3]) << 24U);
    return true;
}

bool PutS32(PreferenceStore & store, std::string_view prefix, const char * key,
            std::int32_t value)
{
    return PutU32(store, prefix, key, static_cast<std::uint32_t>(value));
}

bool ReadS32(const PreferenceStore & store, std::string_view prefix, const char * key,
             std::int32_t & value, std::string * error)
{
    std::uint32_t bits = 0;
    if (!ReadU32(store, prefix, key, bits, error)) return false;
    std::memcpy(&value, &bits, sizeof(value));
    return true;
}

bool PutSound(PreferenceStore & store, std::string_view keyPrefix, const char * name,
              const NativeSoundSettings & sound)
{
    const std::string mutedKey = std::string(name) + ".Muted";
    const std::string volumeKey = std::string(name) + ".Volume";
    return PutU32(store, keyPrefix, mutedKey.c_str(), sound.muted ? 1U : 0U) &&
        PutS32(store, keyPrefix, volumeKey.c_str(), sound.volume);
}

bool ReadSound(const PreferenceStore & store, std::string_view keyPrefix, const char * name,
               NativeSoundSettings & sound, std::string * error)
{
    std::uint32_t muted = 0;
    const std::string mutedKey = std::string(name) + ".Muted";
    const std::string volumeKey = std::string(name) + ".Volume";
    if (!ReadU32(store, keyPrefix, mutedKey.c_str(), muted, error) || muted > 1U ||
        !ReadS32(store, keyPrefix, volumeKey.c_str(), sound.volume, error))
    {
        if (muted > 1U) SetError(error, "native sound mute value is not boolean");
        return false;
    }
    sound.muted = muted != 0;
    return true;
}

bool Valid(const NativeUserSettings & settings)
{
    const auto fog = static_cast<std::int32_t>(settings.fogMode);
    return std::isfinite(settings.scrollRate) &&
        (fog == -1 || fog == 0 || fog == 1) &&
        (settings.flags & ~NativeUserSettingFlagMask) == 0;
}
}

bool StoreNativeUserSettingsAtPrefix(
    const NativeUserSettings & settings, PreferenceStore & store, std::string_view prefix,
    std::string * error)
{
    SetError(error, {});
    if (!Valid(settings))
    {
        SetError(error, "native user settings contain an invalid float, fog mode or flag");
        return false;
    }

    PreferenceStore updated = store;
    const bool stored =
        PutU32(updated, prefix, SchemaKey, SchemaVersion) &&
        PutS32(updated, prefix, "Window.MainX", settings.mainX) &&
        PutS32(updated, prefix, "Window.MainY", settings.mainY) &&
        PutS32(updated, prefix, "Window.Width", settings.mainWidth) &&
        PutS32(updated, prefix, "Window.Height", settings.mainHeight) &&
        PutU32(updated, prefix, "Window.ShowState", settings.showState) &&
        PutS32(updated, prefix, "Viewer.Width", settings.viewerWidth) &&
        PutS32(updated, prefix, "Viewer.Height", settings.viewerHeight) &&
        PutU32(updated, prefix, "Network.MinLatency", settings.minLatency) &&
        PutU32(updated, prefix, "Network.PacketLossPercent", settings.packetLossPercent) &&
        PutS32(updated, prefix, "Input.MouseSpeed", settings.mouseSpeed) &&
        PutU32(updated, prefix, "Camera.ScrollRate", FloatBits(settings.scrollRate)) &&
        PutSound(updated, prefix, "Sound.Music", settings.music) &&
        PutSound(updated, prefix, "Sound.Effects", settings.effects) &&
        PutSound(updated, prefix, "Sound.Comms", settings.comms) &&
        PutSound(updated, prefix, "Sound.Chat", settings.chat) &&
        PutS32(updated, prefix, "Video.GammaCorrection", settings.gammaCorrection) &&
        PutS32(updated, prefix, "Simulation.GameSpeed", settings.gameSpeed) &&
        PutS32(updated, prefix, "Simulation.FogMode",
               static_cast<std::int32_t>(settings.fogMode)) &&
        PutU32(updated, prefix, "UserDefaults.Flags", settings.flags) &&
        PutU32(updated, prefix, "Simulation.MaxProjectiles", settings.maxProjectiles);
    if (!stored)
    {
        SetError(error, "could not encode native user settings");
        return false;
    }
    store = std::move(updated);
    return true;
}

NativeUserSettingsLoadResult LoadNativeUserSettingsAtPrefix(
    const PreferenceStore & store, NativeUserSettings & settings, std::string_view prefix,
    std::string * error)
{
    SetError(error, {});
    if (store.Find(Key(prefix, SchemaKey)) == nullptr)
        return NativeUserSettingsLoadResult::NoSettings;

    std::uint32_t version = 0;
    if (!ReadU32(store, prefix, SchemaKey, version, error))
        return NativeUserSettingsLoadResult::Invalid;
    if (version != SchemaVersion)
    {
        SetError(error, "native user settings version is unsupported");
        return NativeUserSettingsLoadResult::Invalid;
    }

    NativeUserSettings parsed;
    std::int32_t fog = 0;
    std::uint32_t scroll_bits = 0;
    if (!ReadS32(store, prefix, "Window.MainX", parsed.mainX, error) ||
        !ReadS32(store, prefix, "Window.MainY", parsed.mainY, error) ||
        !ReadS32(store, prefix, "Window.Width", parsed.mainWidth, error) ||
        !ReadS32(store, prefix, "Window.Height", parsed.mainHeight, error) ||
        !ReadU32(store, prefix, "Window.ShowState", parsed.showState, error) ||
        !ReadS32(store, prefix, "Viewer.Width", parsed.viewerWidth, error) ||
        !ReadS32(store, prefix, "Viewer.Height", parsed.viewerHeight, error) ||
        !ReadU32(store, prefix, "Network.MinLatency", parsed.minLatency, error) ||
        !ReadU32(store, prefix, "Network.PacketLossPercent",
                 parsed.packetLossPercent, error) ||
        !ReadS32(store, prefix, "Input.MouseSpeed", parsed.mouseSpeed, error) ||
        !ReadU32(store, prefix, "Camera.ScrollRate", scroll_bits, error) ||
        !ReadSound(store, prefix, "Sound.Music", parsed.music, error) ||
        !ReadSound(store, prefix, "Sound.Effects", parsed.effects, error) ||
        !ReadSound(store, prefix, "Sound.Comms", parsed.comms, error) ||
        !ReadSound(store, prefix, "Sound.Chat", parsed.chat, error) ||
        !ReadS32(store, prefix, "Video.GammaCorrection", parsed.gammaCorrection, error) ||
        !ReadS32(store, prefix, "Simulation.GameSpeed", parsed.gameSpeed, error) ||
        !ReadS32(store, prefix, "Simulation.FogMode", fog, error) ||
        !ReadU32(store, prefix, "UserDefaults.Flags", parsed.flags, error) ||
        !ReadU32(store, prefix, "Simulation.MaxProjectiles",
                 parsed.maxProjectiles, error))
        return NativeUserSettingsLoadResult::Invalid;

    parsed.scrollRate = BitsFloat(scroll_bits);
    parsed.fogMode = static_cast<NativeFogMode>(fog);
    if (!Valid(parsed))
    {
        SetError(error, "native user settings contain an invalid float, fog mode or flag");
        return NativeUserSettingsLoadResult::Invalid;
    }
    settings = parsed;
    return NativeUserSettingsLoadResult::Loaded;
}

bool StoreNativeUserSettings(const NativeUserSettings & settings, PreferenceStore & store,
                             std::string * error)
{
    return StoreNativeUserSettingsAtPrefix(settings, store, {}, error);
}

NativeUserSettingsLoadResult LoadNativeUserSettings(
    const PreferenceStore & store, NativeUserSettings & settings, std::string * error)
{
    return LoadNativeUserSettingsAtPrefix(store, settings, {}, error);
}
}
