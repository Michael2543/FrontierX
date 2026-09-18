#ifndef FRONTIERX_MATH_CORE_H
#define FRONTIERX_MATH_CORE_H

#include <cstddef>

namespace frontierx {

struct Vec3 {
	float x;
	float y;
	float z;
};

struct Matrix3 {
	float d[3][3];
};

struct Transform3 {
	Matrix3 rotation;
	Vec3 translation;
};

struct Quaternion4 {
	float w;
	float x;
	float y;
	float z;
};

enum class MathResult {
	ok,
	singular,
};

float determinant(const Matrix3 &matrix);
MathResult inverse(Matrix3 &destination, const Matrix3 &matrix);
MathResult general_inverse(Transform3 &destination, float &w, const Transform3 &transform);
void scale(Matrix3 &destination, const Matrix3 &matrix, float scalar);
void multiply(Matrix3 &destination, const Matrix3 &left, const Matrix3 &right);
void multiply(Transform3 &destination, const Transform3 &left, const Transform3 &right);

void transform(Vec3 &destination, const Matrix3 &matrix, const Vec3 &vector);
void rotate(Vec3 &destination, const Transform3 &transform, const Vec3 &vector);
void transform(Vec3 &destination, const Transform3 &transform, const Vec3 &vector);
void transpose_transform(Vec3 &destination, const Matrix3 &matrix, const Vec3 &vector);
void inverse_transform(Vec3 &destination, const Transform3 &transform, const Vec3 &vector);
void transform_list(Vec3 *destination, const Matrix3 &matrix, const Vec3 *source, std::size_t count);
void transform_list(Vec3 *destination, const Transform3 &transform, const Vec3 *source, std::size_t count);
void transpose_transform_list(Vec3 *destination, const Matrix3 &matrix, const Vec3 *source, std::size_t count);
void inverse_transform_list(Vec3 *destination, const Transform3 &transform, const Vec3 *source, std::size_t count);

void matrix_to_quaternion(Quaternion4 &destination, const Matrix3 &matrix);
void quaternion_to_matrix(Matrix3 &destination, const Quaternion4 &quaternion);
void slerp(Quaternion4 &destination, const Quaternion4 &left, const Quaternion4 &right, float amount);
void transform(Vec3 &destination, const Quaternion4 &quaternion, const Vec3 &vector);
void multiply(Quaternion4 &destination, const Quaternion4 &left, const Quaternion4 &right);

float inverse_sqrt(float value);
float legacy_sqrt(float value);

} // namespace frontierx

#endif
