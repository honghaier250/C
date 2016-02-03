#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <fcntl.h>
//#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd, len;
    char *ptr;

    if (argc < 2)
    {
        fprintf(stderr, "please enter a file\n");
        exit(1);
    }

    if ((fd = fopen(argv[1], O_RDWR)) < 0)
    {
        perror("open file failed\n");
        exit(1);
    }

    len = lseek(fd, 0, SEEK_END);
    ptr = mmap(NULL, len, 1, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap failed\n");
        close(fd);
        exit(1);
    }

    close(fd);
    munmap(ptr, len);

    return 0;
}
