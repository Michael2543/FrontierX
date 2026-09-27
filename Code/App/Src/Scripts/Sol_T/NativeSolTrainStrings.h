#ifndef FRONTIERX_NATIVE_SOL_TRAIN_STRINGS_H
#define FRONTIERX_NATIVE_SOL_TRAIN_STRINGS_H

#if defined(_WIN32)
#error NativeSolTrainStrings.h is only for the native script resource path
#endif

#include <cstdint>

struct HINSTANCE__;

namespace FrontierX
{
// Returns a stable Win32-style counted string, or null for an absent ID.
const wchar_t *GetNativeSolTrainCountedString(std::uint32_t id) noexcept;
HINSTANCE__ *GetNativeSolTrainResourceHandle() noexcept;
}

#endif
