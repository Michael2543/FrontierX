#ifndef FRONTIERX_NATIVE_WEAPON_ORIENTATION_H
#define FRONTIERX_NATIVE_WEAPON_ORIENTATION_H

#if !defined(_WIN32)
#include "SuperTrans.h"

namespace FrontierX
{
inline SINGLE NativeWeaponSightPitch(const TRANSFORM & orientation)
{
    // Turret hardpoints aim along -K, unlike the ship navigation J axis.
    return orientation.get_pitch_zbased();
}
}
#endif

#endif
