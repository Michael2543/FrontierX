//--------------------------------------------------------------------------//
//                                                                          //
//                                VfxRead.cpp                               //
//                                                                          //
//                  COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
	$Header: /Conquest/App/Src/VfxRead.cpp 5     3/18/00 12:31a Jasony $
*/
//------------------------------- #INCLUDES --------------------------------//

#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppImage.h"
#include <pixel.h>
#include <VFX.H>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <vector>
#endif
#include "IImageReader.h"

#include <TComponent.h>
#include <HeapObj.h>

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//

struct DACOM_NO_VTABLE VFXREADER : IImageReader
{
	BEGIN_DACOM_MAP_INBOUND(VFXREADER)
  	DACOM_INTERFACE_ENTRY(IImageReader)
  	END_DACOM_MAP()
		
  	//------------------------------
  	// data members
  	//------------------------------
  
	COLORREF  palette[256];
	U8 * indexMap;
	U32 width, height;
	
	VFXREADER (void) { };
	
	~VFXREADER (void) { free(); }
	
    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	void free (void);

	/* IImageReader methods */
	
	DEFMETHOD(LoadImage) (void *fileImage, U32 fileSize, U32 imageNumber);
	
	DEFMETHOD(GetImage) (U32 desiredFormat, void * buffer, const RECT * rect) const;
	
	DEFMETHOD(GetColorTable) (U32 desiredFormat, void * buffer) const;
	
	DEFMETHOD_(U32,GetWidth) (void) const;
	
	DEFMETHOD_(U32,GetHeight) (void) const;
	
	/* VFXREADER methods */
	
	void getRGBFormat (void *buffer) const;

	void getRGBAFormat (void *buffer) const;

	void getIndexFormat (U8 *buffer, const RECT * rect) const;

	void getRGBFormat (void *buffer, const RECT * rect) const;

	void getRGBAFormat (void *buffer, const RECT * rect) const;

	BOOL32 verifyPoint (S32 x, S32 y) const
	{
		if ((U32)y >= height ||	(U32)x >= width)
		{
			return 0;
		}
		return 1;
	}

	IDAComponent * GetBase (void)
	{
		return (IImageReader *) this;
	}
};    


//---------------------------------------------------------------------------
//-----------------------------VFXREADER METHODS ----------------------------
//---------------------------------------------------------------------------

