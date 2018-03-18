#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int
func(void)
{
	puts("modify me!");
}

int
modify(void *fn, char *data, size_t len)
{
	if (!fn)
		return (-1);

	int f = open("/proc/self/mem", O_RDWR);
	lseek(f, (off_t)fn, SEEK_SET);
	write(f, data, len);
	close(f);
	
	return (0);
}

int
main(void)
{
	int ret;

	puts("calling func()");
	ret = func();
	printf("returned %d\n\n", ret);

	puts("calling modify()\n");
	modify(&func, "\xB8\x42\x00\x00\x00\xC3", 6);

	puts("calling func()");
	ret = func();
	printf("returned %d\n", ret);
}
