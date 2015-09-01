#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/**
 * 使用读取文件属性的方法得到文件的大小，效率高，稳定
 */

unsigned long get_file_size(const char *path)
{
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
        return filesize;
    }else{
        filesize = statbuff.st_size;
    }
    return filesize;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("uags: %s infile\n", argv[0]);
        exit(1);
    }

    printf("The file size : %d\n", get_file_size(argv[1]));

    return 0;
}
