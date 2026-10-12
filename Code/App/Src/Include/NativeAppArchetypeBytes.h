#ifndef FRONTIERX_NATIVE_APP_ARCHETYPE_BYTES_H
#define FRONTIERX_NATIVE_APP_ARCHETYPE_BYTES_H

#include <cstddef>
#include <cstdint>

constexpr std::size_t NativeArchetypeEditorFlagOffset = 4;
constexpr std::size_t NativeArchetypeStorageAlignment = alignof(std::max_align_t);

inline std::size_t NativeArchetypeBytePadding(const void *bytes)
{
    const std::size_t remainder = reinterpret_cast<std::uintptr_t>(bytes) %
        NativeArchetypeStorageAlignment;
    return remainder ? NativeArchetypeStorageAlignment - remainder : 0;
}

// ObjectList collects every archive leaf, including text headers. Read the
// editor flag as an archive byte, without forming a noncanonical C++ bool.
inline bool NativeArchetypeEditorFlag(const void *bytes, std::size_t size)
{
    constexpr std::size_t offset = NativeArchetypeEditorFlagOffset;
    return bytes && size > offset &&
        static_cast<const unsigned char *>(bytes)[offset] != 0;
}

#endif
