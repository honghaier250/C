#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>

int convert(const char *src)
{
    int sum = 0;
    int minus = 0;
    int begin = 0;

    while(*src)
    {
        while(' '== *src)
            src++;
        if((0 == begin) && (('+' == *src) || ('-' == *src)))
        {
            begin = 1;
            if(('-' == *src++))
                minus = 1;
            continue;
        }
        else if(isdigit((int)*src))
        {
            sum = sum*10 + (*(src++) - '0');
        }
        else
        {
            fprintf(stderr, "%s\n", "invaild char");
            break;
        }
    }

    return minus ? -sum : sum;
}

int main()
{
    char *src = " -1234ab";

    printf("%d\n", convert(src));
    printf("%d\n", atoi(src));

    return 0;
}
