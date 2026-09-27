#include "DACOM.H"
#include "HeapObj.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
struct Enumeration
{
    U32 count = 0;
    void * expected = nullptr;
    bool sawExpected = false;
};

BOOL32 COMAPI CountBlock(IHeap *, void * allocation, U32 flags, void * context)
{
    auto & enumeration = *static_cast<Enumeration *>(context);
    ++enumeration.count;
    enumeration.sawExpected = enumeration.sawExpected || allocation == enumeration.expected;
    return (flags & DAHEAPFLAG_ALLOCATED_BLOCK) != 0 ? TRUE : FALSE;
}

void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}
}

int main()
{
    try
    {
        constexpr long flags = DAHEAPFLAG_MULTITHREADED | DAHEAPFLAG_GROWHEAP |
            DAHEAPFLAG_NOMSGS;
        Require(InitializeDAHeap(4096, 1024, flags) == TRUE && HEAP,
            "native InitializeDAHeap failed");
        IHeap * acquired = HEAP_Acquire();
        Require(acquired == HEAP, "HEAP_Acquire did not return the initialized heap");
        Require(HEAP->GetHeapFlags() == static_cast<U32>(flags) && HEAP->GetHeapSize() == 4096,
            "native heap descriptor state changed");
        constexpr unsigned long too_wide =
            static_cast<unsigned long>(std::numeric_limits<U32>::max()) + 1UL;
        static_assert(sizeof(long) > sizeof(U32), "native LP64 boundary probe requires wide long");
        Require(InitializeDAHeap(-1, 0, 0) == FALSE &&
                InitializeDAHeap(static_cast<long>(too_wide), 0, 0) == FALSE &&
                HEAP->GetHeapSize() == 4096,
            "native heap accepted an invalid LP64 descriptor value");

        SetDefaultHeapMsg("default-owner");
        auto * cleared = static_cast<unsigned char *>(HEAP->ClearAllocateMemory(32));
        Require(cleared, "native clear allocation failed");
        Require(std::all_of(cleared, cleared + 32,
                    [](unsigned char value) { return value == 0; }),
            "native clear allocation was not zero-filled");
        Require(HEAP->DidAlloc(cleared) == TRUE && HEAP->GetBlockSize(cleared) == 32 &&
                std::strcmp(HEAP->GetBlockMessage(cleared), "default-owner") == 0,
            "native heap allocation metadata changed");
        Require(HEAP->SetBlockOwner(cleared, 0x12345678U) == TRUE &&
                HEAP->GetBlockOwner(cleared) == 0x12345678U &&
                HEAP->SetBlockMessage(cleared, "resizable") == TRUE,
            "native heap owner/message mutation failed");
        cleared[0] = 0x5a;
        cleared = static_cast<unsigned char *>(HEAP->ReallocateMemory(cleared, 64, "grown"));
        Require(cleared && cleared[0] == 0x5a && HEAP->GetBlockSize(cleared) == 64 &&
                std::strcmp(HEAP->GetBlockMessage(cleared), "grown") == 0,
            "native heap reallocation failed or lost metadata/data");

        void * second = HEAP->AllocateMemory(16, "second");
        Require(second, "second native heap allocation failed");
        Enumeration enumeration{0, cleared, false};
        Require(HEAP->EnumerateBlocks(CountBlock, &enumeration) == TRUE &&
                enumeration.count == 2 && enumeration.sawExpected,
            "native heap enumeration changed");
        int foreign = 0;
        Require(HEAP->FreeMemory(&foreign) == FALSE && HEAP->DidAlloc(&foreign) == FALSE,
            "native heap accepted a foreign pointer");
        Require(HEAP->GetAvailableMemory() == 4096 - 80 &&
                HEAP->GetLargestBlock() == HEAP->GetAvailableMemory(),
            "native diagnostic capacity accounting changed");
        Require(HEAP->FreeMemory(cleared) == TRUE && HEAP->FreeMemory(second) == TRUE &&
                HEAP->GetAvailableMemory() == 4096,
            "native heap release/accounting failed");

        Require(InitializeDAHeap(2048, 0, DAHEAPFLAG_NOMSGS) == TRUE &&
                HEAP->GetHeapSize() == 6144 &&
                HEAP->GetHeapFlags() == DAHEAPFLAG_NOMSGS,
            "repeated native heap initialization did not expand/update the shared owner");

		// NetBuffer and NetPacket request separate private heaps; their
		// ownership diagnostics must not inspect unrelated App allocations.
		DAHEAPDESC privateDescriptor;
		privateDescriptor.heapSize = 256;
		privateDescriptor.flags = DAHEAPFLAG_PRIVATE | DAHEAPFLAG_NOMSGS;
		IHeap *privateHeap = nullptr;
		Require(DACOM_Acquire()->CreateInstance(&privateDescriptor,
			reinterpret_cast<void **>(&privateHeap)) == GR_OK && privateHeap &&
			privateHeap != HEAP && privateHeap->GetHeapSize() == 256 &&
			privateHeap->GetHeapFlags() == privateDescriptor.flags,
			"native private heap did not get a separate owner");
		void *privateBlock = privateHeap->AllocateMemory(32, "packet-owner");
		Require(privateBlock && privateHeap->DidAlloc(privateBlock) == TRUE &&
			HEAP->DidAlloc(privateBlock) == FALSE &&
			privateHeap->GetLargestBlock() == 224,
			"native private heap allocation escaped into the shared owner");
		Require(privateHeap->FreeMemory(privateBlock) == TRUE &&
			privateHeap->GetLargestBlock() == 256 &&
			privateHeap->Release() == 0,
			"native private heap release/accounting failed");
        acquired->Release();

        std::cout << "native IHeap bootstrap passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "native heap probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
