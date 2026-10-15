#pragma once
#include <cstdint>
#include <DACOM.H>
#include <Streamer.h>

struct ICOManager;
namespace FrontierX
{
// Native extension; the legacy IStreamer module vtable stays unchanged.
struct INativeSpatialAudio : IDAComponent
{
    virtual BOOL32 SetPan(HSTREAM handle, S32 hundredthsOfDecibel) = 0;
    virtual BOOL32 PreloadSample(const char *filename, IFileSystem *source) = 0;
    virtual void ForgetSamples(IFileSystem *source) = 0;
    virtual BOOL32 GetPlaybackFraction(HSTREAM handle, SINGLE *fraction) const = 0;
};
bool CheckNativeStereoPan();
bool RegisterNativeStreamer(ICOManager * manager);
// Called on the application thread by the native window pump.
void UpdateNativeStreamers();
// Read-only journal context; set and consumed on the application thread.
void SetNativeStreamerDiagnosticTick(std::uint32_t tick);
}