void VFXREADER::free (void)
{
    ::free(indexMap);
	indexMap = 0;
}
//---------------------------------------------------------------------------
//
GENRESULT VFXREADER::LoadImage (void *fileImage, U32 fileSize, U32 imageNumber)
{
#if defined(_WIN32)
	GENRESULT result = GR_OK;
	VFX_CRGB colors[256];
	S32 numColors, i;
	U32 _widthHeight;
	VFX_WINDOW window;
	PANE pane;
	U8 transparent=0xFF;

	free();

	if (imageNumber >= ((VFX_SHAPETABLE *)fileImage)->shape_count)
		return GR_GENERIC;

	numColors = VFX_shape_colors((VFX_SHAPETABLE *)fileImage, imageNumber, colors);
	_widthHeight = VFX_shape_bounds((VFX_SHAPETABLE *)fileImage, imageNumber);

	width = (_widthHeight >> 16)+1;
	height = (_widthHeight & 0xFFFF)+1;

	memset(palette, 0, sizeof(palette));
	for (i = 0; i < numColors; i++)
		palette[colors[i].color] = RGB(colors[i].rgb.r<<2, colors[i].rgb.g<<2, colors[i].rgb.b<<2) | 0xFF000000;

	for (i = 255; i >= 0; i--)
		if ((palette[i] & 0xFF000000) == 0)		// if index is not used
		{
			transparent = (U8) i;
			break;
		}

	::free(indexMap);
	indexMap = (U8 *) malloc(width * height);
	memset(indexMap, transparent, width * height);

	//
	// draw the shape into the buffer
	//
	window.buffer = indexMap;
	window.x_max = width - 1;
	window.y_max = height - 1;
	window.pixel_pitch = 1;
	window.bytes_per_pixel = 1;

	pane.window = &window;
	pane.x0 = pane.y0 = 0;
	pane.x1 = width - 1;
	pane.y1 = height - 1;

	VFX_shape_draw8(&pane, (VFX_SHAPETABLE *)fileImage, imageNumber, 0, 0);
	
	return result;
#else
    free();
    width = height = 0;
    std::memset(palette, 0, sizeof(palette));
    if (!fileImage || fileSize < 8) return GR_INVALID_PARMS;
    const auto *bytes = static_cast<const U8 *>(fileImage);
    const auto word = [bytes](std::size_t offset) -> U32 {
        return U32(bytes[offset]) | (U32(bytes[offset+1]) << 8) |
            (U32(bytes[offset+2]) << 16) | (U32(bytes[offset+3]) << 24);
    };
    const auto signedWord = [&word](std::size_t offset) -> S32 {
        const U32 value = word(offset);
        return value <= 0x7fffffffU ? static_cast<S32>(value) :
            static_cast<S32>(static_cast<std::int64_t>(value) - 0x100000000LL);
    };
    // SHP tables store 32-bit relative shape/palette offsets, never native pointers.
    const U32 count = word(4);
    if (imageNumber >= count || count > (fileSize-8)/8) return GR_GENERIC;
    const std::size_t entry = 8 + std::size_t(imageNumber)*8;
    const U32 shape = word(entry), colors = word(entry+4);
    if (shape > fileSize || fileSize-shape < 24 ||
        (colors && (colors > fileSize || fileSize-colors < 4))) return GR_GENERIC;
    const U32 colorCount = colors ? word(colors) : 0;
    if (colorCount > 256 || (colors && colorCount > (fileSize-colors-4)/4)) return GR_GENERIC;
    for (U32 color = 0; color < colorCount; ++color)
    {
        const std::size_t item = colors+4+std::size_t(color)*4;
        const U8 index = bytes[item];
        palette[index] = RGB(bytes[item+1]<<2, bytes[item+2]<<2, bytes[item+3]<<2) | 0xff000000U;
    }
    const U32 bounds = word(shape);
    const U32 candidateWidth = (bounds >> 16)+1;
    const U32 candidateHeight = (bounds & 0xffff)+1;
    const S32 xmin = signedWord(shape+8), ymin = signedWord(shape+12);
    const S32 xmax = signedWord(shape+16), ymax = signedWord(shape+20);
    // Bound this original UI reader's allocation and encoded row/column traversal.
    // The original VFX drawer returns without reading packets for empty bounds.
    // Retail edit-shp uses INT_MAX/INT_MIN extrema for these transparent frames.
    const bool empty = xmax < xmin || ymax < ymin;
    if (candidateWidth > 4096 || candidateHeight > 4096 ||
        (!empty && (std::int64_t(xmax)-xmin+1 > 4096 ||
                    std::int64_t(ymax)-ymin+1 > 4096)))
        return GR_GENERIC;
    U8 transparent = 0xff;
    for (S32 index=255; index>=0; --index)
        if (!(palette[index] & 0xff000000U)) { transparent=static_cast<U8>(index); break; }
    std::vector<U8> decoded(std::size_t(candidateWidth)*candidateHeight, transparent);
    std::size_t cursor = std::size_t(shape)+24;
    for (std::int64_t y=ymin; !empty && y<=ymax; ++y)
    {
        std::int64_t x=xmin;
        for (;;)
        {
            if (cursor >= fileSize) return GR_GENERIC;
            const U8 token = bytes[cursor++];
            if (!token) break; // End of encoded row.
            if (token == 1)
            {
                if (cursor >= fileSize) return GR_GENERIC;
                x += bytes[cursor++]; // Transparent skip.
                if (x > std::int64_t(xmax)+1) return GR_GENERIC;
                continue;
            }
            const U32 pixels = token >> 1;
            const bool literal = (token & 1) != 0;
            const U32 payload = literal ? pixels : 1;
            if (payload > fileSize-cursor || x+pixels > std::int64_t(xmax)+1)
                return GR_GENERIC;
            for (U32 pixel=0; pixel<pixels; ++pixel, ++x)
                if (x>=0 && x<candidateWidth && y>=0 && y<candidateHeight)
                    decoded[std::size_t(y)*candidateWidth+std::size_t(x)] = bytes[cursor+(literal ? pixel : 0)];
            cursor += payload;
        }
    }
    indexMap = static_cast<U8 *>(std::malloc(decoded.size()));
    if (!indexMap) return GR_OUT_OF_MEMORY;
    std::memcpy(indexMap, decoded.data(), decoded.size());
    width = candidateWidth;
    height = candidateHeight;
    return GR_OK;
#endif
}
//---------------------------------------------------------------------------
//
GENRESULT VFXREADER::GetImage (U32 desiredFormat, void * buffer, const RECT * rect) const
{
	GENRESULT result = GR_OK;

	if (buffer==0)
	{
		result = GR_INVALID_PARMS;
		goto Done;
	}

	if (indexMap == 0)
	{
		result = GR_GENERIC;
		goto Done;
	}

	switch (desiredFormat)
	{
	case PF_RGB:
		if (rect)
			getRGBFormat(buffer, rect);
		else
			getRGBFormat(buffer);
		break;
	
	case PF_RGBA:
		if (rect)
			getRGBAFormat(buffer, rect);
		else
			getRGBAFormat(buffer);
		break;

	case PF_COLOR_INDEX:
		if (rect)
			getIndexFormat((U8 *)buffer, rect);
		else
			memcpy(buffer, indexMap, width * height);
		break;

	default:
		result = GR_INVALID_PARMS;
		goto Done;
	}

Done:
	return result;
}
//---------------------------------------------------------------------------
//
GENRESULT VFXREADER::GetColorTable (U32 desiredFormat, void * buffer) const
{
	GENRESULT result = GR_OK;

	if (buffer==0)
	{
		result = GR_INVALID_PARMS;
		goto Done;
	}

	if (indexMap == 0)
	{
		result = GR_GENERIC;
		goto Done;
	}

	switch (desiredFormat)
	{
	case PF_RGB:
		{
			const COLORREF * colors = palette;
			VFX_RGB * rgb = (VFX_RGB *) buffer;
			int i;

			for (i = 0; i < 256; i++, rgb++, colors++)
				rgb[0] = *((VFX_RGB *)(&colors[0]));
		}
		break;
	
	case PF_RGBA:
		memcpy(buffer, palette, sizeof(palette));
		break;

	default:
		result = GR_INVALID_PARMS;
		goto Done;
	}

Done:
	return result;
}
//---------------------------------------------------------------------------
//
U32 VFXREADER::GetWidth (void) const
{
	return width;
}
//---------------------------------------------------------------------------
//
U32 VFXREADER::GetHeight (void) const
{
	return height;
}
//-------------------------------------------------------------------------//
//
void VFXREADER::getRGBFormat (void *buffer) const
{
    const int pixels_left = width * height;  // number of pixels
	int i;
	VFX_RGB *rgb = (VFX_RGB *) buffer;
	U8 * indexes = indexMap;

	for (i = 0; i < pixels_left; i++, rgb++, indexes++)
		rgb[0] = *((VFX_RGB *)(&palette[indexes[0]]));
}
//-------------------------------------------------------------------------//
//
void VFXREADER::getRGBFormat (void *buffer, const RECT * rect) const
{
	VFX_RGB *rgb = (VFX_RGB *) buffer;
	S32 rectWidth = rect->right - rect->left;
	S32 rectHeight = rect->bottom - rect->top;
	S32 i_inc = (rectWidth>0) ? 1:-1;
	rectWidth += i_inc;
	S32 y_inc = (rectHeight>0)? (width-rectWidth):(-rectWidth-width);
	S32 j_inc = (rectHeight>0)? 1:-1;
	rectHeight += j_inc;
	S32 i,j;
	S32 x, y;

	const U8 * src = indexMap + rect->left + (rect->top * width);

	for (y = rect->top, j = 0; j != rectHeight; j += j_inc, src += y_inc, y+=j_inc)
	{
		for (x = rect->left, i = 0; i != rectWidth; i += i_inc, src += i_inc, rgb++, x+=i_inc)
			if (verifyPoint(x, y))
				rgb[0] = *((VFX_RGB *)(&palette[src[0]]));
	}
}
//-------------------------------------------------------------------------//
//
void VFXREADER::getRGBAFormat (void *buffer) const
{
    const int pixels_left = width * height;  // number of pixels
	COLORREF *rgba = (COLORREF *) buffer;
	U8 * indexes = indexMap;
	int i;

	for (i = 0; i < pixels_left; i++, rgba++, indexes++)
		rgba[0] = *((COLORREF *)(&palette[indexes[0]]));
}
//-------------------------------------------------------------------------//
//
void VFXREADER::getRGBAFormat (void *buffer, const RECT * rect) const
{
	COLORREF *rgba = (COLORREF *) buffer;
	S32 rectWidth = rect->right - rect->left;
	S32 rectHeight = rect->bottom - rect->top;
	S32 i_inc = (rectWidth>0) ? 1:-1;
	rectWidth += i_inc;
	S32 y_inc = (rectHeight>0)? (width-rectWidth):(-rectWidth-width);
	S32 j_inc = (rectHeight>0)? 1:-1;
	rectHeight += j_inc;
	S32 i,j;
	S32 x, y;

	const U8 * src = indexMap + rect->left + (rect->top * width);

	for (y = rect->top, j = 0; j != rectHeight; j += j_inc, src += y_inc, y+=j_inc)
	{
		for (x = rect->left, i = 0; i != rectWidth; i += i_inc, src += i_inc, rgba++, x+=i_inc)
			if (verifyPoint(x, y))
				rgba[0] = *((COLORREF *)(&palette[src[0]]));
	}
}
//-------------------------------------------------------------------------//
//
void VFXREADER::getIndexFormat (U8 *buffer, const RECT * rect) const
{
	S32 rectWidth = rect->right - rect->left;
	S32 rectHeight = rect->bottom - rect->top;
	S32 i_inc = (rectWidth>0) ? 1:-1;
	rectWidth += i_inc;
	S32 y_inc = (rectHeight>0)? (width-rectWidth):(-rectWidth-width);
	S32 j_inc = (rectHeight>0)? 1:-1;
	rectHeight += j_inc;
	S32 i,j;
	S32 x, y;

	const U8 * src = indexMap + rect->left + (rect->top * width);

	for (y = rect->top, j = 0; j != rectHeight; j += j_inc, src += y_inc, y+=j_inc)
	{
		for (x = rect->left, i = 0; i != rectWidth; i += i_inc, src += i_inc, x += i_inc, buffer++)
			if (verifyPoint(x, y))
				*buffer = *src;
	}
}
//-------------------------------------------------------------------------//
//-------------------------------------------------------------------------//
//-------------------------------------------------------------------------//
//
void __stdcall CreateVFXReader (struct IImageReader ** reader)
{
	*reader = new DAComponent<VFXREADER>;
}
//-------------------------------------------------------------------------//
//----------------------------END VFXREAD.CPP------------------------------//
//-------------------------------------------------------------------------//

