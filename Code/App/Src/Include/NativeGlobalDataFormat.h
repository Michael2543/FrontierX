#ifndef FRONTIERX_NATIVE_GLOBAL_DATA_FORMAT_H
#define FRONTIERX_NATIVE_GLOBAL_DATA_FORMAT_H

#if defined(_WIN32)
#error NativeGlobalDataFormat.h is only for the Linux App loader
#endif

#include <array>
#include <cstddef>
#include <cstdint>

struct MT_GlobalData;

namespace FrontierX
{
constexpr std::size_t GlobalDataDiskSize = 3236;
using GlobalDataDiskBytes = std::array<std::uint8_t, GlobalDataDiskSize>;

void DecodeGlobalData(const GlobalDataDiskBytes & disk, MT_GlobalData & runtime);
bool EncodeGlobalData(const MT_GlobalData & runtime, GlobalDataDiskBytes & disk);
}

#endif
