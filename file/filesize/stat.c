#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
 * lhs < rhs:  return <0
 * lhs == rhs: return 0
 * lhs > rhs:  return >0
 */
static inline int timespec_compare(const struct timespec *lhs, const struct timespec *rhs)
{
    if (lhs->tv_sec < rhs->tv_sec)
        return -1;
    if (lhs->tv_sec > rhs->tv_sec)
        return 1;
    return lhs->tv_nsec - rhs->tv_nsec;
}

int main()
{
    int ret;
    struct stat statbuff_file1;
    struct stat statbuff_file2;
    char *file1 = "/data/HRP/cfg/1/client_policy.conf";
    char *file2 = "/data/HRP/cfg/2/client_policy.conf";

    stat(file1, &statbuff_file1);
    stat(file2, &statbuff_file2);

    //循环执行，每秒一次
    while(1)
    {
        //判断文件是否更新
        ret = timespec_compare(&statbuff_file1.st_mtim, &statbuff_file2.st_mtim);
        if(ret > 0)
        {
            system("echo test");
        }

        sleep(1);
    }

    return 0;
}
