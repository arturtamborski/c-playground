#include <math.h>

#include "vec3.h"


external struct vec3
vec3_norm(struct vec3 a)
{
	f32 length;
	struct vec3 ret;

	length = sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));

	ret.x = a.x / length;
	ret.y = a.y / length;
	ret.z = a.z / length;

	return (ret);
}

external f32
vec3_dot(struct vec3 a, struct vec3 b)
{
	f32 ret = 0;

	ret = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);

	return (ret);
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
	struct vec3 ret;

	ret.x = a.x + b.x;
	ret.y = a.y + b.y;
	ret.z = a.z + b.z;

	return (ret);
}

external struct vec3
vec3_sub(struct vec3 a, struct vec3 b)
{
	struct vec3 ret;

	ret.x = a.x - b.x;
	ret.y = a.y - b.y;
	ret.z = a.z - b.z;

	return (ret);
}


external struct vec3
vec3_mul(struct vec3 a, struct vec3 b)
{
	struct vec3 ret;

	ret.x = a.x * b.x;
	ret.y = a.y * b.y;
	ret.z = a.z * b.z;

	return (ret);
}

external struct vec3
vec3_div(struct vec3 a, struct vec3 b)
{
	struct vec3 ret;

	ret.x = b.x != 0.0f ? a.x / b.x : 0.0f;
	ret.y = b.y != 0.0f ? a.y / b.y : 0.0f;
	ret.z = b.z != 0.0f ? a.z / b.z : 0.0f;

	return (ret);
}
