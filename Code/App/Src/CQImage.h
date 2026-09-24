#ifndef CQIMAGE_H
#define CQIMAGE_H
//--------------------------------------------------------------------------//
//                                                                          //
//                               CQImage.h                                  //
//                                                                          //
//               COPYRIGHT (C) 2000 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*
   $Header: /Conquest/App/Src/CQImage.h 8     9/29/00 9:48a Jasony $

*/
//--------------------------------------------------------------------------//

#include "FDump.h"
#if !defined(_WIN32)
#include "FrontierX/LegacyWindowTypes.h"
#endif

#undef CQIMEXTERN
#if !defined(_WIN32)
#define CQIMEXTERN
#elif defined(BUILD_TRIM)
#define CQIMEXTERN __declspec(dllexport)
#else
#define CQIMEXTERN __declspec(dllimport)
#endif

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE ICQImage : IDAComponent
{
	virtual void LoadSymTable (HINSTANCE hInstance) = 0;

	virtual void UnloadSymTable (HINSTANCE hInstance) = 0;

	virtual void Report (const char * szInfo) = 0;

	virtual void MemoryReport (struct IHeap * heap) = 0;

	virtual void SetMessagesEnabled(bool bSetting) = 0;

CQIMEXTERN static bool COMAPI Assert (const char *exp, void *file, unsigned line);

CQIMEXTERN static bool DACOM_CDECL Bomb (const char *exp, ...);

CQIMEXTERN static bool DACOM_CDECL Error (const char *exp, ...);

CQIMEXTERN static int COMAPI Exception (struct _EXCEPTION_POINTERS * exceptionInfo);

CQIMEXTERN static int DACOM_CDECL STANDARD_DUMP (ErrorCode code, const C8 *fmt, ...);

};
//--------------------------------------------------------------------------//
//----------------------------End CQImage.h---------------------------------//
//--------------------------------------------------------------------------//
#endif
