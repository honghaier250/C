#include <stdio.h>
#include <stdlib.h>

int main()
{
    int ret = 0;
    char *key = NULL;
    char *token = NULL;
    char *psUserInfo = NULL;

    psUserInfo = (char *) malloc(512 * sizeof(char));

    ret = JzgkVerifyToken(key, token, psUserInfo);
    if (0 == ret)
    {
        fprintf(stdout, "%s\n", psUserInfo);
    }
    else
    {
        fprintf(stderr, "%s\n", "something wrong, detail in cc.log");
    }

    free(psUserInfo);

    return 0;
}
