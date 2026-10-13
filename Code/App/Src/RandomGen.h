#ifndef RANDOMGEN_H
#define RANDOMGEN_H

//--------------------------------------------------------------------------//
//                                                                          //
//                              RandomGen.h                                //
//                                                                          //
//                  COPYRIGHT (C) 1999 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Header: /Conquest/App/Src/RandomGen.h 2     4/28/00 11:26p Tmauer $
*/			    
//------------------------------- #INCLUDES --------------------------------//
//--------------------------------------------------------------------------//
class RandomGen
{
	U32 lastRandomNumber;

public:

	RandomGen(U16 seed);

	void srand(U16 seed);

#if defined(_WIN32)
	U32 rand(U32 max = RAND_MAX);
#else
	// Original Win32 caller domain; the libc RAND_MAX is unrelated to this owner.
	U32 rand(U32 max = 32767);
#endif
};

//------------------------------------------------------------------------------//
//-------------------------------End AntiMatter.h-------------------------------//
//------------------------------------------------------------------------------//

#endif
