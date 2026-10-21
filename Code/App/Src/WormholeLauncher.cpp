//--------------------------------------------------------------------------//
//                                                                          //
//                                WormholeLauncher.cpp                           //
//                                                                          //
//                  COPYRIGHT (C) 1999 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Header:
*/			    
//------------------------------- #INCLUDES --------------------------------//
//--------------------------------------------------------------------------//

#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "Include/CQTrace.h"
#include <ICamera.h>
#include <IMeshManager.h>
#include <IHardPoint.h>
#include <VFX.H>
#include <3dmath.h>
#include <renderer.h>
#include <ENGINE.H>
#include <TComponent.h>
#include <TSmartPointer.h>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include "IRenderPrimitive.h"
#include <rendpipeline.h>
#include "MyVertex.h"
struct IDrawAgent;
#endif

#include "SuperTrans.h"
#include "ObjList.h"
#include "SFX.h"

#include "Mission.h"
#include "IMissionActor.h"
#include "Startup.h"
#include "ArchHolder.h"
#include "TerrainMap.h"
#include "MPart.h"
#include "TObject.h"
#include <DWormholeLauncher.h>
#include "ILauncher.h"
#include "IWeapon.h"
#include <DSpaceship.h>
#include "IGotoPos.h"
#include "OpAgent.h"
#include "IShipMove.h"
#include "ObjMapIterator.h"
#include "CommPacket.h"
#include "Sector.h"
#include "ObjSet.h"
#include "Camera.h"
#include "IWormholeBlast.h"
#if !defined(_WIN32)
#include "Include/NativeWormholeState.h"
#include "Include/MScript.h"
#include "Include/MPartRef.h"
#include <cmath>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>


#endif

#include <ENGINE.H>
#include <TSmartPointer.h>
#include <IAnim.h>
#include <Filesys.h>
#include <TComponent.h>
#include <IConnection.h> 
#include <MGlobals.h>
#include <DMTechNode.h>

#include <stdlib.h>

// so that we can use the techtree stuff
using namespace TECHTREE;

struct WormLauncherArchetype
{
	PARCHETYPE pWormEffectType;
};

#define CIRC_TIME 4.0

#define LAST_CHANCE_TIME 15

#if !defined(_WIN32)
struct NativeWormholeLauncherState
{
    U32 version;
    SINGLE accumedDamage,circleTime;
    U32 portalMask;
    NativeWormholeVisual portals[2];
};
static_assert(sizeof(NativeWormholeLauncherState)==1304);
static_assert(sizeof(WORMHOLE_LAUNCHER_SAVELOAD)==112);

static bool WriteWormholeRecord(IFileSystem *directory,const char *name,const void *data,U32 size)
{
    DAFILEDESC descriptor(name);
    descriptor.lpImplementation="DOS";
    descriptor.dwDesiredAccess=GENERIC_READ|GENERIC_WRITE;
    descriptor.dwShareMode=0; descriptor.dwCreationDistribution=CREATE_ALWAYS;
    COMPTR<IFileSystem> file;
    DWORD written=0;
    return directory && directory->CreateInstance(&descriptor,file)==GR_OK &&
        file->WriteFile(0,data,size,&written,0) && written==size;
}

static bool ReadWormholeRecord(IFileSystem *directory,const char *name,void *data,U32 size)
{
    DAFILEDESC descriptor(name);
    descriptor.lpImplementation="DOS";
    COMPTR<IFileSystem> file;
    DWORD read=0;
    return directory && directory->CreateInstance(&descriptor,file)==GR_OK &&
        file->GetFileSize(0,nullptr)==size && file->ReadFile(0,data,size,&read,0) && read==size;
}

static bool ValidWormholeState(const WORMHOLE_LAUNCHER_SAVELOAD &state,const NativeWormholeLauncherState &extra)
{
    U32 mode=0;
    std::memcpy(&mode,&state.mode,sizeof(mode));
    if (mode>WORMHOLE_LAUNCHER_SAVELOAD::WORM_DONE || state.numTargets>MAX_WORM_VICTIMS ||
        state.targetsLeft>state.numTargets || !std::isfinite(state.lastChanceTimer) ||
        extra.version!=1 || extra.portalMask>3 ||
        !std::isfinite(extra.accumedDamage) || extra.accumedDamage<0 ||
        !std::isfinite(extra.circleTime) || extra.circleTime<0 || extra.circleTime>CIRC_TIME)
        return false;
    if (mode==WORMHOLE_LAUNCHER_SAVELOAD::WORM_WORKING &&
        (!state.workingID || !state.targetSystemID || extra.portalMask!=3)) return false;
    for (unsigned i=0;i<2;++i)
        if ((extra.portalMask&(1U<<i)) && !ValidateNativeWormholeVisual(extra.portals[i])) return false;
    return true;
}
#endif

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
struct _NO_VTABLE WormholeLauncher : IBaseObject, ILauncher, ISaveLoad, WORMHOLE_LAUNCHER_SAVELOAD
{
	BEGIN_MAP_INBOUND(WormholeLauncher)
	_INTERFACE_ENTRY(IBaseObject)
	_INTERFACE_ENTRY(ISaveLoad)
	_INTERFACE_ENTRY(ILauncher)
	END_MAP()

	BT_WORMHOLE_LAUNCHER * pData;

	OBJPTR<ILaunchOwner> owner;	// person who created us
	OBJPTR<IWormholeBlast> hole1;
	OBJPTR<IWormholeBlast> hole2;

