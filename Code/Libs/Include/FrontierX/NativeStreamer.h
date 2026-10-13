#pragma once
#include <cstdint>

struct ICOManager;
namespace FrontierX
{
bool RegisterNativeStreamer(ICOManager * manager);
// Called on the application thread by the native window pump.
void UpdateNativeStreamers();
// Read-only journal context; set and consumed on the application thread.
void SetNativeStreamerDiagnosticTick(std::uint32_t tick);
}
