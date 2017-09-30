#pragma once

#include <sys/types.h>

typedef struct state state;

typedef struct api {
	state 	*(*init)(void);
	int 	(*load)(state *);
	int 	(*step)(state *);
	void 	(*unload)(state *);
	void 	(*finish)(state *);
} api;

typedef struct app {
	void 	*handle;
	ino_t 	id;
	api 	api;
	state 	*state;
} app;
