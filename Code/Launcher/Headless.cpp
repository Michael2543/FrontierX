#include <FrontierX/DataRoot.h>
#include <FrontierX/ProfileDocument.h>
#include <FrontierX/UtfMetadata.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace
{
int run(int argc, char ** argv)
{
    std::filesystem::path cliRoot, configPath, metadataPath, entryPath;
    for (int i = 1; i < argc; ++i)
    {
        const std::string option(argv[i]);
        if (option == "--help" && argc == 2)
        {
            std::cout << "Usage: frontierx_headless [--config FILE] [--data-root DIRECTORY] [--metadata FILE [--entry VIRTUAL_PATH]]\n"
                         "Checks native startup paths only; does not start the game.\n";
            return 0;
        }
        if ((option != "--config" && option != "--data-root" && option != "--metadata" && option != "--entry") || i + 1 == argc)
            throw std::invalid_argument("expected --config FILE, --metadata FILE or --data-root DIRECTORY");
        auto & destination = option == "--config" ? configPath : option == "--metadata" ? metadataPath : option == "--entry" ? entryPath : cliRoot;
        if (!destination.empty()) throw std::invalid_argument("duplicate option: " + option);
        const std::string value(argv[++i]);
        if (value.empty() || value.rfind("--", 0) == 0) throw std::invalid_argument("missing option value");
        destination = value;
    }

    if (!entryPath.empty() && metadataPath.empty()) throw std::invalid_argument("--entry requires --metadata");
    std::filesystem::path configRoot;
    if (!configPath.empty())
    {
        FrontierX::ProfileDocument config;
        std::string error;
        if (!config.LoadFile(configPath, error))
        { std::cerr << "Configuration error: " << error << '\n'; return 2; }
        auto root = config.Value("FrontierX", "DataRoot");
        if (!root || root->empty() || root->find_first_not_of(" \t\r\n") == std::string::npos)
        { std::cerr << "Configuration requires [FrontierX] DataRoot\n"; return 2; }
        // The legacy parser can retain CR immediately before a section marker.
        // Path values reject control characters instead of silently rewriting them.
        for (unsigned char c : *root)
            if (c < 32 || c == 127)
            { std::cerr << "Configuration DataRoot contains a control character\n"; return 2; }
        configRoot = *root;
        if (configRoot.is_relative()) configRoot = configPath.parent_path() / configRoot;
    }
    const char * environment = std::getenv("FRONTIERX_DATA_ROOT");
    const std::filesystem::path envRoot = environment ? environment : "";
    const auto root = FrontierX::Platform::SelectDataRootPath(cliRoot, envRoot,
        configRoot.empty() ? std::filesystem::path("Data") : configRoot);
    const char * source = !cliRoot.empty() ? "command-line" : !envRoot.empty() ? "environment" :
        !configRoot.empty() ? "configuration" : "default";
    std::cout << "Data root source: " << source << '\n';
    std::error_code error;
    if (!std::filesystem::is_directory(root, error) || error)
    { std::cerr << "Data root is not an accessible directory; set --data-root or FRONTIERX_DATA_ROOT\n"; return 3; }
    const auto entries = FrontierX::Platform::InspectDataRoot(root);
    for (const auto & entry : entries)
        std::cout << entry.expectedPath.generic_string() << ": "
                  << FrontierX::Platform::DataRootEntryStatusName(entry.status)
                  << (entry.required ? " [required]" : " [optional]") << '\n';
    if (!FrontierX::Platform::IsDataRootUsable(entries))
    { std::cerr << "Required data directories are missing or have incompatible names/types\n"; return 3; }
    if (!metadataPath.empty())
    {
        FrontierX::UtfMetadata metadata;
        std::string metadataError;
        if (!FrontierX::InspectUtfMetadata(metadataPath, metadata, metadataError))
        { std::cerr << "Metadata error: " << metadataError << '\n'; return 5; }
        std::cout << "UTF 1.01 metadata passed: " << metadata.directoryEntries << " directory slots, "
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
    catch (const std::invalid_argument & error) { std::cerr << "Usage error: " << error.what() << '\n'; return 1; }
    catch (const std::exception & error) { std::cerr << "Startup check failed: " << error.what() << '\n'; return 4; }
}
