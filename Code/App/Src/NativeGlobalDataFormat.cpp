#include "Include/NativeGlobalDataFormat.h"

#include "FrontierX/LegacyFileTypes.h"
#include "CQTrace.h"
#define MAX_PLAYERS 8
#include <DGlobalData.h>

#include <cstddef>
#include <cstring>
#include <type_traits>

namespace
{
constexpr std::size_t DiskStringsSize = (1 + MAX_PLAYERS) * M_MAX_STRING * 2;
constexpr std::size_t RuntimeStringsSize = (1 + MAX_PLAYERS) * sizeof(M_STRINGW);

static_assert(sizeof(void *) == 8, "The native App target requires x86_64");
static_assert(sizeof(wchar_t) == 4, "Native wide strings require Linux wchar_t");
static_assert(sizeof(M_STRINGW) == M_MAX_STRING * sizeof(wchar_t));
static_assert(DiskStringsSize == 576);
static_assert(RuntimeStringsSize == 1152);
static_assert(sizeof(MT_GlobalData) == 3812);
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);
#endif
static_assert(FrontierX::GlobalDataDiskSize - DiskStringsSize ==
              sizeof(MT_GlobalData) - RuntimeStringsSize);
static_assert(std::is_trivially_copyable<MT_GlobalData>::value);

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif
static_assert(offsetof(MT_GlobalData, gameDescription) == 0);
static_assert(offsetof(MT_GlobalData, playerNames) == sizeof(M_STRINGW));
static_assert(offsetof(MT_GlobalData, playerAssignments) == RuntimeStringsSize);
static_assert(offsetof(MT_GlobalData, gameSettings) == 3736);
static_assert(offsetof(MT_GlobalData, gameSettings) + sizeof(CQGAME) ==
              sizeof(MT_GlobalData));
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
}

namespace FrontierX
{
void DecodeGlobalData(const GlobalDataDiskBytes & disk, MT_GlobalData & runtime)
{
    MT_GlobalData decoded = {};
    for (std::size_t name = 0; name < 1 + MAX_PLAYERS; ++name)
        for (std::size_t character = 0; character < M_MAX_STRING; ++character)
        {
            const std::size_t offset = (name * M_MAX_STRING + character) * 2;
            M_STRINGW & string = name == 0 ? decoded.gameDescription : decoded.playerNames[name - 1];
            string.string[character] = static_cast<wchar_t>(
                std::uint16_t(disk[offset]) |
                (std::uint16_t(disk[offset + 1]) << 8));
        }
    std::memcpy(reinterpret_cast<std::uint8_t *>(&decoded) + RuntimeStringsSize,
                disk.data() + DiskStringsSize, GlobalDataDiskSize - DiskStringsSize);
    runtime = decoded;
}

bool EncodeGlobalData(const MT_GlobalData & runtime, GlobalDataDiskBytes & disk)
{
    GlobalDataDiskBytes encoded = {};
    for (std::size_t name = 0; name < 1 + MAX_PLAYERS; ++name)
        for (std::size_t character = 0; character < M_MAX_STRING; ++character)
        {
            const M_STRINGW & string = name == 0 ? runtime.gameDescription : runtime.playerNames[name - 1];
            const auto value = static_cast<std::uint32_t>(string.string[character]);
            if (value > 0xffffU) return false;
            const std::size_t offset = (name * M_MAX_STRING + character) * 2;
            encoded[offset] = static_cast<std::uint8_t>(value);
            encoded[offset + 1] = static_cast<std::uint8_t>(value >> 8);
        }
    std::memcpy(encoded.data() + DiskStringsSize,
                reinterpret_cast<const std::uint8_t *>(&runtime) + RuntimeStringsSize,
                GlobalDataDiskSize - DiskStringsSize);
    disk = encoded;
    return true;
}
}
