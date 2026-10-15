#ifndef FRONTIERX_NATIVE_SHIP_WARP_IN_TRANSFORM_H
#define FRONTIERX_NATIVE_SHIP_WARP_IN_TRANSFORM_H

#if !defined(_WIN32)
#include "SuperTrans.h"
#include <cmath>

// Current ship controls read -J as forward and K as up. The old warp-in
// initializer used the separate legacy -K-forward/J-up convention.
inline TRANSFORM NativeShipWarpInTransform(SINGLE heading, const Vector &position)
{
    const Vector facing(sin(heading), cos(heading), 0);
    TRANSFORM trans;
    trans.set_i(Vector(-facing.y, facing.x, 0));
    trans.set_j(-facing);
    trans.set_k(Vector(0, 0, 1));
    trans.translation = position;
    return trans;
}
#endif

#endif
