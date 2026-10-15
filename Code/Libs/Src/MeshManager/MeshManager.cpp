// MeshManager.cpp : Defines the entry point for the dll application.
//

#include "stdafx.h"
#if !defined(_WIN32)
#include <ICamera.h>
#include "FrontierX/NativeMeshManager.h"
#include <ITextureLibrary.h>
#endif
#include <IMeshManager.h>
#include <TComponent.h>
#include <da_heap_utility.h>
#include <TSmartPointer.h>
#include <rendpipeline.h>
#include <SYSTEM.H>

#include "MeshArch.h"
#include <IAnim.h>
#include "MeshInstance.h"

#define CLSID_MeshManager "IMeshManager"

#if !defined(_WIN32)
namespace FrontierXMeshManager
{
#endif

ICOManager * DACOM = NULL;

struct MeshManager : IMeshManager, public ISystemComponent, IInternalMeshManager
{
	BEGIN_DACOM_MAP_INBOUND(MeshManager)
		DACOM_INTERFACE_ENTRY(IMeshManager)
		DACOM_INTERFACE_ENTRY2(IID_IMeshManager,IMeshManager)
		DACOM_INTERFACE_ENTRY(IAggregateComponent)
		DACOM_INTERFACE_ENTRY2(IID_IAggregateComponent,IAggregateComponent)
		DACOM_INTERFACE_ENTRY(ISystemComponent)
		DACOM_INTERFACE_ENTRY2(IID_ISystemComponent,ISystemComponent)
	END_DACOM_MAP()

	// ISystemComponent 
	virtual GENRESULT COMAPI Initialize(void);

	virtual void COMAPI Update (void);

	virtual GENRESULT COMAPI ShutdownAggregate(void);

	GENRESULT COMAPI init( AGGDESC *desc );

	//IInternalMeshManager
	virtual IEngine * GetEngine();

	virtual IAnimation * GetAnim();

	virtual IRenderer * GetRenderer();

	virtual IRenderPipeline * GetPipe();

	virtual ICamera * GetCamera();

	virtual struct IHardpoint * GetHardpoint();

	virtual struct IMaterialManager * GetMatManager();

	//virtual void ReleaseArch(IMeshArchetype * arch);

	// IMeshManager
	virtual void InitManager(InitInfo & info);

	virtual IMeshArchetype * CreateMeshArch(const char * filename);

	virtual IMeshArchetype * FindMeshArch(const char * filename);

	virtual IMeshInstance * CreateMesh(IMeshArchetype * arch, IEngineInstance * engInst = NULL);

	virtual IMeshInstance * CreateMesh(const char * filename, IEngineInstance * engInst = NULL);

	virtual IMeshInstance * CreateDynamicMesh(U32 numVerts, IMaterial * mat);

	virtual IMeshInstance * CreateRefMesh(IMeshInstance * source);

	virtual void DestroyMesh(IMeshInstance * mesh);

	virtual void ReleaseArch(IMeshArchetype * arch);

	//MeshManager
	MeshManager();

	~MeshManager();

	//data

