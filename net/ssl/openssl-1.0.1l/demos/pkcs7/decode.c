#include <stdio.h>
#include <stdlib.h>
#include "util.h"

const char *in_file = "encode.p7b";

int main()
{
    int i = 0;
    int ret = 0;
    int signCount;
    char buf[1024];
    BIO *p7bio = NULL;
    BIO *in_bio = NULL;
    BIO *err_bio = NULL;
    PKCS7 *p7 = NULL;
    ENGINE *engine = NULL;
    X509 *x509 = NULL;
    PKCS7_SIGNER_INFO *info = NULL;
    STACK_OF(PKCS7_SIGNER_INFO) * sk_info = NULL;

    err_bio = BIO_new(BIO_s_file());
    if (err_bio != NULL)
        BIO_set_fp(err_bio, stderr, BIO_NOCLOSE | BIO_FP_TEXT);

    engine = load_engine("/usr/local/ssl/lib/engines/libgost.so");
    if (!engine)
    {
        BIO_printf(err_bio, "failed load engine gost\n");
        ERR_print_errors(err_bio);
        goto err;
    }

    // open input data stream
    in_bio = BIO_new(BIO_s_file());
    ret = BIO_read_filename(in_bio, in_file);
    if (!ret)
    {
        BIO_printf(err_bio, "failed open/create file %s\n", in_file);
        ERR_print_errors(err_bio);
        goto err;
    }

    p7 = d2i_PKCS7_bio(in_bio, NULL);
    if (!p7)
    {
        BIO_printf(err_bio, "Bad input format for PKCS#7 file\n");
        ERR_print_errors(err_bio);
        goto err;
    }

    //解析出原始数据
    p7bio = PKCS7_dataDecode(p7, NULL, NULL, NULL);

    //从BIO中读取原始数据,将得到"How are you!"
    ret = BIO_read(p7bio, buf, 1024);

    //获得签名者信息stack
    sk_info = PKCS7_get_signer_info(p7);

    //获得签名者个数(本例只有1个)
    signCount = sk_PKCS7_SIGNER_INFO_num(sk_info);

    for (i = 0; i < signCount; i++)
    {
        //获得签名者信息
        info = sk_PKCS7_SIGNER_INFO_value(sk_info, i);

        //获得签名者证书
        x509 = PKCS7_cert_from_signer_info(p7, info);

        //验证签名
        if (PKCS7_signatureVerify(p7bio, p7, info, x509) != 1)
        {
            printf("signatureVerify Err\n");
        }

    }

    fprintf(stdout, "PkCS7 Signed Decode OK: %s\n", buf);

err:
    /* if(p7) */
    /*     PKCS7_free(p7); */
    if (p7bio)
        BIO_free_all(p7bio);
    if (in_bio)
        BIO_free(in_bio);
    if (err_bio)
        BIO_free(err_bio);
    if (engine)
        ENGINE_free(engine);
    if (sk_info)
        sk_PKCS7_SIGNER_INFO_pop_free(sk_info, PKCS7_SIGNER_INFO_free);

    return 0;
}
