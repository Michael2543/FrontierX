#ifndef FRONTIERX_NATIVE_SYSTEM_MAP_H
#define FRONTIERX_NATIVE_SYSTEM_MAP_H

struct ISystemMap;

namespace FrontierX
{
// Native implementation of the existing App map interface. Caller owns one reference.
ISystemMap *CreateNativeSystemMap();
}

#endif
