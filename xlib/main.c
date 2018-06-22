#include <X11/Xlib.h>

#include <unistd.h>

int
main(int argc, char **argv)
{
	int black, white;
	Display *display;
	Window window;
	GC gcontext;
	Xevent event;
	
	display = XOpenDisplay(0);
	
	if (!display)
		return -1;

	black = BlackPixel(display, DefaultScreen(display));
	white = WhitePixel(display, DefaultScreen(display));

	window = XCreateSimpleWindow(display, DefaultRootWindow(display),
				     0, 0, 200, 100, 0, black, black);
	XMapWindow(display, window);
	XSelectInput(display, window, StructureNotifyMask);

	gcontext = XCreateGC(display, window, 0, 0);
	XSetForeground(display, gcontext, white);

	for (;;) {
		XNextEvent(display, &event);

		if (event.type == MapNotify)
			break;

		XDrawLine(display, window, gcontext, 10, 60, 180, 20);
		XFlush(display);
		sleep(1);
	}

	return 0;
}