	OBJPTR<IShipMove> targets[MAX_WORM_VICTIMS];

	SINGLE radius;
	SINGLE accumedDamage;

	SINGLE circleTime;
#if !defined(_WIN32)
    NativeWormholeLauncherState restoredState{};
    bool restorePending=false;
#endif

	//----------------------------------
	//----------------------------------
	
	WormholeLauncher (void);

	~WormholeLauncher (void);	

	/* IBaseObject methods */

	virtual const TRANSFORM & GetTransform (void) const;

	virtual BOOL32 Update (void);	// returning FALSE causes destruction

	virtual S32 GetObjectIndex (void) const;

	virtual U32 GetPartID (void) const;

	virtual U32 GetSystemID (void) const
	{
		return owner.Ptr()->GetSystemID();
	}

	virtual U32 GetPlayerID (void) const
	{
		return owner.Ptr()->GetPlayerID();
	}

	virtual void Render();

	/* ILauncher methods */

	virtual void InitLauncher (IBaseObject * owner, S32 ownerIndex, LAUNCHER_ANIM_ARCH animArcheIndex, SINGLE range);
	

	virtual void AttackPosition (const struct GRIDVECTOR * position, bool bSpecial)
	{
	}

	virtual void AttackObject (IBaseObject * obj);

	virtual void AttackMultiSystem (struct GRIDVECTOR * position, U32 targSystemID)
	{}

	virtual void WormAttack (IBaseObject * obj)
	{};

	virtual void DoCreateWormhole(U32 systemID);

	virtual const bool TestFightersRetracted (void) const 
	{ 
		return true;
	}

	virtual void SetFighterStance(FighterStance stance)
	{
	}

	virtual void HandlePreTakeover (U32 newMissionID, U32 troopID)
	{
	}

	virtual void TakeoverSwitchID (U32 newMissionID)
	{
	}

	virtual void DoSpecialAbility (U32 specialID);

	virtual void DoSpecialAbility (IBaseObject *obj)
	{
	}

	virtual void DoCloak (void)
	{
	}
	
	virtual void SpecialAttackObject (IBaseObject * obj)
	{
	}

	virtual void  GetSpecialAbility (UNIT_SPECIAL_ABILITY & ability, bool & bSpecialEnabled)
	{
		MPart part = owner.Ptr();

		ability = USA_WORMHOLE;
		bSpecialEnabled = part->caps.createWormholeOk && checkSupplies();
	}

	virtual const U32 GetApproxDamagePerSecond (void) const
	{
		return 0;
	}

	virtual void InformOfCancel();

	virtual void LauncherOpCreated(U32 agentID, void * buffer, U32 bufferSize);

	virtual void LauncherReceiveOpData(U32 agentID, void * buffer, U32 bufferSize);

	virtual void LauncherOpCompleted(U32 agentID);

	virtual bool CanCloak(){return false;};

	virtual bool IsToggle() {return false;};

	virtual bool CanToggle(){return false;};

	virtual bool IsOn() {return false;};

	// the following methods are for network synchronization of realtime objects
	virtual U32 GetSyncDataSize (void) const
	{
		return 0;
	}

	virtual U32 GetSyncData (void * buffer)
	{
		return 0;
	}

	virtual void PutSyncData (void * buffer, U32 bufferSize)
	{
	}

	virtual void OnAllianceChange (U32 allyMask)
	{
	}

	virtual IBaseObject * FindChildTarget(U32 childID){return NULL;};

	/* ISaveLoad methods */

	virtual BOOL32 Save (struct IFileSystem * file)
	{
#if defined(_WIN32)
		return TRUE;
#else
        NativeWormholeLauncherState extra{};
        extra.version=1; extra.accumedDamage=accumedDamage; extra.circleTime=circleTime;
        if (hole1) { extra.portalMask|=1; if (!CaptureNativeWormholeVisual(hole1,extra.portals[0])) return FALSE; }
        if (hole2) { extra.portalMask|=2; if (!CaptureNativeWormholeVisual(hole2,extra.portals[1])) return FALSE; }
        const auto &base=*static_cast<WORMHOLE_LAUNCHER_SAVELOAD *>(this);
        return ValidWormholeState(base,extra) &&
            WriteWormholeRecord(file,"WORMHOLE_LAUNCHER_SAVELOAD",&base,sizeof(base)) &&
            WriteWormholeRecord(file,"NativeWormholeStateV1",&extra,sizeof(extra));
#endif
	}

	virtual BOOL32 Load (struct IFileSystem * file)
	{
#if !defined(_WIN32)
        WORMHOLE_LAUNCHER_SAVELOAD state{};
        NativeWormholeLauncherState extra{};
        U8 bytes[sizeof(state)]{};
        if (!ReadWormholeRecord(file,"WORMHOLE_LAUNCHER_SAVELOAD",bytes,sizeof(bytes)) ||
            !MISSION || MISSION->CorrelateSymbol("WORMHOLE_LAUNCHER_SAVELOAD",bytes,&state)!=GR_OK ||
            !ReadWormholeRecord(file,"NativeWormholeStateV1",&extra,sizeof(extra)) ||
            !ValidWormholeState(state,extra) || hole1 || hole2 || restorePending) return FALSE;
        *static_cast<WORMHOLE_LAUNCHER_SAVELOAD *>(this)=state;
        accumedDamage=extra.accumedDamage; circleTime=extra.circleTime;
        restoredState=extra; restorePending=true;
        return TRUE;
#else
		return FALSE;
#endif
	}
	
