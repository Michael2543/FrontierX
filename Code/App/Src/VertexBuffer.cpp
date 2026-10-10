//--------------------------------------------------------------------------//
//                                                                          //
//                           VertexBuffer.cpp                               //
//                                                                          //
//                  COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*

    $Author: Jasony $

    $Header: /Conquest/App/Src/VertexBuffer.cpp 8     10/25/00 1:05a Jasony $
*/			    
//--------------------------------------------------------------------------//
#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include <cstdlib>
#endif

#include "IVertexBuffer.h"
#include "CQBatch.h"
#if defined(_WIN32)
#include "CQLight.h"
#include <IDDBackDoor.h>
#endif
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//

#if defined(_WIN32)
ICQ_VB_Manager *vb_mgr;
#endif

struct CQ_VB_Manager : ICQ_VB_Manager
{
	IVertexBufferOwner *list;

	virtual void Add (IVertexBufferOwner *obj);
	virtual void Delete (IVertexBufferOwner *obj);
	void RestoreSurfaces();

	CQ_VB_Manager()
	{
		list = 0;
		vb_mgr = this;
	}
} mgr;

void CQ_VB_Manager::Add(IVertexBufferOwner *obj)
{
	obj->next = list;
	if (list)
		list->prev = obj;

	list = obj;
	list->prev = 0;
}

void CQ_VB_Manager::Delete(IVertexBufferOwner *obj)
{
	if (obj->prev)
		obj->prev->next = obj->next;
	if (obj->next)
		obj->next->prev = obj->prev;

	if (list == obj)
		list = obj->next;
}

void CQ_VB_Manager::RestoreSurfaces()
{
#if !defined(_WIN32)
	// Native device-loss restoration is not wired into the SDL GPU owner.
	std::abort();
#else
	IVertexBufferOwner *obj=list;

	VBMEMORYUSED = 0;
	while(obj)
	{
		obj->RestoreVertexBuffers();
		obj = obj->next;
	}
#endif
}

//----------------------------------------------------------------------------------
// This function waits for the proper cooperative level to be returned
// after the application has lost focus. It needs to be called before
// restoring any surfaces.
static int zvid_WaitForCooperativeLevel (void)
{
	return 0;
}

void __stdcall RestoreAllSurfaces()
{
#if !defined(_WIN32)
	std::abort();
#else
	if (CQFLAGS.b3DEnabled)
	{
		zvid_WaitForCooperativeLevel();
		mgr.RestoreSurfaces();
		CQBATCH->RestoreSurfaces();
		LIGHTS->RestoreAllLights();
	}
#endif
}

//----------------------------------------------------------------------------------
//-------------------------END VertexBuffer.cpp-------------------------------------
//----------------------------------------------------------------------------------
