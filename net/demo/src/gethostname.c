/*
 * @brief:返回本地主机的标准主机名
 *
 */
#include <stdio.h>
#include <unistd.h>

int main()
{
    char name[65];

    gethostname(name, sizeof(name));

    printf("hostname = %s\n", name);

    return 0;
}