	virtual void ResolveAssociations()
	{
#if !defined(_WIN32)
        if (!restorePending) return;
        for (unsigned i=0;i<numTargets;++i)
            if (targetIDs[i]) OBJLIST->FindObject(targetIDs[i],NONSYSVOLATILEPTR,targets[i],IShipMoveID);
        auto *arch=static_cast<WormLauncherArchetype *>(ARCHLIST->GetArchetypeHandle(pArchetype));
        std::unique_ptr<IBaseObject> restoredPortals[2];
        for (unsigned i=0;i<2;++i)
        {
            if (!(restoredState.portalMask&(1U<<i))) continue;
            restoredPortals[i].reset(ARCHLIST->CreateInstance(arch->pWormEffectType));
            OBJPTR<IWormholeBlast> portal;
            if (!restoredPortals[i] || !restoredPortals[i]->QueryInterface(IWormholeBlastID,portal,NONSYSVOLATILEPTR) ||
                !RestoreNativeWormholeVisual(portal,restoredState.portals[i]))
                throw std::runtime_error("Native saved wormhole portal restoration failed");
        }
        for (unsigned i=0;i<2;++i)
            if (restoredPortals[i])
            {
                auto &portal=i==0 ? hole1 : hole2;
                restoredPortals[i]->QueryInterface(IWormholeBlastID,portal,NONSYSVOLATILEPTR);
                OBJLIST->AddObject(restoredPortals[i].release());
            }
        restorePending=false;
#endif
	}

	/* WormholeLauncher methods */
	
	void init (PARCHETYPE pArchetype);

	bool checkSupplies();
};

