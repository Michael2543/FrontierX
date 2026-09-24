#ifndef TMANAGER_H
#define TMANAGER_H
//--------------------------------------------------------------------------//
//                                                                          //
//                               TManager.h                                 //
//                                                                          //
//               COPYRIGHT (C) 2000 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*
   $Header: /Conquest/App/Src/TManager.h 4     4/28/00 1:41a Jasony $
*/
//--------------------------------------------------------------------------//

#ifndef DACOM_H
#include <DACOM.H>
#endif

#include "pixel.h"

#define IID_ITManager MAKE_IID("ITManager",1)

namespace DA
{
	enum FILETYPE
	{
		UNKTYPE,
		BMP=1,
		TGA,
		VFX
	};

}

//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE ITManager : public IDAComponent
{
	struct InitInfo
	{
		struct IRenderPipeline * PIPE;
	};

	virtual U32 COMAPI CreateTextureFromMemory (void *pMemory, DA::FILETYPE type, const PixelFormat &format, const char *name) = 0;

	virtual U32 COMAPI CreateTextureFromFile (const char *fileName, IComponentFactory *parentFile, DA::FILETYPE type, const PixelFormat &format) = 0;

	virtual U32 COMAPI AddTextureRef (U32 textureID) = 0;		// returns new ref count

	virtual U32 COMAPI ReleaseTextureRef (U32 textureID) = 0;	// returns new ref count

	virtual void COMAPI Flush (void) = 0;

	virtual U32 COMAPI GetFirstTexture() = 0;

	virtual U32 COMAPI GetNextTexture(U32 textureID) = 0;

	virtual U32 COMAPI GetPrevTexture(U32 textureID) = 0;

	virtual void Initialize(InitInfo & info) = 0;

	//
	// methods for optimizing drawAgent usage
	//
	
	// use a texture from special drawAgent cache, or create a new texture
	virtual U32 COMAPI CreateDrawAgentTexture (U32 resolution, bool bAlpha) = 0;

	virtual void COMAPI ReleaseDrawAgentTexture (U32 textureID) = 0;
};


//---------------------------------------------------------------------------//
//-----------------------------End TManager.h--------------------------------//
//---------------------------------------------------------------------------//
#endif
