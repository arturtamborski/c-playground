#ifndef BMP_H
#define BMP_H


#include "types.h"


#pragma pack(push, 1)
struct bmp
{
	u16 file_type;
	u32 file_size;
	u32 _reserved;
	u32 bmp_offset;
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
	u32 *data;
};
#pragma pack(pop)


struct bmp 	*bmp_create(void);
void		bmp_destroy(struct bmp *bmp);

int		bmp_init(struct bmp *bmp, u32 width, u32 height);
void		bmp_free(struct bmp *bmp);

u32		*bmp_at(struct bmp *bmp, u32 x, u32 y);
int		bmp_resize(struct bmp *bmp, u32 width, u32 height);
int		bmp_copy(struct bmp *from, struct bmp *to);

int		bmp_read(struct bmp *bmp, char *fname);
int		bmp_write(struct bmp *bmp, char *fname);


#endif // BMP_H
