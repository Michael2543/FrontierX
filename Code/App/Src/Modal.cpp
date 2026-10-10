//--------------------------------------------------------------------------//
//                                                                          //
//                                Modal.cpp                                 //
//                                                                          //
//                  COPYRIGHT (C) 1998 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Header: /Conquest/App/Src/Modal.cpp 71    10/20/00 11:24p Jasony $
*/			    
//--------------------------------------------------------------------------//

#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "MyVertex.h"
#include <RPUL/PrimitiveBuilder.h>
struct BaseHotRect;
#endif
#include <stdio.h>
#include <cstdint>

#include "ObjList.h"
#include "NetBuffer.h"
#include "DrawAgent.h"
#include "CQTrace.h"
#if defined(_WIN32)
#include "VideoSurface.h"
#endif
#include "Startup.h"
#if defined(_WIN32)
#include "BaseHotRect.h"
#endif
#include "DEffectOpts.h"
#include "Hotkeys.h"
#include "SoundManager.h"
#include "IBackground.h"
#include "NetPacket.h"
#include "Sector.h"

#include <HeapObj.h>
#include <SYSTEM.H>
#include <EventSys.h>
#include <HKEvent.h>
#include <WindowManager.h>
#include <rendpipeline.h>
#include <IRenderPrimitive.h>
#include <IConnection.h>
#include <TComponent.h>
#include <TSmartPointer.h>
#include <FrontierX/PlatformTime.h>


#if defined(_WIN32)
#include <mmsystem.h>
#endif

#if defined(_WIN32)
typedef IHeap * (__stdcall * GETBATCHHEAP) (void);
static GETBATCHHEAP GetBatchHeap;
#else
using LARGE_INTEGER = S64;
static bool QueryPerformanceCounter(LARGE_INTEGER *tick)
{
	*tick = static_cast<S64>(FrontierX::Platform::MonotonicMicroseconds());
	return true;
}
#endif
//__declspec(dllimport) IHeap * __stdcall GetBatchHeap (void);
//--------------------------------------------------------------------------
//
static S32 currentFrameTime=0;

static S32 get_frame_time (void)
{
	const U64 clockTick = FrontierX::Platform::MonotonicMicroseconds();
	static U64 lastClockTick;

	if (lastClockTick == 0)
		lastClockTick = clockTick;

	const U64 elapsedTime = FrontierX::Platform::ClampedElapsedMicroseconds(
		lastClockTick, clockTick, 250000);
	lastClockTick = clockTick;
	
	// Controls consume elapsed time in 1/1024 millisecond units.
	currentFrameTime = S32(elapsedTime * 1024 / 1000);
	return currentFrameTime;
}

static RPDEVICESTATS stats;
#if defined(_WIN32)
static FILE *g_em_f = NULL;
#define EM_LOG(s) do { if(!g_em_f) g_em_f=fopen("debug/em_diag.txt","w"); if(g_em_f){fputs((s),g_em_f);fflush(g_em_f);} } while(0)
#else
#define EM_LOG(s) ((void)0)
#endif

//--------------------------------------------------------------------------
//
#if defined(_WIN32)
static FILE *g_slam_f = NULL;
#define SLAM_LOG(s) do { if(!g_slam_f) g_slam_f=fopen("debug/slam_diag.txt","w"); if(g_slam_f){fputs((s),g_slam_f);fflush(g_slam_f);} } while(0)
#else
#define SLAM_LOG(s) ((void)0)
#endif

