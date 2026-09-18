#include "FrontierX/MathCore.h"

#include <cmath>
#include <iostream>

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

	std::cout << "portable math core: ok\n";
	return 0;
}
