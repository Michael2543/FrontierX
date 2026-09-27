#ifndef FRONTIERX_NATIVE_MUSIC_DATA_FORMAT_H
#define FRONTIERX_NATIVE_MUSIC_DATA_FORMAT_H

#if defined(_WIN32)
#error NativeMusicDataFormat.h is only for the Linux App loader
#endif

#include <array>
#include <cstddef>
#include <cstdint>

struct SONG;

namespace FrontierX
{
constexpr std::size_t MusicSongDiskSize = 44;
constexpr std::size_t MusicDataDiskSize = 4 * MusicSongDiskSize;
using MusicDataDiskBytes = std::array<std::uint8_t, MusicDataDiskSize>;

// The Win32 stream pointer in each saved SONG is stale process state.
bool DecodeMusicData(const MusicDataDiskBytes & disk, SONG (&runtime)[4]);
bool EncodeMusicData(const SONG (&runtime)[4], MusicDataDiskBytes & disk);
}

#endif
