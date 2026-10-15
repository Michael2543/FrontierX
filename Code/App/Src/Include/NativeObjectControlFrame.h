#ifndef FRONTIERX_NATIVE_OBJECT_CONTROL_FRAME_H
#define FRONTIERX_NATIVE_OBJECT_CONTROL_FRAME_H

#if !defined(_WIN32)
#include "SuperTrans.h"
#include <type_traits>

struct FIGHTER_SAVELOAD;

// Retail fighters retain their authored -K frame. Ship navigation uses -J.
// Select only the existing Fighter control instantiation; no stored state changes.
template <class SaveInfo>
struct NativeObjectControlFrame
{
    static SINGLE Yaw(const TRANSFORM &frame)
    {
        if constexpr (std::is_same_v<SaveInfo, FIGHTER_SAVELOAD>)
            return frame.get_yaw_zbased();
        return frame.get_yaw();
    }
    static SINGLE Pitch(const TRANSFORM &frame)
    {
        if constexpr (std::is_same_v<SaveInfo, FIGHTER_SAVELOAD>)
            return frame.get_pitch_zbased();
        return frame.get_pitch();
    }
    static SINGLE Roll(const TRANSFORM &frame)
    {
        if constexpr (std::is_same_v<SaveInfo, FIGHTER_SAVELOAD>)
            return frame.get_roll_zbased();
        return frame.get_roll();
    }
    static Vector RollAxis(const TRANSFORM &frame)
    {
        if constexpr (std::is_same_v<SaveInfo, FIGHTER_SAVELOAD>)
            return frame.get_k();
        return frame.get_j();
    }
};
#endif

#endif
