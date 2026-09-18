#include "FrontierX/RayGeometry.h"

#include <cmath>
#include <iostream>

namespace {

bool close(float left, float right)
{
	return std::fabs(left - right) <= 1.0e-6F;
}

} // namespace

int main()
{
	using namespace frontierx;
	const RayPoint3 minimum{-1.0F, -1.0F, -1.0F};
	const RayPoint3 maximum{1.0F, 1.0F, 1.0F};
	if (!ray_intersects_box(minimum, maximum, {-2.0F, 0.0F, 0.0F}, {2.0F, 0.0F, 0.0F})) return 1;
	if (!ray_intersects_box(minimum, maximum, {0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F})) return 2;
	if (ray_intersects_box(minimum, maximum, {-2.0F, 2.0F, 0.0F}, {1.0F, 0.0F, 0.0F})) return 3;
	if (ray_intersects_box(minimum, maximum, {2.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F})) return 4;
	if (ray_intersects_box(minimum, maximum, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F})) return 5;

	const RayPoint3 first{0.0F, 0.0F, 0.0F};
	const RayPoint3 second{1.0F, 0.0F, 0.0F};
	const RayPoint3 third{0.0F, 1.0F, 0.0F};
	RayTriangleHit hit{-1.0F, -1.0F, -1.0F};
	if (!ray_intersects_triangle(first, second, third, {0.25F, 0.25F, 1.0F}, {0.0F, 0.0F, -2.0F}, hit)) return 6;
	if (!close(hit.u, 0.25F) || !close(hit.v, 0.25F) || !close(hit.distance, 0.5F)) return 7;
	if (!ray_intersects_triangle(third, second, first, {0.25F, 0.25F, 1.0F}, {0.0F, 0.0F, -2.0F}, hit)) return 8;
	if (ray_intersects_triangle(first, second, third, {1.0F, 1.0F, 1.0F}, {0.0F, 0.0F, -1.0F}, hit)) return 9;
	if (ray_intersects_triangle(first, second, third, {0.25F, 0.25F, -1.0F}, {0.0F, 0.0F, -1.0F}, hit)) return 10;
	if (ray_intersects_triangle(first, second, third, {0.25F, 0.25F, 1.0F}, {1.0F, 0.0F, 0.0F}, hit)) return 11;
	if (!ray_intersects_triangle(first, second, third, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, -1.0F}, hit)) return 12;

	std::cout << "portable ray geometry: ok\n";
	return 0;
}
