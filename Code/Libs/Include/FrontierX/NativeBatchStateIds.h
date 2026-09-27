#ifndef FRONTIERX_NATIVE_BATCH_STATE_IDS_H
#define FRONTIERX_NATIVE_BATCH_STATE_IDS_H

#include <cstdint>

namespace FrontierX
{
// App callers also use texture handles with small offsets as RPR_STATE_ID.
// Keep those IDs below this boundary and reserve eight IDs per mesh material.
constexpr std::uint32_t kMaxTextureStateId = 0x7ffffff0U;
constexpr std::uint32_t kFirstMeshMaterialStateId = 0x80000000U;
constexpr std::uint32_t kMeshMaterialStateStride = 8U;
static_assert(kMaxTextureStateId + kMeshMaterialStateStride <
              kFirstMeshMaterialStateId);
}

#endif
