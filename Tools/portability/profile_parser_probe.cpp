#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/ProfileParserFactory.h"
#include "IProfileParser.h"

#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{
void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

template <class Interface>
class InterfaceOwner
{
public:
    ~InterfaceOwner() { if (value_) value_->Release(); }
    Interface ** Receive() { return &value_; }
    Interface * Get() const { return value_; }
private:
    Interface * value_ = nullptr;
};

struct Callback final : IProfileCallback
{
    BOOL32 COMAPI ProfileCallback(IProfileParser2 *, const C8 * name,
                                  const C8 * value, void * context) override
    {
        Require(context == expectedContext, "profile callback context changed");
        values.emplace_back(name ? name : "", value ? value : "");
        return stopAfterFirst && values.size() == 1 ? FALSE : TRUE;
    }

    void * expectedContext = nullptr;
    bool stopAfterFirst = false;
    std::vector<std::pair<std::string, std::string>> values;
};

struct ScratchFile
{
    ScratchFile()
    {
        path = std::filesystem::temp_directory_path() /
            ("frontierx-production-profile-" + std::to_string(
                std::chrono::steady_clock::now().time_since_epoch().count()) + ".ini");
        writePath = path;
        writePath += ".write";
    }
    ~ScratchFile()
    {
        std::error_code error;
        std::filesystem::remove(path, error);
        std::filesystem::remove(writePath, error);
    }
    std::filesystem::path path;
    std::filesystem::path writePath;
};
}

int main()
{
    try
    {
        FrontierX::NativeComponentManager manager;
        RegisterProfileParsers(&manager);
        Require(manager.RegisteredCount() == 2, "profile factories were not registered");

        PROFPARSEDESC2 descriptor;
        InterfaceOwner<IAggregateComponent> aggregate;
        Require(manager.CreateInstance(&descriptor,
            reinterpret_cast<void **>(aggregate.Receive())) == GR_OK && aggregate.Get(),
            "production profile aggregate creation failed");
        InterfaceOwner<IProfileParser2> parser;
        Require(aggregate.Get()->QueryInterface(IID_IProfileParser2,
            reinterpret_cast<void **>(parser.Receive())) == GR_OK && parser.Get(),
            "IProfileParser2 query failed");
        InterfaceOwner<IProfileParser> legacyParser;
        Require(aggregate.Get()->QueryInterface(IID_IProfileParser,
            reinterpret_cast<void **>(legacyParser.Receive())) == GR_OK && legacyParser.Get(),
            "IProfileParser query failed");

        Require(parser.Get()->Initialize2(nullptr, 1) == GR_INVALID_PARMS &&
            parser.Get()->Initialize2(nullptr, 0) == GR_OK,
            "typed buffer initialization validation changed");

        const char text[] =
            "[First]\nKey=value\nEmpty=\n[Second]\nName=other\n[First]\nKey=last";
        Require(parser.Get()->Initialize2(text, static_cast<U32>(std::strlen(text))) == GR_OK,
            "buffer initialization failed");
        int callbackContext = 0;
        Callback sections;
        sections.expectedContext = &callbackContext;
        Require(parser.Get()->EnumerateSections(&sections, &callbackContext) &&
            sections.values.size() == 3 && sections.values[0].first == "First" &&
            sections.values[1].first == "Second" && sections.values[2].first == "First",
            "section enumeration changed");
        Callback stopped;
        stopped.expectedContext = &callbackContext;
        stopped.stopAfterFirst = true;
        Require(!parser.Get()->EnumerateSections(&stopped, &callbackContext) &&
            stopped.values.size() == 1, "section callback stop result changed");

        HANDLE section = parser.Get()->CreateSection("FIRST", IProfileParser::PP_OPENEXISTING);
        Require(section, "case-insensitive section lookup failed");
        char output[32]{};
        Require(parser.Get()->ReadKeyValue(section, "key", output, sizeof(output)) == 5 &&
            std::strcmp(output, "value") == 0, "key lookup changed");
        Require(parser.Get()->ReadProfileLine(section, 0, output, sizeof(output)) == 9 &&
            std::strcmp(output, "Key=value") == 0, "profile line lookup changed");
        Callback keys;
        keys.expectedContext = &callbackContext;
        Require(parser.Get()->EnumerateKeys(&keys, section, &callbackContext) &&
            keys.values.size() == 2 && keys.values[0] == std::make_pair(
                std::string("Key"), std::string("value")) &&
            keys.values[1].first == "Empty", "key enumeration changed");
        Require(parser.Get()->CloseSection(section), "section close failed");

        ScratchFile scratch;
        {
            std::ofstream file(scratch.path, std::ios::binary);
            file << "[File]\nPath=Data\n";
            Require(bool(file), "could not write profile fixture");
        }
        const std::string path = scratch.path.string();
        Require(parser.Get()->Initialize(path.c_str(), IProfileParser::READ_ACCESS) == GR_OK,
            "native file initialization failed");
        section = parser.Get()->CreateSection("file", IProfileParser::PP_OPENEXISTING);
        Require(section && parser.Get()->ReadKeyValue(section, "PATH", output, sizeof(output)) == 4 &&
            std::strcmp(output, "Data") == 0, "native file parsing changed");
        std::filesystem::path missingPath = scratch.path;
        missingPath += ".missing";
        const std::string missing = missingPath.string();
        Require(parser.Get()->Initialize(missing.c_str(),
            IProfileParser::READ_ACCESS) == GR_FILE_ERROR,
            "missing native profile did not report a file error");
        Require(parser.Get()->ReadKeyValue(section, "Path", output, sizeof(output)) == 4 &&
            std::strcmp(output, "Data") == 0,
            "failed native profile load changed the prior parser state");
        const std::string writePath = scratch.writePath.string();
        Require(parser.Get()->Initialize(writePath.c_str(), IProfileParser::WRITE_ACCESS) == GR_OK &&
            std::filesystem::is_regular_file(scratch.writePath),
            "native writable profile did not create an empty file");

        std::cout << "production profile parser passed\n";
        return 0;
    }
    catch (const std::exception & error)
    {
        std::cerr << "production profile parser probe failed: " << error.what() << '\n';
        return 1;
    }
}