//---------------------------------------------------------------------------
//
WormholeLauncher::WormholeLauncher (void) 
{
	accumedDamage = 0;
}
//---------------------------------------------------------------------------
//
WormholeLauncher::~WormholeLauncher (void)
{
}
//---------------------------------------------------------------------------
//
void WormholeLauncher::init (PARCHETYPE _pArchetype)
{
	pData = (BT_WORMHOLE_LAUNCHER *) ARCHLIST->GetArchetypeData(_pArchetype);

	CQASSERT(pData);
	CQASSERT(pData->type == LC_WORMHOLE_LAUNCH);
	CQASSERT(pData->objClass == OC_LAUNCHER);

	pArchetype = _pArchetype;
	objClass = OC_LAUNCHER;
}
//---------------------------------------------------------------------------
//
void WormholeLauncher::DoSpecialAbility (U32 specialID)
{

}
//---------------------------------------------------------------------------
//
S32 WormholeLauncher::GetObjectIndex (void) const
{
	// not too sure about this...
	return owner.Ptr()->GetObjectIndex();
}
//---------------------------------------------------------------------------
//
U32 WormholeLauncher::GetPartID (void) const
{
	return owner.Ptr()->GetPartID();
}
//---------------------------------------------------------------------------
//
void WormholeLauncher::Render()
{
	if(owner && (owner.Ptr()->bSelected || owner.Ptr()->bHighlight) && MGlobals::GetThisPlayer() == owner.Ptr()->GetPlayerID())
	{
		circleTime += OBJLIST->GetRealRenderTime();
		while(circleTime >CIRC_TIME)
		{
			circleTime -= CIRC_TIME;
		}

		BATCH->set_state(RPR_BATCH,true);
		DisableTextures();
		CAMERA->SetModelView();
		BATCH->set_render_state(D3DRS_ZENABLE,FALSE);
		BATCH->set_render_state(D3DRS_ZWRITEENABLE,FALSE);
		BATCH->set_render_state(D3DRS_ALPHABLENDENABLE,TRUE);
		BATCH->set_render_state(D3DRS_SRCBLEND,D3DBLEND_ONE);
		BATCH->set_render_state(D3DRS_DESTBLEND,D3DBLEND_ONE);
		PB.Begin(PB_LINES);
		PB.Color3ub(255,0,0);
		SINGLE range = radius/GRIDSIZE;
		Vector relDir =Vector(cos((2*PI*circleTime)/CIRC_TIME)*range*GRIDSIZE,
					sin((2*PI*circleTime)/CIRC_TIME)*range*GRIDSIZE,0);
		Vector oldVect = owner.Ptr()->GetPosition()+relDir;
		PB.Vertex3f(oldVect.x,oldVect.y,oldVect.z);
		oldVect = owner.Ptr()->GetPosition()-relDir;
		PB.Vertex3f(oldVect.x,oldVect.y,oldVect.z);
		PB.End();
		drawRangeCircle(range,RGB(255,0,0));
	}
}
//---------------------------------------------------------------------------
//
BOOL32 WormholeLauncher::Update (void)
{
	MPartNC part(owner.Ptr());
	if(part.isValid())
	{
		if(!(part->caps.createWormholeOk))
		{
			if(pData->techNode.raceID)
			{
				if(MGlobals::GetCurrentTechLevel(owner.Ptr()->GetPlayerID()).HasTech(pData->techNode))
				{
					part->caps.createWormholeOk = true;
				}
			}
			else
			{
				part->caps.createWormholeOk = true;
			}
		}
	}

	if(mode == WORM_IDLE && THEMATRIX->IsMaster())
	{
		MPart part(owner.Ptr());
		if(part->bReady)
		{
			accumedDamage += pData->damagePerSec*ELAPSED_TIME;
			if(accumedDamage > 5.0)
			{
				OBJPTR<IWeaponTarget> targ;
				owner.Ptr()->QueryInterface(IWeaponTargetID,targ,NONSYSVOLATILEPTR);
				U32 realDam = accumedDamage;
				if (targ)
					targ->ApplyAOEDamage(owner.Ptr()->GetPlayerID(),realDam);
				accumedDamage -= realDam;
			}
		}
	}

	if(mode == WORM_WORKING && THEMATRIX->IsMaster())
	{
		lastChanceTimer -= ELAPSED_TIME;

		U32 buffer[100];
		U32 numSent = 0;
		for(U32 i = 0; i < numTargets; ++i)
		{
			if(targetIDs[i])
			{
				if(targets[i].Ptr())
				{
					if(((targets[i].Ptr()->GetPosition()-GetPosition()).magnitude_squared() < 1000*1000) || (lastChanceTimer < 0))
					{
						buffer[0] = targetIDs[i];
						owner->LauncherSendOpData(workingID,buffer,sizeof(U32));

						OBJPTR<IPhysicalObject> phys;
						targets[i].Ptr()->QueryInterface(IPhysicalObjectID,phys);
						phys->SetPosition(targetPos,targetSystemID);
						SECTOR->RevealSystem(targetSystemID,targets[i].Ptr()->GetPlayerID());

						THEMATRIX->OperationCompleted(workingID,targetIDs[i]);
						targets[i]->ReleaseShipControl(owner.Ptr()->GetPartID());

						USR_PACKET<USRMOVE> packet;
						packet.objectID[0] = targetIDs[i];
						packet.position.init(targetPos,targetSystemID);
						packet.init(1);
						NETPACKET->Send(HOSTID,0,&packet);
						targetIDs[i] = 0;
						++numSent;
						hole1->Flash();
						hole2->Flash();
					}
				}
				else
				{
					buffer[0] = targetIDs[i];
					owner->LauncherSendOpData(workingID,buffer,sizeof(U32)+1);

					THEMATRIX->OperationCompleted(workingID,targetIDs[i]);

					targetIDs[i] = 0;
					++numSent;
				}
			}
		}
		targetsLeft -= numSent;
		if(!targetsLeft)
		{
			owner->LaunchOpCompleted(this,workingID);
			hole1->CloseUp();
			hole2->CloseUp();
			THEMATRIX->ObjectTerminated(owner.Ptr()->GetPartID(),0);
			mode = WORM_DONE;
		}
		return numSent;
	}

	return 1;
}
//---------------------------------------------------------------------------
//
void WormholeLauncher::InitLauncher (IBaseObject * _owner, S32 ownerIndex, LAUNCHER_ANIM_ARCH animArcheIndex, SINGLE range)
{
	CQASSERT(_owner);
	_owner->QueryInterface(ILaunchOwnerID, owner, LAUNCHVOLATILEPTR);
	CQASSERT(owner!=0);
	radius = range;
}
//---------------------------------------------------------------------------
//
void WormholeLauncher::InformOfCancel()
{
}
//---------------------------------------------------------------------------
//
void WormholeLauncher::LauncherOpCreated(U32 agentID, void * buffer, U32 bufferSize)
{
	workingID = agentID;
	THEMATRIX->SetCancelState(workingID,false);
	U32 * myBuf = (U32 *)buffer;
	targetsLeft = numTargets= (bufferSize - sizeof(GRIDVECTOR))/(sizeof(U32));
	targetPos = ((GRIDVECTOR *)(&(myBuf[numTargets])))[0];
	for(U32 i = 0; i < numTargets; ++i)
	{
		targetIDs[i] = myBuf[i];
		OBJLIST->FindObject(targetIDs[i],NONSYSVOLATILEPTR,targets[i],IShipMoveID);
		CQASSERT(targets[i].Ptr() && "Beware game may be out of sync");
		if(targets[i].Ptr())
		{
			VOLPTR(IMissionActor) actor=targets[i].Ptr();
//			actor->PrepareTakeover(targetIDs[i],0);
			targets[i]->PushShipTo(owner.Ptr()->GetPartID(),owner.Ptr()->GetPosition(),targets[i]->GetCurrentCruiseVelocity());				
		}
	}

	TRANSFORM trans;
	WormLauncherArchetype * arch = (WormLauncherArchetype *) ARCHLIST->GetArchetypeHandle(pArchetype);
	IBaseObject * obj = ARCHLIST->CreateInstance(arch->pWormEffectType);
	CQASSERT(obj);
	OBJLIST->AddObject(obj);
	obj->QueryInterface(IWormholeBlastID,hole1,NONSYSVOLATILEPTR);
	CQASSERT(hole1);
	trans = owner.Ptr()->GetTransform();
	hole1->InitWormholeBlast(trans,owner.Ptr()->GetSystemID(),radius,owner.Ptr()->GetPlayerID(),false);

	trans.set_identity();
	trans.set_position(targetPos);

	obj = ARCHLIST->CreateInstance(arch->pWormEffectType);
	CQASSERT(obj);
	OBJLIST->AddObject(obj);
	obj->QueryInterface(IWormholeBlastID,hole2,NONSYSVOLATILEPTR);
	CQASSERT(hole2);
	hole2->InitWormholeBlast(trans,targetSystemID,radius,owner.Ptr()->GetPlayerID(),true);	

	lastChanceTimer = LAST_CHANCE_TIME;
	mode = WORM_WORKING;
}
//---------------------------------------------------------------------------
//
void WormholeLauncher::LauncherReceiveOpData(U32 workingID, void * buffer, U32 bufferSize)
{
	U32 * sendBuffer  = (U32 *)buffer;
	bool bMove = (bufferSize != sizeof(U32)+1);
	for(U32 i = 0; i < numTargets; ++i)
	{
		if(targetIDs[i] == sendBuffer[0])
		{
			if(bMove)
			{
				if(targets[i].Ptr())
				{
					OBJPTR<IPhysicalObject> phys;
					targets[i].Ptr()->QueryInterface(IPhysicalObjectID,phys);
					phys->SetPosition(targetPos,targetSystemID);
					SECTOR->RevealSystem(targetSystemID,targets[i].Ptr()->GetPlayerID());
					targets[i]->ReleaseShipControl(owner.Ptr()->GetPartID());
				}

				hole1->Flash();
				hole2->Flash();
			}
			else
			{
				if(targets[i].Ptr())
				{
					targets[i]->ReleaseShipControl(owner.Ptr()->GetPartID());
				}
			}
			targetIDs[i] = 0;
		}
	}
	THEMATRIX->OperationCompleted(workingID,sendBuffer[0]);
	targetsLeft--;
}
//---------------------------------------------------------------------------
//
void WormholeLauncher::LauncherOpCompleted(U32 agentID)
{
	if(THEMATRIX->IsMaster())
	{
		for(U32 i = 0; i < numTargets; ++i)
		{
			if(targetIDs[i])
			{
				if(targets[i].Ptr())
					targets[i]->ReleaseShipControl(owner.Ptr()->GetPartID());
				U32 buffer[10];
				buffer[0] = targetIDs[i];
				owner->LauncherSendOpData(workingID,buffer,sizeof(U32)+1);

				THEMATRIX->OperationCompleted(workingID,targetIDs[i]);

				targetIDs[i] = 0;
			}
		}
	}
	hole1->CloseUp();
	hole2->CloseUp();
	mode = WORM_DONE;
}
//---------------------------------------------------------------------------
//
void WormholeLauncher::AttackObject (IBaseObject * obj)
{
}
//---------------------------------------------------------------------------
//
void WormholeLauncher::DoCreateWormhole(U32 systemID)
{
	//our owner should have already ended the op for us
	targetSystemID = systemID;

	if(THEMATRIX->IsMaster())
	{
		IBaseObject * posibleTargets[MAX_WORM_VICTIMS];
		U32 numPosibleTargets = 0;
		ObjMapIterator iter(owner.Ptr()->GetSystemID(),owner.Ptr()->GetPosition(),radius,0);
		while(iter)
		{
			MPart part(iter->obj);
			if(part.isValid() && part->caps.jumpOk && part->bReady && part->caps.moveOk && numPosibleTargets < MAX_WORM_VICTIMS)
			{
				posibleTargets[numPosibleTargets] = iter->obj;
				++numPosibleTargets;
			}
			++iter;
		}
		for(U32 i = 0; i < numPosibleTargets; ++i)
		{
			VOLPTR(IMissionActor) actor=posibleTargets[i];
			U32 id =  posibleTargets[i]->GetPartID();
			actor->PrepareTakeover(id,0);
			if(!THEMATRIX->GetWorkingOp(actor.Ptr()))
			{
				THEMATRIX->FlushOpQueueForUnit(actor.Ptr());
				targetIDs[numTargets] =id;
				posibleTargets[i]->QueryInterface(IShipMoveID,targets[numTargets],NONSYSVOLATILEPTR);

				targets[numTargets]->PushShipTo(owner.Ptr()->GetPartID(),owner.Ptr()->GetPosition(),targets[numTargets]->GetCurrentCruiseVelocity());
			
				++numTargets;
			}
		}
		targetsLeft = numTargets;
		if(numTargets)
		{
			COMPTR<ITerrainMap> map;
			SECTOR->GetTerrainMap(targetSystemID,map);
			RECT rect;
			SECTOR->GetSystemRect(targetSystemID,&rect);
			U32 width = rect.right-rect.left;
			targetPos = Vector(((rand()%1000)/1000.0f)*width,((rand()%1000)/1000.0f)*width,0);
			while(!(map->IsGridEmpty(targetPos,0,true)))
			{
				targetPos = Vector(((rand()%1000)/1000.0f)*width,((rand()%1000)/1000.0f)*width,0);
			}
			
			ObjSet set;
			set.objectIDs[0] = owner.Ptr()->GetPartID();
			U8 buffer[256];
			U32 * intBuf = (U32 *)(&buffer);
			for(U32 i  = 0; i < numTargets; ++i)
			{
				intBuf[i] = targetIDs[i];
				set.objectIDs[i+1] = targetIDs[i];
			}
			set.numObjects = numTargets+1;
			((GRIDVECTOR *)(&(intBuf[numTargets])))[0] = targetPos;
			workingID = owner->CreateLauncherOp(this,set,&buffer,numTargets*sizeof(U32)+sizeof(GRIDVECTOR));
			THEMATRIX->SetCancelState(workingID,false);
			lastChanceTimer = LAST_CHANCE_TIME;
			mode = WORM_WORKING;

			TRANSFORM trans;
			WormLauncherArchetype * arch = (WormLauncherArchetype *) ARCHLIST->GetArchetypeHandle(pArchetype);
			IBaseObject * obj = ARCHLIST->CreateInstance(arch->pWormEffectType);
			CQASSERT(obj);
			OBJLIST->AddObject(obj);
			obj->QueryInterface(IWormholeBlastID,hole1,NONSYSVOLATILEPTR);
			CQASSERT(hole1);
			trans = owner.Ptr()->GetTransform();
			hole1->InitWormholeBlast(trans,owner.Ptr()->GetSystemID(),radius,owner.Ptr()->GetPlayerID(),false);

			trans.set_identity();
			trans.set_position(targetPos);

			obj = ARCHLIST->CreateInstance(arch->pWormEffectType);
			CQASSERT(obj);
			OBJLIST->AddObject(obj);
			obj->QueryInterface(IWormholeBlastID,hole2,NONSYSVOLATILEPTR);
			CQASSERT(hole2);
			hole2->InitWormholeBlast(trans,targetSystemID,radius,owner.Ptr()->GetPlayerID(),true);
		
		}
		else
		{
			THEMATRIX->ObjectTerminated(owner.Ptr()->GetPartID(),0);
		}
	}

}

