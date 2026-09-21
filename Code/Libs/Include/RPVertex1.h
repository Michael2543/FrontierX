// RPVertex1.h
//
//
//


#ifndef __RPVertex_h__
#define __RPVertex_h__

#include "da_d3dtypes.h"
#include "vector.h"

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4201 )		// disable WARNING: nonstandard extension used : nameless struct/union
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif


//

typedef struct RPVertex1
{
	Vector			pos;
	union {
		struct {
		unsigned char	b, g, r, a;
		};
		unsigned int color;
	};
	float			u, v;
} RPVertex;


#define D3DFVF_RPVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//

#if defined(_MSC_VER)
#pragma warning( pop )
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif // EOF
