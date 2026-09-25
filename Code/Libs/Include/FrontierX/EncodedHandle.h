#ifndef FRONTIERX_ENCODED_HANDLE_H
#define FRONTIERX_ENCODED_HANDLE_H

#include <cstddef>
#include <cstdint>
#include <limits>

namespace FrontierX
{
using EncodedHandleValue = std::uintptr_t;

inline EncodedHandleValue DecodeEncodedHandle(const void * handle) noexcept
{
    return reinterpret_cast<EncodedHandleValue>(handle);
}

inline void * EncodeHandleValue(EncodedHandleValue value) noexcept
{
    return reinterpret_cast<void *>(value);
}

inline bool TryDecodeHandleIndex(const void * handle, std::size_t limit,
                                 std::size_t & index) noexcept
{
    const EncodedHandleValue value = DecodeEncodedHandle(handle);
    if (value >= limit || value > (std::numeric_limits<std::size_t>::max)())
        return false;

    index = static_cast<std::size_t>(value);
    return true;
}

inline bool TryDecodeOffsetHandleIndex(const void * handle, EncodedHandleValue base,
                                       std::size_t limit, std::size_t & index) noexcept
{
    const EncodedHandleValue value = DecodeEncodedHandle(handle);
    if (value < base)
        return false;

    const EncodedHandleValue offset = value - base;
    if (offset >= limit || offset > (std::numeric_limits<std::size_t>::max)())
        return false;

    index = static_cast<std::size_t>(offset);
    return true;
}
}

#endif
