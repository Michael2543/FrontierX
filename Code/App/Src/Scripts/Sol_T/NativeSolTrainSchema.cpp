#include "NativeSolTrainSchema.h"

#include "NativeSolTrainSchema.inc"

namespace FrontierX
{
const void *GetNativeSolTrainSchema(std::size_t &length) noexcept
{
    length = sizeof(solTrainSchemaBytes);
    return solTrainSchemaBytes;
}
}
