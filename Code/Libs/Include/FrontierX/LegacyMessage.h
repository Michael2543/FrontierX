#ifndef FRONTIERX_LEGACY_MESSAGE_H
#define FRONTIERX_LEGACY_MESSAGE_H

#include <cstdint>

namespace FrontierX
{
using LegacyWindowToken = std::uintptr_t;
using LegacyMessageId = std::uint32_t;
using LegacyWParam = std::uintptr_t;
using LegacyLParam = std::intptr_t;

template <class Type>
inline LegacyWindowToken LegacyWindowTokenFromPointer(Type * pointer) noexcept
{
    return reinterpret_cast<LegacyWindowToken>(pointer);
}

template <class Type>
inline LegacyLParam LegacyLParamFromPointer(Type * pointer) noexcept
{
    return reinterpret_cast<LegacyLParam>(pointer);
}

template <class Type>
inline Type * PointerFromLegacyLParam(LegacyLParam parameter) noexcept
{
    return reinterpret_cast<Type *>(parameter);
}
}

#endif