	IEngine * ENGINE;
	IAnimation * ANIM;
	IFileSystem * OBJDIR;
	IRenderer * REND;
	IRenderPipeline * PIPE;
	ICamera * CAMERA;
	IHardpoint *HARDPOINT;
	IMaterialManager * MATMAN;

#if !defined(_WIN32)
	// Borrowed aggregate parent, whose lifetime contains this component.
	IDAComponent * nativeSystem = NULL;
#endif
	MeshArch * meshArchList;
	MeshInstance * dynamicMeshCache;

};

GENRESULT COMAPI MeshManager::Initialize(void) 
{ 
	return GR_OK; 
};

void COMAPI MeshManager::Update (void)
{
}

GENRESULT COMAPI MeshManager::ShutdownAggregate(void)
{
	while(dynamicMeshCache)
	{
		MeshInstance * tmp = dynamicMeshCache;
		dynamicMeshCache = dynamicMeshCache->next;
		delete tmp;
	}
	while(meshArchList)
	{
		MeshArch * tmp = meshArchList;
		meshArchList = meshArchList->next;
		delete tmp;
	}
	if(ENGINE)
	{
		ENGINE->Release();
		ENGINE = NULL;
	}
	if(ANIM)
	{
		ANIM->Release();
		ANIM = NULL;
	}
	return GR_OK;
}

GENRESULT COMAPI MeshManager::init( AGGDESC *desc )
{
#if !defined(_WIN32)
	if (!desc || !desc->outer) return GR_INVALID_PARMS;
	nativeSystem = desc->outer;
#endif
	return GR_OK;
}

IEngine * MeshManager::GetEngine()
{
	return ENGINE;
}

IAnimation * MeshManager::GetAnim()
{
	return ANIM;
}

IRenderer * MeshManager::GetRenderer()
{
	return REND;
}

IRenderPipeline * MeshManager::GetPipe()
{
	return PIPE;
}

ICamera * MeshManager::GetCamera()
{
	return CAMERA;
}

IHardpoint * MeshManager::GetHardpoint()
{
	return HARDPOINT;
}

struct IMaterialManager * MeshManager::GetMatManager()
{
	return MATMAN;
}

void MeshManager::InitManager(InitInfo & info)
{
	ENGINE = info.ENGINE;
	ENGINE->AddRef();
	ANIM = info.ANIM;
	ANIM->AddRef();
	OBJDIR = info.OBJDIR;
	REND = info.REND;
	PIPE = info.PIPE;
	CAMERA = info.CAMERA;
	HARDPOINT = info.HARDPOINT;
	MATMAN = info.MATMAN;
}

IMeshArchetype * MeshManager::CreateMeshArch(const char * filename)
{
	MeshArch  * arch = (MeshArch *) FindMeshArch(filename);
	if(arch)
	{
		arch->refCount++;
		return arch;
	}

	DAFILEDESC fdesc = filename;
	COMPTR<IFileSystem> objFile;
	if (OBJDIR->CreateInstance(&fdesc, objFile) != GR_OK)
	{
		DACOM->CreateInstance(&fdesc,objFile);
	}

	if(objFile)
	{
#if !defined(_WIN32)
		// Original compound tools load the owning file's shared texture and
		// texture-animation libraries before Engine visits individual parts.
		// Child XMesh libraries alone cannot resolve maps stored at CMP root.
		bool hasSharedLibrary = false;
		for (const char *name : {"Texture library", "Animation library"})
			if (objFile->SetCurrentDirectory(name))
			{
				hasSharedLibrary = true;
				if (!objFile->SetCurrentDirectory(".."))
				{
					std::fprintf(stderr, "Native MeshManager cannot restore shared library directory: %s\n", filename);
					return NULL;
				}
			}
		if (hasSharedLibrary)
		{
			COMPTR<ITextureLibrary> textures;
			if (!nativeSystem || nativeSystem->QueryInterface(IID_ITextureLibrary, textures) != GR_OK || !textures)
			{
				std::fprintf(stderr, "Native MeshManager shared texture owner unavailable: %s\n", filename);
				return NULL;
			}
			if (textures->load_library(objFile, NULL) != GR_OK)
			{
				std::fprintf(stderr, "Native MeshManager shared texture library rejected: %s\n", filename);
				return NULL;
			}
		}
		// Files without an outer library retain their original child-library
		// loading. Required map failures remain fatal in the material owner.
#endif
		const ARCHETYPE_INDEX engineArch = ENGINE->create_archetype(filename,objFile);
		const SCRIPT_SET_ARCH animationArch = ANIM->create_script_set_arch(objFile);
		if(engineArch == INVALID_ARCHETYPE_INDEX)
		{
			if(animationArch != INVALID_SCRIPT_SET_ARCH)
				ANIM->release_script_set_arch(animationArch);
			return NULL;
		}
		MeshArch * arch = new MeshArch(engineArch,animationArch);
		arch->next = meshArchList;
		meshArchList = arch;

		arch->owner = this;

		arch->Realize();

		return arch;
	}
	return NULL;
}

IMeshArchetype * MeshManager::FindMeshArch(const char * filename)
{
	MeshArch * search = meshArchList;
	while(search)
	{
		if((!(search->bDynamic)) && (!(search->bRef)))
		{
			const char *existingName = ENGINE->get_archetype_name(search->GetArchIndex());
			if(existingName && strcmp(filename,existingName) == 0)
				return search;
		}
		search = search->next;
	}
	return NULL;
}

IMeshInstance * MeshManager::CreateMesh(IMeshArchetype * arch, struct IEngineInstance * engInst)
{
	MeshInstance * mesh = new MeshInstance();

	mesh->owner = this;

	if(!mesh->Initialize((MeshArch*)arch,engInst))
	{
		delete mesh;
		return NULL;
	}

	return mesh;
}

IMeshInstance * MeshManager::CreateMesh(const char * filename, struct IEngineInstance * engInst)
{
	MeshArch * arch = (MeshArch*)CreateMeshArch(filename);//+1 arch count
	if(arch)
	{
		IMeshInstance * mesh = CreateMesh(arch,engInst);//+1 arch count (extra)
		if(!mesh)
		{
			arch->Release();
			return NULL;
		}
		(arch->refCount)--;//fix arch count
		return mesh;
	}
	return NULL;
}

IMeshInstance * MeshManager::CreateDynamicMesh(U32 numVerts, IMaterial * mat)
{
	MeshInstance * inst = dynamicMeshCache;
	MeshInstance * prev = NULL;
	while(inst)
	{
		if(inst->arch->allocatedVerts >= numVerts)
		{
			if(prev)
				prev->next = inst->next;
			else
				dynamicMeshCache = inst->next;
			inst->arch->ReinitDynamic(mat,numVerts);
			inst->timer = 0.0f;
			return inst;
		}
		prev = inst;
		inst = inst->next;
	}
	MeshArch * arch = new MeshArch(mat,numVerts);
	arch->next = meshArchList;
	meshArchList = arch;

	arch->owner = this;

	arch->Realize();

	IMeshInstance * mesh = CreateMesh(arch);
	(arch->refCount)--;//fix arch count
	return mesh;
}

IMeshInstance * MeshManager::CreateRefMesh(IMeshInstance * source)
{
	MeshArch * arch = new MeshArch((MeshArch*)(source->GetArchtype()));
	arch->next = meshArchList;
	meshArchList = arch;

	arch->owner = this;

	arch->Realize();

	return CreateMesh(arch);
}

void MeshManager::DestroyMesh(IMeshInstance * mesh)
{
	MeshInstance * inst = ((MeshInstance *) mesh);
	if(inst->arch->bDynamic)
	{
		MeshInstance * sort = dynamicMeshCache;
		MeshInstance * prev = NULL;
		while(sort)
		{
			if(sort->arch->allocatedVerts > inst->arch->allocatedVerts)
			{
				if(prev)
					prev->next = inst;
				else 
					dynamicMeshCache = inst;
				inst->next = sort;
				return;
			}
			prev = sort;
			sort = sort->next;
		}
		//at the end of the list
		if(prev)
			prev->next = inst;
		else 
			dynamicMeshCache = inst;
		inst->next = NULL;
	}
	else
		delete inst;
}

void MeshManager::ReleaseArch(IMeshArchetype * arch)
{
	MeshArch * meshArch = (MeshArch *)arch;
	meshArch->refCount--;
	if(!(meshArch->refCount))
	{
		MeshArch * search = meshArchList;
		MeshArch * prev = NULL;
		while(search)
		{
			if(search == meshArch)
			{
				if(prev)
					prev->next = search->next;
				else
					meshArchList = search->next;

				if(!(search->bRef))
				{
					if(search->GetArchIndex() != INVALID_ARCHETYPE_INDEX)
					{
						ENGINE->release_archetype(search->GetArchIndex());
					}

					if(search->GetAnimArchtype() != INVALID_SCRIPT_SET_ARCH)
					{
						ANIM->release_script_set_arch(search->GetAnimArchtype());
					}
				}
				delete search;
				return;
			}
			prev = search;
			search = search->next;
		}
	}
}

MeshManager::MeshManager()
{
	ENGINE = NULL;
	ANIM = NULL;
	OBJDIR = NULL;
	REND = NULL;
	PIPE = NULL;
	CAMERA = NULL;
	HARDPOINT = NULL;
	MATMAN = NULL;
	meshArchList = NULL;
	dynamicMeshCache = NULL;
}

MeshManager::~MeshManager()
{
	while(dynamicMeshCache)
	{
		MeshInstance * tmp = dynamicMeshCache;
		dynamicMeshCache = dynamicMeshCache->next;
		delete tmp;
	}
	while(meshArchList)
	{
		MeshArch * tmp = meshArchList;
		meshArchList = meshArchList->next;
		delete tmp;
	}
}

#if !defined(_WIN32)
}
#endif

