#include <math.h>

#include "vec3.h"


external f32
vec3_len(struct vec3 a)
{
	return sqrt(vec3_len2(a));
}

external f32
vec3_len2(struct vec3 a)
{
	return (a.x * a.x + a.y * a.y + a.z * a.z);
}

external struct vec3
vec3_norm(struct vec3 a)
{
	f32 len, inv_len;

	len = vec3_len2(a);

	if (len > 0)
	{
		inv_len = 1 / sqrt(len);

		a.x *= inv_len;
		a.y *= inv_len;
		a.z *= inv_len;
	}

	return (a);
}

external f32
vec3_dot(struct vec3 a, struct vec3 b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

external struct vec3
vec3_cross(struct vec3 a, struct vec3 b)
{
	struct vec3 ret;

	ret.x = a.y * b.z - a.z * b.y;
	ret.y = a.z * b.x - a.x * b.z;
	ret.z = a.x * b.y - a.y * b.x;

	return (ret);
}

external struct vec3
vec3_add(struct vec3 a, struct vec3 b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;

	return (a);
}

external struct vec3
vec3_sub(struct vec3 a, struct vec3 b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;

	return (a);
}


external struct vec3
vec3_mul(struct vec3 a, struct vec3 b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;

	return (a);
}
