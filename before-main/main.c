#include <stdio.h>

static void init(void) __attribute__((constructor));
void init(void)
{
	puts("before main()!");
}

int
main(int argc, char **argv)
{
	puts("main()!");

	return 0;
}
