#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

char *strxmov(char *dst, char *src, ...)
{
    va_list arg;
    char *p = dst;

    if(NULL != p)
    {
        va_start(arg, src);
        while(NULL != src)
        {
            while((*p++ = *src++));
            p = p-1;
            src = va_arg(arg, char*);
        }

        va_end(arg);
    }
    else
        return NULL;

    *p = 0;
    return dst;
}

int main()
{
    char *src = "hello";
    char dst[25];

    printf("dst = %s\n", strxmov(dst, src, "world", "world", NULL));

    return 0;
}
