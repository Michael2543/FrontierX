#ifndef FRONTIERX_DACOM_RUNTIME_H
#define FRONTIERX_DACOM_RUNTIME_H

#include <cstddef>
#include <cstdint>
#include <limits>

namespace FrontierX
{
using DacomRuntimeOffset = std::size_t;
using ConnectionToken = std::uint32_t;

constexpr DacomRuntimeOffset DacomReferenceFlag =
    DacomRuntimeOffset{1} << (std::numeric_limits<DacomRuntimeOffset>::digits - 1);

template <class Base, class Derived>
inline DacomRuntimeOffset BaseSubobjectOffset() noexcept
{
    constexpr std::uintptr_t anchor = 4096;
    Derived * derived = reinterpret_cast<Derived *>(anchor);
    Base * base = static_cast<Base *>(derived);
    return static_cast<DacomRuntimeOffset>(reinterpret_cast<std::uintptr_t>(base) - anchor);
}

template <class Owner, class Member>
inline DacomRuntimeOffset MemberOffset(Member Owner::* member) noexcept
{
    constexpr std::uintptr_t anchor = 4096;
    Owner * owner = reinterpret_cast<Owner *>(anchor);
    Member * memberAddress = &(owner->*member);
    return static_cast<DacomRuntimeOffset>(
        reinterpret_cast<std::uintptr_t>(memberAddress) - anchor);
}

template <class Member>
inline Member * MemberAtOffset(void * owner, DacomRuntimeOffset offset) noexcept
{
    return reinterpret_cast<Member *>(static_cast<unsigned char *>(owner) + offset);
}

template <class Owner, class Member>
inline Owner * OwnerFromMember(Member * member, DacomRuntimeOffset offset) noexcept
{
    return reinterpret_cast<Owner *>(reinterpret_cast<unsigned char *>(member) - offset);
}

template <class Node>
inline ConnectionToken NextConnectionToken(const Node * first,
                                           ConnectionToken maximum =
                                               (std::numeric_limits<ConnectionToken>::max)()) noexcept
{
    for (ConnectionToken candidate = 1; candidate != 0 && candidate <= maximum; ++candidate)
    {
        bool inUse = false;
        for (const Node * node = first; node; node = node->pNext)
        {
            if (node->connectionToken == candidate)
            {
                inUse = true;
                break;
            }
        }
        if (!inUse)
            return candidate;
    }
    return 0;
}
}

#endif
