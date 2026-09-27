#ifndef FRONTIERX_NATIVE_SOL_TRAIN_SCHEMA_H
#define FRONTIERX_NATIVE_SOL_TRAIN_SCHEMA_H

#if defined(_WIN32)
#error NativeSolTrainSchema.h is only for the native script resource path
#endif

#include <cstddef>

namespace FrontierX
{
// The source Sol_T.rc embeds DataDef.i as its Windows PARSER resource.
const void *GetNativeSolTrainSchema(std::size_t &length) noexcept;
}

#endif
