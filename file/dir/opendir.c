#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[])
{

    DIR *pDir = NULL;
    struct dirent *pDirEntry = NULL;

    pDir = opendir("/root");
    if(pDir == NULL)
    {
        printf("opendir failed!\n");
        return -1;
    }

    while((pDirEntry = readdir(pDir)))
    {
        printf("索引节点:%lu\t 偏移量：%lu\t 文件名长：%d\t文件类型：%d\t 文件名：%s\n",
               pDirEntry->d_ino, pDirEntry->d_off,
               pDirEntry->d_reclen,pDirEntry->d_type,pDirEntry->d_name);
    }

    closedir(pDir);
    return 0;
}
