#ifndef VEC2_H
#define VEC2_H


#include <stdarg.h>

#include "types.h"


#define VEC2(...) \
	((struct vec2){__VA_ARGS__})


struct vec2
{
	union
	{
		struct
		{
			f32 x, y;
		};
		struct
		{
			f32 u, v;
		};

		u32 data[2];
	};
};


#endif // VEC2_H