#if !defined(_WIN32)
namespace FrontierX
{
bool CheckNativeVfxReader()
{
    // One original SHP table entry, 4x2 canvas: literal, run, skip and EOL packets.
    std::vector<U8> fixture(70, 0);
    const auto put = [&fixture](std::size_t offset, U32 value) {
        for (U32 byte=0; byte<4; ++byte) fixture[offset+byte]=static_cast<U8>(value>>(byte*8));
    };
    put(4,1); put(8,16); put(12,54); put(16,(3U<<16)|1);
    put(32,3); put(36,1); put(54,3);
    const U8 colors[12] = {1,63,0,0, 2,0,63,0, 3,0,0,63};
    std::memcpy(fixture.data()+58,colors,sizeof(colors));
    const U8 packets[14] = {9,1,2,3,1,0, 1,1,4,2,1,1,0,0};
    std::memcpy(fixture.data()+40,packets,sizeof(packets));
    IImageReader *reader = nullptr;
    CreateVFXReader(&reader);
    if (!reader) return false;
    U8 indices[8] = {};
    COLORREF rgba[8] = {};
    bool valid = reader->LoadImage(fixture.data(), fixture.size(), 0) == GR_OK &&
        reader->GetWidth()==4 && reader->GetHeight()==2 &&
        reader->GetImage(PF_COLOR_INDEX,indices)==GR_OK &&
        reader->GetImage(PF_RGBA,rgba)==GR_OK;
    const U8 expected[8] = {1,2,3,1,255,2,2,255};
    valid = valid && std::memcmp(indices,expected,sizeof(indices))==0 &&
        rgba[0]==0xff0000fcU && rgba[1]==0xff00fc00U && rgba[2]==0xfffc0000U && rgba[4]==0;
    valid = valid && reader->LoadImage(fixture.data(), fixture.size(), 1)!=GR_OK &&
        reader->GetImage(PF_COLOR_INDEX,indices)!=GR_OK;
    put(24,0x7ffffffeU); put(28,0x7ffffffeU);
    put(32,0x80000000U); put(36,0x80000000U);
    valid = valid && reader->LoadImage(fixture.data(),fixture.size(),0)==GR_OK &&
        reader->GetWidth()==4 && reader->GetHeight()==2 &&
        reader->GetImage(PF_RGBA,rgba)==GR_OK;
    for (COLORREF pixel : rgba) valid = valid && pixel==0;
    put(12,0); // Source no-palette frame: indices remain fully transparent.
    valid = valid && reader->LoadImage(fixture.data(),fixture.size(),0)==GR_OK &&
        reader->GetImage(PF_RGBA,rgba)==GR_OK;
    for (COLORREF pixel : rgba) valid = valid && pixel==0;
    put(24,0); put(28,0); put(32,3); put(36,1);
    put(12, fixture.size()-2);
    valid = valid && reader->LoadImage(fixture.data(),fixture.size(),0)!=GR_OK;
    put(12,54); fixture[40]=255; // Literal packet would overrun the row.
    valid = valid && reader->LoadImage(fixture.data(),fixture.size(),0)!=GR_OK &&
        reader->LoadImage(fixture.data(),0,0)!=GR_OK;
    reader->Release();
    return valid;
}
}
#endif
