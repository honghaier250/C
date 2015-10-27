/*
 *@brief: 整形转化为字符串型
 *
 */
#include <stdio.h>

int main()
{
    char string[25];
    int integer = 123456;

    sprintf(string, "%d", integer);

    printf("Before Convert: %d\n", integer);
    printf("After Convert: %s\n", string);

    return 0;
}
