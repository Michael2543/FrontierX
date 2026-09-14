#ifndef DMISSIONDATALAYOUT_H
#define DMISSIONDATALAYOUT_H

#ifndef US_TYPEDEFS
#include "TYPEDEFS.H"
#endif

struct ResourceCost
{
	U32 gas:8;
	U32 metal:8;
	U32 crew:8;
	U32 commandPt:8;
};

// Binary layout stored in gametypes.db by the original VS6 build.
struct MISSION_DATA_BIN
{
	unsigned int mObjClass:8;
	unsigned int race:4;
	unsigned int displayName:18;

	struct M_CAPS
	{
		U8 padding;
		bool moveOk:1;
		bool attackOk:1;
		bool specialAttackOk:1;
		bool specialEOAOk:1;
		bool specialAbilityOk:1;
		bool specialAttackWormOk:1;
		bool defendOk:1;
		bool supplyOk:1;
		bool harvestOk:1;
		bool buildOk:1;
		bool repairOk:1;
		bool jumpOk:1;
		bool admiralOk:1;
		bool captureOk:1;
		bool salvageOk:1;
		bool probeOk:1;
		bool mimicOk:1;
		bool recoverOk:1;
		bool createWormholeOk:1;
		bool synthesisOk:1;
		bool cloakOk:1;
		bool specialAttackShipOk:1;
		bool targetPositionOk:1;
		bool specialTargetPlanetOk:1;

#ifndef _ADB
		M_CAPS & operator |= (const M_CAPS & other);
		M_CAPS & operator &= (const M_CAPS & other);
#endif
	} caps;

	U16 hullPointsMax;
	U16 supplyPointsMax;
	U16 scrapValue;
	U16 buildTime;

	ResourceCost resourceCost;

	SINGLE sensorRadius;
	SINGLE cloakedSensorRadius;
	SINGLE maxVelocity;
	SINGLE baseWeaponAccuracy;
	SINGLE baseShieldLevel;
};

#ifndef _ADB
static_assert(sizeof(ResourceCost) == 4, "ResourceCost must be 4 bytes to match VS6 binary");
static_assert(sizeof(MISSION_DATA_BIN::M_CAPS) == 4, "M_CAPS must be 4 bytes to match VS6 binary");
static_assert(sizeof(MISSION_DATA_BIN) == 40, "MISSION_DATA_BIN must be 40 bytes to match VS6 binary");
#endif

#endif