static void slam2Dcomponents (S32 frame_time, bool bUseLocking)
{
	S32 fps;
#if !defined(FINAL_RELEASE) && defined(_WIN32)
	C8  buffer[256];
#endif

	if (frame_time)
		fps = ((10000000 / frame_time) + 5) / 10;
	else
		fps = RENDER_FRAMERATE;
	if (fps + 1 == RENDER_FRAMERATE || fps - 1 == RENDER_FRAMERATE)
		fps = RENDER_FRAMERATE;

	SLAM_LOG("slam2D: entered\n");
	if (bUseLocking)
	{
#if defined(_WIN32)
		SLAM_LOG("slam2D: before SURFACE->Lock\n");
		if (SURFACE->Lock() == false)
			return;
		SLAM_LOG("slam2D: after SURFACE->Lock\n");
#else
		CQASSERT(!bUseLocking); // Native presentation has no CPU framebuffer lock.
		return;
#endif
	}

	SLAM_LOG("slam2D: before DrawHighlightedList\n");
	if (CQFLAGS.bFullScreenMap==0)
		OBJLIST->DrawHighlightedList();
	SLAM_LOG("slam2D: after DrawHighlightedList\n");

	SLAM_LOG("slam2D: before BATCH->set_state\n");
	BATCH->set_state(RPR_BATCH,false);
	SLAM_LOG("slam2D: before BATCH->flush\n");
	BATCH->flush(RPR_OPAQUE | RPR_TRANSLUCENT_UNSORTED_ONLY | RPR_TRANSLUCENT_DEPTH_SORTED_ONLY);
	SLAM_LOG("slam2D: before EVENTSYS->Send\n");
	EVENTSYS->Send(CQE_ENDFRAME);
	SLAM_LOG("slam2D: after EVENTSYS->Send\n");
	
#if !defined(FINAL_RELEASE) && defined(_WIN32)
	if (CheckHotkeyPressed (IDH_DEBUG_PRINT))
	{
		CQFLAGS.debugPrint++;
	}
	
	if (CQFLAGS.debugPrint!=DBG_NONE)
	{
		if (CQFLAGS.debugPrint == DBG_ALL)
		{	
			IHeap *batchHeap = (*GetBatchHeap)();
			U32 batchMem = batchHeap->GetHeapSize()-batchHeap->GetAvailableMemory();
			
			wsprintf(buffer,"FPS: %2d, TEX: %d, VB: %d, SND: %d, HEAP: %d, BATCH: %d, POLYS: %3d%s", 
				fps, TEXMEMORYUSED, VBMEMORYUSED, SNDMEMORYUSED, HEAP->GetLargestBlock(), batchMem, stats.num_dp_primitives+stats.num_dip_primitives, stats.is_thrashing ? " TEXTURE THRASHING" : "");
			
			DEBUGFONT->StringDraw(0, 8, 14, buffer, RGB(200,200,200));
			
		}
		else
		{
			wsprintf(buffer,"FPS: %2d, HEAP: %d",
			fps, HEAP->GetLargestBlock());

			DEBUGFONT->StringDraw(0, 8, 14, buffer, RGB(200,200,200));
		}

#ifdef _ROB
		wsprintf(buffer,"DP: %3d DIP: %3d  TEXTURES_IN_SCENE: %3d",stats.num_dp_calls,stats.num_dip_calls,stats.num_texture_activated);
			
		DEBUGFONT->StringDraw(0, 8, 1, buffer, RGB(200,200,200));
#endif // end _ROB

		if (CQFLAGS.bGameActive)
			OBJLIST->DEBUG_print();	// print debugging info, if any
		else
			NETPACKET->DEBUG_print(DEBUGFONT);
	}
#endif  // end !FINAL_RELEASE

	if (bUseLocking)
#if defined(_WIN32)
		SURFACE->Unlock();
#else
		CQASSERT(!bUseLocking);
#endif
}
//--------------------------------------------------------------------------
//
#ifdef _DEBUG
static void readJoystick (void)
{
	JOYINFOEX joyinfo;
	
	joyinfo.dwSize = sizeof(joyinfo);
	joyinfo.dwFlags =  JOY_RETURNBUTTONS | JOY_RETURNPOV | JOY_RETURNX | JOY_RETURNY;

	if (joyGetPosEx(JOYSTICKID1, &joyinfo) == JOYERR_NOERROR)
	{
		if (joyinfo.dwButtons)
		{
			joyinfo.dwButtons++;
			joyinfo.dwButtons--;
		}

		DBHOTKEY->SystemMessage(FrontierX::LegacyWindowTokenFromPointer(hMainWindow),
			CQE_JOYSTICK, 0, FrontierX::LegacyLParamFromPointer(&joyinfo));
	}
}
#endif
//--------------------------------------------------------------------------
//
struct DACOM_NO_VTABLE ModalEventCallback : public IEventCallback
{
	BEGIN_DACOM_MAP_INBOUND(ModalEventCallback)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	END_DACOM_MAP()

