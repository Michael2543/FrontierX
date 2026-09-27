// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


// Insert your headers here
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#else
#include "FrontierX/LegacyFileTypes.h"
#endif
#include <stdio.h>
#include <3dmath.h>

#if !defined(_WIN32)
#include <ICamera.h>
#include <IAnim.h>
#endif

//Include ODS headers
#ifdef __cplusplus
extern "C" {
#endif 

#if defined(_WIN32)
#include <Srv.h>		// Main header file that includes all other header files
#endif

#ifdef __cplusplus
}
#endif 
