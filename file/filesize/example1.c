#include <stdio.h>
#include <stdlib.h>

/**
 * 以打开文件的方法取得文件的大小，不适合大文件，会出现访问冲突，效率低
 */
unsigned long get_file_size(const char *path)
{
    unsigned long filesize = -1;
    FILE *fp;
    fp = fopen(path, "r");
    if(fp == NULL)
        return filesize;
    fseek(fp, 0L, SEEK_END);
    filesize = ftell(fp);
    fclose(fp);
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
