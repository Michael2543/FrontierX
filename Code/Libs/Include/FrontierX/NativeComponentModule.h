#ifndef FRONTIERX_NATIVE_COMPONENT_MODULE_H
#define FRONTIERX_NATIVE_COMPONENT_MODULE_H

#include "DACOM.H"

namespace FrontierX
{
inline constexpr const char * NativeComponentRegisterSymbol =
    "FrontierXRegisterComponents";
inline constexpr const char * NativeComponentCanUnloadSymbol =
    "FrontierXCanUnloadComponents";

using NativeComponentRegister = GENRESULT (*)(ICOManager * manager);
using NativeComponentCanUnload = BOOL32 (*)();
}

#endif
