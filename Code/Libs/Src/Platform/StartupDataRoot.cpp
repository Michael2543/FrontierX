#include "FrontierX/StartupDataRoot.h"

#include "FrontierX/ProfileDocument.h"

namespace FrontierX
{
bool LoadConfiguredDataRoot(const std::filesystem::path & config_path,
                            std::filesystem::path & data_root,
                            std::string & error)
{
    data_root.clear();
    error.clear();
    ProfileDocument config;
    if (!config.LoadFile(config_path, error)) return false;
    const auto root = config.Value("FrontierX", "DataRoot");
    if (!root || root->empty() || root->find_first_not_of(" \t\r\n") == std::string::npos)
    {
        error = "configuration requires [FrontierX] DataRoot";
        return false;
    }
    for (const unsigned char character : *root)
    {
        if (character < 32 || character == 127)
        {
            error = "configuration DataRoot contains a control character";
            return false;
        }
    }
    data_root = *root;
    if (data_root.is_relative()) data_root = config_path.parent_path() / data_root;
    return true;
}

StartupDataRoot ResolveStartupDataRoot(const std::filesystem::path & command_line_path,
                                       const std::filesystem::path & environment_path,
                                       const std::filesystem::path & configuration_path)
{
    StartupDataRoot result;
    result.path = Platform::SelectDataRootPath(command_line_path, environment_path,
        configuration_path.empty() ? std::filesystem::path("Data") : configuration_path);
    result.source = !command_line_path.empty() ? StartupDataRootSource::CommandLine :
        !environment_path.empty() ? StartupDataRootSource::Environment :
        !configuration_path.empty() ? StartupDataRootSource::Configuration :
        StartupDataRootSource::Default;
    std::error_code filesystem_error;
    result.directoryAccessible = std::filesystem::is_directory(result.path, filesystem_error) &&
        !filesystem_error;
    if (result.directoryAccessible) result.entries = Platform::InspectDataRoot(result.path);
    return result;
}

const char * StartupDataRootSourceName(StartupDataRootSource source) noexcept
{
    switch (source)
    {
    case StartupDataRootSource::CommandLine:
        return "command-line";
    case StartupDataRootSource::Environment:
        return "environment";
    case StartupDataRootSource::Configuration:
        return "configuration";
    case StartupDataRootSource::Default:
        return "default";
    }
    return "unknown";
}
}
