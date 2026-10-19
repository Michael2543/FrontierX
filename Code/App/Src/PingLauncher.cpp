//--------------------------------------------------------------------------//
//                                                                          //
//                                PingLaunch.cpp                           //
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
#include <memory>
#include "Include/AppArchetypeNode.h"
using std::min;
using std::max;
struct IDrawAgent;
#endif

#include "PingLauncher.h"

#include "SuperTrans.h"
#include "ObjList.h"
#include "SFX.h"

#include "Mission.h"
#include "IMissionActor.h"
#include "Startup.h"
#include "ArchHolder.h"
#include "TerrainMap.h"
#include "MPart.h"

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

//---------------------------------------------------------------------------
//
PingLaunch::PingLaunch (void) 
{
	pingLaunchIndex = -1;
}
//---------------------------------------------------------------------------
//
PingLaunch::~PingLaunch (void)
{
}
//---------------------------------------------------------------------------
//
void PingLaunch::init (PARCHETYPE _pArchetype)
{
	const BT_PING_LAUNCHER * data = (const BT_PING_LAUNCHER *) ARCHLIST->GetArchetypeData(_pArchetype);

	CQASSERT(data);
	CQASSERT(data->type == LC_PING_LAUNCH);
	CQASSERT(data->objClass == OC_LAUNCHER);

	pArchetype = _pArchetype;
	objClass = OC_LAUNCHER;

}
//---------------------------------------------------------------------------
//
void PingLaunch::DoSpecialAbility (U32 specialID)
{
	{
		MPart part = owner.Ptr();
		if (part.isValid() && part->hullPoints==0)
			return;		// don't do this ability if already dead!
	}

	if (checkSupplies())
	{
		bPingIt = true;
	}
}
//---------------------------------------------------------------------------
//
S32 PingLaunch::GetObjectIndex (void) const
{
	// not too sure about this...
	return owner.Ptr()->GetObjectIndex();
}
//---------------------------------------------------------------------------
//
U32 PingLaunch::GetPartID (void) const
{
	return owner.Ptr()->GetPartID();
}
//---------------------------------------------------------------------------
//
BOOL32 PingLaunch::Update (void)
{
	checkTechLevel();

	return 1;
}
//---------------------------------------------------------------------------
//
void PingLaunch::InitLauncher (IBaseObject * _owner, S32 ownerIndex, LAUNCHER_ANIM_ARCH animArcheIndex, SINGLE range)
{
	const BT_PING_LAUNCHER * data = (const BT_PING_LAUNCHER *) ARCHLIST->GetArchetypeData(pArchetype);

	pingSupplyCost = data->supplyCost;
	techNode =		 data->techNode;

	CQASSERT(_owner);
	_owner->QueryInterface(ILaunchOwnerID, owner, LAUNCHVOLATILEPTR);
	CQASSERT(owner!=0);
}
//---------------------------------------------------------------------------
//
const TRANSFORM & PingLaunch::GetTransform (void) const
{
	return owner.Ptr()->GetTransform();
}
//---------------------------------------------------------------------------
//
bool PingLaunch::checkSupplies()
{
	return ((owner.Ptr()->effectFlags.canShoot()) && owner->UseSupplies(pingSupplyCost));
}
//---------------------------------------------------------------------------
//
void PingLaunch::checkTechLevel()
{
	// check if we are able to use this special weapon
	TECHNODE techLevel = MGlobals::GetCurrentTechLevel(owner.Ptr()->GetPlayerID());

	// here, we are going to set the caps bit
	MPartNC partNC(owner.Ptr());

	if (techLevel.HasTech(techNode))
	{
		partNC->caps.specialAbilityOk = true;
	}
	else
	{
		partNC->caps.specialAbilityOk = false;
	}
}


