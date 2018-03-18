#ifndef HASHMAP_H_INCLUDED
#define HASHMAP_H_INCLUDED

#include "types.h"

struct hashmap;
struct hashmap_element;

enum
{
	HASHMAP_OK			= 0,
	HASHMAP_ERROR_OUT_OF_MEMORY 	= -1,
	HASHMAP_ERROR_NO_SUCH_ELEMENT	= -2,
	HASHMAP_ERROR_FULL		= -3,

	HASHMAP_MAX_CHAIN_LENGTH	= 4,
};

struct hashmap
{
	size_t 			size;
	size_t 			nelements;
	struct hashmap_element	*elements;
};

struct hashmap_element
{
	str 	*key;
	void	*data;
};

struct hashmap	*hashmap_create(void);
void		hashmap_destroy(struct hashmap *map);

int		hashmap_init(struct hashmap *map, size_t nelements);
void		hashmap_finish(struct hashmap *map);

int		hashmap_insert(struct hashmap *map, const str *key, void *data);
int		hashmap_get(struct hashmap *map, const str *key, void **data);
int		hashmap_remove(struct hashmap *map, const str *key, void **data);

#define		HASHMAP_ITERATE(map, item, code) \
			do { size_t __iter; \
				for (__iter = 0; __iter < map->nelements; __iter++) { \
					if (map->elements[__iter].key == null) \
						continue; \
					item = map->elements[__iter].data; \
					code \
			} } while (0)

#endif // HASHMAP_H_INCLUDED
