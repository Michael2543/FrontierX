#ifndef DBASEDATA_H
#define DBASEDATA_H
//--------------------------------------------------------------------------//
//                                                                          //
//                               DBaseData.h                                //
//                                                                          //
//                  COPYRIGHT (C) 1998 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Author: Rmarr $

    $Header: /Conquest/App/DInclude/DBaseData.h 102   9/27/00 10:09p Rmarr $
*/			    
//--------------------------------------------------------------------------//
#ifndef US_TYPEDEFS
#include "TYPEDEFS.H"
#endif

#ifndef DMISSIONDATALAYOUT_H
#include "DMissionDataLayout.h"
#endif

#ifndef OBJCLASS_H
#include "ObjClass.h"
#endif

#ifndef SFXID_H
#include "Sfxid.h"
#endif

#ifndef _ADB
#ifndef SUPERTRANS_H
#include "SuperTrans.h"
#endif
#endif

#ifndef DTYPES_H
#include "DTypes.h"
#endif

#ifndef GT_PATH
#define GT_PATH 32
#endif

#ifndef HP_PATH				// size of hardpoint path
#define HP_PATH 64
#endif

#include "..\Src\Resource.h"

#ifndef DOBJNAMES_H
#include "DObjNames.h"
#endif

#ifndef DSILHOUETTE_H
#include "DSilhouette.h"
#endif

#ifndef DUNITSOUNDS_H
#include "DUnitSounds.h"
#endif

#ifndef DMISSIONENUM_H
#include "DMissionEnum.h"
#endif

#ifdef _ADB 
typedef unsigned __int64 U64;
#else
#define __hexview
#define __readonly
#endif

struct DYNAMICS_DATA
{
    SINGLE linearAcceleration;
    SINGLE angAcceleration;

    SINGLE maxLinearVelocity;
    SINGLE maxAngVelocity;				// used for yaw
};

struct FLASH_DATA
{
	SINGLE lifeTime;
	SINGLE range;
	U8 red,green,blue;
};

struct TRANS_SAVELOAD
{
	Vector position;
	SINGLE ang_position;
	Vector velocity, ang_velocity;
};

struct EXTENSION_SAVELOAD
{
	U8 levelsAdded;
	S8 extensionLevel;
	S8 workingExtLevel;
	SINGLE percentExt;
};

struct CLOAK_SAVELOAD_BASE
{
	SINGLE cloakTimer,cloakPercent;
//	bool bDrawCloaking:1;  //mimic will not draw cloaking
	bool bCloakPending:1;
	bool bCloaking:1;
	U32 cloakCount;
};

struct CLOAK_SAVELOAD
{
	CLOAK_SAVELOAD_BASE baseCloak;
	BOOL32 bCloaked;
};

enum ARMOR_TYPE
{
	NO_ARMOR,
	LIGHT_ARMOR,
	MEDIUM_ARMOR,
	HEAVY_ARMOR
};

struct ARMOR_DATA
{
	ARMOR_TYPE myArmor;

#ifdef _ADB
	union FIELDS			// transmorgrify function doesn't like nameless unions
#else
	union 
#endif
	{
		struct ARMOR_DAMAGE
		{
			SINGLE noArmor;
			SINGLE lightArmor;
			SINGLE mediumArmor;
			SINGLE heavyArmor;
		} damageTable;

		SINGLE _damageTable[4];
#ifdef _ADB
	} fields;

#else
	};
#endif
};

struct DamageSave
{
	U32 damage;
	Vector pos;
	bool bActive;
};

#define DAMAGE_RECORDS 5
struct DAMAGE_SAVELOAD
{
	U32 lastRepairSlot;
	SINGLE lastDamage;
	struct DamageSave damageSave[DAMAGE_RECORDS];
};
//----------------------------------------------------------------
//
struct BASIC_DATA		// every game type must inherit from this
{
    OBJCLASS objClass;
	bool bEditDropable;		// include in dropdown list of objects for edit mode
};
//----------------------------------------------------------------
//
enum GENBASE_TYPE 
{
	GBT_FONT = 1,
	GBT_BUTTON,
	GBT_STATIC,
	GBT_EDIT,
	GBT_LISTBOX,
	GBT_DROPDOWN,
	GBT_VFXSHAPE,
	GBT_HOTBUTTON,
	GBT_SCROLLBAR,
	GBT_HOTSTATIC,
	GBT_SHIPSILBUTTON,
	GBT_COMBOBOX,
	GBT_SLIDER,
	GBT_TABCONTROL,
	GBT_ICON,
	GBT_QUEUECONTROL,
	GBT_ANIMATE,
	GBT_PROGRESS_STATIC,
	GBT_DIPLOMACYBUTTON
};

struct GENBASE_DATA		// every non-game type must inherit from this
{
	GENBASE_TYPE type;
};

//----------------------------------------------------------------
//
struct GT_VFXSHAPE : GENBASE_DATA
{
	char filename[GT_PATH];
	// bHiRes was a CQ2 addition absent from retail. Removed 2026-08-29.
};
//----------------------------------------------------------------
//



//----------------------------------------------------------------
//---------------------Mission definitions------------------------
//----------------------------------------------------------------
//
// Runtime MISSION_DATA extends the binary portion with fields that were added
// to the source after gametypes.db was compiled.  These are NOT read from the
// binary; they are zero-initialised and then populated via MISSION_DATA_OVERRIDE
// scripts at mission load time.
struct MISSION_DATA : MISSION_DATA_BIN
{
	ARMOR_DATA  armorData;
	SILNAMES::M_SILHOUETTE_NAME silhouetteImage;
	UNIT_SPECIAL_ABILITY    specialAbility;
	UNITSOUNDS::PRIORITY speechPriority;

#ifndef _ADB
	// specialAbility1/2 were added post-VS6 and are NOT in the binary archetype
	// files.  Static const so existing call sites compile without changing layout.
	static const UNIT_SPECIAL_ABILITY specialAbility1 = USA_NONE;
	static const UNIT_SPECIAL_ABILITY specialAbility2 = USA_NONE;
#endif
};

#ifndef _ADB
static_assert(sizeof(MISSION_DATA) == 72, "MISSION_DATA runtime size must be 72 bytes");
#endif

//----------------------------------------------------------------
//---------------------Mission data overrides---------------------
//----------------------------------------------------------------
//
struct MISSION_DATA_OVERRIDE
{
	U16			 hullPointsMax;
	U16			 supplyPointsMax;
	U16			 scrapValue;
	U16			 buildTime;
	U8           commandPoints;
	SINGLE		 sensorRadius;
	SINGLE		 cloakedSensorRadius;
	SINGLE		 maxVelocity;
	SINGLE		 baseShieldLevel;
	ARMOR_DATA   armorData;
	char         scriptHandle[64];
};

#endif
