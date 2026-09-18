#ifndef FRONTIERX_PREFERENCE_STORE_H
#define FRONTIERX_PREFERENCE_STORE_H

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace FrontierX
{
enum class PreferenceType : std::uint8_t
{
    Binary = 1,
    String = 2,
};

struct PreferenceValue
{
    PreferenceType type = PreferenceType::Binary;
    std::vector<std::uint8_t> bytes;
};

enum class PreferenceLoadResult
{
    Loaded,
    NotFound,
    Invalid,
    IoError,
};

class PreferenceStore
{
public:
    static constexpr std::size_t MaxFileBytes = 1024U * 1024U;
    static constexpr std::size_t MaxEntries = 4096;
    static constexpr std::size_t MaxKeyBytes = 255;
    static constexpr std::size_t MaxValueBytes = 64U * 1024U;

    bool SetBinary(const std::string & key, const void * data, std::size_t size);
    bool SetString(const std::string & key, const std::string & value);
    bool Remove(const std::string & key) noexcept;
    std::size_t RemovePrefix(std::string_view prefix) noexcept;
    const PreferenceValue * Find(const std::string & key) const noexcept;
    std::size_t Size() const noexcept;
    void Clear() noexcept;

    PreferenceLoadResult Load(const std::filesystem::path & path, std::string * error = nullptr);
    bool Save(const std::filesystem::path & path, std::string * error = nullptr) const;

private:
    std::map<std::string, PreferenceValue> values_;
};
}

#endif
