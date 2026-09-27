//--------------------------------------------------------------------------//
//                                                                          //
//                                 StaticInit.cpp                           //
//                                                                          //
//                  COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Author: Tmauer $
*/			    
//------------------------------- #INCLUDES --------------------------------//
//--------------------------------------------------------------------------//
#if defined(_WIN32)
#include "pch.h"
#include "globals.h"
#else
#include "Include/NativeAppGlobals.h"
#include "MyVertex.h"
#include <3dmath.h>
#include <cmath>
#endif

Vector circleLines[NUM_CIRCLE_SEGS];

class Initer
{
public:
	Initer()
	{
		for(int i = 0; i < NUM_CIRCLE_SEGS; ++i)
		{
			circleLines[i] = Vector(cos((2*PI*i)/NUM_CIRCLE_SEGS),
				sin((2*PI*i)/NUM_CIRCLE_SEGS),0);
		}
	}
};

static Initer initObj;

//---------------------------------------------------------------------------
//--------------------------End StaticInit.cpp------------------------------------
//---------------------------------------------------------------------------
