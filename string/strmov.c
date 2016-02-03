#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *strmov(char const *src, char *dst)
{
    char *p = dst;
    if (NULL != p)
        while ((*p++ = *src++)) ;
    else
        return NULL;

    return dst;
}

int main()
{
    char *src = "hello";
    char *dst = NULL;

    dst = (char *) malloc(strlen(src) + 1);
    printf("dst = %s\n", strmov(src, dst));

    free(dst);
    return 0;
}
