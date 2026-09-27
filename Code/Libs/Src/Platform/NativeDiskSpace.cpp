#include "FrontierX/NativeDiskSpace.h"

#include <filesystem>
#include <system_error>

bool FrontierX::Platform::AvailableDiskBytes(const char * path,
                                             std::uint64_t & output) noexcept
{
    try
    {
        std::error_code error;
        const std::filesystem::path target = path ? std::filesystem::path(path) :
            std::filesystem::current_path(error);
        if (error || target.empty()) return false;
        const auto space = std::filesystem::space(target, error);
        if (error || space.available == static_cast<std::uintmax_t>(-1)) return false;
        output = static_cast<std::uint64_t>(space.available);
        return true;
    }
    catch (...)
    {
        return false;
    }
}
