#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>

#include "game.h"

#define LIBGAME       "./libgame.so"
#define LIBGAMESYMBOL "GAME_API"

struct library
{
    char* name;
    char* symbol;

    void* api;
    void* ctx;

    void* handle;
    struct stat attr;
};

//  0 - lib opened and loaded sucessfully
// -1 - trying to open same library as loaded one
// -2 - dlopen cannot open lib->name
// -3 - dlsym cannot find lib->symbol
static int library_open(struct library* lib);

//  0 - lib unloaded and closed sucessfully
// -1 - lib->handle is not valid
static int library_close(struct library* lib);


int
library_open(struct library* lib)
{
    struct stat attr;
    // return if cant get lib stats or lib inode is the same as the loaded one
    if((stat(lib->name, &attr)) || (lib->attr.st_ino == attr.st_ino))
        return(-1);

    // close lib before opening new one
    library_close(lib);

    // open library
    void* handle = dlopen(lib->name, RTLD_NOW);
    if(!handle)
        return(-2);

    lib->handle = handle;
    lib->attr   = attr;

    // link with symbol
    void* api = dlsym(lib->handle, lib->symbol);
    if(!api)
    {
        library_close(lib);
        return(-3);
    }

    lib->api = api;

    return(0);
}

int
library_close(struct library* lib)
{
    if(!lib->handle)
        return(-1);

    dlclose(lib->handle);

    lib->ctx    = NULL;
    lib->handle = NULL;
    lib->attr.st_ino = 0;

    return(0);
}

int
main(int argc, char** argv)
{
    struct game_ctx* ctx = malloc(sizeof(struct game_ctx));
    if(!ctx)
        return(-1);

    ctx->argc   = argc;
    ctx->argv   = argv;
    ctx->memlen = 1024;
    ctx->mem    = malloc(ctx->memlen);
    if(!ctx->mem)
        return(-2);

    struct library lib = { 0 };
    lib.name    = LIBGAME;
    lib.symbol  = LIBGAMESYMBOL;
    lib.ctx     = ctx;

    for(;;)
    {
        library_open(&lib);

        // note: should be valid
        struct game_api* api = lib.api;

        api->main(ctx);

        library_close(&lib);

        sleep(5);
        puts("next iteration...");
    }

    return(0);
}