//---------------------------------------------------------------------------
//
inline IBaseObject * createPingLaunch (PARCHETYPE pArchetype)
{
	PingLaunch * pingLaunch = new ObjectImpl<PingLaunch>;

	pingLaunch->init(pArchetype);

	return pingLaunch;
}
//------------------------------------------------------------------------------------------
//---------------------------PingLaunch Factory------------------------------------------
//------------------------------------------------------------------------------------------
//
struct DACOM_NO_VTABLE PingLaunchFactory : public IObjectFactory
{
	struct OBJTYPE 
	{
		PARCHETYPE pArchetype;
		PARCHETYPE pBoltType;

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
			if (pBoltType)
				ARCHLIST->Release(pBoltType, OBJREFNAME);
		}
	};
	//------------------------------------------------------------------------------------------
	//
	U32 factoryHandle;		// handles to callback

	//
	// Interface mapping
	//

	BEGIN_DACOM_MAP_INBOUND(PingLaunchFactory)
	DACOM_INTERFACE_ENTRY(IObjectFactory)
	END_DACOM_MAP()

	PingLaunchFactory (void) { }

	~PingLaunchFactory (void);

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

	/* PingLaunchFactory methods */

	IDAComponent * getBase (void)
	{
		return static_cast<IObjectFactory *> (this);
	}
};
//--------------------------------------------------------------------------//
//
PingLaunchFactory::~PingLaunchFactory (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (OBJLIST && OBJLIST->QueryOutgoingInterface("IObjectFactory", connection) == GR_OK)
		connection->Unadvise(factoryHandle);
}
//--------------------------------------------------------------------------//
//
void PingLaunchFactory::init (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (OBJLIST->QueryOutgoingInterface("IObjectFactory", connection) == GR_OK)
		connection->Advise(getBase(), &factoryHandle);
}
//-----------------------------------------------------------------------------
//
HANDLE PingLaunchFactory::CreateArchetype (const char *szArchname, OBJCLASS objClass, void *_data)
{
	OBJTYPE * result = 0;

	if (objClass == OC_LAUNCHER)
	{
		BT_PING_LAUNCHER * data = (BT_PING_LAUNCHER *) _data;

		if (data->type == LC_PING_LAUNCH)	   
		{
			result = new OBJTYPE;
			result->pArchetype = ARCHLIST->GetArchetype(szArchname);
		}
	}

	return (HANDLE) result;
}
//-------------------------------------------------------------------
//
BOOL32 PingLaunchFactory::DestroyArchetype (HANDLE hArchetype)
{
	OBJTYPE * objtype = (OBJTYPE *)hArchetype;

//	EditorStopObjectInsertion(objtype->pArchetype);		// not supported
	delete objtype;

	return 1;
}
//-------------------------------------------------------------------
//
IBaseObject * PingLaunchFactory::CreateInstance (HANDLE hArchetype)
{
	OBJTYPE * objtype = (OBJTYPE *)hArchetype;
	return createPingLaunch(objtype->pArchetype);
}
//-------------------------------------------------------------------
//
void PingLaunchFactory::EditorCreateInstance (HANDLE hArchetype, const MISSION_INFO & info)
{
	// not supported
}
//---------------------------------------------------------------------------
//-------------------------GLOBAL STARTUP------------------------------------
//---------------------------------------------------------------------------
//
struct _pingLaunch : GlobalComponent
{
	PingLaunchFactory * sfactory;

	virtual void Startup (void)
	{
		sfactory = new DAComponent<PingLaunchFactory>;
		AddToGlobalCleanupList((IDAComponent **) &sfactory);
	}

	virtual void Initialize (void)
	{
		sfactory->init();
	}
};

static _pingLaunch __ship;

