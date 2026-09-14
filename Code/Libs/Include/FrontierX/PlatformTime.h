#ifndef FRONTIERX_PLATFORM_TIME_H
#define FRONTIERX_PLATFORM_TIME_H

#include <cstdint>

namespace FrontierX
{
namespace Platform
{

std::uint64_t MonotonicMicroseconds() noexcept;
std::uint32_t MonotonicMilliseconds() noexcept;
std::uint64_t ClampedElapsedMicroseconds(
	std::uint64_t previous,
	std::uint64_t current,
	std::uint64_t maximum) noexcept;
void SleepForMilliseconds(std::uint32_t milliseconds) noexcept;
void YieldThread() noexcept;

}
}

#endif