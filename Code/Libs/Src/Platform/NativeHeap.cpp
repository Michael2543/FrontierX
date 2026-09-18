#include "FrontierX/NativeHeap.h"

#include "HeapObj.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <map>
#include <mutex>
#include <new>
#include <string>
#include <utility>
#include <vector>

IHeap * HEAP = nullptr;

namespace FrontierX
{
namespace
{
struct Block
{
    U32 size = 0;
    U32 owner = 0;
    std::string message;
};

class NativeHeap final : public IHeap
{
public:
    GENRESULT QueryInterface(const C8 * requested, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (!requested || (std::strcmp(requested, "IHeap") != 0 &&
            std::strcmp(requested, IID_IHeap) != 0))
            return GR_INTERFACE_UNSUPPORTED;
        *instance = static_cast<IHeap *>(this);
        AddRef();
        return GR_OK;
    }

    U32 AddRef() override { return 1; }
    U32 Release() override { return 1; }

    GENRESULT CreateInstance(DACOMDESC * descriptor, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (!descriptor || descriptor->size != sizeof(DAHEAPDESC) ||
            !descriptor->interface_name || std::strcmp(descriptor->interface_name, "IHeap") != 0)
            return GR_INTERFACE_UNSUPPORTED;
        const auto & heap_descriptor = *static_cast<DAHEAPDESC *>(descriptor);
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        flags_ = heap_descriptor.flags;
        const std::uint64_t expanded =
            static_cast<std::uint64_t>(configured_size_) + heap_descriptor.heapSize;
        configured_size_ = static_cast<U32>(std::min<std::uint64_t>(
            expanded, std::numeric_limits<U32>::max()));
        *instance = static_cast<IHeap *>(this);
        AddRef();
        return GR_OK;
    }

    void * AllocateMemory(U32 num_bytes, const C8 * message = nullptr) override
    {
        if (!num_bytes) return nullptr;
        void * allocation = std::malloc(num_bytes);
        if (!allocation) return nullptr;
        try
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            blocks_.emplace(allocation,
                Block{num_bytes, 0, message ? message : default_message_});
            allocated_size_ += num_bytes;
        }
        catch (const std::bad_alloc &)
        {
            std::free(allocation);
            return nullptr;
        }
        return allocation;
    }

    void * ClearAllocateMemory(
        U32 num_bytes, const C8 * message = nullptr, U8 initial = 0) override
    {
        void * allocation = AllocateMemory(num_bytes, message);
        if (allocation) std::memset(allocation, initial, num_bytes);
        return allocation;
    }

    void * ReallocateMemory(
        void * allocation, U32 new_size, const C8 * message = nullptr) override
    {
        if (!allocation) return AllocateMemory(new_size, message);
        if (!new_size)
        {
            FreeMemory(allocation);
            return nullptr;
        }

        std::string replacement_message;
        if (message)
        {
            try
            {
                replacement_message = message;
            }
            catch (const std::bad_alloc &)
            {
                return nullptr;
            }
        }

        std::lock_guard<std::recursive_mutex> lock(mutex_);
        auto position = blocks_.find(allocation);
        if (position == blocks_.end()) return nullptr;
        auto node = blocks_.extract(position);
        const U32 old_size = node.mapped().size;
        void * resized = std::realloc(allocation, new_size);
        if (!resized)
        {
            blocks_.insert(std::move(node));
            return nullptr;
        }
        node.key() = resized;
        node.mapped().size = new_size;
        if (message) node.mapped().message = std::move(replacement_message);
        blocks_.insert(std::move(node));
        allocated_size_ = allocated_size_ - old_size + new_size;
        return resized;
    }

    BOOL32 FreeMemory(void * allocation) override
    {
        if (!allocation) return FALSE;
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        const auto position = blocks_.find(allocation);
        if (position == blocks_.end()) return FALSE;
        allocated_size_ -= position->second.size;
        blocks_.erase(position);
        std::free(allocation);
        return TRUE;
    }

