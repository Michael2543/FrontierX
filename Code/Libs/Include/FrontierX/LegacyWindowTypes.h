#ifndef FRONTIERX_LEGACY_WINDOW_TYPES_H
#define FRONTIERX_LEGACY_WINDOW_TYPES_H

#if defined(_WIN32)
#error LegacyWindowTypes.h is only for the non-Windows declaration path
#endif

#include "FrontierX/LegacyMessage.h"

#include <cstdint>

using HINSTANCE = void *;
using HWND = FrontierX::LegacyWindowToken;
using HDC = void *;
using HANDLE = void *;
using LONG = std::int32_t;
using HRESULT = std::int32_t;
using LRESULT = std::intptr_t;
using UINT = std::uint32_t;
using WPARAM = FrontierX::LegacyWParam;
using LPARAM = FrontierX::LegacyLParam;

struct tagRECT
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};
using RECT = tagRECT;

static_assert(sizeof(RECT) == 16U, "legacy RECT must remain four 32-bit words");

#endif
