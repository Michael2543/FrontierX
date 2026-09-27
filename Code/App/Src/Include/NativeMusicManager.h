#ifndef FRONTIERX_NATIVE_MUSIC_MANAGER_H
#define FRONTIERX_NATIVE_MUSIC_MANAGER_H

struct IMusicManager;

namespace FrontierX
{
// Create the source App music owner after IStreamer and Music are bound.
// Caller owns one reference.
IMusicManager *CreateNativeMusicManager();
}

#endif
