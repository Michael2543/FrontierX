#ifndef FRONTIERX_M3_STATE_OUTPUT_H
#define FRONTIERX_M3_STATE_OUTPUT_H

// A borrowed synchronous output callback; no FILE/allocator crosses a DLL CRT.
typedef void (__cdecl *M3StateOutput)(const char *bytes, U32 length, void *context);

// OpAgent lives in Mission.dll in the original build, alongside MScript.
#if defined(_WIN32) && defined(BUILD_MISSION)
__declspec(dllexport)
#elif defined(_WIN32)
__declspec(dllimport)
#endif
void WriteOperationComparisonState(M3StateOutput output, void *context);

#endif
