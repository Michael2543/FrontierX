#include "FrontierX/MathCore.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace frontierx {
namespace {

constexpr float minimum_determinant = 1.0e-8F;
constexpr float quaternion_epsilon = 0.00001F;
constexpr float half_pi = 1.57079632679489661923F;

Vec3 cross(const Vec3 &left, const Vec3 &right)
{
	return {
		left.y * right.z - left.z * right.y,
		left.z * right.x - left.x * right.z,
		left.x * right.y - left.y * right.x,
	};
}

float dot(const Vec3 &left, const Vec3 &right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

Matrix3 transpose(const Matrix3 &matrix)
{
	return {{{matrix.d[0][0], matrix.d[1][0], matrix.d[2][0]},
	         {matrix.d[0][1], matrix.d[1][1], matrix.d[2][1]},
	         {matrix.d[0][2], matrix.d[1][2], matrix.d[2][2]}}};
}

std::uint32_t float_bits(float value)
{
	std::uint32_t bits = 0;
	static_assert(sizeof(bits) == sizeof(value), "float must be 32 bits");
	std::memcpy(&bits, &value, sizeof(bits));
	return bits;
}

float bits_float(std::uint32_t bits)
{
	float value = 0.0F;
	std::memcpy(&value, &bits, sizeof(value));
	return value;
}

const std::array<std::uint8_t, 1024> &inverse_sqrt_table()
{
	static const std::array<std::uint8_t, 1024> table = [] {
		std::array<std::uint8_t, 1024> result{};
		for (std::uint32_t index = 0; index < result.size(); ++index) {
			const std::uint32_t input_bits = (126U << 23U) | (index << 14U);
			const float reciprocal = 1.0F / std::sqrt(bits_float(input_bits));
			result[index] = static_cast<std::uint8_t>(((float_bits(reciprocal) + (1U << 13U)) >> 15U) & 0xffU);
		}
		result[512] = 0xffU;
		return result;
	}();
	return table;
}

} // namespace

float determinant(const Matrix3 &m)
{
	return m.d[0][0] * m.d[1][1] * m.d[2][2] +
	       m.d[0][1] * m.d[1][2] * m.d[2][0] +
	       m.d[0][2] * m.d[1][0] * m.d[2][1] -
	       m.d[0][0] * m.d[1][2] * m.d[2][1] -
	       m.d[0][1] * m.d[1][0] * m.d[2][2] -
	       m.d[0][2] * m.d[1][1] * m.d[2][0];
}

MathResult inverse(Matrix3 &dst, const Matrix3 &m)
{
	const Matrix3 source = m;
	const float det = determinant(source);
	const bool invertible = std::fabs(det) > minimum_determinant;
	const float factor = invertible ? 1.0F / det : 1.0F;
	dst.d[0][0] = (source.d[1][1] * source.d[2][2] - source.d[1][2] * source.d[2][1]) * factor;
	dst.d[1][0] = -(source.d[1][0] * source.d[2][2] - source.d[1][2] * source.d[2][0]) * factor;
	dst.d[2][0] = (source.d[1][0] * source.d[2][1] - source.d[1][1] * source.d[2][0]) * factor;
	dst.d[0][1] = -(source.d[0][1] * source.d[2][2] - source.d[0][2] * source.d[2][1]) * factor;
	dst.d[1][1] = (source.d[0][0] * source.d[2][2] - source.d[0][2] * source.d[2][0]) * factor;
	dst.d[2][1] = -(source.d[0][0] * source.d[2][1] - source.d[0][1] * source.d[2][0]) * factor;
	dst.d[0][2] = (source.d[0][1] * source.d[1][2] - source.d[0][2] * source.d[1][1]) * factor;
	dst.d[1][2] = -(source.d[0][0] * source.d[1][2] - source.d[0][2] * source.d[1][0]) * factor;
	dst.d[2][2] = (source.d[0][0] * source.d[1][1] - source.d[0][1] * source.d[1][0]) * factor;
	return invertible ? MathResult::ok : MathResult::singular;
}

MathResult general_inverse(Transform3 &dst, float &w, const Transform3 &t)
{
	const Transform3 source = t;
	const Matrix3 &m = source.rotation;
	const float det = determinant(m);
	const bool invertible = std::fabs(det) > minimum_determinant;
	const float factor = invertible ? 1.0F / det : 1.0F;
	inverse(dst.rotation, m);
	const Vec3 &v = source.translation;
	dst.translation.x = -(m.d[0][1] * m.d[1][2] * v.z + m.d[1][1] * m.d[2][2] * v.x + m.d[2][1] * m.d[0][2] * v.y - m.d[0][1] * m.d[2][2] * v.y - m.d[1][1] * m.d[0][2] * v.z - m.d[2][1] * m.d[1][2] * v.x) * factor;
	dst.translation.y = (m.d[0][0] * m.d[1][2] * v.z + m.d[1][0] * m.d[2][2] * v.x + m.d[2][0] * m.d[0][2] * v.y - m.d[0][0] * m.d[2][2] * v.y - m.d[1][0] * m.d[0][2] * v.z - m.d[2][0] * m.d[1][2] * v.x) * factor;
	dst.translation.z = -(m.d[0][0] * m.d[1][1] * v.z + m.d[1][0] * m.d[2][1] * v.x + m.d[2][0] * m.d[0][1] * v.y - m.d[0][0] * m.d[2][1] * v.y - m.d[1][0] * m.d[0][1] * v.z - m.d[2][0] * m.d[1][1] * v.x) * factor;
	w = determinant(m) * factor;
	return invertible ? MathResult::ok : MathResult::singular;
}

void scale(Matrix3 &dst, const Matrix3 &m, float scalar)
{
	for (std::size_t row = 0; row < 3; ++row) {
		for (std::size_t column = 0; column < 3; ++column) {
			dst.d[row][column] = m.d[row][column] * scalar;
		}
	}
}

void multiply(Matrix3 &dst, const Matrix3 &left, const Matrix3 &right)
{
	Matrix3 result{};
	for (std::size_t row = 0; row < 3; ++row) {
		for (std::size_t column = 0; column < 3; ++column) {
			result.d[row][column] = left.d[row][0] * right.d[0][column] + left.d[row][1] * right.d[1][column] + left.d[row][2] * right.d[2][column];
		}
	}
	dst = result;
}

void multiply(Transform3 &dst, const Transform3 &left, const Transform3 &right)
{
	Transform3 result{};
	multiply(result.rotation, left.rotation, right.rotation);
	transform(result.translation, left, right.translation);
	dst = result;
}

void transform(Vec3 &dst, const Matrix3 &m, const Vec3 &v)
{
	const Vec3 result{m.d[0][0] * v.x + m.d[0][1] * v.y + m.d[0][2] * v.z,
	                  m.d[1][0] * v.x + m.d[1][1] * v.y + m.d[1][2] * v.z,
	                  m.d[2][0] * v.x + m.d[2][1] * v.y + m.d[2][2] * v.z};
	dst = result;
}

void rotate(Vec3 &dst, const Transform3 &t, const Vec3 &v) { transform(dst, t.rotation, v); }

void transform(Vec3 &dst, const Transform3 &t, const Vec3 &v)
{
	transform(dst, t.rotation, v);
	dst.x += t.translation.x;
	dst.y += t.translation.y;
	dst.z += t.translation.z;
}

void transpose_transform(Vec3 &dst, const Matrix3 &m, const Vec3 &v)
{
	const Vec3 result{m.d[0][0] * v.x + m.d[1][0] * v.y + m.d[2][0] * v.z,
	                  m.d[0][1] * v.x + m.d[1][1] * v.y + m.d[2][1] * v.z,
	                  m.d[0][2] * v.x + m.d[1][2] * v.y + m.d[2][2] * v.z};
	dst = result;
}

void inverse_transform(Vec3 &dst, const Transform3 &t, const Vec3 &v)
{
	const Vec3 translated{v.x - t.translation.x, v.y - t.translation.y, v.z - t.translation.z};
	transpose_transform(dst, t.rotation, translated);
}

void transform_list(Vec3 *dst, const Matrix3 &m, const Vec3 *src, std::size_t count)
{
	for (std::size_t index = 0; index < count; ++index) transform(dst[index], m, src[index]);
}

void transform_list(Vec3 *dst, const Transform3 &t, const Vec3 *src, std::size_t count)
{
	for (std::size_t index = 0; index < count; ++index) transform(dst[index], t, src[index]);
}

void transpose_transform_list(Vec3 *dst, const Matrix3 &m, const Vec3 *src, std::size_t count)
{
	const Matrix3 transposed = transpose(m);
	transform_list(dst, transposed, src, count);
}

void inverse_transform_list(Vec3 *dst, const Transform3 &t, const Vec3 *src, std::size_t count)
{
	for (std::size_t index = 0; index < count; ++index) inverse_transform(dst[index], t, src[index]);
}

void matrix_to_quaternion(Quaternion4 &dst, const Matrix3 &m)
{
	static constexpr int next[3] = {1, 2, 0};
	const float trace = m.d[0][0] + m.d[1][1] + m.d[2][2];
	if (trace > 0.0F) {
		float scale_value = std::sqrt(trace + 1.0F);
		dst.w = scale_value * 0.5F;
		scale_value = 0.5F / scale_value;
		dst.x = (m.d[2][1] - m.d[1][2]) * scale_value;
		dst.y = (m.d[0][2] - m.d[2][0]) * scale_value;
		dst.z = (m.d[1][0] - m.d[0][1]) * scale_value;
		return;
	}
	int i = m.d[1][1] > m.d[0][0] ? 1 : 0;
	if (m.d[2][2] > m.d[i][i]) i = 2;
	const int j = next[i];
	const int k = next[j];
	float values[4]{};
	float scale_value = std::sqrt((m.d[i][i] - (m.d[j][j] + m.d[k][k])) + 1.0F);
	values[i + 1] = scale_value * 0.5F;
	scale_value = 0.5F / scale_value;
	values[0] = (m.d[k][j] - m.d[j][k]) * scale_value;
	values[j + 1] = (m.d[j][i] + m.d[i][j]) * scale_value;
	values[k + 1] = (m.d[k][i] + m.d[i][k]) * scale_value;
	dst = {values[0], values[1], values[2], values[3]};
}

void quaternion_to_matrix(Matrix3 &dst, const Quaternion4 &q)
{
	const float scale_value = 2.0F / (q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
	const float xs = q.x * scale_value;
	const float ys = q.y * scale_value;
	const float zs = q.z * scale_value;
	const float wx = q.w * xs;
	const float wy = q.w * ys;
	const float wz = q.w * zs;
	const float xx = q.x * xs;
	const float xy = q.x * ys;
	const float xz = q.x * zs;
	const float yy = q.y * ys;
	const float yz = q.y * zs;
	const float zz = q.z * zs;
	dst = {{{1.0F - (yy + zz), xy - wz, xz + wy},
	        {xy + wz, 1.0F - (xx + zz), yz - wx},
	        {xz - wy, yz + wx, 1.0F - (xx + yy)}}};
}

void slerp(Quaternion4 &dst, const Quaternion4 &q1, const Quaternion4 &q2, float amount)
{
	const Quaternion4 sum{q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z};
	const Quaternion4 difference{q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z};
	float left_scale = difference.w * difference.w + difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;
	float right_scale = sum.w * sum.w + sum.x * sum.x + sum.y * sum.y + sum.z * sum.z;
	const Quaternion4 selected = left_scale > right_scale ? Quaternion4{-q2.w, -q2.x, -q2.y, -q2.z} : q2;
	const float cosine = q1.w * selected.w + q1.x * selected.x + q1.y * selected.y + q1.z * selected.z;
	if ((1.0F + cosine) > quaternion_epsilon) {
		if ((1.0F - cosine) > quaternion_epsilon) {
			const float omega = std::acos(cosine);
			const float sine = std::sin(omega);
			left_scale = std::sin((1.0F - amount) * omega) / sine;
			right_scale = std::sin(amount * omega) / sine;
		} else {
			left_scale = 1.0F - amount;
			right_scale = amount;
		}
		dst = {left_scale * q1.w + right_scale * selected.w,
		       left_scale * q1.x + right_scale * selected.x,
		       left_scale * q1.y + right_scale * selected.y,
		       left_scale * q1.z + right_scale * selected.z};
		return;
	}
	Quaternion4 perpendicular{q1.z, -q1.y, q1.x, -q1.w};
	left_scale = std::sin((1.0F - amount) * half_pi);
	right_scale = std::sin(amount * half_pi);
	perpendicular.x = left_scale * q1.x + right_scale * perpendicular.x;
	perpendicular.y = left_scale * q1.y + right_scale * perpendicular.y;
	perpendicular.z = left_scale * q1.z + right_scale * perpendicular.z;
	dst = perpendicular;
}

void transform(Vec3 &dst, const Quaternion4 &q, const Vec3 &v)
{
	const Vec3 qv{q.x, q.y, q.z};
	const Vec3 crossed = cross(qv, v);
	const float vector_factor = q.w * q.w - dot(qv, qv);
	const float projection_factor = 2.0F * dot(qv, v);
	dst = {v.x * vector_factor + qv.x * projection_factor + crossed.x * q.w * 2.0F,
	       v.y * vector_factor + qv.y * projection_factor + crossed.y * q.w * 2.0F,
	       v.z * vector_factor + qv.z * projection_factor + crossed.z * q.w * 2.0F};
}

void multiply(Quaternion4 &dst, const Quaternion4 &left, const Quaternion4 &right)
{
	const Vec3 lv{left.x, left.y, left.z};
	const Vec3 rv{right.x, right.y, right.z};
	const Vec3 crossed = cross(lv, rv);
	dst = {left.w * right.w - dot(lv, rv),
	       lv.x * right.w + rv.x * left.w + crossed.x,
	       lv.y * right.w + rv.y * left.w + crossed.y,
	       lv.z * right.w + rv.z * left.w + crossed.z};
}

float inverse_sqrt(float value)
{
	const std::uint32_t bits = float_bits(value);
	const std::uint32_t exponent = (bits >> 23U) & 0xffU;
	const std::uint32_t extended_mantissa = (bits >> 14U) & 0x3ffU;
	const std::uint32_t seed_exponent = ((3U * 127U - 1U) - exponent) >> 1U;
	const std::uint32_t seed_mantissa = static_cast<std::uint32_t>(inverse_sqrt_table()[extended_mantissa]) << 15U;
	float seed = bits_float((seed_exponent << 23U) | seed_mantissa);
	seed = (3.0F - seed * seed * value) * seed * 0.5F;
	seed = (3.0F - seed * seed * value) * seed * 0.5F;
	return seed;
}

float legacy_sqrt(float value) { return value * inverse_sqrt(value); }

} // namespace frontierx