    BOOL32 EnumerateBlocks(IHEAP_ENUM_PROC callback = nullptr, void * context = nullptr) override
    {
        if (!callback) return TRUE;
        std::vector<void *> snapshot;
        try
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            snapshot.reserve(blocks_.size());
            for (const auto & entry : blocks_) snapshot.push_back(entry.first);
        }
        catch (const std::bad_alloc &)
        {
            return FALSE;
        }
        for (void * allocation : snapshot)
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            if (blocks_.find(allocation) == blocks_.end()) continue;
            if (!callback(this, allocation, DAHEAPFLAG_ALLOCATED_BLOCK, context)) break;
        }
        return TRUE;
    }

    U32 GetBlockSize(void * allocation) override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        const auto position = blocks_.find(allocation);
        return position == blocks_.end() ? 0 : position->second.size;
    }

    const C8 * GetBlockMessage(void * allocation) override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        const auto position = blocks_.find(allocation);
        if (position == blocks_.end()) return nullptr;
        thread_local std::string message;
        try
        {
            message = position->second.message;
        }
        catch (const std::bad_alloc &)
        {
            return nullptr;
        }
        return message.c_str();
    }

    BOOL32 DidAlloc(void * allocation) override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return blocks_.find(allocation) == blocks_.end() ? FALSE : TRUE;
    }

    U32 GetAvailableMemory() override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (!configured_size_) return std::numeric_limits<U32>::max();
        return allocated_size_ >= configured_size_
            ? 0 : static_cast<U32>(configured_size_ - allocated_size_);
    }

    U32 GetLargestBlock() override { return GetAvailableMemory(); }
    U32 GetHeapSize() override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return configured_size_;
    }

    DA_ERROR_HANDLER SetErrorHandler(DA_ERROR_HANDLER handler) override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        DA_ERROR_HANDLER previous = error_handler_;
        error_handler_ = handler;
        return previous;
    }

    DA_ERROR_HANDLER GetErrorHandler() override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return error_handler_;
    }

    BOOL32 SetBlockOwner(void * allocation, U32 owner) override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        const auto position = blocks_.find(allocation);
        if (position == blocks_.end()) return FALSE;
        position->second.owner = owner;
        return TRUE;
    }

    U32 GetBlockOwner(void * allocation) override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        const auto position = blocks_.find(allocation);
        return position == blocks_.end() ? 0 : position->second.owner;
    }

    BOOL32 SetBlockMessage(void * allocation, const C8 * message) override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        const auto position = blocks_.find(allocation);
        if (position == blocks_.end()) return FALSE;
        try
        {
            position->second.message = message ? message : "";
        }
        catch (const std::bad_alloc &)
        {
            return FALSE;
        }
        return TRUE;
    }

    void HeapMinimize() override {}
    U32 GetHeapFlags() override
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return flags_;
    }

    void SetDefaultMessage(const C8 * message)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        try
        {
            default_message_ = message ? message : "";
        }
        catch (const std::bad_alloc &)
        {
        }
    }

private:
    void * malloc_pass_through(const C8 *) override { return nullptr; }
    void * realloc_pass_through(const C8 *) override { return nullptr; }
    void * calloc_pass_through(const C8 *) override { return nullptr; }

    std::recursive_mutex mutex_;
    std::map<void *, Block, std::less<void *>> blocks_;
    std::uint64_t allocated_size_ = 0;
    U32 configured_size_ = 0;
    U32 flags_ = 0;
    DA_ERROR_HANDLER error_handler_ = nullptr;
    std::string default_message_;
};

NativeHeap & HeapOwner()
{
    static NativeHeap heap;
    return heap;
}
}

void RegisterNativeHeap(ICOManager * manager)
{
    if (!manager) return;
    NativeHeap & heap = HeapOwner();
    manager->RegisterComponent(&heap, "IHeap");
    HEAP = &heap;
}

bool ConfigureNativeHeap(
    ICOManager * manager, std::uint32_t heap_size, std::uint32_t grow_size,
    std::uint32_t flags)
{
    if (!manager) return false;
    DAHEAPDESC descriptor;
    descriptor.heapSize = heap_size;
    descriptor.growSize = grow_size;
    descriptor.flags = flags;
    IHeap * heap = nullptr;
    if (manager->CreateInstance(
        &descriptor, reinterpret_cast<void **>(&heap)) != GR_OK || !heap)
        return false;
    HEAP = heap;
    return true;
}
}

extern "C" IHeap * DACOM_CDECL HEAP_Acquire()
{
    if (!HEAP) HEAP = &FrontierX::HeapOwner();
    HEAP->AddRef();
    return HEAP;
}

extern "C" void DACOM_CDECL SetDefaultHeapMsg(const C8 * message)
{
    FrontierX::HeapOwner().SetDefaultMessage(message);
}
