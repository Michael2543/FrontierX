#ifndef DDIPLOMACYBUTTON_H
#define DDIPLOMACYBUTTON_H
//--------------------------------------------------------------------------//
//                                                                          //
//                               DDiplomacyButton.h							//
//                                                                          //
//                  COPYRIGHT (C) 1998 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Header: /Conquest/App/DInclude/DDiplomacyButton.h 2     9/08/00 7:14p Sbarton $
*/			    
//--------------------------------------------------------------------------//

#ifndef DBASEDATA_H
#include "DBaseData.h"
#endif

#ifndef GT_PATH
#define GT_PATH 32
#endif

//---------------------------------------------------------------------------
// order of the images in the button shape file
//---------------------------------------------------------------------------
#define GT_DIPL_DISABLED		0
#define GT_DIPL_NORMAL			1
#define GT_DIPL_MOUSE_FOCUS		2			
#define GT_DIPL_DEPRESSED		3
#define GT_DIPL_KEYB_FOCUS		4			// overlay

#define GT_DIPL_STATE0			5
#define GT_DIPL_STATE1			6
#define GT_DIPL_STATE2			7
#define GT_DIPL_STATE3			8

#define GT_DIPL_MAX_SHAPES		9


//---------------------------------------------------------------------------
//
struct GT_DIPLOMACYBUTTON : GENBASE_DATA
{
#if defined(_WIN32) || defined(_ADB)
	S32 leftMargin:8;		// skip pixels on the edge
	S32 topMargin:8;
#else
    // MSVC reserves the full four-byte bitfield allocation unit in retail data.
    S8 leftMargin;
    S8 topMargin;
    U16 reservedMargins;
#endif

	char shapeFile[GT_PATH];			// vfx shape file
};
#if !defined(_WIN32) && !defined(_ADB)
#include <cstddef>
static_assert(offsetof(GT_DIPLOMACYBUTTON, shapeFile)==8 &&
    sizeof(GT_DIPLOMACYBUTTON)==40, "Retail diplomacy button record layout");
#endif
//---------------------------------------------------------------------------
//
struct DIPLOMACYBUTTON_DATA
{
	char buttonType[GT_PATH];
	S32 xOrigin, yOrigin;
};


#endif
