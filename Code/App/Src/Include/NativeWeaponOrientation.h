#ifndef FRONTIERX_NATIVE_WEAPON_ORIENTATION_H
#define FRONTIERX_NATIVE_WEAPON_ORIENTATION_H

#if !defined(_WIN32)
#include "SuperTrans.h"

namespace FrontierX
{
inline SINGLE NativeWeaponSightPitch(const TRANSFORM & orientation)
{
    // Weapon sights and missile flight use -K, unlike ship navigation's J axis.
    return orientation.get_pitch_zbased();
}
}
#endif

#endif