//---------------------------------------------------------------------------
//
const TRANSFORM & WormholeLauncher::GetTransform (void) const
{
	return owner.Ptr()->GetTransform();
}
//---------------------------------------------------------------------------
//
bool WormholeLauncher::checkSupplies()
{
	return true;
}
//---------------------------------------------------------------------------
//
inline IBaseObject * createWormholeLauncher (PARCHETYPE pArchetype)
{
	WormholeLauncher * wormholeLauncher = new ObjectImpl<WormholeLauncher>;

	wormholeLauncher->init(pArchetype);

	return wormholeLauncher;
}
#if !defined(_WIN32)
// Explicit skirmish diagnostic: synthetic active launcher state, real factories,
// schema correlation, filesystem, portal placement and association restoration.
// It issues no ship orders and does not establish actual ability acceptance.
bool CheckNativeWormholePersistence(IFileSystem *directory)
{
    struct FindLauncher : IArchetypeEnum
    {
        std::string name;
        BOOL32 __stdcall ArchetypeEnum(const char *candidate,void *data,U32 bytes) override
        {
            U32 words[2]{};
            if (bytes>=sizeof(BASIC_DATA)+sizeof(U32))
            {
                std::memcpy(&words[0],data,sizeof(U32));
                std::memcpy(&words[1],static_cast<const char *>(data)+sizeof(BASIC_DATA),sizeof(U32));
            }
            if (bytes==sizeof(BT_WORMHOLE_LAUNCHER) && words[0]==OC_LAUNCHER && words[1]==LC_WORMHOLE_LAUNCH)
                name=candidate;
            else if (words[0]==OC_LAUNCHER && words[1]==LC_WORMHOLE_LAUNCH)
                std::fprintf(stderr,"Native wormhole launcher extent=%u expected=%zu\n",bytes,sizeof(BT_WORMHOLE_LAUNCHER));
            return TRUE;
        }
    } found;
    if (!directory || !MISSION || !ARCHLIST->EnumerateArchetypeData(&found) || found.name.empty())
    { std::fputs("Native wormhole diagnostic could not locate a validated launcher record\n",stderr); return false; }
    PARCHETYPE archetype=ARCHLIST->LoadArchetype(found.name.c_str());
    if (!archetype) { std::fputs("Native wormhole diagnostic archetype load failed\n",stderr); return false; }
    std::unique_ptr<IBaseObject> sourceObject(ARCHLIST->CreateInstance(archetype));
    std::unique_ptr<IBaseObject> restoredObject(ARCHLIST->CreateInstance(archetype));
    auto *source=dynamic_cast<WormholeLauncher *>(sourceObject.get());
    auto *restored=dynamic_cast<WormholeLauncher *>(restoredObject.get());
    if (!source || !restored) { std::fputs("Native wormhole diagnostic factory type mismatch\n",stderr); return false; }
    struct PortalCleanup
    {
        std::vector<IBaseObject *> objects;
        ~PortalCleanup() { for (auto *object : objects) delete object; }
    } cleanup;
    IBaseObject *anchor=OBJLIST->GetObjectList();
    while (anchor && (!anchor->GetSystemID() || !anchor->GetPlayerID())) anchor=anchor->next;
    if (!anchor) { std::fputs("Native wormhole diagnostic anchor absent\n",stderr); return false; }
    auto *arch=static_cast<WormLauncherArchetype *>(ARCHLIST->GetArchetypeHandle(archetype));
    for (unsigned i=0;i<2;++i)
    {
        IBaseObject *object=ARCHLIST->CreateInstance(arch->pWormEffectType);
        if (!object) { std::fputs("Native wormhole diagnostic portal factory failed\n",stderr); return false; }
        cleanup.objects.push_back(object);
        auto &portal=i==0 ? source->hole1 : source->hole2;
        if (!object->QueryInterface(IWormholeBlastID,portal,NONSYSVOLATILEPTR))
        { std::fputs("Native wormhole diagnostic portal interface absent\n",stderr); return false; }
        TRANSFORM trans; trans.set_identity(); trans.set_position(anchor->GetPosition());
        portal->InitWormholeBlast(trans,anchor->GetSystemID(),1200,anchor->GetPlayerID(),i!=0);
        portal->Flash();
        OBJLIST->AddObject(object);
        object->Update();
    }
    source->mode=WORMHOLE_LAUNCHER_SAVELOAD::WORM_WORKING;
    source->numTargets=source->targetsLeft=1;
    source->targetIDs[0]=anchor->GetPartID(); source->workingID=0x1234;
    source->targetSystemID=anchor->GetSystemID(); source->lastChanceTimer=8.5f;
    source->targetPos=anchor->GetPosition();
    source->accumedDamage=2.75f; source->circleTime=1.25f;
    if (!source->Save(directory)) { std::fputs("Native wormhole diagnostic Save failed\n",stderr); return false; }
    if (!restored->Load(directory)) { std::fputs("Native wormhole diagnostic Load failed\n",stderr); return false; }
    restored->ResolveAssociations();
    cleanup.objects.push_back(dynamic_cast<IBaseObject *>(restored->hole1.Ptr()));
    cleanup.objects.push_back(dynamic_cast<IBaseObject *>(restored->hole2.Ptr()));
    NativeWormholeLauncherState before{},after{};
    if (!ReadWormholeRecord(directory,"NativeWormholeStateV1",&before,sizeof(before)) ||
        !restored->Save(directory) ||
        !ReadWormholeRecord(directory,"NativeWormholeStateV1",&after,sizeof(after)) ||
        std::memcmp(&before,&after,sizeof(before)) ||
        std::memcmp(static_cast<WORMHOLE_LAUNCHER_SAVELOAD *>(source),
                    static_cast<WORMHOLE_LAUNCHER_SAVELOAD *>(restored),sizeof(WORMHOLE_LAUNCHER_SAVELOAD)))
    { std::fputs("Native wormhole diagnostic restored state differs\n",stderr); return false; }
    std::unique_ptr<IBaseObject> rejectedObject(ARCHLIST->CreateInstance(archetype));
    auto *rejected=dynamic_cast<WormholeLauncher *>(rejectedObject.get());
    before.version=2;
    if (!rejected || !WriteWormholeRecord(directory,"NativeWormholeStateV1",&before,sizeof(before)) ||
        rejected->Load(directory)) return false;
    before.version=1;
    if (!WriteWormholeRecord(directory,"NativeWormholeStateV1",&before,sizeof(before)-1) ||
        rejected->Load(directory)) return false;
    return true;
}

