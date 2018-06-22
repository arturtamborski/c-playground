#include <stdio.h>

#include "debug.h"
#include "types.h"

#include "bmp.h"
#include "vec3.h"
#include "raytracer.h"

u32
cast_ray(struct world *world, struct vec3 origin, struct vec3 direction)
{
	return (0xabcdef);
}

int
main(void)
{
	u32 *data;
	f32 screen_x, screen_y;
	struct vec3 direction;

	struct bmp bmp = {0};
	bmp_init(&bmp, 800, 600);
	data = bmp.data;

	struct camera camera;
	camera.position = VEC3(0, 10, 1);
	camera.z = vec3_norm(camera.position);
	camera.x = vec3_norm(vec3_cross(camera.z, V3(0, 0, 1)));
	camera.y = vec3_norm(vec3_cross(camera.z, camera.x));

	struct screen screen;
	screen.distance = 1.0f;
	screen.width = 1.0f;
	screen.height = 1.0f;
	screen.half_width = 0.5f * screen.width;
	screen.half_height = 0.5f * screen.height;
	screen.center = vec3_sub(camera.position, vec3_mul(sreen.distance, camera.z));

	struct material materials[2];
	materials[0].color.argb = 0x000000;
	materials[1].color.argb = 0xff0000;

	struct plane planes[2];
	planes[0].mat_index = 1;
	planes[1].mat_index = 1;

	struct sphere spheres[2];
	spheres[0].position = VEC3(1, 1, 1);
	spheres[0].radius = 2.0f;
	spheres[0].mat_index = 1;

	struct world world;
	world.materials_count = 2;
	world.materials = materials;
	world.planes_count = 1;
	world.planes = planes;
	world.spheres_count = 0;
	world.spheres = spheres;

	for (u32 y = 0; y < bmp.height; y++)
	{
		screen_y = -1.0f + 2.0f * ((f32)y / (f32)bmp.height);

		for (u32 x = 0; x < bmp.width; x++)
		{
			screen_x = -1.0f + 2.0f * ((f32)x / (f32)bmp.height);

			screen.position = screen.center + screen_x * screen.half_width * camera.x + screen_y * screen.half_width * camera.y;

			direction = normalize(screen.position - camera.position);
			*data++ = cast_ray(&world, camera.position, direction);
		}
	}

	bmp_write(&bmp, "test.bmp");
	bmp_free(&bmp);

	return (0);
}
