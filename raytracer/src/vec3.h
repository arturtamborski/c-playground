#ifndef VEC3_H
#define VEC3_H


#include <stdarg.h>

#include "types.h"
#include "vec2.h"


#define VEC3(...) \
	((struct vec3){__VA_ARGS__})


struct vec3
{
	union
	{
		struct
		{
			f32 x, y, z;
		};
		struct
		{
			struct vec2 xy;
		};
		struct
		{
			f32 _;
			struct vec2 yz;
		};
		struct
		{
			f32 u, v;
		};
		struct
		{
			struct vec2 uv;
		};

		u32 data[3];
	};
};


external f32 vec3_len(struct vec3 a);
external f32 vec3_len2(struct vec3 a);
external struct vec3 vec3_norm(struct vec3 a);
external f32 vec3_dot(struct vec3 a, struct vec3 b);
external struct vec3 vec3_cross(struct vec3 a, struct vec3 b);
external struct vec3 vec3_add(struct vec3 a, struct vec3 b);
external struct vec3 vec3_sub(struct vec3 a, struct vec3 b);
external struct vec3 vec3_mul(struct vec3 a, struct vec3 b);



#endif // VEC3_H
