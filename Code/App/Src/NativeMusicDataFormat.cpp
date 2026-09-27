#include "Include/NativeMusicDataFormat.h"

#include "FrontierX/LegacyFileTypes.h"
#include <TYPEDEFS.H>
#include <DMusic.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace
{
static_assert(sizeof(void *) == 8, "The native App target requires x86_64");
static_assert(NUM_SONGS == 4);
static_assert(sizeof(SONG) == 56, "Native SONG contains a pointer-sized stream handle");
static_assert(offsetof(SONG, handle) == 8);
static_assert(offsetof(SONG, volume) == 16);
static_assert(offsetof(SONG, filename) == 20);
static_assert(FrontierX::MusicSongDiskSize == 44);
static_assert(FrontierX::MusicDataDiskSize == 176);
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);
#endif

std::uint32_t ReadWord(const std::uint8_t * bytes)
{
    return std::uint32_t(bytes[0]) | (std::uint32_t(bytes[1]) << 8) |
           (std::uint32_t(bytes[2]) << 16) | (std::uint32_t(bytes[3]) << 24);
}

void WriteWord(std::uint8_t * bytes, std::uint32_t value)
{
    for (unsigned index = 0; index < 4; ++index)
        bytes[index] = static_cast<std::uint8_t>(value >> (index * 8));
}
}

namespace FrontierX
{
bool DecodeMusicData(const MusicDataDiskBytes & disk, SONG (&runtime)[4])
{
    MT_MUSIC_DATA decoded = {};
    for (std::size_t index = 0; index < NUM_SONGS; ++index)
    {
        const std::uint8_t * const song = disk.data() + index * MusicSongDiskSize;
        const std::uint32_t flags = ReadWord(song);
        const std::uint32_t volumeBits = ReadWord(song + 8);
        std::memcpy(&decoded[index].volume, &volumeBits, sizeof(volumeBits));
        if (!std::isfinite(decoded[index].volume) ||
            !std::memchr(song + 12, 0, sizeof(decoded[index].filename)))
            return false;
        decoded[index].playing = (flags & 1U) != 0;
        decoded[index].looping = (flags & 2U) != 0;
        decoded[index].handle = nullptr;
        std::memcpy(decoded[index].filename, song + 12, sizeof(decoded[index].filename));
    }
    std::memcpy(runtime, decoded, sizeof(decoded));
    return true;
}

bool EncodeMusicData(const SONG (&runtime)[4], MusicDataDiskBytes & disk)
{
    MusicDataDiskBytes encoded = {};
    for (std::size_t index = 0; index < NUM_SONGS; ++index)
    {
        const SONG & source = runtime[index];
        if (!std::isfinite(source.volume) ||
            !std::memchr(source.filename, 0, sizeof(source.filename)))
            return false;
        std::uint8_t * const song = encoded.data() + index * MusicSongDiskSize;
        WriteWord(song, (source.playing ? 1U : 0U) | (source.looping ? 2U : 0U));
        std::uint32_t volumeBits = 0;
        std::memcpy(&volumeBits, &source.volume, sizeof(volumeBits));
        WriteWord(song + 8, volumeBits);
        std::memcpy(song + 12, source.filename, sizeof(source.filename));
    }
    disk = encoded;
    return true;
}
}
