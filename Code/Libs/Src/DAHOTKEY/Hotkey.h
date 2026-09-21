#ifndef HOTKEY_H
#define HOTKEY_H

#include "TYPEDEFS.H"

#include <cstring>

#ifndef DACOM_FASTCALL
#if defined(_MSC_VER)
#define DACOM_FASTCALL __fastcall
#else
#define DACOM_FASTCALL
#endif
#endif
//--------------------------------------------------------------------------//
//                                                                          //
//                                HotKey.h                                  //
//                                                                          //
//                  COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Logfile: /Libs/Src/DAHotkey/Hotkey.h $

    $Revision: 2 $

    $Date: 8/02/97 3:34p $

    $Author: Jasony $
*/			    
//--------------------------------------------------------------------------//

#pragma pack ( push, 4 )

struct DAHOTKEY_STRUCT
{
	unsigned mKEY:3;
	unsigned mJOY:14;
	unsigned mDIR:8;		// 8 way directional joystick buttons
	unsigned mHAT:4;
	unsigned mMOUSE:3;
	unsigned vKEY:9;
	unsigned bType:1;
	unsigned bValid:1;
	unsigned symbol:12;
	unsigned bDirty:1;
	unsigned bPressed:1;

	int init (void)
	{
		std::memset(this, 0, sizeof(*this));
		return (bValid = 1);
	}

	int operator == (DAHOTKEY_STRUCT & key)
	{
		return (std::memcmp(this, &key, sizeof(*this))==0);
	}

	U32 getModifiers (void)
	{
		U32 result;

		result = ((U32 *)this)[0];		// mods take up first 32 bits of field
		return result;
	}

	void setModifiers (U32 dwMods)
	{
		U32 * ptr = (U32 *)this;

		*ptr = dwMods;						// mods take up first 32 bits of field
	}

	S32 DACOM_FASTCALL compare (DAHOTKEY_STRUCT & rec);

	BOOL32 operator > (DAHOTKEY_STRUCT & rec)
	{
		return (compare(rec) > 0);
	}

	BOOL32 operator < (DAHOTKEY_STRUCT & rec)
	{
		return (compare(rec) < 0);
	}
};			

struct KB_MODIFIERS
{
	U8 bLeftShift:1;
	U8 bRightShift:1;
	U8 bLeftControl:1;
	U8 bRightControl:1;
	U8 bLeftAlt:1;
	U8 bRightAlt:1;

	void reset (void)
	{
		std::memset(this, 0, sizeof(*this));
	}

	int isEmpty (void)
	{
		return (((U8 *)this)[0] == 0);
	}
};


#pragma pack ( pop )

static_assert(sizeof(DAHOTKEY_STRUCT) == 8, "retail hotkey record layout changed");
static_assert(sizeof(KB_MODIFIERS) == 1, "hotkey modifier layout changed");

#endif
