#include <stdio.h>
#include <pthread.h>

void
*thread_start(void *arg)
{
	puts(__func__);
	while (1)
		/* nothing */;
}

int
main(void)
{
	pthread_t tid;

	pthread_create(&tid, NULL, thread_start, NULL);
	puts(__func__);

	while (1)
		/* nothing */;

	return (0);
}
