#pragma once

struct ICOManager;
namespace FrontierX
{
bool RegisterNativeStreamer(ICOManager * manager);
// Called on the application thread by the native window pump.
void UpdateNativeStreamers();
}
