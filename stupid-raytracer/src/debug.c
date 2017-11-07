#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "debug.h"

int
_debug_die(const char *file, const char *func, int ln, const char *fmt, ...)
{
	va_list vargs;

	fprintf(stderr, "%s/%s:%d - ", file, func, ln);
	va_start(vargs, fmt);
	vfprintf(stderr, fmt, vargs);
	va_end(vargs);

	exit(1);
}
