#include "FrontierX/MathCore.h"
#include "FrontierX/NativeD3dxMath.h"
#include "FrontierX/NativeAppMath.h"
#include "Include/NativeAppGlobals.h"
#include "vector.h"
#include "GridVector.h"
#include "RandomGen.h"

#include <cmath>
#include <cstring>
#include <iostream>

extern Vector circleLines[NUM_CIRCLE_SEGS];

namespace {

bool close(float left, float right, float tolerance = 1.0e-5F)
{
	return std::fabs(left - right) <= tolerance;
}

bool vector_is(const frontierx::Vec3 &value, float x, float y, float z)
{
	return close(value.x, x) && close(value.y, y) && close(value.z, z);
}

} // namespace

int main()
{
	RandomGen fieldRandom(42);
	const U32 fieldValues[] = {667, 288, 870, 165, 162, 556};
	for (U32 expected : fieldValues)
		if (fieldRandom.rand(1000) != expected) return 24;
	using namespace frontierx;
	const Matrix3 matrix{{{1.0F, 2.0F, 3.0F}, {0.0F, 1.0F, 4.0F}, {5.0F, 6.0F, 0.0F}}};
	if (!close(determinant(matrix), 1.0F)) return 1;
	Matrix3 inverse_matrix{};
	if (inverse(inverse_matrix, matrix) != MathResult::ok) return 2;
	Matrix3 identity{};
	multiply(identity, matrix, inverse_matrix);
	for (int row = 0; row < 3; ++row) {
		for (int column = 0; column < 3; ++column) {
			if (!close(identity.d[row][column], row == column ? 1.0F : 0.0F)) return 3;
		}
	}
	const Matrix3 singular{{{1.0F, 2.0F, 3.0F}, {2.0F, 4.0F, 6.0F}, {0.0F, 0.0F, 0.0F}}};
	if (inverse(inverse_matrix, singular) != MathResult::singular) return 4;
	Matrix3 aliased = matrix;
	if (inverse(aliased, aliased) != MathResult::ok || !close(aliased.d[0][0], -24.0F)) return 10;

	const Transform3 transform_value{matrix, {10.0F, -2.0F, 3.0F}};
	const Vec3 input{2.0F, -1.0F, 0.5F};
	Vec3 transformed{};
	transform(transformed, transform_value, input);
	Vec3 restored{};
	inverse_transform(restored, transform_value, transformed);
	// inverse_transform is deliberately the legacy transpose-based rotation inverse.
	const Matrix3 rotation{{{0.0F, -1.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 1.0F}}};
	const Transform3 rigid{rotation, {10.0F, -2.0F, 3.0F}};
	transform(transformed, rigid, input);
	inverse_transform(restored, rigid, transformed);
	if (!vector_is(restored, input.x, input.y, input.z)) return 5;
	Transform3 rigid_inverse{};
	float homogeneous_w = 0.0F;
	if (general_inverse(rigid_inverse, homogeneous_w, rigid) != MathResult::ok || !close(homogeneous_w, 1.0F)) return 11;
	Transform3 composed{};
	multiply(composed, rigid, rigid_inverse);
	if (!vector_is(composed.translation, 0.0F, 0.0F, 0.0F)) return 12;
	const Vec3 list_input[2] = {input, {1.0F, 2.0F, 3.0F}};
	Vec3 list_output[2]{};
	transform_list(list_output, rigid, list_input, 2);
	if (!vector_is(list_output[0], transformed.x, transformed.y, transformed.z)) return 13;
	inverse_transform_list(list_output, rigid, list_output, 2);
	if (!vector_is(list_output[0], input.x, input.y, input.z) || !vector_is(list_output[1], 1.0F, 2.0F, 3.0F)) return 14;

	const Quaternion4 quarter_turn{std::sqrt(0.5F), 0.0F, 0.0F, std::sqrt(0.5F)};
	Matrix3 quaternion_matrix{};
	quaternion_to_matrix(quaternion_matrix, quarter_turn);
	Vec3 rotated{};
	transform(rotated, quarter_turn, Vec3{1.0F, 0.0F, 0.0F});
	if (!vector_is(rotated, 0.0F, 1.0F, 0.0F)) return 6;
	Quaternion4 round_trip{};
	matrix_to_quaternion(round_trip, quaternion_matrix);
	if (!close(std::fabs(round_trip.w), std::fabs(quarter_turn.w)) || !close(std::fabs(round_trip.z), std::fabs(quarter_turn.z))) return 7;

	Quaternion4 midpoint{};
	slerp(midpoint, Quaternion4{1.0F, 0.0F, 0.0F, 0.0F}, quarter_turn, 0.5F);
	if (!close(midpoint.w, 0.9238795F) || !close(midpoint.z, 0.3826834F)) return 8;
	Quaternion4 product{};
	multiply(product, Quaternion4{1.0F, 0.0F, 0.0F, 0.0F}, quarter_turn);
	if (!close(product.w, quarter_turn.w) || !close(product.z, quarter_turn.z)) return 15;
	if (!close(inverse_sqrt(4.0F), 0.5F, 1.0e-6F) || !close(legacy_sqrt(9.0F), 3.0F, 1.0e-5F)) return 9;

	// The App mesh's D3DX replacement must preserve row-vector ordering and in-place calls.
	D3DXMATRIX mesh_scale{};
	D3DXMATRIX mesh_translation{};
	D3DXMatrixScaling(&mesh_scale, 2.0F, 3.0F, 4.0F);
	D3DXMatrixTranslation(&mesh_translation, 5.0F, -2.0F, 1.0F);
	D3DXMatrixMultiply(&mesh_scale, &mesh_scale, &mesh_translation);
	if (!close(mesh_scale.m[0][0], 2.0F) || !close(mesh_scale.m[1][1], 3.0F) ||
		!close(mesh_scale.m[2][2], 4.0F) || !close(mesh_scale.m[3][0], 5.0F) ||
		!close(mesh_scale.m[3][1], -2.0F) || !close(mesh_scale.m[3][2], 1.0F)) return 16;
	D3DXMatrixTranspose(&mesh_scale, &mesh_scale);
	if (!close(mesh_scale.m[0][3], 5.0F) || !close(mesh_scale.m[3][0], 0.0F)) return 17;
	D3DXMATRIX mesh_rotation{};
	D3DXMatrixRotationX(&mesh_rotation, 1.57079632679F);
	if (!close(mesh_rotation.m[1][2], 1.0F) || !close(mesh_rotation.m[2][1], -1.0F)) return 18;
	D3DXMatrixRotationY(&mesh_rotation, 1.57079632679F);
	if (!close(mesh_rotation.m[0][2], -1.0F) || !close(mesh_rotation.m[2][0], 1.0F)) return 19;
	D3DXMatrixRotationZ(&mesh_rotation, 1.57079632679F);
	if (!close(mesh_rotation.m[0][1], 1.0F) || !close(mesh_rotation.m[1][0], -1.0F)) return 20;
	D3DXVECTOR3 mesh_x(1.0F, 0.0F, 0.0F);
	const D3DXVECTOR3 mesh_y(0.0F, 1.0F, 0.0F);
	D3DXVec3Cross(&mesh_x, &mesh_x, &mesh_y);
	if (!close(mesh_x.x, 0.0F) || !close(mesh_x.y, 0.0F) || !close(mesh_x.z, 1.0F)) return 21;
	D3DXVec3Normalize(&mesh_x, &mesh_x);
	if (!close(D3DXVec3LengthSq(&mesh_x), 1.0F)) return 22;
	D3DXVECTOR3 mesh_zero(0.0F, 0.0F, 0.0F);
	D3DXVec3Normalize(&mesh_zero, &mesh_zero);
	if (!close(D3DXVec3LengthSq(&mesh_zero), 0.0F)) return 23;
	const float quarter = std::atan2(1.0F, 1.0F);
	if (!close(get_angle(1.0F, 1.0F), quarter) ||
		!close(get_angle(-1.0F, 1.0F), -quarter) ||
		!close(get_angle(1.0F, -1.0F), 3.0F * quarter) ||
		!close(get_angle(-1.0F, -1.0F), -3.0F * quarter)) return 24;
	if (get_angle(1.0F, 0.0F) != static_cast<float>(1.570796) ||
		get_angle(-1.0F, 0.0F) != static_cast<float>(-1.570796) ||
		get_angle(0.0F, 0.0F) != static_cast<float>(-1.570796)) return 25;

	// ObjectList's proximity path consumes the production quarter-grid table.
	GRIDVECTOR origin{};
	GRIDVECTOR point{};
	origin.zero();
	point.init(0.75F, 1.0F);
	if ((point - origin) != 1.25F || (origin - point) != 1.25F)
	{
		std::cerr << "grid distance 3-4: " << (point - origin) << '\n';
		return 26;
	}
	point.init(16.0F, 0.0F);
	if ((point - origin) != 16.0F) return 27;
	std::uint64_t grid_hash = 14695981039346656037ULL;
	for (int x = 0; x < 64; ++x)
		for (int y = 0; y < 64; ++y)
		{
			point.init(static_cast<float>(x) * 0.25F,
				static_cast<float>(y) * 0.25F);
			const float distance = point - origin;
			std::uint32_t bits = 0;
			std::memcpy(&bits, &distance, sizeof(bits));
			grid_hash ^= bits;
			grid_hash *= 1099511628211ULL;
		}
	std::cout << "production grid-distance table hash: " << grid_hash << '\n';
	if (grid_hash != 10760374359831800577ULL) return 28;

	// BaseObject selection/range rings consume this original static table.
	for (int segment = 0; segment < NUM_CIRCLE_SEGS; ++segment)
	{
		const Vector &point_on_ring = circleLines[segment];
		if (!close(point_on_ring.x * point_on_ring.x +
			point_on_ring.y * point_on_ring.y, 1.0F, 2.0e-5F) ||
			point_on_ring.z != 0.0F) return 29;
	}
	if (!close(circleLines[0].x, 1.0F) || !close(circleLines[0].y, 0.0F) ||
		!close(circleLines[NUM_CIRCLE_SEGS / 4].x, 0.0F) ||
		!close(circleLines[NUM_CIRCLE_SEGS / 4].y, 1.0F) ||
		!close(circleLines[NUM_CIRCLE_SEGS / 2].x, -1.0F) ||
		!close(circleLines[NUM_CIRCLE_SEGS / 2].y, 0.0F)) return 30;

	std::cout << "portable math core: ok\n";
	return 0;
}
