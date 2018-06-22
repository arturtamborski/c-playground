#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "debug.h"
#include "bmp.h"


internal inline u32
_bmp_count_data_size(u32 width, u32 height)
{
	return (u32)(width * height * sizeof(u32));
}

internal inline void
_bmp_update_header(struct bmp *bmp, u32 width, u32 height)
{
	u32 size;
	
	size = _bmp_count_data_size(width, height);
	bmp->file_size = (u32)(sizeof(*bmp) - sizeof(bmp->data) + size);
	bmp->width = width;
	bmp->height = height;
	bmp->data_size = size;
}

external struct bmp *
bmp_create(void)
{
	struct bmp *bmp;

	if ((bmp = malloc(sizeof(*bmp))) == null)
		return (null);

	memset(bmp, 0, sizeof(*bmp));

	return (bmp);
}

external void
bmp_destroy(struct bmp *bmp)
{
	if (bmp)
		free(bmp);
}

external int
bmp_init(struct bmp *bmp, u32 width, u32 height)
{
	u32 *data;
	u32 size;

	if (bmp->data)
		bmp_free(bmp);

	bmp->file_type = 0x4d42;
	bmp->_reserved = 0;
	bmp->bmp_offset = sizeof(*bmp) - sizeof(bmp->data);
	bmp->header_size = bmp->bmp_offset - 30;
	bmp->planes = 1;
	bmp->bits_per_pixel = 32;
	bmp->compression = 0;
	bmp->horz_resolution = 0x0b13;
	bmp->vert_resolution = 0x0b13;
	bmp->colors_used = 0;
	bmp->colors_important = 0;
	bmp->red_mask = 0x00ff0000;
	bmp->green_mask = 0x00ff00;
	bmp->blue_mask = 0x000000ff;
	bmp->alpha_mask = 0xff000000;
	bmp->data = null;

	size = _bmp_count_data_size(width, height);

	if ((data = malloc(size)) == null)
		return (-1);

	_bmp_update_header(bmp, width, height);
	memset(data, 0, size);
	bmp->data = data;

	return (0);
}

external void
bmp_free(struct bmp *bmp)
{
	_bmp_update_header(bmp, 0, 0);

	if (bmp->data)
		free(bmp->data);

	bmp->data = null;
}

external inline u32 *
bmp_at(struct bmp *bmp, u32 x, u32 y)
{
	return &bmp->data[y * bmp->width + x];
}

external int
bmp_resize(struct bmp *bmp, u32 width, u32 height)
{
	u32 *data;
	u32 size;
	u32 h, min_w, min_h;

	size = _bmp_count_data_size(width, height);

	if ((data = malloc(size)) == null)
		return (-1);

	memset(data, 0, size);

	if (bmp->data)
	{
		min_w = width < bmp->width ? width : bmp->width;
		min_h = height < bmp->height ? height : bmp->height;
		size = min_w * sizeof(u32);

		for (h = 0; h < min_h; h++)
			memcpy(data + h * width, bmp->data + h * bmp->width, size);
		
		free(bmp->data);
	}

	_bmp_update_header(bmp, width, height);
	bmp->data = data;

	return (0);
}

external int
bmp_copy(struct bmp *from, struct bmp *to)
{
	int err;

	if (to->data)
		bmp_free(to);

	err = bmp_init(to, from->width, from->height);
	if (err)
		return (err);

	memcpy(to->data, from->data, from->data_size);

	return (0);
}

external int
bmp_read(struct bmp *bmp, char *fname)
{
	FILE *f;

	if ((f = fopen(fname, "rb")) == null)
		return (-1);

	if (bmp->data)
		bmp_free(bmp);

	fread(bmp, bmp->bmp_offset, 1, f);
	bmp_init(bmp, bmp->width, bmp->height);
	fread(bmp->data, bmp->data_size, 1, f);
	fclose(f);

	return (0);
}

external int
bmp_write(struct bmp *bmp, char *fname)
{
	FILE *f;

	if ((f = fopen(fname, "wb")) == null)
		return (-1);

	// flip image horizontally
	//bmp->header.height *= -1;

	fwrite(bmp, bmp->bmp_offset, 1, f);
	fwrite(bmp->data, bmp->data_size, 1, f);
	fclose(f);

	return (0);
}
