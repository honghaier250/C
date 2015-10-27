/*
 *@brief: 字符型转化为整形
 *
 */
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *string="123456";
    int integer=0;

    integer = atoi(string);

    printf("Before Convert: %s\n", string);
    printf("After Convert: %d\n", integer);

    return 0;
}
