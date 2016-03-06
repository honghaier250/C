#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/asn1.h>

int main(void)
{
    int ret;
    BIO *bp;
    char buf[] = "test";

    bp = BIO_new_file("./sample.log", "wb");

    BIO_printf(bp, "%s", buf);

    BIO_free(bp);
    return 0;
}
