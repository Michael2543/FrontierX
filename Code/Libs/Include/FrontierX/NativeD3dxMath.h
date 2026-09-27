#ifndef FRONTIERX_NATIVE_D3DX_MATH_H
#define FRONTIERX_NATIVE_D3DX_MATH_H

#if defined(_WIN32)
#error NativeD3dxMath.h is only for the non-Windows render path
#endif

#include <cmath>
#include <cstddef>

// Only the D3DX math operations used by the active App mesh owner live here.
struct D3DXVECTOR2
{
	float x, y;
};

struct D3DXVECTOR3
{
	float x, y, z;
	D3DXVECTOR3() = default;
	D3DXVECTOR3(float x_value, float y_value, float z_value) : x(x_value), y(y_value), z(z_value) {}
	D3DXVECTOR3 &operator+=(const D3DXVECTOR3 &other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
};

struct D3DXMATRIX
{
	float m[4][4];
};

static_assert(sizeof(D3DXMATRIX) == 64, "native D3DX matrix must have 16 floats");
static_assert(sizeof(D3DXVECTOR3) == 12, "native D3DX vector must have three floats");

inline D3DXMATRIX *D3DXMatrixIdentity(D3DXMATRIX *out)
{
	for (std::size_t row = 0; row < 4; ++row)
		for (std::size_t column = 0; column < 4; ++column)
			out->m[row][column] = row == column ? 1.0f : 0.0f;
	return out;
}

inline D3DXMATRIX *D3DXMatrixScaling(D3DXMATRIX *out, float x, float y, float z)
{
	D3DXMatrixIdentity(out);
	out->m[0][0] = x;
	out->m[1][1] = y;
	out->m[2][2] = z;
	return out;
}

inline D3DXMATRIX *D3DXMatrixTranslation(D3DXMATRIX *out, float x, float y, float z)
{
	D3DXMatrixIdentity(out);
	out->m[3][0] = x;
	out->m[3][1] = y;
	out->m[3][2] = z;
	return out;
}

inline D3DXMATRIX *D3DXMatrixRotationX(D3DXMATRIX *out, float angle)
{
	D3DXMatrixIdentity(out);
	const float cosine = std::cos(angle);
	const float sine = std::sin(angle);
	out->m[1][1] = cosine;
	out->m[1][2] = sine;
	out->m[2][1] = -sine;
	out->m[2][2] = cosine;
	return out;
}

inline D3DXMATRIX *D3DXMatrixRotationY(D3DXMATRIX *out, float angle)
{
	D3DXMatrixIdentity(out);
	const float cosine = std::cos(angle);
	const float sine = std::sin(angle);
	out->m[0][0] = cosine;
	out->m[0][2] = -sine;
	out->m[2][0] = sine;
	out->m[2][2] = cosine;
	return out;
}

inline D3DXMATRIX *D3DXMatrixRotationZ(D3DXMATRIX *out, float angle)
{
	D3DXMatrixIdentity(out);
	const float cosine = std::cos(angle);
	const float sine = std::sin(angle);
	out->m[0][0] = cosine;
	out->m[0][1] = sine;
	out->m[1][0] = -sine;
	out->m[1][1] = cosine;
	return out;
}

inline D3DXMATRIX *D3DXMatrixMultiply(D3DXMATRIX *out, const D3DXMATRIX *left, const D3DXMATRIX *right)
{
	D3DXMATRIX result{};
	for (std::size_t row = 0; row < 4; ++row)
		for (std::size_t column = 0; column < 4; ++column)
			for (std::size_t index = 0; index < 4; ++index)
				result.m[row][column] += left->m[row][index] * right->m[index][column];
	*out = result;
	return out;
}

inline D3DXMATRIX *D3DXMatrixTranspose(D3DXMATRIX *out, const D3DXMATRIX *source)
{
	D3DXMATRIX result{};
	for (std::size_t row = 0; row < 4; ++row)
		for (std::size_t column = 0; column < 4; ++column)
			result.m[row][column] = source->m[column][row];
	*out = result;
	return out;
}

inline float D3DXVec3LengthSq(const D3DXVECTOR3 *value)
{
	return value->x * value->x + value->y * value->y + value->z * value->z;
}

inline D3DXVECTOR3 *D3DXVec3Cross(D3DXVECTOR3 *out, const D3DXVECTOR3 *left, const D3DXVECTOR3 *right)
{
	const D3DXVECTOR3 result(
		left->y * right->z - left->z * right->y,
		left->z * right->x - left->x * right->z,
		left->x * right->y - left->y * right->x);
	*out = result;
	return out;
}

inline D3DXVECTOR3 *D3DXVec3Normalize(D3DXVECTOR3 *out, const D3DXVECTOR3 *source)
{
	const float length_squared = D3DXVec3LengthSq(source);
	if (length_squared == 0.0f)
	{
		*out = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		return out;
	}
	const float scale = 1.0f / std::sqrt(length_squared);
	const D3DXVECTOR3 result(source->x * scale, source->y * scale, source->z * scale);
	*out = result;
	return out;
}

#endif
