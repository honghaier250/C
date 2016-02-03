#include <stdio.h>
#include <string.h>

char *strnshift(char *src, int n)
{
    int i;
    char *p = src;
    char first;
    int len = strlen(src);

    while(n--)
    {
        first = *p;
        for(i=0; i<len; i++)
        {
            *(p+i) = *(p+i+1);
        }

        *(p+i-1) = first;
    }

    return src;
}

int main()
{
    char src[] = "hello";

    printf("%s\n", strnshift(src, 3));

    return 0;
}
