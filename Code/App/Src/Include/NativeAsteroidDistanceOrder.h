#ifndef FRONTIERX_NATIVE_ASTEROID_DISTANCE_ORDER_H
#define FRONTIERX_NATIVE_ASTEROID_DISTANCE_ORDER_H

#if !defined(_WIN32)
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>

// RadixSort orders unsigned keys stably. Nonnegative IEEE binary32 bit patterns
// have the same ascending order as their values. Floor preserves the original
// defined float-to-U32 integer ties, without overflowing for distant asteroids.
inline std::uint32_t NativeAsteroidDistanceKey(float distance)
{
    static_assert(sizeof(float) == sizeof(std::uint32_t) &&
                  std::numeric_limits<float>::is_iec559 &&
                  std::numeric_limits<float>::radix == 2 &&
                  std::numeric_limits<float>::digits == 24,
                  "Native asteroid keys require IEEE binary32");
    if (!std::isfinite(distance) || distance < 0.0F)
        throw std::runtime_error("Native asteroid distance key requires finite nonnegative distance");
    const float floored = std::floor(distance);
    if (floored == 0.0F) return 0U; // Normalize negative zero as the old U32 cast did.
    std::uint32_t key;
    std::memcpy(&key, &floored, sizeof(key));
    return key;
}
#endif

#endif
