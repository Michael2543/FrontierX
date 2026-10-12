//
//
//

#ifndef PIXEL_H
#define PIXEL_H

//

#include "TYPEDEFS.H"

#if defined(_WIN32)
#include <ddraw.h>
#else
// DirectDraw's public pixel-format value is a fixed eight-DWORD record.  Keep
// the declaration available to renderer clients without importing DirectDraw.
struct DDMULTISAMPLECAPS
{
	U16 wFlipMSTypes;
	U16 wBltMSTypes;
};

struct DDPIXELFORMAT
{
	U32 dwSize;
	U32 dwFlags;
	U32 dwFourCC;
	union
	{
		U32 dwRGBBitCount;
		U32 dwYUVBitCount;
		U32 dwZBufferBitDepth;
		U32 dwAlphaBitDepth;
		U32 dwLuminanceBitCount;
		U32 dwBumpBitCount;
		U32 dwPrivateFormatBitCount;
	};
	union
	{
		U32 dwRBitMask;
		U32 dwYBitMask;
		U32 dwStencilBitDepth;
		U32 dwLuminanceBitMask;
		U32 dwBumpDuBitMask;
		U32 dwOperations;
	};
	union
	{
		U32 dwGBitMask;
		U32 dwUBitMask;
		U32 dwZBitMask;
		U32 dwBumpDvBitMask;
		DDMULTISAMPLECAPS MultiSampleCaps;
	};
	union
	{
		U32 dwBBitMask;
		U32 dwVBitMask;
		U32 dwStencilBitMask;
		U32 dwBumpLuminanceBitMask;
	};
	union
	{
		U32 dwRGBAlphaBitMask;
		U32 dwYUVAlphaBitMask;
		U32 dwLuminanceAlphaBitMask;
		U32 dwRGBZBitMask;
		U32 dwYUVZBitMask;
	};
};

static_assert(sizeof(DDPIXELFORMAT) == 32U,
	"DirectDraw pixel format layout must remain eight 32-bit words");

inline constexpr U32 DDPF_ALPHAPIXELS = 0x00000001U;
inline constexpr U32 DDPF_ALPHA = 0x00000002U;
inline constexpr U32 DDPF_FOURCC = 0x00000004U;
inline constexpr U32 DDPF_PALETTEINDEXED8 = 0x00000020U;
inline constexpr U32 DDPF_RGB = 0x00000040U;
inline constexpr U32 DDPF_LUMINANCE = 0x00020000U;
inline constexpr U32 DDPF_BUMPLUMINANCE = 0x00040000U;
inline constexpr U32 DDPF_BUMPDUDV = 0x00080000U;

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
	(static_cast<U32>(static_cast<U8>(ch0)) | \
	 (static_cast<U32>(static_cast<U8>(ch1)) << 8U) | \
	 (static_cast<U32>(static_cast<U8>(ch2)) << 16U) | \
	 (static_cast<U32>(static_cast<U8>(ch3)) << 24U))
#endif
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

//


typedef enum {
	// Pixelformats
	//
	// NOTE: These are defined to be consistent with the
	// Microsoft OpenGL enum values.
	//
	PF_COLOR_INDEX         = 0x1900,
	PF_ALPHA               = 0x1906,
	PF_RGB                 = 0x1907,
	PF_RGBA                = 0x1908,
	PF_LUMINANCE           = 0x1909,
	PF_LUMINANCE_ALPHA     = 0x190A,
	PF_ALPHA8              = 0x803C,
	PF_LUMINANCE8          = 0x8040,
	PF_LUMINANCE8_ALPHA8   = 0x8045,
	PF_RGB8                = 0x8051,
	PF_RGBA4               = 0x8056,
	PF_RGB5_A1             = 0x8057,
	PF_RGBA8               = 0x8058,
	PF_BGR_EXT             = 0x80E0,
	PF_BGRA_EXT            = 0x80E1,
	PF_U8V8L8,
	PF_U8V8,
	PF_4CC_DAOP			   = MAKEFOURCC('D','A','O','P'),
	PF_4CC_DAOT			   = MAKEFOURCC('D','A','O','T'),
	PF_4CC_DAAA			   = MAKEFOURCC('D','A','A','A'),
	PF_4CC_DAAL			   = MAKEFOURCC('D','A','A','L'),
	PF_4CC_DAA1			   = MAKEFOURCC('D','A','A','1'),
	PF_4CC_DAA4			   = MAKEFOURCC('D','A','A','4'),
	PF_4CC_DAA8			   = MAKEFOURCC('D','A','A','8')

} PFenum;


