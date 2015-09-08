#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pthread.h"

#define MAX (10)
pthread_t thread[2];
pthread_mutex_t mut;
int number = 0, i;

void *thread1()
{
    printf("thread1\n");

    for(i=0; i < MAX; i++)
    {
        printf("thread1: number = %d \n", number);

        pthread_mutex_lock(&mut);
        number++;
        pthread_mutex_unlock(&mut);

        sleep(2);
    }

    pthread_exit(NULL);
}

void *thread2()
{
    printf("thread2\n");

    for(i=0; i < MAX; i++)
    {
        printf("thread2: number = %d \n", number);

        pthread_mutex_lock(&mut);
        number++;
        pthread_mutex_unlock(&mut);

        sleep(2);
    }

    pthread_exit(NULL);
}

void thread_create(void)
{
    int temp;
    memset(&thread, 0, sizeof(thread));

    if((temp = pthread_create(&thread[0], NULL, thread1, NULL)) != 0)
    {
        printf("create thread1 failed\n");
    }

    if((temp = pthread_create(&thread[1], NULL, thread2, NULL)) != 0)
    {
        printf("create thread2 failed\n");
    }
}

void thread_wait(void)
{
    if(thread[0] != 0)
    {
        pthread_join(thread[0], NULL);
        printf("thread1 exit\n");
    }

    if(thread[1] != 0)
    {
        pthread_join(thread[1], NULL);
        printf("thread2 exit\n");
    }
}

int main()
{
    pthread_mutex_init(&mut, NULL);

    printf("main\n");

    thread_create();

    thread_wait();

    return 0;
}
