#ifndef FRONTIERX_NATIVE_HEAP_H
#define FRONTIERX_NATIVE_HEAP_H

#include <cstdint>

struct ICOManager;

namespace FrontierX
{
void RegisterNativeHeap(ICOManager * manager);
bool ConfigureNativeHeap(
    ICOManager * manager, std::uint32_t heap_size, std::uint32_t grow_size,
    std::uint32_t flags);
}

#endif
