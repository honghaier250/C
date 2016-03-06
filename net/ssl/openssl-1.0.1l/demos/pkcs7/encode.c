#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

const char *key_file = "client-1.key";
const char *cert_file = "client-1.pem";
const char *out_file = "encode.p7b";

int main()
{
    int i = 0;
    BIO *p7bio = NULL;
    BIO *out_bio = NULL;
    BIO *err_bio = NULL;
    PKCS7 *p7 = NULL;
    ENGINE *engine = NULL;
    EVP_PKEY *pkey = NULL;
    X509 *x509 = NULL;
    STACK_OF(X509) * certs = NULL;
    PKCS7_SIGNER_INFO *info = NULL;

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

    // load GOST private key
    pkey = load_key(key_file);
    if (!pkey)
    {
        BIO_printf(err_bio, "failed load private key from %s\n", key_file);
        ERR_print_errors(err_bio);
        goto err;
    }

    // load GOST certificate (x509)
    x509 = load_certificate(cert_file);
    if (!x509)
    {
        BIO_printf(err_bio, "failed load certificate from %s\n", cert_file);
        ERR_print_errors(err_bio);
        goto err;
    }

    /* Create a new PKCS7 signed object. */
    p7 = PKCS7_new();
    if (NULL == p7)
    {
        fprintf(stderr, "PKCS7_new Failed\n");
    }

    //设置类型为NID_pkcs7_signed
    if (!PKCS7_set_type(p7, NID_pkcs7_signed))
    {
        fprintf(stderr, "PKCS7_set_type Failed !\n");
    }

    PKCS7_content_new(p7, NID_pkcs7_data);

    PKCS7_set_detached(p7, 0);

    //添加签名者信息，
    //x509：签名证书，pkey：签名者私钥。EVP_sha1()签名者摘要算法。
    info = PKCS7_add_signature(p7, x509, pkey, EVP_sha1());
    if (NULL == info)
    {
        fprintf(stderr, "PKCS7_add_signature Failed !\n");
    }

    certs = sk_X509_new_null();
    sk_X509_push(certs, x509);

    /* Add signer certificate and optional certificate chain. */
    PKCS7_add_certificate(p7, x509);

    //添加签名者的CA证书链
    for (i = 0; i < sk_X509_num(certs); i++)
    {
        PKCS7_add_certificate(p7, sk_X509_value(certs, i));
    }

    /* Add the DER encoded tst_info to the PKCS7 structure. */
    p7bio = PKCS7_dataInit(p7, NULL);
    if (NULL == p7bio)
    {
        fprintf(stderr, "PKCS7_dataInit Failed !\n");
    }

    //加入原始数据
    BIO_write(p7bio, "How are you !", strlen("How are you !"));

    BIO_flush(p7bio);
    /* Create the signature and add it to the signer info. */
    PKCS7_dataFinal(p7, p7bio);

    out_bio = BIO_new_file(out_file, "wb");
    //转换为der编码输出
    i2d_PKCS7_bio(out_bio, p7);

    fprintf(stdout, "PkCS7 Signed Encode OK !\n");

err:
    if (p7)
        PKCS7_free(p7);
    if (p7bio)
        BIO_free_all(p7bio);
    if (out_bio)
        BIO_free(out_bio);
    if (err_bio)
        BIO_free(err_bio);
    if (engine)
        ENGINE_free(engine);
    if (pkey)
        EVP_PKEY_free(pkey);
    if (certs)
        sk_X509_pop_free(certs, X509_free);

    return 0;
}
