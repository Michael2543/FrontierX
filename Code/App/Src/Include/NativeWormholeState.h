#ifndef FRONTIERX_NATIVE_WORMHOLE_STATE_H
#define FRONTIERX_NATIVE_WORMHOLE_STATE_H

#include <cstdint>
#include <cstddef>
#include <cmath>

struct IWormholeBlast;

// Versioned native sidecar, independent of the original 112-byte launcher
// schema. Never serialize runtime pointers, texture handles or OBJMAP links.
struct NativeWormholeStrip
{
    std::uint32_t active, startIndex;
    float time, maxTime;
    std::uint32_t segments;
    float timeStep, altitude;
};
struct NativeWormholeVisual
{
    std::uint32_t version;
    float transform[12];
    float flashTime;
    std::uint32_t systemID, playerID;
    float radius;
    std::uint32_t reverse, closing, numStrips;
    float lastStripTime;
    NativeWormholeStrip strips[20];
};
static_assert(sizeof(NativeWormholeStrip)==28);
static_assert(sizeof(NativeWormholeVisual)==644);
static_assert(offsetof(NativeWormholeVisual,strips)+sizeof(NativeWormholeVisual::strips)==644);

inline bool ValidateNativeWormholeVisual(const NativeWormholeVisual &state)
{
    if (state.version!=1 || !state.systemID || state.playerID>8 ||
        !std::isfinite(state.radius) || state.radius<=0 ||
        !std::isfinite(state.flashTime) || state.flashTime<0 || state.flashTime>1 ||
        !std::isfinite(state.lastStripTime) || state.lastStripTime<0 ||
        state.reverse>1 || state.closing>1 || state.numStrips>20) return false;
    for (float value : state.transform) if (!std::isfinite(value)) return false;
    unsigned active=0;
    for (const auto &strip : state.strips)
    {
        if (strip.active>1) return false;
        if (!strip.active) continue;
        ++active;
        if (strip.startIndex>=40 || strip.segments<10 || strip.segments>24 ||
            !std::isfinite(strip.time) || strip.time<0 ||
            !std::isfinite(strip.maxTime) || strip.maxTime<=0 ||
            !std::isfinite(strip.timeStep) || strip.timeStep<=0 ||
            !std::isfinite(strip.altitude)) return false;
    }
    return active==state.numStrips;
}

bool CaptureNativeWormholeVisual(IWormholeBlast *effect,NativeWormholeVisual &state);
bool RestoreNativeWormholeVisual(IWormholeBlast *effect,const NativeWormholeVisual &state);

#endif
