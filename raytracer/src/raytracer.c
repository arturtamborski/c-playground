#include "raytracer.h"


external f32 mix(f32 a, f32 b, f32 mix)
{
	return (b * mix + a * (1 - mix));
}

external u32
trace(struct scene *scene, struct vec3 origin, struct vec3 direction, int depth)
{
	u32 i;
	f32 tnear, t0, t1;
	struct sphere *sphere;

	tnear = 1e8;

	for (i = 0; i < scene->spheres_count; i++)
	{
		t0 = 1e8;
		t1 = 1e8;

		if (sphere_intersects(world->spheres[i], origin, direction, t0, t1))
		{
			if (t0 < 0)
				t0 = t1;

			if (t0 < tnear)
				tnear = t0, sphere = &world->spheres[i];
		}
	}

	if (!sphere)
		return 0xabcdef;

	return sphere.color;
}