//

typedef unsigned char	byte;
typedef unsigned short	word;
typedef U32			dword;

//

struct PixelFormat
{
	DDPIXELFORMAT	ddpf;
	int				rr, rl, rwidth;
	int				gr, gl, gwidth;
	int				br, bl, bwidth;
	int				ar, al, awidth;

	void init(DDPIXELFORMAT format)
	{
		ddpf = format;
		if (!is_indexed())
		{
			U32 rmask = format.dwRBitMask;
			U32 gmask = format.dwGBitMask;
			U32 bmask = format.dwBBitMask;
			U32 amask = format.dwRGBAlphaBitMask;
			int i;
			for (i = 31; i >= 0; i--)
			{
				if (rmask & (1 << i))
				{
					rl = i;
				}
				if (gmask & (1 << i))
				{
					gl = i;
				}
				if (bmask & (1 << i))
				{
					bl = i;
				}
				if (amask & (1 << i))
				{
					al = i;
				}
			}

			for (i = 0; i <= 31; i++)
			{
				if (rmask & (1 << i))
				{
					rwidth = i - rl + 1;
				}

				if (gmask & (1 << i))
				{
					gwidth = i - gl + 1;
				}

				if (bmask & (1 << i))
				{
					bwidth = i - bl + 1;
				}

				if (amask & (1 << i))
				{
					awidth = i - al + 1;
				}
			}

#if defined(_WIN32)
			rr = 8 - rwidth;
			gr = 8 - gwidth;
			br = 8 - bwidth;
			ar = 8 - awidth;
#else
			rr = rmask ? 8 - rwidth : 0;
			gr = gmask ? 8 - gwidth : 0;
			br = bmask ? 8 - bwidth : 0;
			ar = amask ? 8 - awidth : 0;
#endif

			if( !rmask ) {
				rl = rr = rwidth = 0;
			}
			if( !gmask ) {
				gl = gr = gwidth = 0;
			}
			if( !bmask ) {
				bl = br = bwidth = 0;
			}
			if (!has_alpha_channel())
			{
				al = ar = awidth = 0;
			}
		}
	}

	inline dword compute(byte r, byte g, byte b, byte a = 0) const
	{
		dword result;
		if (is_indexed())
		{
			result = 0xffffffff;
		}
		else
		{
			result = ( ((r >> rr) << rl) |
					   ((g >> gr) << gl) |
					   ((b >> br) << bl) );
			if (awidth)
			{
				result |= ((a >> ar) << al);
			}
		}
		return result;
	}

	inline bool is_indexed(void) const
	{
		return ((ddpf.dwFlags & DDPF_PALETTEINDEXED8) != 0);
	}

	inline bool has_alpha_channel(void) const
	{
		return ((ddpf.dwFlags & DDPF_ALPHAPIXELS) != 0);
	}

	inline dword get_r_mask(void) const
	{
		return ddpf.dwRBitMask;
	}

	inline dword get_g_mask(void) const
	{
		return ddpf.dwGBitMask;
	}

	inline dword get_b_mask(void) const
	{
		return ddpf.dwBBitMask;
	}

	inline dword get_a_mask(void) const
	{
		return ddpf.dwRGBAlphaBitMask;
	}

	inline dword num_r_bits(void) const
	{
		return rwidth;
	}

