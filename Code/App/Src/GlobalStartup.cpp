#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#endif

#include "Startup.h"
#include <DACOM.H>

#include <cstdlib>

// Keep the original App registrar order and two-phase startup on both hosts.
#define NUM_STARTUP_PTRS 26
struct STARTUP_NODE
{
	STARTUP_NODE * pNext;
	U32 numUsed;
	GlobalComponent * component[NUM_STARTUP_PTRS];

	void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void operator delete (void *ptr)
	{
		::free(ptr);
	}
};

static STARTUP_NODE startupNode;
static STARTUP_NODE *startupList = &startupNode;

void __stdcall AddToGlobalStartupList (GlobalComponent & component)
{
	if (startupList->numUsed >= NUM_STARTUP_PTRS)
	{
		STARTUP_NODE * node = new STARTUP_NODE;
		node->pNext = startupList;
		startupList = node;
	}

	startupList->component[startupList->numUsed++] = &component;
}

static void __stdcall initializeGlobalComponents (void)
{
	S32 numUsed;
	STARTUP_NODE * node = startupList;

	while (node)
	{
		numUsed = node->numUsed;
		while (numUsed-- > 0)
			node->component[numUsed]->Initialize();
		if ((node = node->pNext) != 0)
		{
			delete startupList;
			startupList = node;
		}
	}
}

void __stdcall CreateGlobalComponents (void)
{
	S32 numUsed;
	STARTUP_NODE * node = startupList;

	while (node)
	{
		numUsed = node->numUsed;
		while (numUsed-- > 0)
				node->component[numUsed]->Startup();
		node = node->pNext;
	}

	initializeGlobalComponents();
}

#define NUM_CLEANUP_PTRS 26
struct CLEANUP_NODE
{
	CLEANUP_NODE * pNext;
	U32 numUsed;
	IDAComponent ** component[NUM_CLEANUP_PTRS];

	void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void operator delete (void *ptr)
	{
		::free(ptr);
	}
};

static CLEANUP_NODE * cleanupList;

#if defined(_WIN32)
template <>
#endif
void AddToGlobalCleanupList (IDAComponent ** component)
{
	if (cleanupList == 0 || cleanupList->numUsed >= NUM_CLEANUP_PTRS)
	{
		CLEANUP_NODE * node = new CLEANUP_NODE;
		node->pNext = cleanupList;
		cleanupList = node;
	}
	cleanupList->component[cleanupList->numUsed++] = component;
}

void CleanupGlobals (void)
{
	S32 numUsed;
	CLEANUP_NODE * node = cleanupList;

	while (node)
	{
		numUsed = node->numUsed;
		while (numUsed-- > 0)
		{
			if (*node->component[numUsed])
				(*node->component[numUsed])->Release();
			*node->component[numUsed] = 0;
		}
		node = node->pNext;
		delete cleanupList;
		cleanupList = node;
	}
}
