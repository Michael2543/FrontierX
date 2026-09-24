//--------------------------------------------------------------------------//
//                                                                          //
//                              MathLib..cpp                                //
//                                                                          //
//               COPYRIGHT (C) 1998 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*
  $Header: /Libs/Src/x86math/MathLib.cpp 4     5/20/98 4:16p Jasony $
*/
//--------------------------------------------------------------------------//
//
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "3dmath.h"




I3DMathEngine * COMAPI __MATH_ENGINE (void)
{
	DA3DMATHDESC math_info;
	I3DMathEngine * math_engine = NULL;

	ICOManager *manager = DACOM_Acquire();
	if (manager != NULL)
		manager->CreateInstance(&math_info, (void **) &math_engine);

	return math_engine;
}





//--------------------------------------------------------------------------//
//------------------------End MathLib.cpp-----------------------------------//
//--------------------------------------------------------------------------//
