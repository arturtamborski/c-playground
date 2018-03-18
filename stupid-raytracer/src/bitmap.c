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

internal inline void
_bitmap_update_header(struct bitmap *bmp, u32 width, u32 height)
{
	u32 size;
	
	size = _bitmap_count_data_size(width, height);
	bmp->header.file_size = (u32)sizeof(bmp->header) + size;
	bmp->header.width = width;
	bmp->header.height = height;
	bmp->header.data_size = size;
}

external struct bitmap *
bitmap_create(void)
{
	struct bitmap *bmp;

	if ((bmp = malloc(sizeof(*bmp))) == null)
		return (null);

	_bitmap_update_header(bmp, 0, 0);
	bmp->header.file_type = 0x4D42;
	bmp->header._reserved = 0;
	bmp->header.bitmap_offset = sizeof(bmp->header);
	bmp->header.header_size = sizeof(bmp->header) - 14 - 16;
	bmp->header.planes = 1;
	bmp->header.bits_per_pixel = 32;
	bmp->header.compression = 0;
	bmp->header.horz_resolution = 0xb13;
	bmp->header.vert_resolution = 0xb13;
	bmp->header.colors_used = 0;
	bmp->header.colors_important = 0;
	bmp->header.red_mask = 0xff0000;
	bmp->header.green_mask = 0xff00;
	bmp->header.blue_mask = 0xff;
	bmp->header.alpha_mask = 0xff000000;
	bmp->data = null;

	return (bmp);
}

external void
bitmap_destroy(struct bitmap *bmp)
{
	if (bmp->data)
		bitmap_free(bmp);

	free(bmp);
}

external int
bitmap_init(struct bitmap *bmp, u32 width, u32 height)
{
	u32 *data;
	u32 size;

	if (bmp->data)
		bitmap_free(bmp);

	size = _bitmap_count_data_size(width, height);

	if ((data = malloc(size)) == null)
		return (-1);

	_bitmap_update_header(bmp, width, height);
	memset(data, 0, size);
	bmp->data = data;

	return (0);
}

external void
bitmap_free(struct bitmap *bmp)
{
	_bitmap_update_header(bmp, 0, 0);
	free(bmp->data);
	bmp->data = null;
}

external inline u32 *
bitmap_at(struct bitmap *bmp, u32 width, u32 height)
{
	return &bmp->data[height * bmp->header.width + width];
}

external int
bitmap_resize(struct bitmap *bmp, u32 width, u32 height)
{
	u32 *data;
	u32 size;
	u32 h, min_w, min_h;

	size = _bitmap_count_data_size(width, height);

	if ((data = malloc(size)) == null)
		return (-1);

	memset(data, 0, size);

	if (bmp->data) {
		min_w = width < bmp->header.width ? width : bmp->header.width;
		min_h = height < bmp->header.height ? height : bmp->header.height;

		for (h = 0; h < min_h; h++)
			memcpy(data + h * width, bmp->data + h * bmp->header.width, min_w * sizeof(u32));
	}

	_bitmap_update_header(bmp, width, height);
	free(bmp->data);
	bmp->data = data;

	return (0);
}

external int
bitmap_copy(struct bitmap *from, struct bitmap *to)
{
	int ret;

	if (to->data)
		bitmap_free(to);

	ret = bitmap_init(to, from->header.width, from->header.height);
	if (ret)
		return (ret);

	memcpy(to->data, from->data, from->header.data_size);

	return (0);
}

external int
bitmap_read(struct bitmap *bmp, cstr *fname)
{
	FILE *f;

	if ((f = fopen(fname, "rb")) == null)
		return (-1);

	if (bmp->data)
		bitmap_free(bmp);

	fread(&bmp->header, sizeof(bmp->header), 1, f);
	bitmap_init(bmp, bmp->header.width, bmp->header.height);
	fread(bmp->data, bmp->header.data_size, 1, f);
	fclose(f);

	return (0);
}

external int
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