// Explicit skirmish diagnostic: original script creation/order, full mission
// save/load, and live portal visuals. Never used by a normal player session.
int CheckNativeWormholeMissionRoundTrip(U32 &partID)
{
    if (!partID)
    {
        const U32 player=MGlobals::GetThisPlayer();
        IBaseObject *anchor=nullptr;
        U32 destination=0;
        for (auto *object=OBJLIST->GetObjectList();object;object=object->next)
            if (object->GetPlayerID()==player && object->GetSystemID() &&
                object->objClass==OC_SPACESHIP) { anchor=object; break; }
        if (!anchor) { std::fprintf(stderr,"Native Portal diagnostic: no ship for player %u\n",player); return -1; }
        for (auto *object=OBJLIST->GetObjectList();object;object=object->next)
            if (object->GetSystemID() && object->GetSystemID()!=anchor->GetSystemID())
            { destination=object->GetSystemID(); break; }
        if (!destination) { std::fputs("Native Portal diagnostic: no destination\n",stderr); return -1; }
        const auto portal=MScript::CreatePart("PLATGUN!!S_TesPortal",
            MScript::GetPartByID(anchor->GetPartID()),player,"Native Portal regression");
        if (!portal) { std::fputs("Native Portal diagnostic: stock creation failed\n",stderr); return -1; }
        partID=portal->dwMissionID;
        MScript::OrderDoWormhole(portal,destination);
        return 0;
    }
    std::vector<NativeWormholeVisual> before;
    for (auto *object=OBJLIST->GetObjectList();object;object=object->next)
    {
        OBJPTR<IWormholeBlast> portal;
        if (object->QueryInterface(IWormholeBlastID,portal,NONSYSVOLATILEPTR))
        {
            NativeWormholeVisual state{};
            if (!CaptureNativeWormholeVisual(portal,state)) { std::fputs("Native Portal diagnostic: capture failed\n",stderr); return -1; }
            before.push_back(state);
        }
    }
    if (before.empty()) return 0;
    if (before.size()!=2) { std::fprintf(stderr,"Native Portal diagnostic: count=%zu\n",before.size()); return -1; }
    if (!MISSION->QuickSaveFile()) { std::fputs("Native Portal diagnostic: mission save failed\n",stderr); return -1; }
    if (!MISSION->QuickLoadFile()) { std::fputs("Native Portal diagnostic: mission load failed\n",stderr); return -1; }
    if (!OBJLIST->FindObject(partID)) { std::fputs("Native Portal diagnostic: owner missing\n",stderr); return -1; }
    std::vector<NativeWormholeVisual> after;
    for (auto *object=OBJLIST->GetObjectList();object;object=object->next)
    {
        OBJPTR<IWormholeBlast> portal;
        if (object->QueryInterface(IWormholeBlastID,portal,NONSYSVOLATILEPTR))
        {
            NativeWormholeVisual state{};
            if (!CaptureNativeWormholeVisual(portal,state)) return -1;
            after.push_back(state);
        }
    }
    if (after.size()!=before.size()) { std::fprintf(stderr,"Native Portal diagnostic: restored count=%zu\n",after.size()); return -1; }
    // Object insertion order may change during association resolution.
    for (const auto &expected:before)
        if (std::none_of(after.begin(),after.end(),[&](const auto &actual) {
                return std::memcmp(&expected,&actual,sizeof(expected))==0;
            })) { std::fputs("Native Portal diagnostic: restored visuals differ\n",stderr); return -1; }
    return 1;
}
#endif
//------------------------------------------------------------------------------------------
//---------------------------WormholeLauncher Factory------------------------------------------
//------------------------------------------------------------------------------------------
//
struct DACOM_NO_VTABLE WormholeLauncherFactory : public IObjectFactory
{
	struct OBJTYPE : WormLauncherArchetype
	{
		PARCHETYPE pArchetype;

