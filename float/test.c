#include <stdio.h>

int convert(const char *src)
{
    int dst = 0;

    while(*src)
    {
        dst = dst*2 + (*src++) - '0';
    }

    return dst;
}

int main()
{
    char src[10];
    char *p = src;

    fscanf(stdin, "%s", src);

    printf("%d\n", convert(p));

    return 0;
}
