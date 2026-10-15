//--------------------------------------------------------------------------//
//                                                                          //
//                               Aniamte.cpp                                 //
//                                                                          //
//                  COPYRIGHT (C) 1998 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Header: /Conquest/App/Src/Animate.cpp 37    4/25/01 11:34a Tmauer $
*/			    
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "NativeAppGlobals.h"
#include "NativeAppUiInput.h"
#include "NativeAppTextResources.h"
#include "NativeAppImage.h"
#include "NativeAppMissionMedia.h"
#include "MyVertex.h"
#include <IRenderPrimitive.h>
#include <VFX.H>
#include <algorithm>
#include <cstring>
#include <cwchar>
#include <cstdlib>
#include <cstdio>
#include <vector>
using namespace FrontierX::NativeAppUi;
#endif

#include <DAnimate.h>

#include "IAnimate.h"
#include "Startup.h"
#include "BaseHotRect.h"
#include "GenData.h"
#include "DrawAgent.h"
#include "IShapeLoader.h"
#if !defined(_WIN32)
#include "IImageReader.h"
#endif
#include "TManager.h"

#include <TComponent.h>
#include <Filesys.h>
#include <HKEvent.h>

#define TIMER_FOR_30FPS		33

#if !defined(_WIN32)
static void BuildNativeAnimationBorder(IImageReader *source, U32 portraitWidth, U32 portraitHeight,
                                       std::vector<U8> &image)
{
	const U32 width = source->GetWidth(), height = source->GetHeight();
	// The original border surrounds a 64-by-80 portrait at the source's
	// (-5,-8) draw offset. Briefing fuzz cells are larger than that aperture.
	if (width != 80 || height != 89 || !portraitWidth || !portraitHeight ||
		portraitWidth > 2032 || portraitHeight > 2039) std::abort();
	const U32 targetWidth = portraitWidth + 16, targetHeight = portraitHeight + 9;
	std::vector<U8> pixels(width * height);
	U8 palette[256 * 4];
	if (source->GetImage(PF_COLOR_INDEX, pixels.data()) != GR_OK ||
		source->GetColorTable(PF_RGBA, palette) != GR_OK) std::abort();
	image.assign(18 + sizeof(palette) + targetWidth * targetHeight, 0);
	image[1] = 1;
	image[2] = 1; // Preserve indexed source pixels and the complete alpha palette.
	image[6] = 1; // 256 entries, little endian.
	image[7] = 32;
	image[12] = targetWidth & 255;
	image[13] = targetWidth >> 8;
	image[14] = targetHeight & 255;
	image[15] = targetHeight >> 8;
	image[16] = 8;
	image[17] = 0x20;
	for (U32 i = 0; i < 256; ++i)
	{
		// Match the original TGA reader's alpha-first 32-bit color-map input.
		U8 *color = image.data() + 18 + i * 4;
		color[0] = palette[i * 4 + 3]; color[1] = palette[i * 4 + 2];
		color[2] = palette[i * 4 + 1]; color[3] = palette[i * 4];
	}
	const auto coordinate = [](U32 value, U32 target, U32 sourceSize, U32 leading, U32 trailing) {
		if (value < leading) return value;
		if (value >= target - trailing) return sourceSize - (target - value);
		return leading + (value - leading) * (sourceSize - leading - trailing) /
			(target - leading - trailing);
	};
	for (U32 y = 0; y < targetHeight; ++y)
		for (U32 x = 0; x < targetWidth; ++x)
		{
			const U32 sx = coordinate(x, targetWidth, width, 5, 11);
			const U32 sy = coordinate(y, targetHeight, height, 8, 1);
			image[18 + sizeof(palette) + y * targetWidth + x] = pixels[sy * width + sx];
		}
}

