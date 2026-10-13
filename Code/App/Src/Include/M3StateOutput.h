#ifndef FRONTIERX_M3_STATE_OUTPUT_H
#define FRONTIERX_M3_STATE_OUTPUT_H

// A borrowed synchronous output callback; no FILE/allocator crosses a DLL CRT.
typedef void (__cdecl *M3StateOutput)(const char *bytes, U32 length, void *context);

void WriteOperationComparisonState(M3StateOutput output, void *context);

#endif
