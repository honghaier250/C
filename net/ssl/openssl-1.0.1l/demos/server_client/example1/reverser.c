#include <stdio.h>
#include <string.h>

#define OK          (0)
#define ERR         (1)

#define BUF_SIZE    (0x100)

/* 交换两个变量的值 */
#define SWAP(a, b)  \
    do { \
        char x; \
        x = (a); \
        (a) = (b); \
        (b) = x; \
    } while (0)

/* 将数据逆序处理 */
int reverser(char *data, unsigned int len)
{
    int i;

    if (NULL == data)
    {
        return ERR;
    }

    for (i = 0; i < len / 2; ++i)
    {
        SWAP(*(data + i), *(data + len - i - 1));
    }

    return OK;
}

/* 主函数 */
int main(int argc, char *argv[])
{
    char str[BUF_SIZE] = { 0 };
    int ret = OK;

    int i = 0;

    strcpy(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    printf("original string : %s \r\n", str);

    for (i = 0; i < 0x800001; ++i)
    {
        ret = reverser(str, strlen(str));
    }

    printf("reversely string : %s \r\n", str);

    return 0;
}
