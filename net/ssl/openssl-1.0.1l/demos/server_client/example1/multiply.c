#include <stdio.h>
int fast_multiply(x, y)
{
    return x * y;
}

int slow_multiply(x, y)
{
    int i, j, z;

    for (i = 0, z = 0; i < x; i++)
        z = z + y;
    return z;
}

int main(int argc, char *argv[])
{
    int i, j;

    int x, y;

    for (i = 0; i < 200; i++)
    {
        for (j = 0; j < 30; j++)
        {
            x = fast_multiply(i, j);
            y = slow_multiply(i, j);
        }
    }
    printf("x=%d, y=%d\n", x, y);
    return 0;
}
