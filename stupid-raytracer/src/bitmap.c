#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "debug.h"
#include "bitmap.h"

internal inline u32
_bitmap_count_data_size(u32 width, u32 height)
{
	//return (height * (4 * ((bmp->header.bits_per_pixel * width + 31) / 32)))
	return (width * height * (u32)sizeof(u32));
}

internal void
_bitmap_update_header(struct bitmap *bmp, u32 width, u32 height)
{
	u32 size;
	
	size = _bitmap_count_data_size(width, height);
	bmp->header.file_size = (u32)sizeof(bmp->header) + size;
	bmp->header.width = width;
	bmp->header.height = height;
	bmp->header.data_size = size;

}

internal void
_bitmap_print_debug(struct bitmap *bmp)
{
	debug_printv(bmp->header.file_type, u);
	debug_printv(bmp->header.file_size, u);
	debug_printv(bmp->header.bitmap_offset, u);
	debug_printv(bmp->header.header_size, u);
	debug_printv(bmp->header.width, i);
	debug_printv(bmp->header.height, i);
	debug_printv(bmp->header.planes, u);
	debug_printv(bmp->header.bits_per_pixel, u);
	debug_printv(bmp->header.compression, u);
	debug_printv(bmp->header.data_size, u);
	debug_printv(bmp->header.horz_resolution, u);
	debug_printv(bmp->header.vert_resolution, u);
	debug_printv(bmp->header.colors_used, u);
	debug_printv(bmp->header.colors_important, u);
	debug_printv(bmp->header.red_mask, u);
	debug_printv(bmp->header.green_mask, u);
	debug_printv(bmp->header.blue_mask, u);
	debug_printv(bmp->header.alpha_mask, u);
}

struct bitmap *
bitmap_create(void)
{
	struct bitmap *bmp;

	if ((bmp = malloc(sizeof(*bmp))) == null)
		return (null);

	memset(bmp, 0, sizeof(*bmp));
	_bitmap_update_header(bmp, 0, 0);

	bmp->header.file_type = 0x4D42;
	bmp->header.bitmap_offset = sizeof(bmp->header);
	bmp->header.header_size = sizeof(bmp->header) - 14 - 16;
	bmp->header.planes = 1;
	bmp->header.bits_per_pixel = 32;
	bmp->header.horz_resolution = 0xb13;
	bmp->header.vert_resolution = 0xb13;
	bmp->header.alpha_mask = 0xff000000;
	bmp->header.red_mask = 0xff0000;
	bmp->header.green_mask = 0xff00;
	bmp->header.blue_mask = 0xff;
	bmp->data = null;

	return (bmp);
}

void
bitmap_destroy(struct bitmap *bmp)
{
	if (bmp->data)
		bitmap_free(bmp);

	free(bmp);
}

int
bitmap_init(struct bitmap *bmp, u32 width, u32 height)
{
	u32 *data;
	u32 size;

	if (bmp->data)
		bitmap_free(bmp);

	size = _bitmap_count_data_size(width, height);

	if ((data = malloc(size)) == null)
		return (-1);

	memset(data, 0, size);
	_bitmap_update_header(bmp, width, height);
	bmp->data = data;

	return (0);
}

void
bitmap_free(struct bitmap *bmp)
{
	_bitmap_update_header(bmp, 0, 0);
	free(bmp->data);
	bmp->data = null;
}

int
bitmap_resize(struct bitmap *bmp, u32 width, u32 height)
{
	u32 *data;
	u32 size;

	size = _bitmap_count_data_size(width, height);

	if ((data = malloc(size)) == null)
		return (-1);

	memset(data, 0, size);

	if (bmp->data)
		memcpy(data, bmp->data, bmp->header.data_size);

	bitmap_free(bmp);
	_bitmap_update_header(bmp, width, height);
	bmp->data = data;

	return (0);
}

int
bitmap_read(struct bitmap *bmp, cstr *fname)
{
	FILE *f;

	if ((f = fopen(fname, "rb")) == null)
		return (-1);

	if (bmp->data)
		bitmap_free(bmp);

	fread(&bmp->header, sizeof(bmp->header), 1, f);
	bitmap_resize(bmp, bmp->header.width, bmp->header.height);
	fread(bmp->data, bmp->header.data_size, 1, f);
	fclose(f);

	return (0);
}

int
bitmap_write(struct bitmap *bmp, cstr *fname)
{
	FILE *f;

	if ((f = fopen(fname, "wb")) == null)
		return (-1);

	fwrite(&bmp->header, sizeof(bmp->header), 1, f);
	fwrite(bmp->data, bmp->header.data_size, 1, f);
	fclose(f);

	return (0);
}
