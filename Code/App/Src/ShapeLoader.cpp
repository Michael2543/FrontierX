//--------------------------------------------------------------------------//
//                                                                          //
//                              ShapeLoader.cpp                             //
//                                                                          //
//                  COPYRIGHT (C) 1999 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Header: /Conquest/App/Src/ShapeLoader.cpp 5     8/29/00 5:00p Sbarton $
*/			    
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
#ifdef _WIN32
#include "pch.h"
#include <globals.h>
#else
#include "NativeAppGlobals.h"
#include "NativeAppImage.h"
#include <cstring>
#include <cstdlib>
#endif

#include "IShapeLoader.h"
#include "DrawAgent.h"
#include <TManager.h>
#include "Startup.h"
#include "GenData.h"
#include "CQTrace.h"
#include "IImageReader.h"

#include <TComponent.h>
#include <Filesys.h>
#include <HKEvent.h>
#include <HeapObj.h>
#include <TSmartPointer.h>
#include <IConnection.h>

//--------------------------------------------------------------------------//
//
struct SHPTYPE
{
	PGENTYPE pArchetype;
	const void * pImage;				
	DA::FILETYPE fileType;
#ifndef _WIN32
    U32 imageBytes;
#endif

	SHPTYPE (void)
	{
	}

	bool init (const char * filename)
	{
		// funky way to determine the file type
		#ifdef _WIN32
        U32 file_type = ((U32 *)(filename + strlen(filename) - 4))[0] & ~0x20202000;
#else
        if (!filename || strlen(filename) < 4 || !INTERFACEDIR) return false;
        const auto *suffix = reinterpret_cast<const unsigned char *>(filename + strlen(filename) - 4);
        const U32 file_type = (U32(suffix[0]) | (U32(suffix[1]) << 8) |
            (U32(suffix[2]) << 16) | (U32(suffix[3]) << 24)) & ~0x20202000U;
#endif

		switch (file_type)
		{
		case 'PMB.':
			fileType = DA::BMP;
			break;
		case 'PHS.':
			fileType = DA::VFX;
			break;
		case 'AGT.':
			fileType = DA::TGA;
			break;

		default:
			CQBOMB0("Unknown file type");
            return false;
		}

		// now load the file
		struct IFileSystem * const parent = INTERFACEDIR;
		DAFILEDESC fdesc = filename;
		HANDLE hFile;
		if ((hFile = parent->OpenChild(&fdesc)) == INVALID_HANDLE_VALUE)
		{
			CQFILENOTFOUND(fdesc.lpFileName);
			return false;
		}
		U32 size = parent->GetFileSize(hFile);
#ifndef _WIN32
        imageBytes = size;
#endif
		pImage = (const void *) malloc(size);
		DWORD dwRead = 0;
        if (!pImage) { parent->CloseHandle(hFile); return false; }
		parent->ReadFile(hFile, (void *)pImage, size, &dwRead, 0);
		parent->CloseHandle(hFile);
		return (dwRead == size);
	}

	~SHPTYPE (void)
	{
		::free((void *)pImage);
	}

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}
};

