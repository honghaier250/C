#include <stdio.h>
#include <stdlib.h>
#include "util.h"

const char *key_file = "client-1.key";
const char *cert_file = "client-1.pem";

const char *in_file = "sign.c";
const char *out_file = "test.p7b";

int main()
{
    PKCS7 *p7 = NULL;
    ENGINE *engine = NULL;
    EVP_PKEY *key = NULL;
    X509 *cert = NULL;
    int flags = 0;
    int ret = 0;

    BIO *in_bio = NULL;
    BIO *out_bio = NULL;
    BIO *err_bio = NULL;

    err_bio = BIO_new(BIO_s_file());
    if (err_bio != NULL)
        BIO_set_fp(err_bio, stderr, BIO_NOCLOSE | BIO_FP_TEXT);

    engine = load_engine("/usr/local/ssl/lib/engines/libgost.so");
    if (!engine)
    {
        BIO_printf(err_bio, "failed load engine gost\n");
        ERR_print_errors(err_bio);
        goto end;
    }

    // load GOST private key
    key = load_key(key_file);
    if (!key)
    {
        BIO_printf(err_bio, "failed load private key from %s\n", key_file);
        ERR_print_errors(err_bio);
        goto end;
    }

    // load GOST certificate (x509)
    cert = load_certificate(cert_file);
    if (!cert)
    {
        BIO_printf(err_bio, "failed load certificate from %s\n", cert_file);
        ERR_print_errors(err_bio);
        goto end;
    }

    // open input data stream
    in_bio = BIO_new_file(in_file, "rb");
    if (!in_bio)
    {
        BIO_printf(err_bio, "failed open/create file %s\n", in_file);
        ERR_print_errors(err_bio);
        goto end;
    }

    // open output data stream
    out_bio = BIO_new_file(out_file, "wb");
    if (!out_bio)
    {
        BIO_printf(err_bio, "failed open/create file %s\n", out_file);
        ERR_print_errors(err_bio);
        goto end;
    }

    flags = PKCS7_BINARY;
    flags |= PKCS7_PARTIAL;

    // sign
    p7 = PKCS7_sign(cert, key, NULL, in_bio, flags);
    if (!p7)
    {
        BIO_printf(err_bio, "failed PKCS7_sign()\n");
        ERR_print_errors(err_bio);
        goto end;
    }

#if 1
    if (!PKCS7_final(p7, in_bio, flags))
    {
        BIO_printf(err_bio, "failed PKCS7_final()\n");
        ERR_print_errors(err_bio);
        goto end;
    }
#endif

    ret = PEM_write_bio_PKCS7(out_bio, p7);
    if (ret <= 0)
    {
        BIO_printf(err_bio, "PEM_write_bio_PKCS7 return %i \n", ret);
        ERR_print_errors(err_bio);
    }

    puts("sign Ok\n");

end:
    if (key)
        EVP_PKEY_free(key);
    if (cert)
        X509_free(cert);

    if (p7)
        PKCS7_free(p7);

    if (in_bio)
        BIO_free(in_bio);
    if (out_bio)
        BIO_free_all(out_bio);

    if (engine)
        ENGINE_finish(engine);

    ENGINE_cleanup();

    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();

    return 0;
}
