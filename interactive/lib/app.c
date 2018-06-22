#include <stdio.h>
#include <stdlib.h>

#include "app.h"

struct state {
	int x, y;
};

static state *
init(void)
{
	puts("initializing...");
	state *s = malloc(sizeof(*s));

	return (s);
}

static int
load(state *s)
{
	puts("loading...");

	return (0);
}

static int
step(state *s)
{
	printf("x - %d\n", s->x++);
	puts("stepping...\n\n");

	return (0);
}

static void
unload(state *s)
{
	puts("unloading...");
}

static void
finish(state *s)
{
	puts("finishing...");
	free(s);
}

const api API = {
	.init = init,
	.load = load,
	.step = step,
	.unload = unload,
	.finish = finish
};
