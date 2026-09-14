#include <FrontierX/PlatformTime.h>

#include <chrono>
#include <thread>

namespace FrontierX
{
namespace Platform
{

std::uint64_t MonotonicMicroseconds() noexcept
{
	const auto elapsed = std::chrono::steady_clock::now().time_since_epoch();
	return static_cast<std::uint64_t>(
		std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());
}

std::uint32_t MonotonicMilliseconds() noexcept
{
	return static_cast<std::uint32_t>(MonotonicMicroseconds() / 1000);
}

std::uint64_t ClampedElapsedMicroseconds(
	std::uint64_t previous,
	std::uint64_t current,
	std::uint64_t maximum) noexcept
{
	if (current < previous || current - previous > maximum)
		return maximum;

	return current - previous;
}

void SleepForMilliseconds(std::uint32_t milliseconds) noexcept
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void YieldThread() noexcept
{
	std::this_thread::yield();
}

}
}