#if !defined(_WIN32)
bool ValidateNativeStatelessPingRestart(IBaseObject *launcher, PARCHETYPE archetype,
    IBaseObject *owner, IFileSystem *directory, const char *directoryName)
{
	PingLaunch *ping = dynamic_cast<PingLaunch *>(launcher);
	if (!ping || !archetype || !owner || !directory || !directoryName || !ARCHLIST ||
		ping->objClass != OC_LAUNCHER || ping->pArchetype != archetype || ping->bPingIt)
		return false;

	const BT_PING_LAUNCHER *data = static_cast<const BT_PING_LAUNCHER *>(ARCHLIST->GetArchetypeData(archetype));
	if (!data || data->objClass != OC_LAUNCHER || data->type != LC_PING_LAUNCH ||
		ping->pingSupplyCost != static_cast<SINGLE>(data->supplyCost) ||
		std::memcmp(&ping->techNode, &data->techNode, sizeof(data->techNode)) != 0)
		return false;

	OBJPTR<ILaunchOwner> expectedOwner;
	if (!owner->QueryInterface(ILaunchOwnerID, expectedOwner, LAUNCHVOLATILEPTR) ||
		ping->owner.Ptr() != expectedOwner.Ptr())
		return false;

	char path[1024];
	const DWORD length = directory->GetCurrentDirectory(sizeof(path), path);
	if (!length || length >= sizeof(path))
		return false;
	const char *leaf = path;
	for (const char *next = path; *next; ++next)
		if (*next == '/' || *next == '\\') leaf = next + 1;
	if (std::strcmp(leaf, directoryName) != 0)
		return false;

	WIN32_FIND_DATA found = {};
	HANDLE search = directory->FindFirstFile("*", &found);
	if (search != INVALID_HANDLE_VALUE)
	{
		directory->FindClose(search);
		return false;
	}
	const DWORD error = directory->GetLastError();
	return error == ERROR_FILE_NOT_FOUND || error == ERROR_NO_MORE_FILES;
}
namespace
{
// Synthetic callers exercise the production factory and restart contract without
// entering simulation. Any unexpected caller operation fails explicitly.
struct NativePingRecordProvider : IArchetypeList
{
 BT_PING_LAUNCHER record{};
 ARCHNODE node;
 NativePingRecordProvider() { record.objClass=OC_LAUNCHER; record.type=LC_PING_LAUNCH; record.supplyCost=7; }
 GENRESULT QueryInterface(const C8 *, void **) override { std::abort(); }
 U32 AddRef() override { std::abort(); }
 U32 Release() override { std::abort(); }
 PARCHETYPE GetArchetype(const C8 *name) override { return std::strcmp(name,"NativePingCheck")==0?&node:nullptr; }
 PARCHETYPE LoadArchetype(const C8 *name) override { return GetArchetype(name); }
 BOOL32 UnloadArchetype(const C8 *) override { std::abort(); }
 IBaseObject *CreateInstance(PARCHETYPE) override { std::abort(); }
 IBaseObject *CreateInstance(const char *) override { std::abort(); }
 const char *GetArchName(PARCHETYPE) override { std::abort(); }
 const char *GetArchName(U32) override { std::abort(); }
 void *GetArchetypeData(PARCHETYPE archetype) override { return archetype==&node?&record:nullptr; }
 void *GetArchetypeData(const C8 *name) override { return GetArchetypeData(GetArchetype(name)); }
 U32 GetArchetypeDataID(const C8 *) override { std::abort(); }
 U32 GetArchetypeDataID(PARCHETYPE) override { std::abort(); }
 void *GetArchetypeData(U32) override { std::abort(); }
 PARCHETYPE LoadArchetype(U32) override { std::abort(); }
 void *GetArchetypeHandle(PARCHETYPE) override { std::abort(); }
 BOOL32 EnumerateArchetypeData(IArchetypeEnum *) override { std::abort(); }
 void AddRef(PARCHETYPE,const char *) override { std::abort(); }
 void Release(PARCHETYPE,const char *) override { std::abort(); }
 IBaseObject *CreateUIAnim(UIANIMTYPE,const Vector &) override { std::abort(); }
};
struct NativePingCheckOwner : IBaseObject, ILaunchOwner
{
 BEGIN_MAP_INBOUND(NativePingCheckOwner)
 _INTERFACE_ENTRY(IBaseObject)
 _INTERFACE_ENTRY(ILaunchOwner)
 END_MAP()
 bool UseSupplies(U32,bool) override { std::abort(); }
 bool TestLOS(const GRIDVECTOR &) override { std::abort(); }
 void GetLauncher(const U32,OBJPTR<ILauncher> &) override { std::abort(); }
 void LauncherCancelAttack() override { std::abort(); }
 void GotoLauncherPosition(GRIDVECTOR) override { std::abort(); }
 void LauncherSendOpData(U32,void *,U32) override { std::abort(); }
 void LaunchOpCompleted(ILauncher *,U32) override { std::abort(); }
 U32 CreateLauncherOp(ILauncher *,ObjSet &,void *,U32) override { std::abort(); }
 SINGLE GetWeaponRange() override { std::abort(); }
 SINGLE GetOptimalWeaponRange() override { std::abort(); }
 IBaseObject *GetTarget() override { std::abort(); }
 bool HasAttackAgent() override { std::abort(); }
 IBaseObject *FindChildTarget(U32) override { std::abort(); }
};
struct NativePingArchetypeScope
{
 IArchetypeList *previous;
 explicit NativePingArchetypeScope(IArchetypeList *records) : previous(ARCHLIST) { ARCHLIST=records; }
 ~NativePingArchetypeScope() { ARCHLIST=previous; }
};
}

