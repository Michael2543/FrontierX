#include <FrontierX/DataRoot.h>
#include <FrontierX/StartupDataRoot.h>
#include <FrontierX/StartupPreferences.h>
#include <FrontierX/UtfMetadata.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace
{
int run(int argc, char ** argv)
{
    std::filesystem::path cliRoot, configPath, metadataPath, entryPath, preferencesPath;
    for (int i = 1; i < argc; ++i)
    {
        const std::string option(argv[i]);
        if (option == "--help" && argc == 2)
        {
            std::cout << "Usage: frontierx_headless [--config FILE] [--preferences FILE] "
                         "[--data-root DIRECTORY] [--metadata FILE [--entry VIRTUAL_PATH]]\n"
                         "Checks native startup paths only; does not start the game.\n";
            return 0;
        }
        if ((option != "--config" && option != "--preferences" && option != "--data-root" &&
             option != "--metadata" && option != "--entry") || i + 1 == argc)
            throw std::invalid_argument(
                "expected --config, --preferences, --metadata, --entry or --data-root");
        auto & destination = option == "--config" ? configPath :
            option == "--preferences" ? preferencesPath :
            option == "--metadata" ? metadataPath : option == "--entry" ? entryPath : cliRoot;
        if (!destination.empty()) throw std::invalid_argument("duplicate option: " + option);
        const std::string value(argv[++i]);
        if (value.empty() || value.rfind("--", 0) == 0)
            throw std::invalid_argument("missing option value");
        destination = value;
    }

    if (!entryPath.empty() && metadataPath.empty())
        throw std::invalid_argument("--entry requires --metadata");
    std::filesystem::path configRoot;
    if (!configPath.empty())
    {
        std::string error;
        if (!FrontierX::LoadConfiguredDataRoot(configPath, configRoot, error))
        { std::cerr << "Configuration error: " << error << '\n'; return 2; }
    }

    const char * xdg_config_environment = std::getenv("XDG_CONFIG_HOME");
    const char * home_environment = std::getenv("HOME");
    FrontierX::StartupPreferences preferences;
    std::string preferences_error;
    if (!FrontierX::LoadStartupPreferences(
        preferencesPath,
        xdg_config_environment ? xdg_config_environment : "",
        home_environment ? home_environment : "",
        preferences, &preferences_error))
    {
        std::cerr << "Preferences error: " << preferences_error << '\n';
        return 6;
    }
    std::cout << "Preferences source: "
              << FrontierX::SettingsPathSourceName(preferences.selection.source)
              << "; status: "
              << FrontierX::PreferenceLoadResultName(preferences.storeResult)
              << "; entries: " << preferences.store.Size()
              << "; user-defaults: "
              << FrontierX::UserSettingsLoadResultName(preferences.userSettingsResult)
              << "; settings-source: "
              << FrontierX::NativeUserSettingsSourceName(
                     preferences.userSettings.Source()) << '\n';

    const char * environment = std::getenv("FRONTIERX_DATA_ROOT");
    const std::filesystem::path envRoot = environment ? environment : "";
    const auto startupRoot = FrontierX::ResolveStartupDataRoot(cliRoot, envRoot, configRoot);
    std::cout << "Data root source: "
              << FrontierX::StartupDataRootSourceName(startupRoot.source) << '\n';
    if (!startupRoot.directoryAccessible)
    {
        std::cerr << "Data root is not an accessible directory; set --data-root or "
                     "FRONTIERX_DATA_ROOT\n";
        return 3;
    }
    for (const auto & entry : startupRoot.entries)
        std::cout << entry.expectedPath.generic_string() << ": "
                  << FrontierX::Platform::DataRootEntryStatusName(entry.status)
                  << (entry.required ? " [required]" : " [optional]") << '\n';
    if (!FrontierX::Platform::IsDataRootUsable(startupRoot.entries))
    {
        std::cerr << "Required data sources are missing or have incompatible names/types\n";
        return 3;
    }
    if (!metadataPath.empty())
    {
        FrontierX::UtfMetadata metadata;
        std::string metadataError;
        if (!FrontierX::InspectUtfMetadata(metadataPath, metadata, metadataError))
        { std::cerr << "Metadata error: " << metadataError << '\n'; return 5; }
        std::cout << "UTF 1.01 metadata passed: " << metadata.directoryEntries
                  << " directory slots, "
                  << metadata.nameBytesUsed << " used name bytes, " << metadata.reachableFiles
                  << " reachable files; payload contents are not validated.\n";
    }
    if (!entryPath.empty())
    {
        std::vector<unsigned char> bytes;
        std::string readError;
        if (!FrontierX::ReadUtfEntry(metadataPath, entryPath.generic_string(), bytes, readError))
        { std::cerr << "Entry error: " << readError << '\n'; return 5; }
        std::uint32_t fingerprint = 2166136261u;
        for (unsigned char byte : bytes) fingerprint = (fingerprint ^ byte) * 16777619u;
        std::cout << "Entry read: " << bytes.size() << " bytes; FNV-1a " << fingerprint
                  << "; schema not validated.\n";
    }
    std::cout << "Directory checks passed; game startup and media contents are not validated.\n";
    return 0;
}
}
int main(int argc, char ** argv)
{
    try { return run(argc, argv); }
    catch (const std::invalid_argument & error)
    {
        std::cerr << "Usage error: " << error.what() << '\n';
        return 1;
    }
    catch (const std::exception & error)
    {
        std::cerr << "Startup check failed: " << error.what() << '\n';
        return 4;
    }
}
