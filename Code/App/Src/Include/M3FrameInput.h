#ifndef FRONTIERX_M3_FRAME_INPUT_H
#define FRONTIERX_M3_FRAME_INPUT_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace FrontierXM3
{
// Defined beside the original ObjectList owner; called after its iteration.
void AfterFrame();
void PrepareFrameInput();
constexpr unsigned int AfterFrameEvent = 0x7f4d3301U;

inline bool ControlledInput()
{
    static const bool enabled = [] {
        const char *value = std::getenv("FRONTIERX_M3_CONTROLLED_INPUT");
        if (!value || !*value) return false;
        const char *trace = std::getenv("FRONTIERX_M3_TRACE");
        if (std::strcmp(value, "1") || !trace || !*trace) std::abort();
        return true;
    }();
    return enabled;
}

// Test input to the original frame owners, not a replacement update loop.
// The normal game path continues to use its measured elapsed times.
inline bool FixedFrameInput()
{
    static const bool enabled = [] {
        const char *value = std::getenv("FRONTIERX_M3_FIXED_FRAME");
        if (!value || !*value) return false;
        const char *trace = std::getenv("FRONTIERX_M3_TRACE");
        if (std::strcmp(value, "1") || !trace || !*trace)
        {
            std::fputs("M3 fixed frame input requires private tick observation\n", stderr);
            std::abort();
        }
        return true;
    }();
    return enabled;
}
}

#endif
