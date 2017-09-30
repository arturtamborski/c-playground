/* This is just a ripoff code originally written by Chris Wellons
 * If you don't understand this then read his great blog post here:
 * http://nullprogram.com/blog/2014/12/23/
 */

#include <stdio.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "app.h"


void
unload_app(app *app)
{
	if (!app->handle)
		return;

	app->api.finish(app->state);
	app->state = NULL;

	dlclose(app->handle);
	app->handle = NULL;
	app->id = 0;
}

int
load_app(app *app, char *lib, char *symbol)
{
	struct stat attr;
	void *handle = NULL;
	api *api;

	// can't access file info
	if (stat(lib, &attr) != 0)
		return (-1);

	// file is the same as loaded one
	if (app->id == attr.st_ino)
		return (0);

	// unload loaded lib
	if (app->handle) {
		app->api.unload(app->state);
		dlclose(app->handle);
	}

	// can't open/load lib
	if ((handle = dlopen(lib, RTLD_NOW)) == NULL) {
		app->handle = NULL;
		app->id = 0;
		return (-2);
	}

	app->handle = handle;
	app->id = attr.st_ino;
	
	// can't find symbol
	if ((api = dlsym(app->handle, symbol)) == NULL) {
		dlclose(app->handle);
		app->handle = NULL;
		app->id = 0;
		return (-3);
	}

	app->api = *api;

	// state doesn't exist yet
	if (!app->state) {
		app->state = app->api.init();

		// api init failed
		if (!app->state) {
			unload_app(app);
			return (-4);
		}
	}

	return (app->api.load(app->state));
}

int
main(void)
{
	int ret;
	app app = { 0 };

	while (1) {
		if ((ret = load_app(&app, "./libapp.so", "API")) != 0) {
			printf("load_app(): %d\n", ret);
			usleep(100000);
			continue;
		}

		if (app.api.step(app.state))
			break;

		usleep(100000);
	}
	
	unload_app(&app);

	return (0);
}
