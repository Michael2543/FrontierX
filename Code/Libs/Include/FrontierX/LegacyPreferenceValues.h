#ifndef FRONTIERX_LEGACY_PREFERENCE_VALUES_H
#define FRONTIERX_LEGACY_PREFERENCE_VALUES_H

#include "FrontierX/PreferenceStore.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace FrontierX
{
class LegacyPreferenceValues
{
public:
    static constexpr std::uint32_t BufferTooSmall = 0xffffffffU;
    static constexpr std::size_t MruEntries = 4;

    explicit LegacyPreferenceValues(PreferenceStore & store) noexcept;

    std::uint32_t GetBinary(const std::string & key, void * output,
                            std::size_t capacity) const noexcept;
    bool SetBinary(const std::string & key, const void * data, std::size_t size);
    std::uint32_t GetString(const std::string & key, char * output,
                            std::size_t capacity) const noexcept;
    bool SetString(const std::string & key, const char * value);

    bool GetMru(std::uint32_t resourceId, std::size_t position,
                std::string & value) const;
    bool SetMru(std::uint32_t resourceId, std::size_t position,
                const std::string & value);
    std::int32_t FindMru(std::uint32_t resourceId, const std::string & value) const;
    bool RemoveMruAt(std::uint32_t resourceId, std::size_t position);
    bool RemoveMruValue(std::uint32_t resourceId, const std::string & value);
    bool InsertMru(std::uint32_t resourceId, const std::string & value);

private:
    static std::string MruKey(std::uint32_t resourceId, std::size_t position);

    PreferenceStore & store_;
};
}

#endif
