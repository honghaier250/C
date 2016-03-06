#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

const unsigned char intext[] = "Some Encode Text";

//Base64编码

int do_base64()
{
    int ret = 0;
    int inlen = 0;
    int outlen = 0;
    unsigned char inbuf[1024];
    unsigned char outbuf[1024];
    EVP_ENCODE_CTX ctx;

    EVP_EncodeInit(&ctx);

    EVP_EncodeUpdate(&ctx, outbuf, &outlen, intext, strlen(intext));

    EVP_EncodeFinal(&ctx, outbuf, &outlen);

    fprintf(stdout, "Encode Text: %s\n", outbuf);

    EVP_DecodeInit(&ctx);

    ret = EVP_DecodeUpdate(&ctx, inbuf, &inlen, outbuf, outlen);

    ret = EVP_DecodeFinal(&ctx, inbuf, &inlen);

    fprintf(stdout, "Plain Text: %s\n", inbuf);

    return 0;
}

int main()
{
    int ret = 0;

    ret = do_base64();
    if (0 > ret)
    {
        fprintf(stderr, "Something Wrong !\n");
        return -1;
    }

    return 0;
}
