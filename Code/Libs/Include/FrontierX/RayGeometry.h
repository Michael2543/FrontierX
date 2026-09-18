#ifndef FRONTIERX_RAY_GEOMETRY_H
#define FRONTIERX_RAY_GEOMETRY_H

#include <cmath>

namespace frontierx {

struct RayPoint3 {
	float x;
	float y;
	float z;
};

struct RayTriangleHit {
	float u;
	float v;
	float distance;
};

namespace ray_geometry_detail {

inline RayPoint3 subtract(const RayPoint3 &left, const RayPoint3 &right)
{
	return {left.x - right.x, left.y - right.y, left.z - right.z};
}

inline RayPoint3 cross(const RayPoint3 &left, const RayPoint3 &right)
{
	return {
		left.y * right.z - left.z * right.y,
		left.z * right.x - left.x * right.z,
		left.x * right.y - left.y * right.x,
	};
}

inline float dot(const RayPoint3 &left, const RayPoint3 &right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

inline bool intersects_slab(float minimum, float maximum, float origin, float direction,
	float &near_distance, float &far_distance)
{
	if (direction == 0.0F) {
		return origin >= minimum && origin <= maximum;
	}
	float first = (minimum - origin) / direction;
	float second = (maximum - origin) / direction;
	if (first > second) {
		const float temporary = first;
		first = second;
		second = temporary;
	}
	if (first > near_distance) near_distance = first;
	if (second < far_distance) far_distance = second;
	return near_distance <= far_distance;
}

} // namespace ray_geometry_detail

// Direction need not be normalized, matching the D3DXBoxBoundProbe contract.
inline bool ray_intersects_box(const RayPoint3 &minimum, const RayPoint3 &maximum,
	const RayPoint3 &origin, const RayPoint3 &direction)
{
	if (direction.x == 0.0F && direction.y == 0.0F && direction.z == 0.0F) return false;
	float near_distance = 0.0F;
	float far_distance = 3.402823466e+38F;
	return ray_geometry_detail::intersects_slab(minimum.x, maximum.x, origin.x, direction.x, near_distance, far_distance) &&
	       ray_geometry_detail::intersects_slab(minimum.y, maximum.y, origin.y, direction.y, near_distance, far_distance) &&
	       ray_geometry_detail::intersects_slab(minimum.z, maximum.z, origin.z, direction.z, near_distance, far_distance);
}

inline bool ray_intersects_triangle(const RayPoint3 &first, const RayPoint3 &second,
	const RayPoint3 &third, const RayPoint3 &origin, const RayPoint3 &direction,
	RayTriangleHit &hit)
{
	const RayPoint3 edge1 = ray_geometry_detail::subtract(second, first);
	const RayPoint3 edge2 = ray_geometry_detail::subtract(third, first);
	const RayPoint3 direction_cross_edge2 = ray_geometry_detail::cross(direction, edge2);
	const float determinant = ray_geometry_detail::dot(edge1, direction_cross_edge2);
	if (std::fabs(determinant) < 1.0e-6F) return false;

	const float inverse_determinant = 1.0F / determinant;
	const RayPoint3 origin_offset = ray_geometry_detail::subtract(origin, first);
	const float u = ray_geometry_detail::dot(origin_offset, direction_cross_edge2) * inverse_determinant;
	if (u < 0.0F || u > 1.0F) return false;

	const RayPoint3 origin_cross_edge1 = ray_geometry_detail::cross(origin_offset, edge1);
	const float v = ray_geometry_detail::dot(direction, origin_cross_edge1) * inverse_determinant;
	if (v < 0.0F || u + v > 1.0F) return false;

	const float distance = ray_geometry_detail::dot(edge2, origin_cross_edge1) * inverse_determinant;
	if (distance < 0.0F) return false;

	hit = {u, v, distance};
	return true;
}

} // namespace frontierx

#endif
