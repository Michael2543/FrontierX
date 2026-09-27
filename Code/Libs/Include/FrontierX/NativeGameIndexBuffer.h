#ifndef FRONTIERX_NATIVE_GAME_INDEX_BUFFER_H
#define FRONTIERX_NATIVE_GAME_INDEX_BUFFER_H

#if defined(_WIN32)
#error NativeGameIndexBuffer.h is only for the native game-material boundary
#endif

#include "rendpipeline.h"

#include <atomic>
#include <cstddef>
#include <vector>

constexpr U32 D3DLOCK_READONLY = 0x00000010U;

// The established renderer vtable carries a D3D9 index-buffer pointer.
// This native-only object retains that slot while owning U16 indices.
struct IDirect3DIndexBuffer9
{
    explicit IDirect3DIndexBuffer9(std::size_t indexCount) : indices_(indexCount) {}

    GENRESULT Lock(UINT byteOffset, UINT byteLength, void ** data, U32)
    {
        if (!data) return GR_INVALID_PARMS;
        *data = nullptr;
        const std::size_t byteCount = indices_.size() * sizeof(U16);
        const std::size_t offset = byteOffset;
        const std::size_t length = byteLength ? byteLength :
            offset <= byteCount ? byteCount - offset : 0U;
        if (locked_ || offset >= byteCount || length == 0U ||
            (offset % sizeof(U16)) != 0U ||
            (length % sizeof(U16)) != 0U || length > byteCount - offset)
            return GR_INVALID_PARMS;
        locked_ = true;
        *data = reinterpret_cast<unsigned char *>(indices_.data()) + offset;
        return GR_OK;
    }

    GENRESULT Unlock()
    {
        if (!locked_) return GR_INVALID_PARMS;
        locked_ = false;
        return GR_OK;
    }

    U32 AddRef() { return ++references_; }
    U32 Release()
    {
        const U32 remaining = --references_;
        if (!remaining) delete this;
        return remaining;
    }

    const U16 * Data() const noexcept { return locked_ ? nullptr : indices_.data(); }
    std::size_t Count() const noexcept { return indices_.size(); }

private:
    std::vector<U16> indices_;
    std::atomic<U32> references_{1U};
    bool locked_ = false;
};

#endif
