#include "FrontierX/LegacyPreferenceValues.h"
#include "FrontierX/PreferenceStore.h"
#include "FrontierX/SettingsPath.h"

#include <array>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

class TemporaryDirectory
{
public:
    TemporaryDirectory()
    {
        const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = std::filesystem::temp_directory_path() /
            ("frontierx-preferences-" + std::to_string(suffix));
        if (!std::filesystem::create_directory(path_))
            throw std::runtime_error("could not create temporary preference directory");
    }

    ~TemporaryDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(path_, error);
    }

    const std::filesystem::path & Path() const noexcept
    {
        return path_;
    }

private:
    std::filesystem::path path_;
};

std::vector<std::uint8_t> ReadFile(const std::filesystem::path & path)
{
    std::ifstream stream(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}

void WriteFile(const std::filesystem::path & path, const std::vector<std::uint8_t> & bytes)
{
    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    stream.write(reinterpret_cast<const char *>(bytes.data()),
                 static_cast<std::streamsize>(bytes.size()));
    if (!stream) throw std::runtime_error("could not write mutated preference file");
}

void AppendU32(std::vector<std::uint8_t> & bytes, std::uint32_t value)
{
    bytes.push_back(static_cast<std::uint8_t>(value));
    bytes.push_back(static_cast<std::uint8_t>(value >> 8U));
    bytes.push_back(static_cast<std::uint8_t>(value >> 16U));
    bytes.push_back(static_cast<std::uint8_t>(value >> 24U));
}

void AppendBinaryEntry(std::vector<std::uint8_t> & bytes, const std::string & key)
{
    AppendU32(bytes, static_cast<std::uint32_t>(key.size()));
    AppendU32(bytes, static_cast<std::uint32_t>(FrontierX::PreferenceType::Binary));
    AppendU32(bytes, 0);
    bytes.insert(bytes.end(), key.begin(), key.end());
}

void RequireString(const FrontierX::PreferenceStore & store, const std::string & key,
                   const std::string & expected)
{
    const FrontierX::PreferenceValue * value = store.Find(key);
    Require(value != nullptr && value->type == FrontierX::PreferenceType::String,
        "string preference is missing or has the wrong type");
    Require(std::string(value->bytes.begin(), value->bytes.end()) == expected,
        "string preference value changed");
}
}