bool CheckNativeStatelessPingRestart(IFileSystem *directory)
{
 // Existing AI persistence fixtures have no stateless launcher or typed owner.
 // A valid source restart must accept only the genuine freshly initialized Ping
 // with its canonical empty marker; no original Ping Load success is fabricated.
 if (!directory || !HEAP || OBJLIST) return false;
 NativePingRecordProvider records;
 NativePingArchetypeScope binding(&records);
 DAFILEDESC descriptor="PingRegression.mission";
 descriptor.lpImplementation="UTF";
 descriptor.dwDesiredAccess=GENERIC_READ|GENERIC_WRITE;
 descriptor.dwCreationDistribution=CREATE_ALWAYS;
 COMPTR<IFileSystem> archive;
 if (directory->CreateInstance(&descriptor,archive)!=GR_OK ||
     !archive->CreateDirectory("Turret-0") || !archive->SetCurrentDirectory("Turret-0")) return false;
 auto owner=std::unique_ptr<IBaseObject>(new ObjectImpl<NativePingCheckOwner>);
 auto otherOwner=std::unique_ptr<IBaseObject>(new ObjectImpl<NativePingCheckOwner>);
 auto factory=std::unique_ptr<DAComponent<PingLaunchFactory>>(new DAComponent<PingLaunchFactory>);
 HANDLE archetype=factory->CreateArchetype("NativePingCheck",OC_LAUNCHER,&records.record);
 if (!archetype) return false;
 auto object=std::unique_ptr<IBaseObject>(factory->CreateInstance(archetype));
 if (!object) { factory->DestroyArchetype(archetype); return false; }
 ++records.node.usage; // The normal ObjectList creation owner holds this usage.
 PingLaunch *ping=dynamic_cast<PingLaunch *>(object.get());
 if (!ping) { object.reset(); factory->DestroyArchetype(archetype); return false; }
 ping->InitLauncher(owner.get(),-1,-1,0);
 bool valid=ping->Save(archive)!=0 && ping->Load(archive)==0 &&
     ValidateNativeStatelessPingRestart(object.get(),&records.node,owner.get(),archive,"Turret-0") &&
     !ValidateNativeStatelessPingRestart(otherOwner.get(),&records.node,owner.get(),archive,"Turret-0") &&
     !ValidateNativeStatelessPingRestart(object.get(),&records.node,otherOwner.get(),archive,"Turret-0") &&
     !ValidateNativeStatelessPingRestart(object.get(),&records.node,owner.get(),archive,"Turret-1");
 if (valid)
 {
  descriptor="UnexpectedPayload";
  descriptor.lpImplementation="DOS";
  descriptor.dwDesiredAccess=GENERIC_READ|GENERIC_WRITE;
  descriptor.dwCreationDistribution=CREATE_ALWAYS;
  COMPTR<IFileSystem> payload;
  U32 written=0; const U8 byte=1;
  valid=archive->CreateInstance(&descriptor,payload)==GR_OK &&
      payload->WriteFile(0,&byte,sizeof(byte),&written,0) && written==sizeof(byte) &&
      !ValidateNativeStatelessPingRestart(object.get(),&records.node,owner.get(),archive,"Turret-0");
  payload.free();
  valid=valid && archive->DeleteFile("UnexpectedPayload") &&
      ValidateNativeStatelessPingRestart(object.get(),&records.node,owner.get(),archive,"Turret-0");
 }
 valid=valid && CheckNativeTalorianRestart(archive,owner.get());
 object.reset();
 valid=factory->DestroyArchetype(archetype) && valid && records.node.usage==0;
 return valid;
}

#endif

//---------------------------------------------------------------------------------------------
//-------------------------------End PingLaunch.cpp------------------------------------------------
//---------------------------------------------------------------------------------------------
