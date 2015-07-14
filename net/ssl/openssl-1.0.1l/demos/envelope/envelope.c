#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/des.h>

char intext[] = "Some Crypto Text";
/**
*
*信封加密解密
*参考openssl编程.chm 21.10编程实例4
*
*/
int do_envelope()
{

    unsigned long e=RSA_3;
    char *ek[2],iv[8],outbuf[1024],inbuf[1024];
    int ret,ekl[2],npubk,outlen,inlen,tmplen;
    EVP_CIPHER_CTX ctx,ctx2;
    EVP_CIPHER *type;
    EVP_PKEY *pubkey[2];
    RSA *rkey;
    BIGNUM *bne;
    int i;


    /* 生成RSA密钥*/
    bne=BN_new();
    ret=BN_set_word(bne,e);
    rkey=RSA_new();
    ret=RSA_generate_key_ex(rkey,1024,bne,NULL);

    pubkey[0]=EVP_PKEY_new();
    //初始化一个公钥为RSA的密钥对公钥
    EVP_PKEY_assign_RSA(pubkey[0],rkey);
    //对称密钥的算法
    type=EVP_des_ede3_cbc();
    //设置一个公钥加密
    npubk=1;

    //初始化信封加密上下文
    EVP_CIPHER_CTX_init(&ctx);

    ek[0]=(char *)malloc(500*sizeof(char));
    ek[1]=(char *)malloc(500*sizeof(char));
    memset(outbuf,0,1024);
    //只有一个公钥，如果为CBC算法IV会被产生
    ret=EVP_SealInit(&ctx,type,(unsigned char**)ek,ekl,iv,pubkey,1);
    if(ret!=1)
    {
        fprintf(stderr, "EVP_SealInit failed\n");
        goto err;
    }

    //加密操作
    ret=EVP_SealUpdate(&ctx,outbuf,&outlen,intext,strlen(intext));
    if(ret!=1)
    {
        fprintf(stderr, "EVP_SealUpdate failed\n");
        goto err;
    }

    //加密剩下的部分
    ret=EVP_SealFinal(&ctx,outbuf+outlen,&tmplen);
    if(ret!=1)
    {
        fprintf(stderr, "EVP_SealFinal failed\n");
        goto err;
    }

    outlen+=tmplen;
    printf("Envelope : %s\n",outbuf);


    memset(inbuf,0,1024);
    //初始化解密信封的上下文
    EVP_CIPHER_CTX_init(&ctx);

    //设置公钥
    ret=EVP_OpenInit(&ctx,EVP_des_ede3_cbc(),ek[0],ekl[0],iv,pubkey[0]);
    if(ret!=1)
    {
        fprintf(stderr, "EVP_OpenInit failed\n");
        goto err;
    }

    ret=EVP_OpenUpdate(&ctx,inbuf,&inlen,outbuf,outlen);
    if(ret!=1)
    {
        fprintf(stderr, "EVP_OpenUpdate failed\n");
        goto err;
    }

    ret=EVP_OpenFinal(&ctx,inbuf+inlen,&tmplen);
    if(ret!=1)
    {
        fprintf(stderr, "EVP_OpenFinal failed\n");
        goto err;
    }

    inlen+=tmplen;
    printf("Open envelope : %s\n",inbuf);

err:
    free(ek[0]);
    free(ek[1]);
    EVP_PKEY_free(pubkey[0]);
    BN_free(bne);
    EVP_CIPHER_CTX_cleanup(&ctx);
    return 0;
}

int main()
{
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();
    do_envelope();

    return 0;
}