int main()
{
    try
    {
        TemporaryDirectory temporary;
        const auto path = temporary.Path() / "preferences.dat";
        const auto second_path = temporary.Path() / "preferences-copy.dat";

        auto selection = FrontierX::SelectSettingsPath("override.dat", "/xdg", "/home/user");
        Require(selection.source == FrontierX::SettingsPathSource::Explicit &&
            selection.path == "override.dat", "explicit settings path lost precedence");
        selection = FrontierX::SelectSettingsPath({}, "/xdg", "/home/user");
        Require(selection.source == FrontierX::SettingsPathSource::XdgConfigHome &&
            selection.path == "/xdg/frontierx/preferences.dat",
            "absolute XDG_CONFIG_HOME was not selected");
        selection = FrontierX::SelectSettingsPath({}, "relative-xdg", "/home/user");
        Require(selection.source == FrontierX::SettingsPathSource::HomeDefault &&
            selection.path == "/home/user/.config/frontierx/preferences.dat",
            "relative XDG_CONFIG_HOME was not ignored");
        selection = FrontierX::SelectSettingsPath({}, {}, "relative-home");
        Require(!selection && selection.path.empty(),
            "relative HOME unexpectedly produced a settings path");

        FrontierX::PreferenceStore legacyStore;
        FrontierX::LegacyPreferenceValues legacy(legacyStore);
        const std::array<std::uint8_t, 4> binary{1, 2, 3, 4};
        Require(legacy.SetBinary("Binary", binary.data(), binary.size()),
            "legacy binary setter failed");
        std::array<std::uint8_t, 4> binaryOutput{9, 9, 9, 9};
        Require(legacy.GetBinary("Binary", binaryOutput.data(), binaryOutput.size()) == 4 &&
            binaryOutput == binary, "legacy binary getter changed bytes/count");
        binaryOutput.fill(9);
        Require(legacy.GetBinary("Binary", binaryOutput.data(), 3) ==
                FrontierX::LegacyPreferenceValues::BufferTooSmall &&
            binaryOutput == std::array<std::uint8_t, 4>{9, 9, 9, 9},
            "legacy short binary buffer result or output changed");
        Require(legacy.GetBinary("Missing", binaryOutput.data(), binaryOutput.size()) == 0,
            "missing legacy binary value did not return zero");

        Require(legacy.SetString("Player", "Blackwell"), "legacy string setter failed");
        std::array<char, 16> stringOutput{};
        Require(legacy.GetString("Player", stringOutput.data(), stringOutput.size()) == 10 &&
            std::string(stringOutput.data()) == "Blackwell",
            "legacy string getter did not include the terminator in its count");
        stringOutput.fill('!');
        Require(legacy.GetString("Player", stringOutput.data(), 4) ==
                FrontierX::LegacyPreferenceValues::BufferTooSmall &&
            stringOutput[0] == 0 && stringOutput[3] == 0,
            "legacy short string buffer sentinel/termination changed");
        Require(legacy.GetString("Binary", stringOutput.data(), stringOutput.size()) == 0,
            "wrong-type legacy string value was accepted");
        Require(legacy.SetString("EmptyString", ""), "empty legacy string setter failed");
        Require(legacy.GetString("EmptyString", stringOutput.data(), stringOutput.size()) == 1 &&
            stringOutput[0] == 0, "empty legacy string count changed");
        Require(legacy.SetString("Player", nullptr) &&
            legacy.GetString("Player", stringOutput.data(), stringOutput.size()) == 0,
            "null legacy string did not delete the value");

        constexpr std::uint32_t resourceId = 42;
        Require(legacy.InsertMru(resourceId, "one") &&
            legacy.InsertMru(resourceId, "two") &&
            legacy.InsertMru(resourceId, "three") &&
            legacy.InsertMru(resourceId, "four") &&
            legacy.InsertMru(resourceId, "five"),
            "legacy MRU insertion failed");
        const std::array<std::string, 4> expectedMru{"five", "four", "three", "two"};
        for (std::size_t position = 0; position < expectedMru.size(); ++position)
        {
            std::string value;
            Require(legacy.GetMru(resourceId, position, value) &&
                value == expectedMru[position], "legacy MRU shift/order changed");
        }
        Require(legacy.FindMru(resourceId, "three") == 2 &&
            legacy.FindMru(resourceId, "missing") == -1,
            "legacy MRU lookup changed");
        Require(legacy.RemoveMruAt(resourceId, 1),
            "legacy MRU positional removal failed");
        std::string mruValue;
        Require(legacy.GetMru(resourceId, 1, mruValue) && mruValue == "three" &&
            legacy.GetMru(resourceId, 2, mruValue) && mruValue == "two" &&
            !legacy.GetMru(resourceId, 3, mruValue) && mruValue.empty(),
            "legacy MRU compaction/gap behavior changed");
        Require(legacy.SetMru(resourceId, 3, "three") &&
            !legacy.RemoveMruValue(resourceId, "three") &&
            legacy.FindMru(resourceId, "three") == -1,
            "legacy duplicate MRU removal or historical return value changed");
        Require(!legacy.SetMru(resourceId, 4, "out-of-range") &&
            !legacy.RemoveMruAt(resourceId, 4),
            "legacy MRU accepted an out-of-range position");

        FrontierX::PreferenceStore store;
        Require(store.SetString("Existing", "preserved"), "could not seed preference store");
        std::string error = "stale";
        Require(store.Load(path, &error) == FrontierX::PreferenceLoadResult::NotFound,
            "missing preference file did not report NotFound");
        Require(error.empty(), "missing preference file reported an error");
        RequireString(store, "Existing", "preserved");

        const std::array<std::uint8_t, 4> width{0x00, 0x04, 0x00, 0x00};
        Require(store.SetBinary("CQWindowWidth", width.data(), width.size()),
            "could not store binary preference");
        Require(store.SetString("CQPlayerName", "Blackwell"),
            "could not store string preference");
        Require(store.SetBinary("Empty", nullptr, 0), "could not store empty binary preference");
        Require(!store.SetBinary("Null", nullptr, 1), "accepted null non-empty binary value");
        Require(!store.SetString("Bad\nKey", "x"), "accepted control character in key");
        Require(!store.SetString("Embedded", std::string("a\0b", 3)),
            "accepted embedded NUL in string value");

        Require(store.Save(path, &error), error.c_str());
        Require(store.Save(second_path, &error), error.c_str());
        const auto saved = ReadFile(path);
        Require(saved == ReadFile(second_path), "preference serialization is not deterministic");
        Require(saved.size() >= 12 && std::string(saved.begin(), saved.begin() + 8) == "FXSET001",
            "preference signature changed");
        Require(saved[8] == 4 && saved[9] == 0 && saved[10] == 0 && saved[11] == 0,
            "preference entry count is not fixed-width little-endian");

        FrontierX::PreferenceStore loaded;
        Require(loaded.Load(path, &error) == FrontierX::PreferenceLoadResult::Loaded,
            error.c_str());
        Require(loaded.Size() == 4, "loaded preference count changed");
        RequireString(loaded, "CQPlayerName", "Blackwell");
        const FrontierX::PreferenceValue * loaded_width = loaded.Find("CQWindowWidth");
        Require(loaded_width != nullptr &&
            loaded_width->type == FrontierX::PreferenceType::Binary &&
            loaded_width->bytes == std::vector<std::uint8_t>(width.begin(), width.end()),
            "binary preference value changed");
        Require(loaded.Remove("Existing") && !loaded.Remove("Existing"),
            "preference removal result changed");

        Require(store.SetString("CQPlayerName", "Hawkes"),
            "could not update string preference");
        Require(store.Save(path, &error), error.c_str());
        Require(!std::filesystem::exists(path.string() + ".frontierx-tmp"),
            "successful replacement left its temporary file behind");
        FrontierX::PreferenceStore replaced;
        Require(replaced.Load(path, &error) == FrontierX::PreferenceLoadResult::Loaded,
            error.c_str());
        RequireString(replaced, "CQPlayerName", "Hawkes");

        auto corrupted = saved;
        corrupted[0] ^= 0xffU;
        WriteFile(path, corrupted);
        Require(loaded.Load(path, &error) == FrontierX::PreferenceLoadResult::Invalid,
            "invalid signature was accepted");
        Require(loaded.Find("CQPlayerName") != nullptr,
            "failed load replaced the existing preference state");

        corrupted = saved;
        corrupted.push_back(0);
        WriteFile(path, corrupted);
        Require(loaded.Load(path, &error) == FrontierX::PreferenceLoadResult::Invalid,
            "trailing preference bytes were accepted");

        corrupted.assign(saved.begin(), saved.end() - 1);
        WriteFile(path, corrupted);
        Require(loaded.Load(path, &error) == FrontierX::PreferenceLoadResult::Invalid,
            "truncated preference value was accepted");

        corrupted = saved;
        corrupted[16] = 3;
        WriteFile(path, corrupted);
        Require(loaded.Load(path, &error) == FrontierX::PreferenceLoadResult::Invalid,
            "unknown preference type was accepted");

        corrupted.assign({'F', 'X', 'S', 'E', 'T', '0', '0', '1'});
        AppendU32(corrupted, 2);
        AppendBinaryEntry(corrupted, "Duplicate");
        AppendBinaryEntry(corrupted, "Duplicate");
        WriteFile(path, corrupted);
        Require(loaded.Load(path, &error) == FrontierX::PreferenceLoadResult::Invalid,
            "duplicate preference key was accepted");

        std::cout << "portable preference store semantics passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "preference store probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