#if defined(_WIN32)
BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		//
		// DLL_PROCESS_ATTACH: Create object server component and register it with DACOM manager
		//
		case DLL_PROCESS_ATTACH:
		{
			DA_HEAP_ACQUIRE_HEAP(HEAP);
			DA_HEAP_DEFINE_HEAP_MESSAGE( hModule );

			DACOM = DACOM_Acquire(); 
			IComponentFactory * server;

			// Register System aggragate factory
			if( DACOM && (server = new DAComponentFactory2<DAComponentAggregate<MeshManager>, AGGDESC>(CLSID_MeshManager)) != NULL ) 
			{
				DACOM->RegisterComponent( server, CLSID_MeshManager, DACOM_NORMAL_PRIORITY );
				server->Release();
			}
			
			break;
		}

		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}
#endif

#if !defined(_WIN32)
namespace FrontierX
{
bool RegisterNativeMeshManager(ICOManager * manager)
{
	if (!manager) return false;
	FrontierXMeshManager::DACOM = manager;
	IComponentFactory * factory =
		new DAComponentFactory2<DAComponentAggregate<FrontierXMeshManager::MeshManager>, AGGDESC>(CLSID_MeshManager);
	const GENRESULT result = manager->RegisterComponent(factory, CLSID_MeshManager);
	factory->Release();
	return result == GR_OK;
}
}
#endif
