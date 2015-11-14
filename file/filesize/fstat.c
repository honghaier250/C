#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
    int fd;
    struct stat statbuff;

    fd = open("/etc/passwd",O_RDONLY);
    if(fstat(fd, &statbuff) < 0)
    {
        printf("fstat failed !\n");
    }

    close(fd);

    return 0;
}
