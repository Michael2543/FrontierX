// Verifies that the production ZBatcher header closure (Code/App/Src/RenderBatcher.h)
// compiles on non-Windows toolchains and that the neighboring production
// vertex-descriptor copier handles an AOS buffer. This is not behavioral
// batching. App-side translation unit dependencies remain listed in
// docs/RENDER_BATCHER_PORTABILITY.md.
// Keep FDump first: its calling convention and export macros must not depend
// on RenderBatcher having already included DACOM.H.
#include "FDump.h"
#include "TManager.h"
#include "IVertexBufferManager.h"
#include "CQBatch.h"
#include "RenderBatcher.h"

#include <type_traits>
#include <cstring>

void copy_vertex_buffer_desc(void * dst_buffer, U32 dst_vertex_format,
    VertexBufferDesc * src_vb_desc);

bool CheckBatcherTraceFormat();

int main()
{
    const PRIMITIVETYPEINFO info = primitive_type_info[0];
    (void) info.type;
    static_assert(sizeof(decltype(((ICQBatch *) 0)->GetTicks())) == 8,
        "ICQBatch ticks must remain 64-bit");
    static_assert(sizeof(decltype(VertexBufferItemDesc::size)) == 4,
        "vertex item size must remain 32-bit");
    static_assert(sizeof(decltype(VertexBufferItemDesc::stride)) == 4,
        "vertex item stride must remain 32-bit");
    static_assert(sizeof(decltype(VertexBufferItemDesc::count)) == 4,
        "vertex item count must remain 32-bit");
    static_assert(std::is_same<decltype(VertexBufferItemDesc::indices), U32 *>::value,
        "vertex item indices must match the U32 arrays allocated by TriMesh");
    static_assert(sizeof(decltype(VertexBufferDesc::vertex_format)) == 4,
        "vertex format must remain 32-bit");
    static_assert(sizeof(decltype(VertexBufferDesc::num_vertices)) == 4,
        "vertex count must remain 32-bit");
    struct TestVertex { float x, y, z; U32 color; float u, v; };
    static_assert(sizeof(TestVertex) == 24, "XYZ/diffuse/UV vertex stride");
    const TestVertex source[] = {
        {1.0f, 2.0f, 3.0f, 0xff102030U, 0.25f, 0.75f},
        {4.0f, 5.0f, 6.0f, 0xff405060U, 0.5f, 0.125f}
    };
    TestVertex destination[2] = {};
    VertexBufferDesc descriptor = {};
    descriptor.flags = VBD_F_AOS;
    descriptor.Ps.data = const_cast<TestVertex *>(source);
    descriptor.vertex_format = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
    descriptor.num_vertices = 2;
    copy_vertex_buffer_desc(destination, descriptor.vertex_format, &descriptor);
    const bool aosCopied = std::memcmp(source, destination, sizeof(source)) == 0;

    struct IndexedVertex {
        float x, y, z, nx, ny, nz;
        U32 color;
        float u, v;
    };
    static_assert(sizeof(IndexedVertex) == 36, "indexed PN/color/UV vertex stride");
    float positions[2][3] = {{1.0f, 2.0f, 3.0f}, {4.0f, 5.0f, 6.0f}};
    float normals[2][3] = {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}};
    U32 colors[2] = {0xff102030U, 0xff405060U};
    float texcoords[2][2] = {{0.25f, 0.75f}, {0.5f, 0.125f}};
    U32 positionIndices[2] = {1U, 0U};
    U32 normalIndices[2] = {0U, 1U};
    U32 colorIndices[2] = {1U, 0U};
    U32 texcoordIndices[2] = {0U, 1U};
    IndexedVertex indexedDestination[2] = {};
    const IndexedVertex indexedExpected[2] = {
        {4.0f, 5.0f, 6.0f, 0.0f, 0.0f, 1.0f, 0xff405060U, 0.25f, 0.75f},
        {1.0f, 2.0f, 3.0f, 0.0f, 1.0f, 0.0f, 0xff102030U, 0.5f, 0.125f}
    };
    VertexBufferDesc indexedDescriptor = {};
    indexedDescriptor.flags = VBD_F_INDEXED;
    indexedDescriptor.vertex_format = D3DFVF_XYZ | D3DFVF_NORMAL |
        D3DFVF_DIFFUSE | D3DFVF_TEX1;
    indexedDescriptor.num_vertices = 2;
    indexedDescriptor.Ps = {positions, sizeof(positions[0]), sizeof(positions[0]),
        positionIndices, 2};
    indexedDescriptor.Ns = {normals, sizeof(normals[0]), sizeof(normals[0]),
        normalIndices, 2};
    indexedDescriptor.C0s = {colors, sizeof(colors[0]), sizeof(colors[0]),
        colorIndices, 2};
    indexedDescriptor.MC0s = {texcoords, sizeof(texcoords[0]), sizeof(texcoords[0]),
        texcoordIndices, 2};
    copy_vertex_buffer_desc(indexedDestination, indexedDescriptor.vertex_format,
        &indexedDescriptor);
    const bool indexedCopied =
        std::memcmp(indexedDestination, indexedExpected, sizeof(indexedExpected)) == 0;
    return CheckBatcherTraceFormat() && aosCopied && indexedCopied ? 0 : 1;
}