	inline dword num_g_bits(void) const
	{
		return gwidth;
	}

	inline dword num_b_bits(void) const
	{
		return bwidth;
	}

	inline dword num_a_bits(void) const
	{
		return awidth;
	}

	// determine if the given pixelformat is the same as this one.
	inline dword is_equal( const PixelFormat &pf )
	{
		if( pf.is_indexed() ) {
			return is_indexed();
		}
		else if( !is_indexed() ) {
			
			if( (num_r_bits() == pf.num_r_bits()) &&
				(get_r_mask() == pf.get_r_mask()) &&
				(num_g_bits() == pf.num_g_bits()) &&
				(get_g_mask() == pf.get_g_mask()) &&
				(num_b_bits() == pf.num_b_bits()) &&
				(get_b_mask() == pf.get_b_mask()) &&
				(num_a_bits() == pf.num_a_bits()) && 
				(get_a_mask() == pf.get_a_mask()) &&
				ddpf.dwBumpDuBitMask == pf.ddpf.dwBumpDuBitMask &&
				ddpf.dwBumpDvBitMask == pf.ddpf.dwBumpDvBitMask &&
				ddpf.dwBumpLuminanceBitMask == pf.ddpf.dwBumpLuminanceBitMask) {
				return 1;

			}
		}

		return 0;
	}

	// determine if this pixelformat is compatible with
	// the given one.  This pixelformat is 'compatible' 
	// iff this pixel format can describe a surface that
	// can contain pixeldata in the given format without
	// loss of data.
	// i.e. An RGB pixelformat is compatible with an indexed 
	// format because an RGB surface can hold indexed data
	// without loss of data.  However, an indexed format
	// is not compatible with an RGB format because an
	// indexed surface cannot hold RGB data without quantization
	//
	inline dword is_compatible( const PixelFormat &pf )
	{
		if( is_indexed() && pf.is_indexed() ) {		
			// both indexed
			return 1;
		}
		else if( (!is_indexed() && pf.is_indexed()) ) { //&& !has_alpha_channel()) ) {
			// this is rgb, that is indexed
			return 1;
		}
		else {
#if 0
			// both are rgb(a), this must have at least as many bits as that.
			// if that has alpha, this must have it too and at least as many
			// bits.
			yup =	(num_r_bits() >= pf.num_r_bits()) &&
					(num_g_bits() >= pf.num_g_bits()) &&
					(num_b_bits() >= pf.num_b_bits()) ;
			
			if( yup && pf.has_alpha_channel() ) {
				return yup && (num_a_bits() >= pf.num_a_bits());
			}
#endif
			return (has_alpha_channel() == pf.has_alpha_channel());
		}

		return 0;
	}

	PixelFormat()
	{
		memset( &ddpf, 0, sizeof(ddpf) );
		ddpf.dwSize = sizeof(ddpf);
		rr = rl = rwidth = 0;
		gr = gl = gwidth = 0;
		br = bl = bwidth = 0;
		ar = al = awidth = 0;
	}

	inline dword extract(char *src, byte r, byte g, byte b, byte a = 0) const
	{
		(void)src;

		dword result;
		if (is_indexed())
		{
			result = 0xffffffff;
		}
		else
		{
			result = (((r >> rr) << rl) |
					  ((g >> gr) << gl) |
					  ((b >> br) << bl));
			if (awidth)
			{
				result |= ((a >> ar) << al);
			}
		}
		return result;
	}

