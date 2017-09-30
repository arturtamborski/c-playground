#include <stdio.h>
#include <pthread.h>

void *thread_start(void *arg)
{
    puts(__func__);
    while (1)
        ;
}

int
main(void)
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, thread_start, NULL);
    puts(__func__);
    while (1)
        ;
    return (0);
}
