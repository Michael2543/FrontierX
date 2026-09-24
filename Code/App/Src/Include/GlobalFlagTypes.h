#ifndef FRONTIERX_GLOBAL_FLAG_TYPES_H
#define FRONTIERX_GLOBAL_FLAG_TYPES_H

#include "TYPEDEFS.H"

struct GlobalFlags
{
	BOOL32 bFullScreen:1;
	BOOL32 bExceptionHappened:1;
	BOOL32 bGameActive:1;
	BOOL32 bDumpWindow:1;
	BOOL32 bTraceMission:1;
	BOOL32 bTraceNetwork:1;
	BOOL32 bNetBlocking:1;
	BOOL32 bFrameLockEnabled:1;
	BOOL32 debugPrint:2;
	BOOL32 bTracePerformance:1;
	BOOL32 bGamePaused:1;
	BOOL32 bWindowModeAllowed:1;
	BOOL32 b3DEnabled:1;
	BOOL32 bDPLobby:1;
	BOOL32 bTextureBias:1;
	BOOL32 bNoGDI:1;				// no GDI support when in full screen 3D mode
	BOOL32 bPrimaryDevice:1;		// primary render device is in use
	BOOL32 bNoExitConfirm:1;		// true if skipping the exit confirmation dialog
	BOOL32 bLoadingObjlist:1;		// inside Objlist::Load()
	BOOL32 bDumpFile:1;
	BOOL32 bTraceRules:1;
	BOOL32 bRuseActive:1;
	BOOL32 bCQBatcher:1;
	BOOL32 bRTGamePaused:1;			// Real-time part of game is paused
	BOOL32 bFullScreenMap:1;
	BOOL32 bInsideCreateInstance:1;	// for test purposes only
	BOOL32 bClientPlayback:1;		// for test, simulate running game on client machine
	BOOL32 bHostRecordMode:1;		// for test, record skirmish / networked games
	BOOL32 bMovieMode:1;			// in movie mode 
	BOOL32 bFPUExceptions:1;		// enable FPU exceptions
	BOOL32 bStoreDPlayInfo:1;		// store DirectPlay information in registry
	BOOL32 bFullScreenMovie:1;		// playing full screen movie
	BOOL32 bDPDelivery:1;			// use DPLAY's guaranteed delivery mechanism
	BOOL32 bLimitDPConnections:1;	// only use tcp, ipx connections
	BOOL32 bLimitResolutions:1;		// only use 640x480 -> 1024x768
	BOOL32 bInProgressAnimActive:1;  
	BOOL32 bAITest:1;				// fill all players with AI 
	BOOL32 bInstantBuilding:1;		// takes no time to build units/buildings
	BOOL32 bEverythingFree:1;		// everything in the game cost nothing
	BOOL32 bLimitGameTypes:1;		// display only one game type
	BOOL32 bLimitMapSettings:1;		
	BOOL32 bSkipMovies:1;			// if true, do not show any movies
	BOOL32 bSkipIntroMovie:1;		// if true, do not show logo movies
	BOOL32 bLimitMaxPlayers:1;		// if true set max players to 4 in lobby
	BOOL32 bForceLateDelivery:1;	// if true, receive all packets late,  (for playback mode)
	BOOL32 bForceEarlyDelivery:1;	// if true, receive all packets early, (for playback mode)
	BOOL32 bNoToolbar:1;			// if true, not using toolbar
	BOOL32 bInsidePlayerResign:1;	// true when we are blowing up units when player resigns
	BOOL32 bInsideOutZoneLaunch:1;	// true if we launched a game 
	BOOL32 bUseBWCursors:1;			// if true, always use monochrome cursors
	BOOL32 bExtCameraZoom:1;		// if true, extend the max camera zoom
	BOOL32 bDShowLog:1;
	BOOL32 bHardwareGeometry:1;		//if true hardware geometry is enabled in the ini for supporting hardware (also see CQRENDERFLAGS.bHardwareGeometry)
	BOOL32 bAltFontName:1;
};

struct GlobalRenderFlags
{
	BOOL32 bNoPerVertexAlpha:1;
	BOOL32 bSoftwareRenderer:1;		// 3D rendering is being done without hardware support
	BOOL32 bMultiTexture:1;
	BOOL32 b32BitTextures:1;
	BOOL32 bHardwareGeometry:1;
	BOOL32 bStallPipeline:1;
	BOOL32 bFSAA:1;
};


#endif
