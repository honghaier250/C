#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

char intext[] = "Some Crypto Text";

static int do_digest (const char *digest)
{
    const EVP_MD *d;
    EVP_MD_CTX ctx;
    unsigned char md[64];
    unsigned int mdn,i;

    d = EVP_get_digestbyname (digest);
    if (!d)
        return 0;

    printf("Plain text : %s\n", intext);
    EVP_MD_CTX_init (&ctx);
    if (!EVP_DigestInit_ex (&ctx, d, NULL))
    {
        fprintf (stderr, "DigestInit failed\n");
        exit(1);
    }
    if (!EVP_DigestUpdate (&ctx, intext, strlen(intext)))
    {
        fprintf (stderr, "DigestUpdate failed\n");
        exit(1);
    }
    if (!EVP_DigestFinal_ex (&ctx, md, &mdn))
    {
        fprintf (stderr, "DigestFinal failed\n");
        exit(1);
    }

    printf("Digest : ");
    for(i=0; i<mdn; i++)
    {
        printf("%d", *(md+i));
    }
    printf("\n");

    EVP_MD_CTX_cleanup (&ctx);

    return 1;
}

int main()
{
    char *digest="SHA1";

    OpenSSL_add_all_digests ();
    do_digest(digest);

    return 0;
}
