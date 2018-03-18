#include <stdio.h>

#include "debug.h"
#include "types.h"
#include "bitmap.h"

int
main(int argc, char **argv)
{
	struct bitmap *bmp;

	bmp = bitmap_create();

	bitmap_init(bmp, 400, 200);
	for (u32 y = 0; y < bmp->header.height; y++)
		for (u32 x = 0; x < bmp->header.width; x++)
			*bitmap_at(bmp, x, y) = y < bmp->header.height / 2 ? 0xff : 0xff0000;
	bitmap_read(bmp, "untitled.bmp");

	bitmap_resize(bmp, bmp->header.width * 2, bmp->header.height * 2);
	bitmap_write(bmp, "test.bmp");

	bitmap_free(bmp);
	bitmap_destroy(bmp);

	return (0);
}
