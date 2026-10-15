#pragma once
#include <cstdint>
struct IVideoSystem;
struct ICOManager;
namespace FrontierX
{
// Genuine MPEG-1/MP2 provider; compiled only with the reviewed shared decoder.
bool RegisterNativeVideoSystem(ICOManager *manager);
struct NativeVideoDiagnostics
{
    std::uint64_t decodedAudioSamples = 0;
    std::uint32_t sampleRate = 0, channels = 0, decodedFrames = 0;
    float peak = 0, rms = 0;
};
// Read-only validation of the genuine provider; no legacy interface slots added.
bool GetNativeVideoDiagnostics(IVideoSystem *owner, std::uint32_t handle,
                               NativeVideoDiagnostics &out);
}
