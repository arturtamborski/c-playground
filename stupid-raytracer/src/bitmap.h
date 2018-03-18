#include "types.h"

#pragma pack(push, 1)
struct bitmap_header
{
	u16 file_type;
	u32 file_size;
	u32 _reserved;
	u32 bitmap_offset;

	u32 header_size;
	u32 width;
	u32 height;
	u16 planes;
	u16 bits_per_pixel;
	u32 compression;
	u32 data_size;
	s32 horz_resolution;
	s32 vert_resolution;
	u32 colors_used;
	u32 colors_important;

	u32 red_mask;
	u32 green_mask;
	u32 blue_mask;
	u32 alpha_mask;
};
#pragma pack(pop)

struct bitmap
{
	struct bitmap_header header;
	u32 *data;
};

struct bitmap 	*bitmap_create(void);
void		bitmap_destroy(struct bitmap *bmp);

int		bitmap_init(struct bitmap *bmp, u32 width, u32 height);
void		bitmap_free(struct bitmap *bmp);

u32		*bitmap_at(struct bitmap *bmp, u32 width, u32 height);
int		bitmap_resize(struct bitmap *bmp, u32 width, u32 height);
int		bitmap_copy(struct bitmap *from, struct bitmap *to);

int		bitmap_read(struct bitmap *bmp, cstr *fname);
int		bitmap_write(struct bitmap *bmp, cstr *fname);
