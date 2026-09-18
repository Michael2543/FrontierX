#ifndef FRONTIERX_STACK_ADDRESS_H
#define FRONTIERX_STACK_ADDRESS_H

#include <cstddef>
#include <cstdint>

namespace FrontierX
{
inline bool AreAddressesWithinDistance(std::uintptr_t first,
                                       std::uintptr_t second,
                                       std::size_t maximumDistance) noexcept
{
    const std::uintptr_t distance =
        first >= second ? first - second : second - first;
    return distance < maximumDistance;
}

inline bool IsAddressNearCurrentStack(const void * address,
                                      std::size_t maximumDistance) noexcept
{
    const unsigned char stackMarker = 0;
    return AreAddressesWithinDistance(
        reinterpret_cast<std::uintptr_t>(address),
        reinterpret_cast<std::uintptr_t>(&stackMarker),
        maximumDistance);
}
}

#endif
