#ifndef FRONTIERX_PARTICLE_CONTEXT_H
#define FRONTIERX_PARTICLE_CONTEXT_H

#include <cstdint>

namespace FrontierX
{
using ParticleContext = std::uintptr_t;

template <class Type>
inline ParticleContext ParticleContextFromPointer(Type * pointer) noexcept
{
    return reinterpret_cast<ParticleContext>(pointer);
}

template <class Type>
inline Type * PointerFromParticleContext(ParticleContext context) noexcept
{
    return reinterpret_cast<Type *>(context);
}
}

#endif