	void init( U32 bpp, U32 rbits, U32 gbits, U32 bbits, U32 abits )
	{
		DDPIXELFORMAT _ddpf;
		
		memset( &_ddpf,0,sizeof(_ddpf) );
		_ddpf.dwSize = sizeof(_ddpf);

		if( bpp == 8 ) {
			U32 total = rbits + gbits + bbits + abits;
			if(  total == 0 || total == 24 || total == 32 ) {
				_ddpf.dwFlags = DDPF_PALETTEINDEXED8;
				_ddpf.dwRGBBitCount = 8;
			}
			else {
				_ddpf.dwFlags = DDPF_ALPHA;
				_ddpf.dwRGBBitCount = bpp;
				_ddpf.dwRBitMask = ((1<<rbits)-1);
				_ddpf.dwGBitMask = 0;
				_ddpf.dwBBitMask = 0;

				if( abits ) {
					_ddpf.dwFlags &= ~(DDPF_ALPHA);
					_ddpf.dwFlags |= DDPF_LUMINANCE | DDPF_ALPHAPIXELS;
					_ddpf.dwRGBAlphaBitMask = ((1<<abits)-1)<<(rbits);
				}
			}
		}
		else {
			_ddpf.dwFlags = DDPF_RGB;
			_ddpf.dwRGBBitCount = bpp;
			_ddpf.dwRBitMask = ((1<<rbits)-1)<<(gbits+bbits);
			_ddpf.dwGBitMask = ((1<<gbits)-1)<<(bbits);
			_ddpf.dwBBitMask = ((1<<bbits)-1);

			if( abits ) {
				_ddpf.dwFlags |= DDPF_ALPHAPIXELS;
				_ddpf.dwRGBAlphaBitMask = ((1<<abits)-1)<<(rbits+gbits+bbits);
			}
		}

		init( _ddpf );
	}

	PixelFormat( DDPIXELFORMAT _ddpf )
	{
		init(_ddpf);
	}

	PixelFormat( U32 bpp, U32 rbits, U32 gbits, U32 bbits, U32 abits )
	{
		init(bpp,rbits,gbits,bbits,abits);
	}

	inline dword num_bits(void) const
	{
		return ddpf.dwRGBBitCount;
	}

	inline void init( PixelFormat &pf )
	{
		memcpy( &ddpf, &pf.ddpf, sizeof(ddpf) );
		
		rr  = pf.rr ;
		rl  = pf.rl ;
		rwidth = pf.rwidth;

		gr  = pf.gr ;
		gl  = pf.gl ;
		gwidth = pf.gwidth;
		
		br  = pf.br ;
		bl  = pf.bl ;
		bwidth = pf.bwidth;
		
		ar  = pf.ar ;
		al  = pf.al ;
		awidth = pf.awidth;
	}

