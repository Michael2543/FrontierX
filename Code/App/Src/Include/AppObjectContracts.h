#ifndef APP_OBJECT_CONTRACTS_H
#define APP_OBJECT_CONTRACTS_H

// Shared App interface IDs, timing slots and selected-unit capacity.
// Keep the enum order and numeric values in sync with the supported game.
#define MAX_SELECTED_UNITS 22
#define MAX_PLAYERS 8
#define SYSVOLATILEPTR (MAX_PLAYERS+1)
#define NONSYSVOLATILEPTR 0
#define TOTALLYVOLATILEPTR (0xFFFFFFFF)
#define LAUNCHVOLATILEPTR (0xFFFFFFFE)

enum CQTIMING
{
	// warning! if you change the order or add a new value, you must update the string names in OBJLIST.cpp
	//
	TIMING_FLUSH,
	TIMING_RENDER2D,
	TIMING_RENDERBACKGROUND,
	TIMING_SWAPBUFFERS,
	TIMING_GAMESYSTEM,
	TIMING_UPDATE2D,
	TIMING_CLEARBUFFERS,
	TIMING_PLAYERAI,
	TIMING_OPAGENT,
	//
	TIMING_END		// no other timing values after here!
};


//--------------------------------------------------------------------------//
//--------------------------Interface IDs-----------------------------------//
//--------------------------------------------------------------------------//
// All ID's need to be of the form Interface##ID
//
enum OBJID
{
	IEmptyID = 0,
	IObjectID = 0x10000000,
	IBaseObjectID,
	IGotoPosID,
	ISaveLoadID,
	IWeaponID,
	IWeaponTargetID,
	IAttackID,
	IExplosionID,
	ILauncherID,
	ILaunchOwnerID,
	IExplosionOwnerID,
	IMissionActorID,
	IBlastID,
	IBuildID,
	IHarvestID,
	IBuildShipID,
	IUpgradeID,
	IPlanetID,
	IFabricatorID,
	IFighterID,
	IFighterOwnerID,
	IPlatformID,
	IDebrisID,
	IJumpGateID,
	IMinefieldID,
	IMinelayerID,
	IFireballID,
	IEffectID,
	INuggetID,
	IFieldID,
	ICloakEffectID,
	IAegisShieldID,
	IGroupID,
	IAOEWeaponID,
	IPhysicalObjectID,
	IEngineTrailID,
	ICloakID,
	IAdmiralID,
	ISpenderID,
	IFleetShipID,
	ISupplierID,
	IRepairPlatformID,
	IRepaireeID,
	ITroopshipID,
	IShipDamageID,
	IScrollingTextID,
	IReconProbeID,
	IReconLauncherID,
	IExtentID,
	IMapGenID,
	IBuildEffectID,
	IMovieCameraID,
	IObjectGeneratorID,
	ITriggerID,
	IRecoverShipID,
	IQuickSaveLoadID,
	IBuildQueueID,
	ISpiderDroneID,
	IShuttleID,
	IGameLightID,
	IJumpPlatID,
	IScriptObjectID,
	ITerranDroneID,
	IMimicID,
	IShipMoveID,
	IWormGeneratorID,
	IWormholeSyncID,
	IPlayerBombID,
	IWormholeBlastID,
	ITalorianEffectID,
	IToggleID,
	ISpaceWaveID,
	IHarvestBuilderID,
	ISystemSupplierID,
	IParticleCircleID,
	INovaExplosionID,
	IArtifactID,
	IArtifactHolderID,
	IEffectTargetID,
};
#if !defined(_WIN32)
static_assert(sizeof(OBJID) == 4, "OBJID must remain 32 bits");
static_assert(IObjectID == 0x10000000 && IBaseObjectID == 0x10000001,
    "OBJID values must match the Windows interface contract");
static_assert(TIMING_FLUSH == 0 && TIMING_END == 9,
    "CQTIMING slots must match ObjList's timing names");

struct IBaseObject;
struct ARCHNODE;
using PARCHETYPE = ARCHNODE *;
class Vector;
template <class Type> class OBJPTR;
template <> class OBJPTR<IBaseObject>;
#endif

#endif
