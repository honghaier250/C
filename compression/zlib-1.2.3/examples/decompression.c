#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>

int main(int argc, char* argv[])
{
    FILE* file;
    uLong flen;
    unsigned char* fbuf = NULL;
    uLong ulen;
    unsigned char* ubuf = NULL;

    /* 通过命令行参数将srcfile文件的数据解压缩后存放到dstfile文件中 */
    if(argc < 3)
    {
        printf("Usage: zudemo srcfile dstfile\n");
        return -1;
    }

    if((file = fopen(argv[1], "rb")) == NULL)
    {
        printf("Can\'t open %s!\n", argv[1]);
        return -1;
    }
    /* 装载源文件数据到缓冲区 */
    fread(&ulen, sizeof(uLong), 1, file);   /* 获取缓冲区大小 */
    fread(&flen, sizeof(uLong), 1, file);   /* 获取数据流大小 */
    if((fbuf = (unsigned char*)malloc(sizeof(unsigned char) * flen)) == NULL)
    {
        printf("No enough memory!\n");
        fclose(file);
        return -1;
    }
    fread(fbuf, sizeof(unsigned char), flen, file);
    /* 解压缩数据 */
    if((ubuf = (unsigned char*)malloc(sizeof(unsigned char) * ulen)) == NULL)
    {
        printf("No enough memory!\n");
        fclose(file);
        return -1;
    }
    if(uncompress(ubuf, &ulen, fbuf, flen) != Z_OK)
    {
        printf("Uncompress %s failed!\n", argv[1]);
        return -1;
    }
    fclose(file);

    if((file = fopen(argv[2], "wb")) == NULL)
    {
        printf("Can\'t create %s!\n", argv[2]);
        return -1;
    }
    /* 保存解压缩后的数据到目标文件 */
    fwrite(ubuf, sizeof(unsigned char), ulen, file);
    fclose(file);

    free(fbuf);
    free(ubuf);

    return 0;
}
