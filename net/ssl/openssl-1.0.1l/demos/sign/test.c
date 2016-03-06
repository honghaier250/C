#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

const char intext[] = "Some Sign Text";

int do_sign_verify()
{
    int ret = 0;
    unsigned int siglen = 1024;
    unsigned char sigbuf[1024];
    const char certfile[] = "cert.pem";
    const char keyfile[] = "key.pem";
    EVP_PKEY *pkey = NULL;
    EVP_MD_CTX md_ctx;
    X509 *x509 = NULL;
    FILE *fp = NULL;

    fp = fopen(keyfile, "r");
    if (NULL == fp)
    {
        fprintf(stderr, "fopen keyfile failed !\n");
        return -1;
    }

    pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
    if (NULL == pkey)
    {
        fprintf(stderr, "PEM_read_PrivateKey Failed !\n");
        fclose(fp);
        return -1;
    }

    ret = EVP_SignInit(&md_ctx, EVP_sha1());
    if (0 == ret)
    {
        fprintf(stderr, "EVP_SignInit Failed !\n");
        goto err;
    }

    ret = EVP_SignUpdate(&md_ctx, intext, strlen(intext));
    if (0 == ret)
    {
        fprintf(stderr, "EVP_SignUpdate Failed !\n");
        goto err;
    }

    ret = EVP_SignFinal(&md_ctx, sigbuf, &siglen, pkey);
    if (0 == ret)
    {
        fprintf(stderr, "EVP_SignFinal Failed !\n");
        goto err;
    }

    fp = fopen(certfile, "r");
    if (NULL == fp)
    {
        fprintf(stderr, "fopen certfile failed !\n");
        return -1;
    }

    x509 = PEM_read_X509(fp, NULL, NULL, NULL);
    if (NULL == x509)
    {
        fprintf(stderr, "PEM_read_X509 Failed !\n");
        return -1;
    }

    pkey = X509_get_pubkey(x509);
    if (NULL == pkey)
    {
        fprintf(stderr, "X509_get_pubkey Failed !\n");
        return -1;
    }

    ret = EVP_VerifyInit(&md_ctx, EVP_sha1());
    if (0 == ret)
    {
        fprintf(stderr, "EVP_VerifyInit Failed !\n");
        goto err;
    }

    ret = EVP_VerifyUpdate(&md_ctx, intext, strlen(intext));
    if (0 == ret)
    {
        fprintf(stderr, "EVP_VerifyUpdate Failed !\n");
        goto err;
    }

    ret = EVP_VerifyFinal(&md_ctx, sigbuf, siglen, pkey);
    if (0 == ret)
    {
        fprintf(stderr, "EVP_VerifyFinal Failed !\n");
        goto err;
    }

    fprintf(stdout, "Signaturn Verify OK.\n");
    return 0;

err:
    EVP_PKEY_free(pkey);
    return -1;
}

int main()
{
    int ret = 0;

    ret = do_sign_verify();
    if (0 > ret)
    {
        fprintf(stderr, "Something Wrong !\n");
        return -1;
    }

    return 0;
}
