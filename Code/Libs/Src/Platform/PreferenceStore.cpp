#include "FrontierX/PreferenceStore.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <limits>
#include <system_error>
#include <utility>

namespace FrontierX
{
namespace
{
constexpr std::array<std::uint8_t, 8> Magic{'F', 'X', 'S', 'E', 'T', '0', '0', '1'};
constexpr std::size_t FileHeaderBytes = Magic.size() + sizeof(std::uint32_t);
constexpr std::size_t EntryHeaderBytes = sizeof(std::uint32_t) + sizeof(std::uint32_t) +
    sizeof(std::uint32_t);

void SetError(std::string * error, const std::string & message)
{
    if (error) *error = message;
}

bool ValidKey(const std::string & key)
{
    if (key.empty() || key.size() > PreferenceStore::MaxKeyBytes) return false;
    return std::all_of(key.begin(), key.end(), [](unsigned char character) {
        return character >= 0x20U && character <= 0x7eU;
    });
}

void AppendU32(std::vector<std::uint8_t> & output, std::uint32_t value)
{
    output.push_back(static_cast<std::uint8_t>(value));
    output.push_back(static_cast<std::uint8_t>(value >> 8U));
    output.push_back(static_cast<std::uint8_t>(value >> 16U));
    output.push_back(static_cast<std::uint8_t>(value >> 24U));
}

bool ReadU32(const std::vector<std::uint8_t> & input, std::size_t & offset,
             std::uint32_t & value)
{
    if (offset > input.size() || input.size() - offset < sizeof(std::uint32_t)) return false;
    value = static_cast<std::uint32_t>(input[offset]) |
        (static_cast<std::uint32_t>(input[offset + 1]) << 8U) |
        (static_cast<std::uint32_t>(input[offset + 2]) << 16U) |
        (static_cast<std::uint32_t>(input[offset + 3]) << 24U);
    offset += sizeof(std::uint32_t);
    return true;
}

bool AppendBytes(std::vector<std::uint8_t> & output, const void * data, std::size_t size)
{
    if (size > PreferenceStore::MaxFileBytes - output.size()) return false;
    if (size == 0) return true;
    const auto * first = static_cast<const std::uint8_t *>(data);
    output.insert(output.end(), first, first + size);
    return true;
}
}

bool PreferenceStore::SetBinary(const std::string & key, const void * data, std::size_t size)
{
    if (!ValidKey(key) || size > MaxValueBytes || (size != 0 && data == nullptr)) return false;
    if (values_.find(key) == values_.end() && values_.size() >= MaxEntries) return false;

    PreferenceValue value;
    value.type = PreferenceType::Binary;
    if (size != 0)
    {
        const auto * first = static_cast<const std::uint8_t *>(data);
        value.bytes.assign(first, first + size);
    }
    values_[key] = std::move(value);
    return true;
}

bool PreferenceStore::SetString(const std::string & key, const std::string & value)
{
    if (value.size() > MaxValueBytes || value.find('\0') != std::string::npos) return false;
    if (!SetBinary(key, value.data(), value.size())) return false;
    values_[key].type = PreferenceType::String;
    return true;
}

bool PreferenceStore::Remove(const std::string & key) noexcept
{
    return values_.erase(key) != 0;
}

std::size_t PreferenceStore::RemovePrefix(std::string_view prefix) noexcept
{
    if (prefix.empty()) return 0;
    std::size_t removed = 0;
    for (auto entry = values_.begin(); entry != values_.end();)
    {
        if (entry->first.size() >= prefix.size() &&
            entry->first.compare(0, prefix.size(), prefix) == 0)
        {
            entry = values_.erase(entry);
            ++removed;
        }
        else
            ++entry;
    }
    return removed;
}

const PreferenceValue * PreferenceStore::Find(const std::string & key) const noexcept
{
    const auto found = values_.find(key);
    return found == values_.end() ? nullptr : &found->second;
}

std::size_t PreferenceStore::Size() const noexcept
{
    return values_.size();
}

void PreferenceStore::Clear() noexcept
{
    values_.clear();
}

PreferenceLoadResult PreferenceStore::Load(const std::filesystem::path & path, std::string * error)
{
    SetError(error, {});
    std::error_code filesystem_error;
    const auto size = std::filesystem::file_size(path, filesystem_error);
    if (filesystem_error)
    {
        std::error_code exists_error;
        if (!std::filesystem::exists(path, exists_error) && !exists_error)
            return PreferenceLoadResult::NotFound;
        SetError(error, "could not inspect preference file");
        return PreferenceLoadResult::IoError;
    }
    if (size > MaxFileBytes || size < FileHeaderBytes)
    {
        SetError(error, "preference file size is invalid");
        return PreferenceLoadResult::Invalid;
    }

    std::ifstream stream(path, std::ios::binary);
    if (!stream)
    {
        SetError(error, "could not open preference file");
        return PreferenceLoadResult::IoError;
    }
    std::vector<std::uint8_t> input(static_cast<std::size_t>(size));
    stream.read(reinterpret_cast<char *>(input.data()), static_cast<std::streamsize>(input.size()));
    if (!stream || stream.peek() != std::ifstream::traits_type::eof())
    {
        SetError(error, "could not read preference file exactly");
        return PreferenceLoadResult::IoError;
    }

    if (!std::equal(Magic.begin(), Magic.end(), input.begin()))
    {
        SetError(error, "preference file signature is invalid");
        return PreferenceLoadResult::Invalid;
    }

    std::size_t offset = Magic.size();
    std::uint32_t entry_count = 0;
    if (!ReadU32(input, offset, entry_count) || entry_count > MaxEntries)
    {
        SetError(error, "preference entry count is invalid");
        return PreferenceLoadResult::Invalid;
    }

    PreferenceStore parsed;
    for (std::uint32_t entry = 0; entry < entry_count; ++entry)
    {
        std::uint32_t key_size = 0;
        std::uint32_t type_word = 0;
        std::uint32_t value_size = 0;
        if (!ReadU32(input, offset, key_size) || !ReadU32(input, offset, type_word) ||
            !ReadU32(input, offset, value_size) || key_size == 0 || key_size > MaxKeyBytes ||
            value_size > MaxValueBytes || offset > input.size() ||
            key_size > input.size() - offset || value_size > input.size() - offset - key_size)
        {
            SetError(error, "preference entry is truncated or out of bounds");
            return PreferenceLoadResult::Invalid;
        }

        std::string key(reinterpret_cast<const char *>(input.data() + offset), key_size);
        offset += key_size;
        if (!ValidKey(key) || parsed.Find(key) != nullptr ||
            (type_word != static_cast<std::uint32_t>(PreferenceType::Binary) &&
             type_word != static_cast<std::uint32_t>(PreferenceType::String)))
        {
            SetError(error, "preference entry key or type is invalid");
            return PreferenceLoadResult::Invalid;
        }

        const auto type = static_cast<PreferenceType>(type_word);
        const void * value_data = input.data() + offset;
        bool stored = type == PreferenceType::String
            ? parsed.SetString(key, std::string(static_cast<const char *>(value_data), value_size))
            : parsed.SetBinary(key, value_data, value_size);
        if (!stored)
        {
            SetError(error, "preference entry value is invalid");
            return PreferenceLoadResult::Invalid;
        }
        offset += value_size;
    }
    if (offset != input.size())
    {
        SetError(error, "preference file has trailing bytes");
        return PreferenceLoadResult::Invalid;
    }

    values_.swap(parsed.values_);
    return PreferenceLoadResult::Loaded;
}

bool PreferenceStore::Save(const std::filesystem::path & path, std::string * error) const
{
    SetError(error, {});
    std::error_code filesystem_error;
    if (values_.size() > MaxEntries)
    {
        SetError(error, "too many preference entries");
        return false;
    }

    std::vector<std::uint8_t> output(Magic.begin(), Magic.end());
    AppendU32(output, static_cast<std::uint32_t>(values_.size()));
    for (const auto & item : values_)
    {
        if (!ValidKey(item.first) || item.second.bytes.size() > MaxValueBytes ||
            (item.second.type != PreferenceType::Binary &&
             item.second.type != PreferenceType::String))
        {
            SetError(error, "preference store contains an invalid entry");
            return false;
        }
        if (output.size() > MaxFileBytes - EntryHeaderBytes)
        {
            SetError(error, "preference file exceeds its size limit");
            return false;
        }
        AppendU32(output, static_cast<std::uint32_t>(item.first.size()));
        AppendU32(output, static_cast<std::uint32_t>(item.second.type));
        AppendU32(output, static_cast<std::uint32_t>(item.second.bytes.size()));
        if (!AppendBytes(output, item.first.data(), item.first.size()) ||
            !AppendBytes(output, item.second.bytes.data(), item.second.bytes.size()))
        {
            SetError(error, "preference file exceeds its size limit");
            return false;
        }
    }

    std::filesystem::path temporary = path;
    temporary += ".frontierx-tmp";
    {
        std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);
        if (!stream)
        {
            SetError(error, "could not create temporary preference file");
            return false;
        }
        stream.write(reinterpret_cast<const char *>(output.data()),
                     static_cast<std::streamsize>(output.size()));
        stream.close();
        if (!stream)
        {
            std::filesystem::remove(temporary, filesystem_error);
            SetError(error, "could not write temporary preference file");
            return false;
        }
    }

    std::filesystem::rename(temporary, path, filesystem_error);
    if (filesystem_error)
    {
        std::filesystem::remove(temporary, filesystem_error);
        SetError(error, "could not replace preference file");
        return false;
    }
    return true;
}
}
