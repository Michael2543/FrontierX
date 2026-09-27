#ifndef FRONTIERX_NATIVE_GAME_VERTEX_BUFFER_H
#define FRONTIERX_NATIVE_GAME_VERTEX_BUFFER_H

#if defined(_WIN32)
#error NativeGameVertexBuffer.h is only for the native game-material boundary
#endif

#include "rendpipeline.h"

// The game material vtable keeps its D3D9 pointer-shaped slot. Native callers
// pass this typed object so the pipeline handle never becomes a pointer value.
struct IDirect3DVertexBuffer9
{
    IRP_VERTEXBUFFERHANDLE pipelineHandle;
};

#endif
