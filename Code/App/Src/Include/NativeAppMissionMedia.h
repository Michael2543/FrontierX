#pragma once
#include <cstdint>

struct ISoundManager;
struct ITeletype;
struct IUnitComm;
struct IStringData;
struct IFileSystem;
struct IBaseObject;
struct IObjectList;
struct IAnimate;

namespace FrontierX
{
struct NativeMissionAudioStats
{
    std::uint32_t unitStarted = 0;
    std::uint32_t unitCompleted = 0;
    std::uint32_t unitStopped = 0;
    std::uint32_t activeStreams = 0;
    std::uint32_t portraitsStarted = 0;
    std::uint64_t portraitFrameChanges = 0;
    std::uint64_t portraitDraws = 0;
};
bool GetNativeAnimationCounts(IAnimate *animation, std::uint64_t &frameChanges,
                             std::uint64_t &draws);
bool GetNativeMissionAudioStats(ISoundManager *manager, NativeMissionAudioStats &stats);
bool CheckNativeMissionSoundCompletion();
// Existing sound fixture needs the real actor map before global mission startup.
IObjectList *CreateNativeSoundCheckObjectList();
ISoundManager *CreateNativeMissionSoundManager(bool skipSpeech, bool skipAnimatedPortraits);
ITeletype *CreateNativeMissionTeletype(bool skipVisuals);
IUnitComm *CreateNativeDisabledUnitComm();
IUnitComm *CreateNativeUnitComm(bool skipAlerts);
bool CheckNativeUnitAlerts();
void SubmitNativeMissionAlertSmoke(IBaseObject *object);
std::uint64_t NativeUnitAlertDrawCount();
IStringData *CreateNativeStringData(IFileSystem *source);
}
