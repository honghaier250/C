#include <stdio.h>
#include <strings.h>

int main()
{
    char *engine="on";

    if(strcasecmp(engine, "ON") == 0)
    {
        printf("hello world\n");
    }

    return 0;
}