	inline void init( PFenum pd )
	{
		DDPIXELFORMAT ddpf;

		memset( &ddpf, 0, sizeof(ddpf) );
		ddpf.dwSize = sizeof(ddpf);

		switch( pd ) {
		
		case PF_U8V8L8:

            ddpf.dwFlags = DDPF_BUMPDUDV | DDPF_BUMPLUMINANCE;
            ddpf.dwBumpBitCount         = 24;
            ddpf.dwBumpDuBitMask        = 0x000000ff;
            ddpf.dwBumpDvBitMask        = 0x0000ff00;
            ddpf.dwBumpLuminanceBitMask = 0x00ff0000;
			break;
		case PF_U8V8:
            ddpf.dwFlags = DDPF_BUMPDUDV;
            ddpf.dwBumpBitCount         = 16;
            ddpf.dwBumpDuBitMask        = 0x000000ff;
            ddpf.dwBumpDvBitMask        = 0x0000ff00;
            ddpf.dwBumpLuminanceBitMask = 0x00000000;
			break;

		case PF_ALPHA:             
		case PF_ALPHA8:             
			ddpf.dwFlags				= DDPF_ALPHA;
			ddpf.dwAlphaBitDepth		= 8;
			ddpf.dwRGBAlphaBitMask		= 0x000000FF;
			break;

		case PF_LUMINANCE:          
		case PF_LUMINANCE8:         
			ddpf.dwFlags				= DDPF_LUMINANCE;
			ddpf.dwLuminanceBitCount	= 8;
			ddpf.dwLuminanceBitMask		= 0x000000FF;
			break;

		case PF_LUMINANCE_ALPHA:    
		case PF_LUMINANCE8_ALPHA8:  
			ddpf.dwFlags				= DDPF_LUMINANCE | DDPF_ALPHAPIXELS;
			ddpf.dwLuminanceBitCount	= 16;
			ddpf.dwLuminanceBitMask		= 0x000000FF;
			ddpf.dwRGBAlphaBitMask		= 0x0000FF00;
			break;

		case PF_COLOR_INDEX:
			ddpf.dwFlags				= DDPF_PALETTEINDEXED8;
			ddpf.dwRGBBitCount			= 8;
			break;

		case PF_RGB:
		case PF_RGB8:
			ddpf.dwFlags				= DDPF_RGB;
			ddpf.dwRGBBitCount			= 24;
			ddpf.dwRBitMask				= 0x000000FF;
			ddpf.dwGBitMask				= 0x0000FF00;
			ddpf.dwBBitMask				= 0x00FF0000;
			ddpf.dwRGBAlphaBitMask		= 0x00000000;
			break;

		case PF_BGR_EXT:
			ddpf.dwFlags				= DDPF_RGB;
			ddpf.dwRGBBitCount			= 24;
			ddpf.dwRBitMask				= 0x00FF0000;
			ddpf.dwGBitMask				= 0x0000FF00;
			ddpf.dwBBitMask				= 0x000000FF;
			ddpf.dwRGBAlphaBitMask		= 0x00000000;
			break;

		case PF_RGBA:
		case PF_RGBA8:
			ddpf.dwFlags				= DDPF_RGB|DDPF_ALPHAPIXELS;
			ddpf.dwRGBBitCount			= 32;
			ddpf.dwRBitMask				= 0x000000FF;
			ddpf.dwGBitMask				= 0x0000FF00;
			ddpf.dwBBitMask				= 0x00FF0000;
			ddpf.dwRGBAlphaBitMask		= 0xFF000000;
			break;

		case PF_BGRA_EXT:
			ddpf.dwFlags				= DDPF_RGB|DDPF_ALPHAPIXELS;
			ddpf.dwRGBBitCount			= 24;
			ddpf.dwRGBAlphaBitMask		= 0xFF000000;
			ddpf.dwRBitMask				= 0x00FF0000;
			ddpf.dwGBitMask				= 0x0000FF00;
			ddpf.dwBBitMask				= 0x000000FF;
			break;

		
		case PF_RGB5_A1:
			ddpf.dwFlags				= DDPF_RGB|DDPF_ALPHAPIXELS;
			ddpf.dwRGBBitCount			= 16;
			ddpf.dwRBitMask				= 0x00007C00;
			ddpf.dwGBitMask				= 0x000003E0;
			ddpf.dwBBitMask				= 0x0000001F;
			ddpf.dwRGBAlphaBitMask		= 0x00008000;
			break;
		
		case PF_RGBA4:
			ddpf.dwFlags				= DDPF_RGB|DDPF_ALPHAPIXELS;
			ddpf.dwRGBBitCount			= 16;
			ddpf.dwRBitMask				= 0x00000F00;
			ddpf.dwGBitMask				= 0x000000F0;
			ddpf.dwBBitMask				= 0x0000000F;
			ddpf.dwRGBAlphaBitMask		= 0x0000F000;
			break;

		case PF_4CC_DAOP:	
		case PF_4CC_DAOT:
		case PF_4CC_DAAA:
		case PF_4CC_DAAL:
		case PF_4CC_DAA1:
		case PF_4CC_DAA4:
		case PF_4CC_DAA8:	
			init( (U32)pd, 0 ); 
			return;

		}

		init( ddpf );
	}

	//

	inline PixelFormat( PFenum pd )
	{
		init( pd );
	}

	//

