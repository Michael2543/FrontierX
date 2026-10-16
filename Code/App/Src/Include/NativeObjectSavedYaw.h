#ifndef FRONTIERX_NATIVE_OBJECT_SAVED_YAW_H
#define FRONTIERX_NATIVE_OBJECT_SAVED_YAW_H

#if !defined(_WIN32)
#include "SuperTrans.h"

inline void NativeRestoreObjectSavedYaw(TRANSFORM &frame, SINGLE savedYaw)
{
    // saveTransform stores the heading of -J. A fresh identity's heading
    // is -PI, so rotating identity by that heading alone reverses the ship.
    // The original format stores yaw only, not an animated pitch/roll pose.
    frame.Matrix::set_identity();
    frame.rotate_about_k(savedYaw - PI);
}
#endif

#endif
