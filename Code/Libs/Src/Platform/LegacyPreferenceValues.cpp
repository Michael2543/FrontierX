#include "FrontierX/LegacyPreferenceValues.h"

#include <cstring>
#include <limits>

namespace FrontierX
{
LegacyPreferenceValues::LegacyPreferenceValues(PreferenceStore & store) noexcept : store_(store)
{
}

std::uint32_t LegacyPreferenceValues::GetBinary(
    const std::string & key, void * output, std::size_t capacity) const noexcept
{
    const PreferenceValue * value = store_.Find(key);
    if (!value || value->type != PreferenceType::Binary)
        return 0;
    if (value->bytes.size() > capacity ||
        value->bytes.size() > std::numeric_limits<std::uint32_t>::max())
        return BufferTooSmall;
    if (!value->bytes.empty() && !output)
        return 0;
    if (!value->bytes.empty())
        std::memcpy(output, value->bytes.data(), value->bytes.size());
    return static_cast<std::uint32_t>(value->bytes.size());
}

bool LegacyPreferenceValues::SetBinary(
    const std::string & key, const void * data, std::size_t size)
{
    return store_.SetBinary(key, data, size);
}

std::uint32_t LegacyPreferenceValues::GetString(
    const std::string & key, char * output, std::size_t capacity) const noexcept
{
    if (output && capacity)
    {
        output[0] = 0;
        output[capacity - 1] = 0;
    }

    const PreferenceValue * value = store_.Find(key);
    if (!value || value->type != PreferenceType::String)
        return 0;
    if (value->bytes.size() >= std::numeric_limits<std::uint32_t>::max())
        return BufferTooSmall;
    const std::size_t required = value->bytes.size() + 1;
    if (required > capacity)
        return BufferTooSmall;
    if (!output)
        return 0;
    if (!value->bytes.empty())
        std::memcpy(output, value->bytes.data(), value->bytes.size());
    output[value->bytes.size()] = 0;
    return static_cast<std::uint32_t>(required);
}

bool LegacyPreferenceValues::SetString(const std::string & key, const char * value)
{
    if (!value)
    {
        store_.Remove(key);
        return true;
    }
    return store_.SetString(key, value);
}

std::string LegacyPreferenceValues::MruKey(
    std::uint32_t resourceId, std::size_t position)
{
    return std::to_string(resourceId) + "-MRU" + std::to_string(position);
}

bool LegacyPreferenceValues::GetMru(
    std::uint32_t resourceId, std::size_t position, std::string & value) const
{
    value.clear();
    if (position >= MruEntries)
        return false;
    const PreferenceValue * stored = store_.Find(MruKey(resourceId, position));
    if (!stored || stored->type != PreferenceType::String)
        return false;
    value.assign(stored->bytes.begin(), stored->bytes.end());
    return true;
}

bool LegacyPreferenceValues::SetMru(
    std::uint32_t resourceId, std::size_t position, const std::string & value)
{
    return position < MruEntries && store_.SetString(MruKey(resourceId, position), value);
}

std::int32_t LegacyPreferenceValues::FindMru(
    std::uint32_t resourceId, const std::string & value) const
{
    for (std::size_t position = 0; position < MruEntries; ++position)
    {
        std::string stored;
        if (GetMru(resourceId, position, stored) && stored == value)
            return static_cast<std::int32_t>(position);
    }
    return -1;
}

bool LegacyPreferenceValues::RemoveMruAt(
    std::uint32_t resourceId, std::size_t position)
{
    if (position >= MruEntries)
        return false;

    while (position + 1 < MruEntries)
    {
        std::string next;
        if (!GetMru(resourceId, position + 1, next))
            break;
        if (!SetMru(resourceId, position, next))
            return false;
        ++position;
    }
    return store_.Remove(MruKey(resourceId, position));
}

bool LegacyPreferenceValues::RemoveMruValue(
    std::uint32_t resourceId, const std::string & value)
{
    std::int32_t position = 0;
    while ((position = FindMru(resourceId, value)) >= 0)
    {
        if (!RemoveMruAt(resourceId, static_cast<std::size_t>(position)))
            break;
    }

    // UserDefaults::RemoveNameFromMRU historically returns its zero-initialized
    // result even after successful removal. Preserve that observable contract.
    return false;
}

bool LegacyPreferenceValues::InsertMru(
    std::uint32_t resourceId, const std::string & value)
{
    for (std::size_t position = MruEntries - 1; position > 0; --position)
    {
        std::string previous;
        if (GetMru(resourceId, position - 1, previous) &&
            !SetMru(resourceId, position, previous))
            return false;
    }
    return SetMru(resourceId, 0, value);
}
}
