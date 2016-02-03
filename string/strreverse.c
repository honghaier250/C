#include <stdio.h>
#include <string.h>

char *strreverse(const char * src)
{
    int i;
    char tmp;
    char *p = src;
    int len = strlen(p)%2 ? (strlen(p)+1) : strlen(p);

    for(i=0; i<len/2; i++)
    {
        tmp = *(p+i);
        *(p+i) = *(p+len-i-(strlen(src)%2 ? 2 : 1 ));
        *(p+len-i-(strlen(src)%2 ? 2 : 1 )) = tmp;
    }

    return p;
}

int main(int argc, char *argv[])
{
    if(1 > argc)
    {
        fprintf(stderr, "%s\n", "usga: strreverse string");
    }

    printf("%s\n", strreverse(argv[1]));

    return 0;
}
