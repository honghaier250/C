#include <stdlib.h>
#include <string.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

/************************************************************************
 * RSA PEM/BIGNUM公钥转换函数
 *
 * file: test_rsa_pubkey.c
 * gcc -Wall -O2 -o test_rsa_pubkey test_rsa_pubkey.c -lcrypto -lssl
 *
 * author: tonglulin@gmail.com by www.qmailer.net
 ************************************************************************/

const char *n = "C7301B330C4E123E4FA9F54F49121E8CE07974D8BFEF1D39EC9245D573D66E7FAC258F86E2B0816C6BA875F10673E655E6A8DF48DEFDDB655E253ED5A4A0FBAD50D68E91D0459F9F2377BB8CA1583E3F83C06343A5A1177C903F498A6D14015CC975522BE4446CD1EB87E88EF05A863AF0DD7C4D413CF603EDF4893EEC063BE3";

const char *pubkey = "-----BEGIN RSA PUBLIC KEY-----\nMIGJAoGBAMcwGzMMThI+T6n1T0kSHozgeXTYv+8dOeySRdVz1m5/rCWPhuKwgWxr\nqHXxBnPmVeao30je/dtlXiU+1aSg+61Q1o6R0EWfnyN3u4yhWD4/g8BjQ6WhF3yQ\nP0mKbRQBXMl1UivkRGzR64fojvBahjrw3XxNQTz2A+30iT7sBjvjAgMBAAE=\n-----END RSA PUBLIC KEY-----";

int main(int argc, char *argv[])
{
    RSA    *rsa = NULL;
    BIO    *bio = NULL;
    BIGNUM *bne = NULL;
    BIGNUM *bnn = NULL;
    FILE *fp = NULL;
    unsigned long e = 65537;

    if (argc < 2)
    {
        printf("%s pem|bignum args\n", argv[0]);
        return -1;
    }

    /* 将PEM转换为大数字符串 */
    if (strcasecmp(argv[1], "bignum") == 0)
    {
        if (argc == 3)
        {
            /* 从外部文件读 */
            fp = fopen(argv[2], "r");
            if (fp == NULL)
            {
                return -1;
            }

            rsa = PEM_read_RSAPublicKey(fp, &rsa, NULL, NULL);
            if (rsa == NULL)
            {
                ERR_print_errors_fp(stdout);
                return -1;
            }
        }
        else
        {
            /* 从内存数据读 */
            bio = BIO_new(BIO_s_mem());
            BIO_puts(bio, pubkey);

            rsa = PEM_read_bio_RSAPublicKey(bio, &rsa, NULL, NULL);
            if (rsa == NULL)
            {
                ERR_print_errors_fp(stdout);
                return -1;
            }
        }

        RSA_print_fp(stdout, rsa, 0);

        //将大数转化为16进制字符串
        printf("BN_bn2hex(rsa->n):\n%s\n", BN_bn2hex(rsa->n));
        printf("BN_bn2hex(rsa->e):\n%s\n", BN_bn2hex(rsa->e));
        //将大数转化为10进制字符串
        printf("BN_bn2dec(rsa->n):\n%s\n", BN_bn2dec(rsa->n));
        printf("BN_bn2dec(rsa->e):\n%s\n", BN_bn2dec(rsa->e));

        //将大数转化为2进制字符串,to的空间必须大于BN_num_bytes(a)
        {
            char to[1024];
            if(1024>BN_num_bytes(rsa->n))
            {
                BN_bn2bin(rsa->n,to);
                printf("BN_bn2bin[rsa->n]:\n%s\n", to);
            }

            if(1024>BN_num_bytes(rsa->e))
            {
                BN_bn2bin(rsa->e,to);
                printf("BN_bn2bin[rsa->e]:\n%s\n", to);
            }
        }

        if (argc == 3)
        {
            fclose(fp);
        }
        else
        {
            BIO_free(bio);
        }
        RSA_free(rsa);
    }
    /* 将大数字符串转换为PEM文件 */
    else if (strcasecmp(argv[1], "pem") == 0)
    {

        bne = BN_new();
        if (bne == NULL)
        {
            return -1;
        }

        bnn = BN_new();
        if (bnn == NULL)
        {
            BN_free(bne);
            return -1;
        }

        rsa = RSA_new();
        if (rsa == NULL)
        {
            BN_free(bnn);
            BN_free(bne);
            return -1;
        }

        rsa->e = bne;
        rsa->n = bnn;

        /* 设置模数 */
        BN_set_word(bne, e);
        if (argc == 3)
        {
            //将16进制字符串转化为大数
            BN_hex2bn(&bnn, argv[2]);
            printf("BN_hex2bn:\n%ld\n",BN_get_word(bnn));

            BN_dec2bn(&bnn, argv[2]);
            printf("BN_dec2bn:\n%ld\n",BN_get_word(bnn));

            BN_bin2bn(n, strlen(n), bnn);
            printf("BN_bin2bn:\n%ld\n",BN_get_word(bnn));
        }
        else
        {
            BN_hex2bn(&bnn, n);
            printf("BN_hex2bn:\n%lld\n",BN_get_word(bnn));

            BN_dec2bn(&bnn, n);
            printf("BN_dec2bn:\n%ld\n",BN_get_word(bnn));

            BN_bin2bn(n, strlen(n), bnn);
            printf("BN_bin2bn:\n%ld\n",BN_get_word(bnn));
        }

        PEM_write_RSAPublicKey(stdout, rsa);

        RSA_free(rsa);
    }
    else
    {
        return -1;
    }

    return 0;
}

