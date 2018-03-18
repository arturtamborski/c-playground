#ifndef GAME_H
#define GAME_H

#include <stdio.h>

struct game_ctx
{
    int     argc;
    char**  argv;
    char*   mem;
    size_t  memlen;
};

void
game_create(struct game_ctx* ctx)
{
    puts("game_create()");
}

void
game_destroy(struct game_ctx* ctx)
{
    puts("game_destroy()");
}

////////////////////////////////////////////////////////////////////////////////
struct game_api
{
    int (*main)(struct game_ctx* ctx);
};

extern const struct game_api GAME_API;

#endif // GAME_H
