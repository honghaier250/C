#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char message[50] = "1745,65520,1678,65520 ";
    char *token = NULL;
    unsigned long Encrypt_time = 0;
    unsigned long Encrypt_size = 0;
    unsigned long Decrypt_time = 0;
    unsigned long Decrypt_size = 0;

    token = strtok(message, ",");
    Encrypt_time = atol(token);
    //strtol();

    if (token != NULL)
    {
        token = strtok(NULL, ",");
    }

    Encrypt_size = atol(token);

    if (token != NULL)
    {
        token = strtok(NULL, ",");
    }

    Decrypt_time = atol(token);

    if (token != NULL)
    {
        token = strtok(NULL, ",");
    }

    Decrypt_size = atol(token);
    printf("%ld,%ld,%ld,%ld", Encrypt_time, Encrypt_size, Decrypt_time, Decrypt_size);

    return 0;
}