//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE ShapeLoader : IShapeLoader
{
	//
	// incoming interface map
	//
	BEGIN_DACOM_MAP_INBOUND(ShapeLoader)
	DACOM_INTERFACE_ENTRY(IShapeLoader)
	END_DACOM_MAP()

	//
	// data items
	// 

	SHPTYPE * pShpType;

	//
	// class methods
	//

	ShapeLoader (void)
	{
	}

	~ShapeLoader (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* IShapeLoader methods  */

	virtual GENRESULT CreateDrawAgent (U32 subImage, struct IDrawAgent ** drawAgent, RECT * pRect);

	virtual GENRESULT CreateImageReader (U32 subImage, struct IImageReader ** imageReader);
};
//--------------------------------------------------------------------------//
//
ShapeLoader::~ShapeLoader (void)
{
	GENDATA->Release(pShpType->pArchetype);
}
//--------------------------------------------------------------------------//
//
GENRESULT ShapeLoader::CreateDrawAgent (U32 subImage, struct IDrawAgent ** drawAgent, RECT * pRect)
{
#ifndef _WIN32
    if (!drawAgent) return GR_INVALID_PARMS;
    *drawAgent = nullptr;
    COMPTR<IImageReader> imageReader;
    if (CreateImageReader(subImage, imageReader) == GR_OK && imageReader)
        ::CreateDrawAgent(imageReader, drawAgent, FALSE, pRect);
#else
	if (pShpType->fileType == DA::VFX)
	{
		GT_VFXSHAPE * data = (GT_VFXSHAPE *)(GENDATA->GetArchetypeData(pShpType->pArchetype));
		::CreateDrawAgent((const VFX_SHAPETABLE *)pShpType->pImage, subImage, drawAgent, FALSE/*bHiRes: CQ2 per-shape flag removed, retail=standard res*/, pRect);
	}
	else
	if (subImage == 0)	// other formats only have one image
	{
		GT_VFXSHAPE * data = (GT_VFXSHAPE *)(GENDATA->GetArchetypeData(pShpType->pArchetype));
		COMPTR<IImageReader> imageReader;
		CreateImageReader(subImage, imageReader);

		if (imageReader)
			::CreateDrawAgent(imageReader, drawAgent, FALSE/*bHiRes removed*/, pRect);
	}

#endif
	return (*drawAgent!=0) ? GR_OK : GR_GENERIC;
}
//--------------------------------------------------------------------------//
//
GENRESULT ShapeLoader::CreateImageReader (U32 subImage, struct IImageReader ** imageReader)
{
	int offset = 0;
#ifndef _WIN32
    if (!imageReader) return GR_INVALID_PARMS;
    *imageReader = nullptr;
#endif

	switch (pShpType->fileType)
	{
	case DA::TGA:
		::CreateTGAReader(imageReader);
		break;

	case DA::BMP:
		::CreateBMPReader(imageReader);
		offset = 14;	// the bmp file has a bitmap header
		break;

	case DA::VFX:
		::CreateVFXReader(imageReader);
		break;
	}

	if (*imageReader)
	{
		#ifdef _WIN32
        GENRESULT result = (*imageReader)->LoadImage(((char*)(pShpType->pImage)) + offset, 0, subImage);
#else
        GENRESULT result = pShpType->imageBytes >= U32(offset) ?
            (*imageReader)->LoadImage(static_cast<char *>(const_cast<void *>(pShpType->pImage)) + offset,
                pShpType->imageBytes - U32(offset), subImage) : GR_GENERIC;
#endif
		if (result != GR_OK)
		{
			(*imageReader)->Release();
			*imageReader = 0;
		}
		return result;
	}
	else
	{
		return GR_GENERIC;
	}
}
//--------------------------------------------------------------------------//
//-----------------------ShapeLoader Factory class-------------------------------//
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE ShapeLoaderFactory : public ICQFactory
{
	//----------------------------------------------------------------------------
	//
	U32 factoryHandle;		// handles to callback

	//
	// Interface mapping
	//

	BEGIN_DACOM_MAP_INBOUND(ShapeLoaderFactory)
	DACOM_INTERFACE_ENTRY(ICQFactory)
	END_DACOM_MAP()

	ShapeLoaderFactory (void) { }

	~ShapeLoaderFactory (void);

	void init (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* ICQFactory methods */

	virtual HANDLE CreateArchetype (PGENTYPE pArchetype, GENBASE_TYPE objClass, void *data);

	virtual BOOL32 DestroyArchetype (HANDLE hArchetype);

	virtual GENRESULT CreateInstance (HANDLE hArchetype, IDAComponent ** pInstance);

	/* FontFactory methods */

	IDAComponent * getBase (void)
	{
		return static_cast<ICQFactory *> (this);
	}
};
//-----------------------------------------------------------------------------------------//
//
ShapeLoaderFactory::~ShapeLoaderFactory (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (GENDATA && GENDATA->QueryOutgoingInterface("ICQFactory", connection) == GR_OK)
		connection->Unadvise(factoryHandle);
}
//-----------------------------------------------------------------------------------------//
//
void ShapeLoaderFactory::init (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (GENDATA->QueryOutgoingInterface("ICQFactory", connection) == GR_OK)
		connection->Advise(this, &factoryHandle);
}
//-----------------------------------------------------------------------------------------//
//
HANDLE ShapeLoaderFactory::CreateArchetype (PGENTYPE pArchetype, GENBASE_TYPE objClass, void *_data)
{
	if (objClass == GBT_VFXSHAPE)
	{
		GT_VFXSHAPE * data = (GT_VFXSHAPE *) _data;
		SHPTYPE * result = new SHPTYPE;

		result->pArchetype = pArchetype;
		if (result->init(data->filename) == false)
		{
			delete result;
			result = 0;
		}

		return result;
	}

	return 0;
}
//-----------------------------------------------------------------------------------------//
//
BOOL32 ShapeLoaderFactory::DestroyArchetype (HANDLE hArchetype)
{
	SHPTYPE * type = (SHPTYPE *) hArchetype;

	delete type;
	return 1;
}
//-----------------------------------------------------------------------------------------//
//
GENRESULT ShapeLoaderFactory::CreateInstance (HANDLE hArchetype, IDAComponent ** pInstance)
{
	SHPTYPE * type = (SHPTYPE *) hArchetype;
	ShapeLoader * result = new DAComponent<ShapeLoader>;

	result->pShpType = type;
	*pInstance = result;
	return GR_OK;
}
//-----------------------------------------------------------------------------------------//
//
struct _vfxshapefactory : GlobalComponent
{
	ShapeLoaderFactory * factory;

	virtual void Startup (void)
	{
		factory = new DAComponent<ShapeLoaderFactory>;
		#ifdef _WIN32
        AddToGlobalCleanupList(&factory);
#else
        AddToGlobalCleanupList(reinterpret_cast<IDAComponent **>(&factory));
#endif
	}

	virtual void Initialize (void)
	{
		factory->init();
	}
};

static _vfxshapefactory startup;
//-----------------------------------------------------------------------------------------//
//---------------------------------End ShapeLoader.cpp-------------------------------------//
//-----------------------------------------------------------------------------------------//
