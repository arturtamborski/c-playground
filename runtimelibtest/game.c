#include <stdio.h>
#include "game.h"

static int
game_main(struct game_ctx* ctx)
{
    //puts("game_main()");
    //printf("*ctx       = %p\n", (void*)ctx);
    //printf("ctx->argc  = %d\n", ctx->argc);
    //printf("ctx->argv  = ");

    //for(int i = 0; i < ctx->argc; i++)
    //    printf("%s\t", ctx->argv[i]);
    //puts("");

    //printf("ctx->mem    = %p\n", ctx->mem);
    //printf("ctx->memlen = %lu\n", ctx->memlen);

    printf("Oh, hello there! !!\n");

    fflush(stdout);

    //game_start(ctx);

    return(0);
}

const struct game_api GAME_API =
{
    .main = game_main
};
