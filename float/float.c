#include <stdio.h>

int main()
{
    int d = 1889785610;
    double c = 5.01;
    int *p = (int*)&c;

    float a = 5.01f;
    int *q = (int *)&a;

    printf("%d\n", 5.01);
    printf("%d\n", *p);
    printf("0x%x\n", *p);
    printf("0x%x\n", *(p+1));
    printf("0x%x\n", d);

    printf("0x%x\n", 5.01f);
    printf("0x%8x\n0x%8x\n", 5.01f);
    return 0;
}