	inline void persist( char *out )
	{
		if( is_indexed() ) {
			strcpy( out, "Format_PAL8_3__8_8_8" );
		}
		else {
			char sz[25+1];
			int num_comps=0;

			if( get_r_mask() != 0 ) {
				num_comps++;
			}
			if( get_g_mask() != 0 ) {
				num_comps++;
			}
			if( get_b_mask() != 0 ) {
				num_comps++;
			}
			if( get_a_mask() != 0 ) {
				num_comps++;
			}
			
			#if defined(_WIN32)
			wsprintf( out, "Format_TRUE_%d_", num_comps );
			#else
			std::sprintf( out, "Format_TRUE_%d_", num_comps );
			#endif
		
			if( get_r_mask() != 0 ) {
				#if defined(_WIN32)
				wsprintf( sz, "_%d", num_r_bits() );
				#else
				std::sprintf( sz, "_%u", static_cast<unsigned int>(num_r_bits()) );
				#endif
				strcat( out, sz );
			}
			if( get_g_mask() != 0 ) {
				#if defined(_WIN32)
				wsprintf( sz, "_%d", num_g_bits() );
				#else
				std::sprintf( sz, "_%u", static_cast<unsigned int>(num_g_bits()) );
				#endif
				strcat( out, sz );
			}
			if( get_b_mask() != 0 ) {
				#if defined(_WIN32)
				wsprintf( sz, "_%d", num_b_bits() );
				#else
				std::sprintf( sz, "_%u", static_cast<unsigned int>(num_b_bits()) );
				#endif
				strcat( out, sz );
			}
			if( get_a_mask() != 0 ) {
				#if defined(_WIN32)
				wsprintf( sz, "_%d", num_a_bits() );
				#else
				std::sprintf( sz, "_%u", static_cast<unsigned int>(num_a_bits()) );
				#endif
				strcat( out, sz );
			}
		}
	}

	//

	inline void unpersist( char *in_src )
	{
		
		if( strncmp( in_src, "Format_PAL8", strlen("Format_PAL8") ) == 0 ) {
			init( 8, 8, 8, 8, 0 );
		}
		else {
			char in[255+1];
			strcpy( in, in_src );
			char *p = &in[12];	// Eat up "Format_TRUE_"
			char *t;

			t = p;
			while( *p && *p != '_' ) p++;
			if( *p ) {
				*p = 0;
				p++;
			}

			int num_comps = atoi( t );
			int sizes[4] = {0,0,0,0};

			p++; // eat extra leading '_'
			for( int n=0; n<num_comps; n++ ) {
				t = p;
				while( *p && *p != '_' ) p++;
				if( *p ) {
					*p = 0;
					p++;
				}
				else if( n < num_comps-1 ) {
					break;	// break out of the for loop
				}
				
				sizes[n] = atoi( t );
			}

			if( num_comps == 2 ) {
				sizes[3] = sizes[1];
				sizes[1] = 0;
			}

			U32 bpp = sizes[0]+sizes[1]+sizes[2]+sizes[3];

			switch( bpp ) {
			case 24:	init( PF_RGB );											break;
			case 32:	init( PF_RGBA );										break;
			default:	init( bpp, sizes[0], sizes[1], sizes[2], sizes[3] );	break;
			}
		}
		
	}


	//

	bool is_fourcc()
	{
		if( ddpf.dwFlags & DDPF_FOURCC ) {
			return true;
		}
		
		return false;
	}

	//

	U32 get_fourcc()
	{
		if( ddpf.dwFlags & DDPF_FOURCC ) {
			return ddpf.dwFourCC;
		}
		
		return 0;
	}

	//

	void init( U32 fourcc, U32 bug_catcher )	// not bug_catcher is just here to catch auto/programmer
												// casts that might otherwise happen if we just did init( U32 )
												// which might conflict with init( PFenum )
	{
		(void)bug_catcher;
		memset( &ddpf, 0, sizeof(ddpf) );
		ddpf.dwFlags = DDPF_FOURCC;
		ddpf.dwFourCC = fourcc;
	}

	//

	PixelFormat( U32 fourcc, U32 bug_catcher )
	{
		init( fourcc, bug_catcher );
	}
};

//

#endif
