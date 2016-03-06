#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

char intext[] = "Some Crypto Text";

int do_crypt(const char *cipher)
{
    unsigned char outbuf[1024], inbuf[1024];
    int outlen, tmplen, inlen;
    unsigned char key[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    unsigned char iv[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    const EVP_CIPHER *c;

    memset(outbuf, 0, 1024);
    memset(inbuf, 0, 1024);

    c = EVP_get_cipherbyname(cipher);
    if (!c)
        return 0;

    printf("Plain text : %s\n", intext);

    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_EncryptInit_ex(&ctx, EVP_bf_cbc(), NULL, key, iv);
    EVP_CIPHER_CTX_set_padding(&ctx, 0);
    if (!EVP_EncryptUpdate(&ctx, outbuf, &outlen, intext, strlen(intext)))
    {
        fprintf(stderr, "EncryptUpdate failed\n");
        exit(1);
    }

    if (!EVP_EncryptFinal_ex(&ctx, outbuf + outlen, &tmplen))
    {
        fprintf(stderr, "EncryptFinal failed\n");
        exit(1);
    }
    outlen += tmplen;

    printf("Cipher text : %s\n", outbuf);

    if (!EVP_DecryptInit_ex(&ctx, EVP_bf_cbc(), NULL, key, iv))
    {
        fprintf(stderr, "DecryptInit failed\n");
        exit(1);
    }

    EVP_CIPHER_CTX_set_padding(&ctx, 0);

    if (!EVP_DecryptUpdate(&ctx, inbuf, &inlen, outbuf, outlen))
    {
        fprintf(stderr, "Decrypt failed\n");
        exit(1);
    }
    if (!EVP_DecryptFinal_ex(&ctx, inbuf + inlen, &tmplen))
    {
        fprintf(stderr, "DecryptFinal failed\n");
        exit(1);
    }

    printf("Plain text : %s\n", inbuf);

    EVP_CIPHER_CTX_cleanup(&ctx);

    return 1;
}

int main()
{
    char *cipher = "DES";

    OpenSSL_add_all_ciphers();
    do_crypt(cipher);

    return 0;
}
