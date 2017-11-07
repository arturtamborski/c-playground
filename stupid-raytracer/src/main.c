#include <stdio.h>

#include "debug.h"
#include "types.h"
#include "bitmap.h"

int
main(int argc, char **argv)
{
	struct bitmap *bmp;

	bmp = bitmap_create();
	bitmap_read(bmp, "template.bmp");
	bitmap_free(bmp);
	bitmap_destroy(bmp);

	bmp = bitmap_create();
	bitmap_init(bmp, 800, 600);

	for (u32 i = 0; i < bmp->header.width * bmp->header.height; i++)
		bmp->data[i] = i * 10;

	bitmap_write(bmp, "test.bmp");
	bitmap_free(bmp);
	bitmap_destroy(bmp);

	return (0);
}
