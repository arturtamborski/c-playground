#ifndef RAYTRACER_H
#define RAYTRACER_H


#include "types.h"
#include "vec3.h"


struct camera
{
	struct vec3 position;
	struct vec3 x, y, z;
};

struct light
{
	struct vec3 position;
};

struct screen
{
	f32 distance;
	f32 width;
	f32 height;
	f32 half_width;
	f32 half_height;
	struct vec3 center;
};

struct material
{
	struct vec3 color;
};

struct plane
{
	struct vec3 normal;
	f32 distance;
	f32 mat_index;
};

struct sphere
{
	struct vec3 position;
	f32 radius;
	f32 mat_index;
};

struct scene
{
	struct camera camera;

	u32 lights_count;
	struct light *lights;

	u32 materials_count;
	struct material *materials;

	u32 planes_count;
	struct plane *planes;

	u32 spheres_count;
	struct sphere *spheres;
};


external f32 mix(f32 a, f32 b, f32 mix);
external f32 trace(struct scene *scene, struct vec3 origin, struct vec3 direction, int depth);

#endif // RAYTRACER_H
