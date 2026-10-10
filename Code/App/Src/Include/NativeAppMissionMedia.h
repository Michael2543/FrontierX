#pragma once
#include <cstdint>

struct ISoundManager;
struct ITeletype;
struct IUnitComm;
struct IStringData;
struct IFileSystem;

namespace FrontierX
{
struct NativeMissionAudioStats
{
    std::uint32_t unitStarted = 0;
    std::uint32_t unitCompleted = 0;
    std::uint32_t unitStopped = 0;
    std::uint32_t activeStreams = 0;
};
bool GetNativeMissionAudioStats(ISoundManager *manager, NativeMissionAudioStats &stats);
ISoundManager *CreateNativeMissionSoundManager(bool skipSpeech, bool skipAnimatedPortraits);
ITeletype *CreateNativeMissionTeletype(bool skipVisuals);
IUnitComm *CreateNativeDisabledUnitComm();
IUnitComm *CreateNativeUnitComm(bool skipAlerts);
IStringData *CreateNativeStringData(IFileSystem *source);
}
