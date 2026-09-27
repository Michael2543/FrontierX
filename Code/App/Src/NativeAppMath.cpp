#include "FrontierX/NativeAppMath.h"

#if !defined(_WIN32)
#include <cmath>

SINGLE __stdcall get_angle(SINGLE x, SINGLE y)
{
    // The supported Win32 routine uses a rounded axis constant and maps
    // the origin to the negative axis result.
    if (y == 0.0F)
        return static_cast<SINGLE>((x > 0.0F || std::isnan(x)) ? 1.570796 : -1.570796);
    return std::atan2(x, y);
}
#endif
