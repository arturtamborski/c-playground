#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <stdlib.h>

#define _TOSTR(x) #x
#define TOSTR(x) _TOSTR(x)

#ifdef DEBUG
#undef NDEBUG
#include <stdio.h>
#include <sys/time.h>
#include <assert.h>

#define DEBUG_PRINT(fmt) \
	fprintf(stderr, "%s:%s():%d: "fmt"\n", __FILE__, __func__, __LINE__)

#define DEBUG_PRINTF(fmt, ...) \
	fprintf(stderr, "%s:%s():%d: "fmt"\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#define DEBUG_PRINTV(var, fmt) \
	fprintf(stderr, "%s:%s():%d: "TOSTR(var)" = %"fmt" (0x%lx)\n", __FILE__, __func__, __LINE__, var, (unsigned long int)var)

#define DEBUG_PRINTA(var) \
	fprintf(stderr, "%s:%s():%d: "TOSTR(var)" = %p\n", __FILE__, __func__, __LINE__, (void *)&var)

#define DEBUG_DIE() \
	do { exit(-1); } while (0)

#define DEBUG_PRINT_AND_DIE(fmt) \
	do { DEBUG_PRINT(fmt); exit(-1); } while (0)

#define DEBUG_PRINTF_AND_DIE(fmt, ...) \
	do { DEBUG_PRINTF(fmt, ##__VA_ARGS__); exit(-1); } while (0)

#define DEBUG_BENCHMARK(expression) \
	do { struct timeval __debug_before, __debug_after; \
		gettimeofday(&__debug_before, null); \
		expression; \
		gettimeofday(&__debug_after, null); \
		DEBUG_PRINTF("benchmark of '"TOSTR(expression)"' = %ld", __debug_after.tv_usec - __debug_before.tv_usec); \
	} while (0)

#define DEBUG_BEGIN_BENCHMARK() \
	do { struct timeval __debug_before, __debug_after; \
	gettimeofday(&__debug_before, null)

#define DEBUG_END_BENCHMARK() \
	gettimeofday(&__debug_after, null); \
	DEBUG_PRINTF("benchmark of '"TOSTR(expression)"' = %ldus", __debug_after.tv_usec - __debug_before.tv_usec); } while (0)

#define DEBUG_ASSERT(expr) \
	assert(expr)

#else

#define DEBUG_PRINT(fmt) \
	do { } while (0)

#define DEBUG_PRINTF(fmt, ...) \
	do { (void)(__VA_ARGS__); } while (0)

#define DEBUG_PRINTV(var, fmt) \
	do { } while (0)

#define DEBUG_PRINTA(var) \
	do { } while (0)

#define DEBUG_DIE() \
	do { exit(-1); } while (0)

#define DEBUG_PRINT_AND_DIE(fmt) \
	do { exit(-1); } while (0)

#define DEBUG_PRINTF_AND_DIE(fmt, ...) \
	do { (void)(__VA_ARGS__); exit(-1); } while (0)

#define DEBUG_BENCHMARK(expression) \
	do { } while (0)

#define DEBUG_BEGIN_BENCHMARK() \
	do { } while (0)

#define DEBUG_END_BENCHMARK() \
	do { } while (0)

#define DEBUG_ASSERT(expr) \
	do { } while (0)

#endif

#endif // DEBUG_H_INCLUDED
