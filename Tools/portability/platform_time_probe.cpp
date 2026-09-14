#include <FrontierX/PlatformTime.h>

#include <cstdint>
#include <iostream>

int main()
{
	if (FrontierX::Platform::ClampedElapsedMicroseconds(100, 150, 250) != 50 ||
		FrontierX::Platform::ClampedElapsedMicroseconds(100, 500, 250) != 250 ||
		FrontierX::Platform::ClampedElapsedMicroseconds(500, 100, 250) != 250)
	{
		std::cerr << "Elapsed-time clamping produced an unexpected result\n";
		return 1;
	}

	const std::uint64_t beforeMicroseconds = FrontierX::Platform::MonotonicMicroseconds();
	const std::uint32_t before = FrontierX::Platform::MonotonicMilliseconds();
	FrontierX::Platform::YieldThread();
	FrontierX::Platform::SleepForMilliseconds(2);
	const std::uint32_t after = FrontierX::Platform::MonotonicMilliseconds();
	const std::uint64_t afterMicroseconds = FrontierX::Platform::MonotonicMicroseconds();
	const std::uint32_t elapsed = after - before;

	if (elapsed < 1 || afterMicroseconds <= beforeMicroseconds)
	{
		std::cerr << "Monotonic clock did not advance across a timed wait\n";
		return 1;
	}

	return 0;
}