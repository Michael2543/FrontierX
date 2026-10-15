#ifndef FRONTIERX_NATIVE_APP_SFX_H
#define FRONTIERX_NATIVE_APP_SFX_H

struct ISFX;

namespace FrontierX
{
// Bind the source SFX ID table to the initialized native IStreamer.
// Caller owns one DACOM reference.
ISFX *CreateNativeSfxManager();
bool CheckNativeSfxPool();
}

#endif
