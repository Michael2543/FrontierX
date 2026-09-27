#include "MyVertex.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>

PrimitiveBuilder2 PB;

PrimitiveBuilder2::~PrimitiveBuilder2()
{
	SetPipeline(NULL);
	SetIRenderPrimitive(NULL);
	std::free(vertex_buffer_real);
}

void PrimitiveBuilder2::VerifyBuffer()
{
	if (current_vertex < num_vertex)
		return;

	const auto maximum = std::numeric_limits<UINT>::max();
	if (block_size == 0 || num_vertex > maximum - block_size)
		throw std::bad_alloc();
	const UINT capacity = num_vertex + block_size;
	static_assert(sizeof(std::size_t) > sizeof(UINT), "native builder requires a 64-bit address space");

	U8 *raw = static_cast<U8 *>(std::malloc(sizeof(FVFVERTEXTYPE) * capacity + 31));
	if (!raw)
		throw std::bad_alloc();
	const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 31) & ~std::uintptr_t(31);
	auto *vertices = reinterpret_cast<FVFVERTEXTYPE *>(aligned);
	if (vertex_buffer)
		std::memcpy(vertices, vertex_buffer, sizeof(FVFVERTEXTYPE) * num_vertex);
	std::free(vertex_buffer_real);
	vertex_buffer_real = raw;
	vertex_buffer = vertices;
	current_vertex_ptr = vertices + current_vertex;
	num_vertex = capacity;
}

void PrimitiveBuilder2::Begin(PBenum primitive)
{
	Reset();
	switch (primitive)
	{
	case PB_POINTS: type = D3DPT_POINTLIST; break;
	case PB_LINES: type = D3DPT_LINELIST; break;
	case PB_LINE_LOOP: type = D3DPT_LINESTRIP; insert_at_end = true; break;
	case PB_LINE_STRIP: type = D3DPT_LINESTRIP; break;
	case PB_TRIANGLES: type = D3DPT_TRIANGLELIST; break;
	case PB_TRIANGLE_STRIP: type = D3DPT_TRIANGLESTRIP; break;
	case PB_POLYGON:
	case PB_TRIANGLE_FAN: type = D3DPT_TRIANGLEFAN; break;
	case PB_QUAD_STRIP:
	case PB_QUADS: type = D3DPT_TRIANGLELIST; watch_quad = true; break;
	default: throw std::invalid_argument("PrimitiveBuilder2 primitive");
	}
}

void PrimitiveBuilder2::Begin(PBenum primitive, U32 vertex_count)
{
	Begin(primitive);
	while (vertex_count >= num_vertex)
	{
		// Preserve the Windows Begin contract of room for an appended loop vertex.
		VerifyBuffer();
		if (vertex_count < num_vertex)
			break;
		current_vertex = num_vertex;
	}
	current_vertex = 0;
	current_vertex_ptr = vertex_buffer;
}
