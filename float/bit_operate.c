#include <stdio.h>

int main()
{
    unsigned int a = 0xABCD;
    unsigned int b = 0;
    unsigned int c = 0;

    b = (a>>8) & 0xff;
    c = (a & 0xff)<< 8;

    printf("0x%X\n", c|b);


    return 0;
}
