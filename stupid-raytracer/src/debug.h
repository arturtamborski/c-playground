#pragma once

#include <stdlib.h> // exit()

#define _TOSTR(x) #x
#define TOSTR(x) _TOSTR(x)

#ifdef DEBUG

#define debug_printf(fmt, ...) \
	fprintf(stderr, "%s:%s():%d: "fmt"\n", __FILE__, __func__, __LINE__, __VA_ARGS__)

#define debug_printv(var, fmt) \
	fprintf(stderr, "%s:%s():%d: "TOSTR(var)" \t= %"TOSTR(fmt)" (0x%x)\n", __FILE__, __func__, __LINE__, var, var)

#define debug_printa(var) \
	fprintf(stderr, "%s:%s():%d: "TOSTR(var)" = %p\n", __FILE__, __func__, __LINE__, (void *)&var)

#define debug_printf_and_die(fmt, ...) \
	do { debug_printf(fmt, __VA_ARGS__); exit(-1); } while(0)

#else

#define debug_printf(fmt, ...) \
	do { } while(0)

#define debug_printv(var, fmt) \
	do { } while(0)

#define debug_printa(var) \
	do { } while(0)

#define debug_printf_and_die(fmt, ...) \
	do { exit(-1); } while(0)

#endif
