#ifndef FRONTIERX_STARTUP_DATA_ROOT_H
#define FRONTIERX_STARTUP_DATA_ROOT_H

#include "FrontierX/DataRoot.h"

#include <filesystem>
#include <string>
#include <vector>

namespace FrontierX
{
enum class StartupDataRootSource
{
    CommandLine,
    Environment,
    Configuration,
    Default
};

struct StartupDataRoot
{
    std::filesystem::path path;
    StartupDataRootSource source = StartupDataRootSource::Default;
    bool directoryAccessible = false;
    std::vector<Platform::DataRootEntry> entries;
};

bool LoadConfiguredDataRoot(const std::filesystem::path & config_path,
                            std::filesystem::path & data_root,
                            std::string & error);
StartupDataRoot ResolveStartupDataRoot(const std::filesystem::path & command_line_path,
                                       const std::filesystem::path & environment_path,
                                       const std::filesystem::path & configuration_path);
const char * StartupDataRootSourceName(StartupDataRootSource source) noexcept;
}

#endif