static void CreateNativeAnimationBorder(IShapeLoader *loader, U32 portraitWidth, U32 portraitHeight,
                                       IDrawAgent **agent)
{
	if (portraitWidth == 64 && portraitHeight == 80)
	{
		if (loader->CreateDrawAgent(0, agent) != GR_OK || !*agent) std::abort();
		return;
	}
	COMPTR<IImageReader> source, resized;
	if (loader->CreateImageReader(0, source) != GR_OK || !source) std::abort();
	std::vector<U8> image;
	BuildNativeAnimationBorder(source, portraitWidth, portraitHeight, image);
	CreateTGAReader(resized);
	if (!resized || resized->LoadImage(image.data(), image.size(), 0) != GR_OK) std::abort();
	CreateDrawAgent(resized, agent);
	if (!*agent) std::abort();
}
#endif

//--------------------------------------------------------------------------//
//
struct ANIMATETYPE
{
	PGENTYPE pArchetype;
	COMPTR<IDrawAgent> shapes[GTASHP_MAX_SHAPES];
	U16 width;
	U16 height;
	U16 nCells;
	bool bRepeatAnimation;
#if !defined(_WIN32)
	U32 sourceWidth, sourceHeight;
#endif

	ANIMATETYPE (void)
	{
	}

	~ANIMATETYPE (void)
	{
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
struct DACOM_NO_VTABLE Animate : BaseHotRect, IAnimate
{
	//
	// incoming interface map
	//
	BEGIN_DACOM_MAP_INBOUND(Animate)
	DACOM_INTERFACE_ENTRY(IResourceClient)
	DACOM_INTERFACE_ENTRY(IAnimate)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	DACOM_INTERFACE_ENTRY(IDAConnectionPointContainer)
	DACOM_INTERFACE_ENTRY2(IID_IDAConnectionPointContainer, IDAConnectionPointContainer)
	END_DACOM_MAP()

	//
	// data items
	// 
	ANIMATETYPE * pAnimateType;

	U32 nCells;
	U32 ticksToNextCell;
	U32 timer, fullTimer;
	U32 currentCell;

	const U32 * pIndexArray;
	U32   nElements;
	U32   currentIndex;
	U32   textureID;
	COMPTR<IDrawAgent> frameAgent;
	U32	  fuzzIndex;

	bool bPaused;
	bool bLooping;
	bool bSeenAllCells;
#if !defined(_WIN32)
	std::uint64_t nativeFrameChanges = 0;
	std::uint64_t nativeDraws = 0;
#endif

	//
	// class methods
	//

	Animate (void) : BaseHotRect ( (IDAComponent *)NULL )
	{
		bLooping = true;
	}

	virtual ~Animate (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* IAnimate methods  */

	virtual void InitAnimate (const ANIMATE_DATA & data, BaseHotRect * parent, const U32 * indexArray, U32 nElements); 

	virtual void SetVisible (bool bVisible);

	virtual void PauseAnim (bool bPause);

	virtual void DeferredDestruction (void);

	virtual void SetAnimationData (const U32 * indexArray, U32 numElements);

	virtual void SetLoopingAnimation (bool _bLooping)
	{
		bLooping = _bLooping;
	}

	virtual bool HasAnimationCompleted (void)
	{
		return bSeenAllCells;
	}

	virtual void SetAnimationFrameRate (U32 timePerCell)
	{
		ticksToNextCell = timePerCell;
	}

	/* Static methods */

	virtual GENRESULT __stdcall	Notify (U32 message, void *param);

	void init (ANIMATETYPE * _pStaticType);

	void draw (void);

	void update (U32 dt);

	IDAComponent * getBase (void)
	{
		return static_cast<IEventCallback *> (this);
	}
};
//--------------------------------------------------------------------------//
//
Animate::~Animate (void)
{
	if (textureID)
	{
		if (TMANAGER)
		{
			TMANAGER->ReleaseTextureRef(textureID);
		}
	}
	frameAgent.free();
	if (GENDATA)
	{
		GENDATA->Release(pAnimateType->pArchetype);
	}
}
//--------------------------------------------------------------------------//
//
void Animate::InitAnimate (const ANIMATE_DATA & data, BaseHotRect * _parent, const U32 * indexArray, U32 _nElements)
{
	if ((parent == NULL) && (_parent != NULL))
	{
		parent = _parent;
		lateInitialize(_parent->GetBase());

		if (parent == FULLSCREEN)
		{
			parent->SetCallbackPriority(this, EVENT_PRIORITY_TALKINGHEAD);
		}
		else
		{
			parent->SetCallbackPriority(this, 0x40000000);
		}
	}

	if (parent)
	{
		screenRect.left = IDEAL2REALX(data.xOrigin) + parent->screenRect.left;
		screenRect.top = IDEAL2REALY(data.yOrigin) + parent->screenRect.top;
		screenRect.bottom = screenRect.top + pAnimateType->height - 1;
		screenRect.right = screenRect.left + pAnimateType->width - 1;
	}
	else
	{
		screenRect.left = IDEAL2REALX(data.xOrigin);
		screenRect.top = IDEAL2REALY(data.yOrigin);
		screenRect.bottom = screenRect.top + pAnimateType->height - 1;
		screenRect.right = screenRect.left + pAnimateType->width - 1;
	}

	ticksToNextCell = data.dwTimer/nCells;

	pIndexArray = indexArray;
	if (pIndexArray && _nElements)
	{
		nElements = _nElements;
		currentIndex = 0;
		currentCell = pIndexArray[currentIndex];
	}

	// create a shape loader and use it to create draw agents until it fails
	COMPTR<IDAComponent> pBase;
	COMPTR<IShapeLoader> loader;


	// make the bullshit texture
	if (data.bFuzzEffect && CQFLAGS.b3DEnabled) 
	{
		GENDATA->CreateInstance("VFXShape!!TalkingHeadBoarder", pBase);
		pBase->QueryInterface("IShapeLoader", loader);
#if defined(_WIN32)
		loader->CreateDrawAgent(0, frameAgent);
#else
		CreateNativeAnimationBorder(loader, pAnimateType->sourceWidth, pAnimateType->sourceHeight, frameAgent);
#endif
		// only do fuzz texture effect if frame locking is disabled
		if (CQFLAGS.bFrameLockEnabled == false)
		{
			textureID = TMANAGER->CreateTextureFromFile("videoFX.tga", TEXTURESDIR,DA::TGA, PF_4CC_DAA4);
#if !defined(_WIN32)
			if (!textureID)
			{
				std::fputs("Native animated portrait requires videoFX.tga DAA4 texture\n", stderr);
				std::abort();
			}
#endif
			fuzzIndex = rand()%16;
		}
	}
	else
	{
		textureID = 0;
	}
}
//--------------------------------------------------------------------------//
//
void Animate::SetVisible (bool bVisible)
{
	bInvisible = !bVisible;
}
//--------------------------------------------------------------------------//
//
void Animate::PauseAnim (bool bPause)
{
	bPaused=bPause;
}
//--------------------------------------------------------------------------//
//
void Animate::SetAnimationData (const U32 * indexArray, U32 numElements)
{
	pIndexArray = indexArray;
	if (pIndexArray && numElements)
	{
		nElements = numElements;
		currentIndex = 0;
		currentCell = pIndexArray[currentIndex];
	}
}
//--------------------------------------------------------------------------//
//
void Animate::DeferredDestruction (void)
{
#ifdef _DEBUG
	IAnimate * anim = static_cast<IAnimate *>(this);
	U32 ref;
	anim->AddRef();
	ref = anim->Release();
	CQASSERT(ref == 1);
#endif
	pIndexArray = NULL;
	if (parent)
		parent->PostMessage(CQE_DELETE_HOTRECT, static_cast<BaseHotRect *>(this));
	else
		getBase()->Release();
}
//--------------------------------------------------------------------------//
//
GENRESULT Animate::Notify (U32 message, void *param)
{
//	MSG *msg = (MSG *) param;

	if (bInvisible==false)
	switch (message)
	{
	case CQE_ENDFRAME:
		draw();
		break;

	case CQE_UPDATE:
		#if defined(_WIN32)
		update((bPaused)? 0 : (U32(param) >> 10));
#else
		update((bPaused)? 0 : (static_cast<U32>(reinterpret_cast<std::uintptr_t>(param)) >> 10));
#endif
		break;
	}

	return BaseHotRect::Notify(message, param);
}
//--------------------------------------------------------------------------//
//
void Animate::update (U32 dt)
{
#if !defined(_WIN32)
	const U32 previousCell = currentCell;
#endif
	timer += dt;
	fullTimer += dt;

	if (pIndexArray && bPaused == false)
	{
		// play back a recorded set of frames
		// assume this is at 30 frames/second
		if (timer > TIMER_FOR_30FPS)
		{
			currentIndex = fullTimer/TIMER_FOR_30FPS;

			if (currentIndex >= nElements)
			{
				if (bLooping)
				{
					currentIndex = 0;
				}
				else
				{
					currentIndex = nElements-1;
				}
			}

			// make sure the current cell is set and is valid
			currentCell = pIndexArray[currentIndex];
			if (currentCell >= nCells)
			{
				currentCell = 0;
			}
			timer = 0;
		}
	}
	else
	{
		if (bPaused == false)
		{
			// simply loop through the animation
			if (timer >= ticksToNextCell)
			{
				currentCell++;
				if (currentCell >= nCells)
				{
					bSeenAllCells = true;

					if (bLooping)
					{
						currentCell = 0;
					}
					else
					{
						currentCell = nCells-1;
					}
				}
				timer = 0;
			}
		}
		else
		{
			// make sure that the current cell is between 0 and nCells-1
			if (currentCell >= nCells || currentCell < 0)
			{
				currentCell = 0;
			}
		}
	}
#if !defined(_WIN32)
	if (currentCell != previousCell) ++nativeFrameChanges;
#endif
}
//--------------------------------------------------------------------------//
//
void Animate::draw (void)
{
	// go through the animation cells
	if (pAnimateType->shapes[currentCell])
	{
#if !defined(_WIN32)
		++nativeDraws;
#endif
		pAnimateType->shapes[currentCell]->Draw(0, screenRect.left, screenRect.top);
		
		// if we can, draw the stupid video effect
		if (textureID && CQFLAGS.bFrameLockEnabled == false && CQFLAGS.b3DEnabled)
		{
			BATCH->set_render_state(D3DRS_ZWRITEENABLE,FALSE);
			BATCH->set_render_state(D3DRS_ZENABLE,FALSE);
			BATCH->set_render_state(D3DRS_ALPHABLENDENABLE,TRUE);
			BATCH->set_render_state(D3DRS_SRCBLEND, D3DBLEND_ONE); 
			BATCH->set_render_state(D3DRS_DESTBLEND, D3DBLEND_ONE);

		  	SetupDiffuseBlend(textureID, TRUE);
			PB.Color3ub(255,255,255);

			if (!bPaused)
			{
				fuzzIndex = (fuzzIndex+1)%16;
			}

			SINGLE xf = (fuzzIndex%4) * 0.25f;
			SINGLE yf = (fuzzIndex/4) * 0.25f; 

			PB.Begin(PB_QUADS);
			PB.TexCoord2f(xf, yf);					PB.Vertex3f(screenRect.left,	screenRect.top, 0);
			PB.TexCoord2f(xf + 0.25f, yf);			PB.Vertex3f(screenRect.right,	screenRect.top, 0);
			PB.TexCoord2f(xf + 0.25f, yf + 0.25f);	PB.Vertex3f(screenRect.right,	screenRect.bottom, 0);
			PB.TexCoord2f(xf, yf + 0.25f);			PB.Vertex3f(screenRect.left,	screenRect.bottom, 0);
			PB.End();
		}

		if(frameAgent)
			frameAgent->Draw(0,screenRect.left-IDEAL2REALX(5),screenRect.top-IDEAL2REALY(8));
	}
}

#if !defined(_WIN32)
namespace FrontierX
{
bool CheckNativeAnimationBorder()
{
	// Protect the production briefing-cell regression without retail artwork:
	// the original trim stays fixed while the portrait aperture expands.
	std::vector<U8> image(18 + 1024 + 80 * 89);
	image[1] = 1; image[2] = 1; image[6] = 1; image[7] = 32;
	image[12] = 80; image[14] = 89; image[16] = 8; image[17] = 0x20;
	for (U32 i = 0; i < 256; ++i)
	{
		U8 *color = image.data() + 18 + i * 4;
		color[0] = i == 255 ? 0 : 255;
		color[1] = i; color[2] = 73; color[3] = 37;
	}
	for (U32 y = 0; y < 89; ++y)
		for (U32 x = 0; x < 80; ++x)
		{
			image[18 + 1024 + y * 80 + x] =
				x >= 5 && x < 69 && y >= 8 && y < 88 ? 255 : (x + y) % 255;
		}
	COMPTR<IImageReader> source, resized;
	CreateTGAReader(source);
	if (!source || source->LoadImage(image.data(), image.size(), 0) != GR_OK) return false;
	BuildNativeAnimationBorder(source, 136, 102, image);
	CreateTGAReader(resized);
	if (!resized || resized->LoadImage(image.data(), image.size(), 0) != GR_OK ||
		resized->GetWidth() != 152 || resized->GetHeight() != 111) return false;
	U8 oldPalette[1024], newPalette[1024];
	if (source->GetColorTable(PF_RGBA, oldPalette) != GR_OK ||
		resized->GetColorTable(PF_RGBA, newPalette) != GR_OK ||
		std::memcmp(oldPalette, newPalette, sizeof(oldPalette))) return false;
	U8 oldRgb[768], newRgb[768];
	if (source->GetColorTable(PF_RGB, oldRgb) != GR_OK ||
		resized->GetColorTable(PF_RGB, newRgb) != GR_OK ||
		std::memcmp(oldRgb, newRgb, sizeof(oldRgb))) return false;
	std::vector<U8> indices(152 * 111);
	if (resized->GetImage(PF_COLOR_INDEX, indices.data()) != GR_OK) return false;
	std::vector<U8> pixels(152 * 111 * 4);
	if (resized->GetImage(PF_RGBA, pixels.data()) != GR_OK) return false;
	const auto matches = [&pixels, &indices](U32 x, U32 y, U8 oldX, U8 oldY, U8 alpha) {
		const U8 *pixel = pixels.data() + (y * 152 + x) * 4;
		const U8 index = alpha ? (oldX + oldY) % 255 : 255;
		return indices[y * 152 + x] == index && pixel[0] == 37 &&
			pixel[1] == 73 && pixel[2] == index && pixel[3] == alpha;
	};
	if (!(matches(0, 0, 0, 0, 255) && matches(151, 0, 79, 0, 255) &&
		matches(0, 110, 0, 88, 255) && matches(151, 110, 79, 88, 255) &&
		matches(5, 8, 5, 8, 0) && matches(140, 109, 68, 87, 0) &&
		matches(141, 109, 69, 87, 255))) return false;
	// The linked TextureManager TGA reader validates the new indexed-alpha
	// path before reading any palette entries or image indices.
	if (resized->LoadImage(image.data(), 17, 0) == GR_OK ||
		resized->LoadImage(image.data(), 18 + 1023, 0) == GR_OK ||
		resized->LoadImage(image.data(), image.size() - 1, 0) == GR_OK) return false;
	std::vector<U8> bad = image;
	bad[7] = 16;
	if (resized->LoadImage(bad.data(), bad.size(), 0) == GR_OK) return false;
	bad = image; bad[5] = 1; // 257 palette entries.
	if (resized->LoadImage(bad.data(), bad.size(), 0) == GR_OK) return false;
	bad = image; bad[5] = 1; bad[6] = 0; bad[22] = 255; // Index beyond a one-entry palette.
	if (resized->LoadImage(bad.data(), bad.size(), 0) == GR_OK) return false;
	bad = image; bad[12] = 1; bad[13] = 8; // Width 2049 exceeds the reader's bound.
	if (resized->LoadImage(bad.data(), bad.size(), 0) == GR_OK) return false;
	bad = image; bad[14] = 1; bad[15] = 8;
	if (resized->LoadImage(bad.data(), bad.size(), 0) == GR_OK) return false;
	bad = image; bad[3] = 1; // Nonzero palette start remains unsupported.
	return resized->LoadImage(bad.data(), bad.size(), 0) != GR_OK;
}

bool GetNativeAnimationCounts(IAnimate *animation, std::uint64_t &frameChanges,
                             std::uint64_t &draws)
{
	auto *owner = dynamic_cast<Animate *>(animation);
	if (!owner) return false;
	frameChanges = owner->nativeFrameChanges;
	draws = owner->nativeDraws;
	return true;
}
}
#endif
//--------------------------------------------------------------------------//
//
void Animate::init (ANIMATETYPE * _pAnimateType)
{
	pAnimateType = _pAnimateType;
	nCells = pAnimateType->nCells;
}

//--------------------------------------------------------------------------//
//-----------------------Animate Factory class------------------------------//
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE AnimateFactory : public ICQFactory
{
	//----------------------------------------------------------------------------
	//
	U32 factoryHandle;		// handles to callback

	//
	// Interface mapping
	//

	BEGIN_DACOM_MAP_INBOUND(AnimateFactory)
	DACOM_INTERFACE_ENTRY(ICQFactory)
	END_DACOM_MAP()

	AnimateFactory (void) { }

	~AnimateFactory (void);

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

	/* AnimateFactory methods */

	IDAComponent * getBase (void)
	{
		return static_cast<ICQFactory *> (this);
	}
};
//-----------------------------------------------------------------------------------------//
//
AnimateFactory::~AnimateFactory (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (GENDATA && GENDATA->QueryOutgoingInterface("ICQFactory", connection) == GR_OK)
		connection->Unadvise(factoryHandle);
}
//-----------------------------------------------------------------------------------------//
//
void AnimateFactory::init (void)
{
	COMPTR<IDAConnectionPoint> connection;

	if (GENDATA->QueryOutgoingInterface("ICQFactory", connection) == GR_OK)
		connection->Advise(this, &factoryHandle);
}
//-----------------------------------------------------------------------------------------//
//
HANDLE AnimateFactory::CreateArchetype (PGENTYPE pArchetype, GENBASE_TYPE objClass, void *_data)
{
	if (objClass == GBT_ANIMATE)
	{
		GT_ANIMATE * data = (GT_ANIMATE *) _data;
		ANIMATETYPE * result = new ANIMATETYPE;

		result->pArchetype = pArchetype;

		// create a shape loader and use it to create draw agents until it fails
		COMPTR<IDAComponent> pBase;
		COMPTR<IShapeLoader> loader;

		GENDATA->CreateInstance(data->vfxType, pBase);
		pBase->QueryInterface("IShapeLoader", loader);
#if !defined(_WIN32)
		COMPTR<IImageReader> image;
		if (loader->CreateImageReader(0, image) != GR_OK || !image) std::abort();
		result->sourceWidth = image->GetWidth();
		result->sourceHeight = image->GetHeight();
#endif

		U32 i;
		U32 nCells = 0;
		
		for (i = 0; i < GTASHP_MAX_SHAPES; i++)
		{
			if (loader->CreateDrawAgent(i, result->shapes[i]) == GR_OK)
			{
				nCells++;
			}
			else
			{
				break;
			}
		}

		// get the width and height
		result->shapes[0]->GetDimensions(result->width, result->height);
		result->nCells = nCells;
		
		return result;
	}

	return 0;
}
//-----------------------------------------------------------------------------------------//
//
BOOL32 AnimateFactory::DestroyArchetype (HANDLE hArchetype)
{
	ANIMATETYPE * type = (ANIMATETYPE *) hArchetype;

	delete type;
	return 1;
}
//-----------------------------------------------------------------------------------------//
//
GENRESULT AnimateFactory::CreateInstance (HANDLE hArchetype, IDAComponent ** pInstance)
{
	ANIMATETYPE * type = (ANIMATETYPE *) hArchetype;
	Animate * result = new DAComponent<Animate>;

	result->init(type);
	*pInstance = result->getBase();
	return GR_OK;
}
//-----------------------------------------------------------------------------------------//
//
struct _AnimateFactory : GlobalComponent
{
	AnimateFactory * factory;

	virtual void Startup (void)
	{
		factory = new DAComponent<AnimateFactory>;
#if defined(_WIN32)
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

static _AnimateFactory startup;
//-----------------------------------------------------------------------------------------//
//----------------------------------End Animate.cpp----------------------------------------//
//-----------------------------------------------------------------------------------------//