	U32 eventHandle;
	IDAConnectionPoint * parentConnection;
	bool bResultSet;
	U32 dwResult;

	static ModalEventCallback * root;
	ModalEventCallback * next;

	ModalEventCallback (void)
	{
		eventHandle = 0;
		parentConnection = 0;
		bResultSet = false;
		dwResult = 0;
		next = root;
		root = this;
	}

	~ModalEventCallback (void)
	{
		unhook();
		root = next;
	}
	
	DEFMETHOD(Notify) (U32 message, void *param)
	{
		switch (message)
		{
		case CQE_DLG_RESULT:
			if (root == this)
			{
				bResultSet = true;
				dwResult = static_cast<U32>(reinterpret_cast<std::uintptr_t>(param));
				return GR_GENERIC;
			}
		}

		return GR_OK;
	}

	/* ModalEventCallback methods */

	void unhook (void)
	{
		if (parentConnection && eventHandle)
			parentConnection->Unadvise(eventHandle);
		eventHandle = 0;
	}

	void init (IDAConnectionPoint * _parent)
	{
		parentConnection = _parent;
		parentConnection->Advise(this, &eventHandle);
	}

};
static bool bResultWasReturned;
ModalEventCallback * ModalEventCallback::root;

bool bInScene=0;

BOOL32 EndModal(SINGLE dt)
{
	bool bNoSwap = false;		// true if viewing a full-screen movie
	S64 time1,time2;
	bool bUseLocking = (CQFLAGS.bFrameLockEnabled!=0);
#if !defined(_WIN32)
	if (bUseLocking)
	{
		std::fputs("Native CQDoModal CPU framebuffer locking is unsupported\n", stderr);
		return FALSE;
	}
#endif
	QueryPerformanceCounter((LARGE_INTEGER *)&time1);

	EM_LOG("EndModal entered\n");
	if (bInScene)
	{
		CQASSERT(CQFLAGS.bInProgressAnimActive==0);   // should not be drawing when progress anim is running!
		
		BATCH->flush(RPR_OPAQUE | RPR_TRANSLUCENT_UNSORTED_ONLY | RPR_TRANSLUCENT_DEPTH_SORTED_ONLY);
		QueryPerformanceCounter((LARGE_INTEGER *)&time2);
		
		OBJLIST->AddTimerTicks(TIMING_FLUSH,time2-time1);
		time1=time2;
		
		BATCH->set_state(RPR_BATCH,FALSE);
		
		if (bUseLocking)
		{
			if (CQFLAGS.b3DEnabled)
			{
				//	PrimitiveBuilder PB(PIPE);
				// glEnable(GL_BLEND);
				DisableTextures();
				PB.Color3ub(0,0,0);
				PB.Begin(PB_TRIANGLES);
				PB.Vertex3f(0,0,0);
				PB.Vertex3f(0,0,0);
				PB.Vertex3f(0,0,0);
				PB.End();
				//	BATCH->flush(RPR_OPAQUE | RPR_TRANSLUCENT_UNSORTED_ONLY | RPR_TRANSLUCENT_DEPTH_SORTED_ONLY);
				//			BATCH->flush(RPR_OPAQUE);
				//	BATCH->flush(RPR_TRANSLUCENT_DEPTH_SORTED_ONLY);
				//	BATCH->flush(RPR_TRANSLUCENT_UNSORTED_ONLY);
				if (PIPE->end_scene() != GR_OK)
				{
					CQTRACE10("Error in end_scene - probably lost surfaces");
#if !defined(_WIN32)
					return FALSE;
#endif
				}
			}
			bInScene = 0;
		}
		
		// when the game is active, draw movies before 2D components
		// when the game is not active (ie. front end menus) then movies are always on top.
		EM_LOG("before slam2D\n");
		EM_LOG("reading bGA\n");
		BOOL32 bGA_snap = CQFLAGS.bGameActive;
		EM_LOG("got bGA\n");
		if (bGA_snap)
		{
			EM_LOG("bGA1 before BltMovies\n");
			SOUNDMANAGER->BltMovies();
			EM_LOG("bGA1 before slam2D\n");
			slam2Dcomponents(dt, bUseLocking);		// lock frame buffer, write 2D stuff, unlock
			EM_LOG("bGA1 after slam2D\n");
		}
		else
/*			if (CQFLAGS.bFullScreenMovie)
			{
				SOUNDMANAGER->BltMovies();
				bNoSwap = true;		// viewing a full-screen movie directly to the front buffer
			}
			else
*/			{
				// okay, we're in the front end, and we are not drawing a movie, so render the 2D stuff
				EM_LOG("bGA0 before slam2D\n");
				slam2Dcomponents(dt, bUseLocking);		// lock frame buffer, write 2D stuff, unlock
				EM_LOG("bGA0 before BltMovies\n");
				SOUNDMANAGER->BltMovies();
				EM_LOG("bGA0 after BltMovies\n");
			}
		EM_LOG("after slam2D\n");

			if (bUseLocking==0)
			{
				if (CQFLAGS.b3DEnabled)
				{
					//	PrimitiveBuilder PB(PIPE);
					// glEnable(GL_BLEND);
					PB.Color3ub(0,0,0);
					PB.Begin(PB_TRIANGLES);
					PB.Vertex3f(0,0,0);
					PB.Vertex3f(0,0,0);
					PB.Vertex3f(0,0,0);
					PB.End();
					//BATCH->flush(RPR_OPAQUE | RPR_TRANSLUCENT_UNSORTED_ONLY | RPR_TRANSLUCENT_DEPTH_SORTED_ONLY);
					//			BATCH->flush(RPR_OPAQUE);
					EM_LOG("before end_scene\n");
					BATCH->flush(RPR_TRANSLUCENT_DEPTH_SORTED_ONLY);
					BATCH->flush(RPR_TRANSLUCENT_UNSORTED_ONLY);
					if (PIPE->end_scene() != GR_OK)
					{
						CQTRACE10("Error in end_scene - probably lost surfaces");
#if !defined(_WIN32)
						return FALSE;
#endif
					}
					EM_LOG("after end_scene\n");
				}
				bInScene = 0;
			}
			
			QueryPerformanceCounter((LARGE_INTEGER *)&time2);
			
			OBJLIST->AddTimerTicks(TIMING_RENDER2D,time2-time1);
			
			time1=time2;
	}

	memset(&stats, 0, sizeof(stats));
	PIPE->get_device_stats(&stats);

	if (bNoSwap==false)
	{
		//PIPE->clear_buffers(RP_CLEAR_COLOR_BIT|RP_CLEAR_DEPTH_BIT, NULL);
		EM_LOG("before swap_buffers\n");
#if defined(_WIN32)
		PIPE->swap_buffers();
#else
		const GENRESULT swapResult = PIPE->swap_buffers();
#endif
		EM_LOG("after swap_buffers\n");
#if defined(_WIN32)
		PIPE->clear_buffers(RP_CLEAR_COLOR_BIT|RP_CLEAR_DEPTH_BIT, NULL);
#else
		// The next mission frame clears before begin_scene. Leave the native
		// pipeline idle when this modal returns.
		if (swapResult != GR_OK) return FALSE;
#endif
	}
	
	QueryPerformanceCounter((LARGE_INTEGER *)&time2);

	OBJLIST->AddTimerTicks(TIMING_SWAPBUFFERS,time2-time1);

	if (CQFLAGS.bGameActive)
		OBJLIST->Update();

	QueryPerformanceCounter((LARGE_INTEGER *)&time1);
	((ISystemContainer *)GS)->Update();			// service windows message queue, etc.
	QueryPerformanceCounter((LARGE_INTEGER *)&time2);
	OBJLIST->AddTimerTicks(TIMING_GAMESYSTEM,time2-time1);

	OBJLIST->EndFrame();	// frame timing stuff, do delay if needed

	return TRUE;
}
//--------------------------------------------------------------------------
//
U32 __stdcall CQDoModal (BaseHotRect * pDialog)
{
	CQASSERT(bInScene == 0);
	S64 time1,time2;

	DAComponent<ModalEventCallback> modalEvent;

#if defined(_WIN32)
	if (pDialog)
	{
		CQASSERT(pDialog->parentEventConnection!=0);
		modalEvent.init(pDialog->parentEventConnection);
	}
	else
	{
		modalEvent.init(FULLSCREEN->parentEventConnection);
	}
#else
	// The native selected mission has no menu hot rectangle. Subscribe to the
	// same EventSystem connection used by the source mission exit events.
	if (pDialog)
	{
		std::fputs("Native CQDoModal requires the mission event connection\n", stderr);
		return 0;
	}
	IDAConnectionPoint *modalPoint = nullptr;
	if (EVENTSYS->QueryOutgoingInterface("IEventCallback", &modalPoint) != GR_OK ||
		!modalPoint)
	{
		std::fputs("Native CQDoModal event connection unavailable\n", stderr);
		return 0;
	}
	modalEvent.init(modalPoint);
	if (!modalEvent.eventHandle)
	{
		modalPoint->Release();
		std::fputs("Native CQDoModal event subscription failed\n", stderr);
		return 0;
	}
#endif

	while (modalEvent.bResultSet==false)
	{
		S32 dt = get_frame_time();

		QueryPerformanceCounter((LARGE_INTEGER *)&time1);

		SECTOR->PrepareTexture();

		EVENTSYS->Send(CQE_UPDATE,reinterpret_cast<void *>(static_cast<std::intptr_t>(dt)));	// update components
		if (modalEvent.bResultSet)
			break;		// break early ( for end of game, don't update objlist last time )
		QueryPerformanceCounter((LARGE_INTEGER *)&time2);
		OBJLIST->AddTimerTicks(TIMING_UPDATE2D,time2-time1);

		OBJLIST->BeginFrame();	// frame timing stuff
		BATCH->set_viewport(0,0,SCREENRESX,SCREENRESY);

		if (CQFLAGS.bGameActive)
		{
			QueryPerformanceCounter((LARGE_INTEGER *)&time1);
#if defined(_WIN32)
			PIPE->clear_buffers(RP_CLEAR_COLOR_BIT|RP_CLEAR_DEPTH_BIT,0);
#else
			if (PIPE->clear_buffers(RP_CLEAR_COLOR_BIT|RP_CLEAR_DEPTH_BIT,0) != GR_OK)
				break;
#endif
			QueryPerformanceCounter((LARGE_INTEGER *)&time2);
			OBJLIST->AddTimerTicks(TIMING_CLEARBUFFERS,time2-time1);
		}
		
		bInScene = 1;
		if (CQFLAGS.b3DEnabled)
		{
			bInScene = 0;
			if(PIPE->begin_scene() == GR_OK)
				bInScene = 1;
#if !defined(_WIN32)
			else break;
#endif
		}
		
		if (bInScene)
		{
			if (CQFLAGS.bGameActive) 
			{
				QueryPerformanceCounter((LARGE_INTEGER *)&time1);
				if (CQFLAGS.bFullScreenMap==0)
					BACKGROUND->RenderNeb();
				BATCH->set_state(RPR_BATCH,TRUE);
				QueryPerformanceCounter((LARGE_INTEGER *)&time2);
				OBJLIST->AddTimerTicks(TIMING_RENDERBACKGROUND, time2-time1);
				if (CQFLAGS.bFullScreenMap==0)
					OBJLIST->Render();
			}
#ifdef _DEBUG
			readJoystick();
#endif
			
			QueryPerformanceCounter((LARGE_INTEGER *)&time1);
			EVENTSYS->Send(CQE_RENDER_LAST3D,reinterpret_cast<void *>(static_cast<std::intptr_t>(dt)));
			QueryPerformanceCounter((LARGE_INTEGER *)&time2);
			OBJLIST->AddTimerTicks(TIMING_RENDER2D, time2-time1);
		}
		if (!EndModal(currentFrameTime)) break;

		QueryPerformanceCounter((LARGE_INTEGER *)&time1);
		EVENTSYS->Send(CQE_FLUSHMESSAGES,reinterpret_cast<void *>(static_cast<std::intptr_t>(dt)));
		QueryPerformanceCounter((LARGE_INTEGER *)&time2);
		OBJLIST->AddTimerTicks(TIMING_UPDATE2D, time2-time1);
	}

	bResultWasReturned = true;

#if !defined(_WIN32)
	modalEvent.unhook();
	modalPoint->Release();
#endif
	return modalEvent.dwResult;
}
//--------------------------------------------------------------------------
//
U32 __stdcall CQNonRenderingModal (BaseHotRect * pDialog)
{
	CQASSERT(bInScene == 0);

	DAComponent<ModalEventCallback> modalEvent;

#if defined(_WIN32)
	if (pDialog)
	{
		CQASSERT(pDialog->parentEventConnection!=0);
		modalEvent.init(pDialog->parentEventConnection);
	}
	else
	{
		modalEvent.init(FULLSCREEN->parentEventConnection);
	}
#else
	if (pDialog)
	{
		std::fputs("Native CQNonRenderingModal requires the mission event connection\n", stderr);
		return 0;
	}
	IDAConnectionPoint *modalPoint = nullptr;
	if (EVENTSYS->QueryOutgoingInterface("IEventCallback", &modalPoint) != GR_OK ||
		!modalPoint)
	{
		std::fputs("Native CQNonRenderingModal event connection unavailable\n", stderr);
		return 0;
	}
	modalEvent.init(modalPoint);
	if (!modalEvent.eventHandle)
	{
		modalPoint->Release();
		std::fputs("Native CQNonRenderingModal event subscription failed\n", stderr);
		return 0;
	}
#endif

	while (modalEvent.bResultSet==false)
	{
		S32 dt = get_frame_time();

		EVENTSYS->Send(CQE_UPDATE,reinterpret_cast<void *>(static_cast<std::intptr_t>(dt)));	// update components
		if (modalEvent.bResultSet)
			break;		// break early ( for end of game, don't update objlist last time )
		((ISystemContainer *)GS)->Update();			// service windows message queue, etc.
		EVENTSYS->Send(CQE_FLUSHMESSAGES,reinterpret_cast<void *>(static_cast<std::intptr_t>(dt)));
	}

	bResultWasReturned = true;

#if !defined(_WIN32)
	modalEvent.unhook();
	modalPoint->Release();
#endif
	return modalEvent.dwResult;
}
//-----------------------------------------------------------------------------------------//
//
struct _modalfactory : GlobalComponent
{
	virtual void Startup (void)
	{
#if defined(_WIN32)
		HINSTANCE hInstance = GetModuleHandle("ZBatcher.dll");
		
		if (hInstance)
			GetBatchHeap = (GETBATCHHEAP) GetProcAddress(hInstance, "_GetBatchHeap@0");
#endif
	}
	virtual void Initialize (void) {}

};

static _modalfactory startup;
//----------------------------------------------------------------------------------
//------------------------END Modal.cpp---------------------------------------------
//----------------------------------------------------------------------------------
