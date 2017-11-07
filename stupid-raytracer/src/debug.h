#pragma once

#define _TOSTR(x) #x
#define TOSTR(x) _TOSTR(x)

#ifdef DEBUG

#define debug_die(fmt, ...) \
	_debug_die(__FILE__, __func__, __LINE__, fmt"\n", __VA_ARGS__)

#define debug_printf(fmt, ...) \
	fprintf(stderr, "%s:%s():%d: "fmt"\n", __FILE__, __func__, __LINE__, __VA_ARGS__)

#define debug_printv(var, fmt) \
	fprintf(stderr, "%s:%s():%d: "TOSTR(var)" \t= %"TOSTR(fmt)" (0x%x)\n", __FILE__, __func__, __LINE__, var, var)

#define debug_printa(var) \
	fprintf(stderr, "%s:%s():%d: "TOSTR(var)" = %p\n", __FILE__, __func__, __LINE__, (void *)&var)

#else

#define debug_die(fmt, ...) \
	exit(1)

#define debug_printf(fmt, ...) \
	do { } while(0)

#define debug_printv(var, fmt) \
	do { } while(0)

#define debug_printa(var) \
	do { } while(0)

#endif

