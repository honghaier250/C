#include <stdio.h>        // for printf()
#include <sys/time.h>     // for gettimeofday()
#include <unistd.h>       // for sleep()

int main()
{
    struct timeval start, end;
    unsigned long spend_time;

    gettimeofday( &start, NULL );
    printf("start : %d.%d\n", start.tv_sec, start.tv_usec);
    sleep(1);
    gettimeofday( &end, NULL );
    printf("end   : %d.%d\n", end.tv_sec, end.tv_usec);

    //微秒时差
    spend_time=1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec);
    printf("%ld\n",spend_time);

    return 0;
}
