#ifndef FRONTIERX_NATIVE_DISK_SPACE_H
#define FRONTIERX_NATIVE_DISK_SPACE_H

#include <cstdint>

namespace FrontierX
{
namespace Platform
{
// Checks free bytes on the filesystem containing path; null selects cwd.
// Leaves output unchanged on failure.
bool AvailableDiskBytes(const char * path, std::uint64_t & output) noexcept;
}
}

#endif
