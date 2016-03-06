#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/des.h>

const unsigned char intext[] = "Some Envelope Text";

void finish(unsigned char *ek, BIGNUM * bne, EVP_PKEY * pkey, EVP_CIPHER_CTX * ctx)
{

    if (NULL != ek)
    {
        free(ek);
    }

    if (NULL != bne)
    {
        BN_free(bne);
    }

    if (NULL != pkey)
    {
        EVP_PKEY_free(pkey);
    }

    if (NULL != ctx)
    {
        EVP_CIPHER_CTX_cleanup(ctx);
    }

}

int do_envolope()
{
    int ret = 0;
    int ekl = 0;
    int inlen = 0;
    int outlen = 0;
    int tmplen = 0;
    unsigned char *ek = NULL;
    unsigned char inbuf[1024];
    unsigned char outbuf[1024];
    unsigned char iv[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    RSA *rsakey = NULL;
    BIGNUM *bne = NULL;
    EVP_CIPHER_CTX ctx;
    EVP_PKEY *pkey = NULL;

    bne = BN_new();
    BN_set_word(bne, RSA_3);

    rsakey = RSA_new();
    RSA_generate_key_ex(rsakey, 1024, bne, NULL);

    pkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pkey, rsakey);

    memset(inbuf, 0, sizeof(inbuf));
    memset(outbuf, 0, sizeof(outbuf));
    ek = (char *) malloc(500 * sizeof(char));
    EVP_CIPHER_CTX_init(&ctx);

    ret = EVP_SealInit(&ctx, EVP_des_ede3_cbc(), &ek, &ekl, iv, &pkey, 1);
    if (1 != ret)
    {
        fprintf(stderr, "EVP_SealInit Failed !\n");
        goto err;
    }

    ret = EVP_SealUpdate(&ctx, outbuf, &outlen, intext, strlen(intext));
    if (1 != ret)
    {
        fprintf(stderr, "EVP_SealUpdate Failed !\n");
        goto err;
    }

    ret = EVP_SealFinal(&ctx, outbuf + outlen, &tmplen);
    if (1 != ret)
    {
        fprintf(stderr, "EVP_SealFinal Failed !\n");
        goto err;
    }

    outlen += tmplen;
    fprintf(stdout, "Envelope Text: %s\n", outbuf);
    EVP_CIPHER_CTX_init(&ctx);

    ret = EVP_OpenInit(&ctx, EVP_des_ede3_cbc(), ek, ekl, iv, pkey);
    if (1 != ret)
    {
        fprintf(stderr, "EVP_VerifyInit Failed !\n");
        goto err;
    }

    ret = EVP_OpenUpdate(&ctx, inbuf, &inlen, outbuf, outlen);
    if (1 != ret)
    {
        fprintf(stderr, "EVP_VerifyUpdate Failed !\n");
        goto err;
    }

    ret = EVP_OpenFinal(&ctx, inbuf + inlen, &tmplen);
    if (1 != ret)
    {
        fprintf(stderr, "EVP_VerifyFinal Failed !\n");
        goto err;
    }

    fprintf(stdout, "Plain Text: %s\n", inbuf);

    finish(ek, bne, pkey, &ctx);
    return 0;

err:

    finish(ek, bne, pkey, &ctx);
    return -1;
}

int main()
{
    int ret = 0;

    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();
    ret = do_envolope();
    if (0 > ret)
    {
        fprintf(stderr, "Something Wrong !\n");
        return -1;
    }

    return 0;
}
