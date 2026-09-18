#ifndef HEAPOBJ_H
#define HEAPOBJ_H
//--------------------------------------------------------------------------//
//                                                                          //
//                              HeapObj.H                                  //
//                                                                          //
//               COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*
   $Author: Rmarr $

		                       Heap Object

*/
//--------------------------------------------------------------------------//


#ifndef DACOM_H
#include "DACOM.H"
#endif

//---------------------
//-- flags for use in DAHEAPDESC
//---------------------

#define DAHEAPFLAG_MULTITHREADED	0x00000001
#define DAHEAPFLAG_NOBESTFIT		0x00000002
#define DAHEAPFLAG_NOVERIFYPTR		0x00000004
#define DAHEAPFLAG_PRIVATE          0x00000008
#define DAHEAPFLAG_NOMSGS           0x00000010
#define DAHEAPFLAG_GROWHEAP			0x00000020
#define DAHEAPFLAG_DEBUGFILL		0x00000040		// fill freed memory with repeating pattern (0xCBCBCBCB)
#define DAHEAPFLAG_DEBUGFILL_SNAN	0x000000C0		// fill freed memory with SNAN ( 0xFFA01006 )
#define DAHEAPFLAG_NOHEAPEXPANDMSG	0x00000100		// do not show a message when the heap is expanding

//---------------------
//-- error codes passed to the error handler
//---------------------

#define DAHEAP_OUT_OF_MEMORY		0x00000001
#define DAHEAP_ALLOC_ZERO           0x00000002
#define DAHEAP_OVERFLOW             0x00000003
#define DAHEAP_HEAP_CORRUPTED       0x00000004
#define DAHEAP_INVALID_PTR          0x00000005

//---------------------
//-- trace codes passed to the error handler
//---------------------

#define DAHEAP_HEAP_EXPANDING       0x00000006		// trace level 2
#define DAHEAP_VALLOC_FAILED		0x00000007		// Error level, a call to VirtualAlloc() failed!

//---------------------
//-- callbacks
//---------------------

// error routine - return TRUE to retry the operation (also defined in FDUMP.h)
// enum  routine - return TRUE to continue the enumeration

typedef int (DACOM_CDECL * DA_ERROR_HANDLER) (struct ErrorCode code, const C8 *fmt, ...);
typedef BOOL32  (COMAPI * IHEAP_ENUM_PROC) (struct IHeap * heap,
												void *allocatedBlock, 
												U32 flags, 
												void *context);

/*
 * Memory error handler format:
 *		ErrorCode = (ERR_MEMORY, SEV_FATAL)
 *		error format string
 *		IHeap instance pointer
 *		error code (see above codes)
 *		{ variable parms, depending on error }
 */

//---------------------
//-- flags used in enumeration procedure
//---------------------

#define DAHEAPFLAG_ALLOCATED_BLOCK	0x00000001
#define DAHEAPFLAG_CORRUPTED_BLOCK	0x00000002


//---------------------
//-- structure used by CreateInstance method
//---------------------


struct DAHEAPDESC : public DACOMDESC
{
	U32			heapSize;
	U32			flags;
	U32			growSize;

	DAHEAPDESC (const C8 *_interfaceName = "IHeap") : DACOMDESC(_interfaceName)
	{
		heapSize = 
		flags = 
		growSize = 0;
		size = sizeof(*this);
	};
};


//

#define IID_IHeap MAKE_IID("IHeap",1)

//

struct DACOM_NO_VTABLE IHeap : public IComponentFactory
{
	virtual void * COMAPI AllocateMemory (U32 numBytes, const C8 *msg = 0) = 0;

	virtual void * COMAPI ClearAllocateMemory (U32 numBytes, const C8 *msg = 0, U8 initChar = 0) = 0;
	
	virtual void * COMAPI ReallocateMemory (void *allocatedBlock, U32 newSize, const C8 *msg = 0) = 0;

	virtual BOOL32 COMAPI FreeMemory (void *allocatedBlock) = 0;

	virtual BOOL32 COMAPI EnumerateBlocks (IHEAP_ENUM_PROC proc = 0, void *context=0) = 0;

	virtual U32 COMAPI GetBlockSize (void *allocatedBlock) = 0;

	virtual const char * COMAPI GetBlockMessage (void *allocatedBlock) = 0;

	virtual BOOL32 COMAPI DidAlloc (void *allocatedBlock) = 0;
	
	virtual U32 COMAPI GetAvailableMemory (void) = 0;

	virtual U32 COMAPI GetLargestBlock (void) = 0;

	virtual U32 COMAPI GetHeapSize (void) = 0;

	virtual DA_ERROR_HANDLER COMAPI SetErrorHandler (DA_ERROR_HANDLER proc) = 0;

	virtual DA_ERROR_HANDLER COMAPI GetErrorHandler (void) = 0;

	virtual BOOL32 COMAPI SetBlockOwner (void *allocatedBlock, U32 owner) = 0;

	virtual U32 COMAPI GetBlockOwner (void *allocatedBlock) = 0;

	virtual BOOL32 COMAPI SetBlockMessage (void *allocatedBlock, const C8 *msg) = 0;

	virtual void COMAPI HeapMinimize (void) = 0;		// return unused memory to the OS

	virtual U32 COMAPI GetHeapFlags (void) = 0;		// return flags used on last heap creation

private:
	// called from Comheap.lib so that the return address is correct
	virtual void * COMAPI malloc_pass_through (const C8 * msg = 0) = 0;
	virtual void * COMAPI realloc_pass_through (const C8 * msg = 0) = 0;
	virtual void * COMAPI calloc_pass_through (const C8 * msg = 0) = 0;
};



#ifdef __cplusplus
extern "C" 
{
#endif

//
// Applications and DLL's that want to use the common heap should statically link to 
// COMHeap.obj. This code overrides the default heap manager supplied by the 
// compiler writer. The static library calls HEAP_Acquire() at initialization time, and
// stores the result in the global variable 'HEAP'.
// 
//

DXDEC IHeap * DACOM_CDECL HEAP_Acquire (void);

//
// The external definitions are defined in COMHeap.asm
// Applications and / or DLLs should statically link to COMHeap.obj
// in order to use these:
//


extern struct IHeap * HEAP;

extern void DACOM_CDECL SetDefaultHeapMsg (const C8 *msg);

extern BOOL32 DACOM_CDECL InitializeDAHeap (long heapSize, long growSize=0, long flags=0);


#ifdef __cplusplus
}
#endif





#endif

//--------------------------------------------------------------------------//
//----------------------------END HeapObj.h---------------------------------//
//--------------------------------------------------------------------------//
