#ifndef FRONTIERX_LEGACY_JOYSTICK_H
#define FRONTIERX_LEGACY_JOYSTICK_H

#include <cstdint>

namespace FrontierX
{
struct LegacyJoystickState
{
    std::uint32_t dwSize = sizeof(LegacyJoystickState);
    std::uint32_t dwFlags = 0;
    std::uint32_t dwXpos = 0;
    std::uint32_t dwYpos = 0;
    std::uint32_t dwZpos = 0;
    std::uint32_t dwRpos = 0;
    std::uint32_t dwUpos = 0;
    std::uint32_t dwVpos = 0;
    std::uint32_t dwButtons = 0;
    std::uint32_t dwButtonNumber = 0;
    std::uint32_t dwPOV = 0xffffU;
    std::uint32_t dwReserved1 = 0;
    std::uint32_t dwReserved2 = 0;
};

static_assert(sizeof(LegacyJoystickState) == 52);

namespace LegacyJoystickFlag
{
constexpr std::uint32_t ReturnX = 0x00000001U;
constexpr std::uint32_t ReturnY = 0x00000002U;
constexpr std::uint32_t ReturnPov = 0x00000040U;
constexpr std::uint32_t ReturnButtons = 0x00000080U;
constexpr std::uint32_t ReturnRawData = 0x00000100U;
constexpr std::uint32_t ReturnPovContinuous = 0x00000200U;
}

namespace LegacyJoystickPov
{
constexpr std::uint32_t Centered = 0xffffU;
constexpr std::uint32_t Forward = 0U;
constexpr std::uint32_t Right = 9000U;
constexpr std::uint32_t Backward = 18000U;
constexpr std::uint32_t Left = 27000U;
}

inline constexpr std::uint32_t LegacyJoystickButton(std::uint32_t one_based_button)
{
    return one_based_button >= 1 && one_based_button <= 32
        ? 1U << (one_based_button - 1U) : 0U;
}
}

#endif