		void * operator new (size_t size)
		{
			return calloc(size,1);
		}
#if !defined(_WIN32)
		void operator delete (void *ptr)
		{
			::free(ptr);
		}
#endif

		OBJTYPE (void)
		{
		}

		~OBJTYPE (void)
		{
			if(pWormEffectType)
				ARCHLIST->Release(pWormEffectType, OBJREFNAME);

		}
	};
	//------------------------------------------------------------------------------------------
	//
	U32 factoryHandle;		// handles to callback

	//
	// Interface mapping
	//

	BEGIN_DACOM_MAP_INBOUND(WormholeLauncherFactory)
	DACOM_INTERFACE_ENTRY(IObjectFactory)
	END_DACOM_MAP()

	WormholeLauncherFactory (void) { }

	~WormholeLauncherFactory (void);

	void init (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* IObjectFactory methods */

	DEFMETHOD_(HANDLE,CreateArchetype) (const char *szArchname, OBJCLASS objClass, void *data);

	DEFMETHOD_(BOOL32,DestroyArchetype) (HANDLE hArchetype);

	DEFMETHOD_(IBaseObject *,CreateInstance) (HANDLE hArchetype);

	DEFMETHOD_(void,EditorCreateInstance) (HANDLE hArchetype, const MISSION_INFO & info);

	/* WormholeLauncherFactory methods */

	IDAComponent * getBase (void)
	{
		return static_cast<IObjectFactory *> (this);
	}
};
//--------------------------------------------------------------------------//
//
WormholeLauncherFactory::~WormholeLauncherFactory (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (OBJLIST && OBJLIST->QueryOutgoingInterface("IObjectFactory", connection) == GR_OK)
		connection->Unadvise(factoryHandle);
}
//--------------------------------------------------------------------------//
//
void WormholeLauncherFactory::init (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (OBJLIST->QueryOutgoingInterface("IObjectFactory", connection) == GR_OK)
		connection->Advise(getBase(), &factoryHandle);
}
//-----------------------------------------------------------------------------
//
HANDLE WormholeLauncherFactory::CreateArchetype (const char *szArchname, OBJCLASS objClass, void *_data)
{
	OBJTYPE * result = 0;

	if (objClass == OC_LAUNCHER)
	{
		BT_WORMHOLE_LAUNCHER * data = (BT_WORMHOLE_LAUNCHER *) _data;

		if (data->type == LC_WORMHOLE_LAUNCH)	   
		{
			result = new OBJTYPE;
			result->pArchetype = ARCHLIST->GetArchetype(szArchname);
			result->pWormEffectType = ARCHLIST->LoadArchetype("WORMEFF!!S_WORM");
			if(result->pWormEffectType)
				ARCHLIST->AddRef(result->pWormEffectType, OBJREFNAME);
#if !defined(_WIN32)
			else
			{
				CQERROR0("WormholeLauncher requires WORMEFF!!S_WORM");
				delete result;
				return 0;
			}
#endif
		}
	}

	return (HANDLE) result;
}
//-------------------------------------------------------------------
//
BOOL32 WormholeLauncherFactory::DestroyArchetype (HANDLE hArchetype)
{
	OBJTYPE * objtype = (OBJTYPE *)hArchetype;

//	EditorStopObjectInsertion(objtype->pArchetype);		// not supported
	delete objtype;

	return 1;
}
//-------------------------------------------------------------------
//
IBaseObject * WormholeLauncherFactory::CreateInstance (HANDLE hArchetype)
{
	OBJTYPE * objtype = (OBJTYPE *)hArchetype;
	return createWormholeLauncher(objtype->pArchetype);
}
//-------------------------------------------------------------------
//
void WormholeLauncherFactory::EditorCreateInstance (HANDLE hArchetype, const MISSION_INFO & info)
{
	// not supported
}
//---------------------------------------------------------------------------
//-------------------------GLOBAL STARTUP------------------------------------
//---------------------------------------------------------------------------
//
struct _wormholeLauncher : GlobalComponent
{
	WormholeLauncherFactory * sfactory;

	virtual void Startup (void)
	{
		sfactory = new DAComponent<WormholeLauncherFactory>;
		AddToGlobalCleanupList((IDAComponent **) &sfactory);
	}

	virtual void Initialize (void)
	{
		sfactory->init();
	}
};

static _wormholeLauncher __ship;

//---------------------------------------------------------------------------------------------
//-------------------------------End WormholeLauncher.cpp------------------------------------------------
//---------------------------------------------------------------------------------------------
