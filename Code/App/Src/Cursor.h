#ifndef CURSOR_H
#define CURSOR_H
//--------------------------------------------------------------------------//
//                                                                          //
//                                Cursor.h                                  //
//                                                                          //
//               COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*
   $Author: Sbarton $

   $Header: /Conquest/App/Src/Cursor.h 9     9/25/00 1:16p Sbarton $

	Cursor resource
*/
//--------------------------------------------------------------------------//

#ifndef IRESOURCE_H
#include "IResource.h"
#endif


//--------------------------------------------------------------------------//
//


struct DACOM_NO_VTABLE ICursorResource : public IResource
{
	virtual BOOL32 SetCursor (U32 resID) = 0;

	virtual U32 GetCursor (void) = 0;

	virtual void SetBusy (BOOL bEnterBusy) = 0;
	
	virtual void DrawMouse (S32 xPosMouse, S32 yPosMouse) = 0;

	virtual BOOL32 IsValid (void) const = 0;

	virtual BOOL32 SetDefaultCursor (void) = 0;

	virtual void SetCursorSpeed (S32 speed) = 0;

	virtual void EnableCursor (bool bEnable) = 0;

#if !defined(_WIN32)
	virtual bool IsCursorEnabled (void) const = 0;
#else
	virtual const bool IsCursorEnabled (void) const = 0;
#endif

	virtual HCURSOR GetWindowsCursor (void) = 0;
};










//--------------------------------------------------------------------------//
//------------------------------End Cursor.h--------------------------------//
//--------------------------------------------------------------------------//
#endif
