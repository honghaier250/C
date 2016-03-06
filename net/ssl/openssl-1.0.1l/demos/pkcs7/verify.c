#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

// const char *key_file = "test12.crt";
// const char *cert_file = "test12.crt";

// const char *key_file = "client-9.key";
const char *cert_file = "client-9.pem";

const char *in_file = "test.p7b";
const char *out_file = "sign.c.new";

int main()
{
    PKCS7 *p7 = NULL;
    ENGINE *engine = NULL;
    int flags = 0;
    int ret = 0;

    BIO *in_bio = NULL;
    BIO *out_bio = NULL;
    BIO *err_bio = NULL;
    /* BIO *data_bio = NULL; */

    STACK_OF(X509) * certs = NULL;
    X509_STORE *store = NULL;

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

#if 0
    X509 *cert = load_certificate(cert_file);
    if (!cert)
    {
        BIO_printf(err_bio, "failed load certificate from %s\n", cert_file);
        ERR_print_errors(err_bio);
        goto end;
    }

    certs = sk_X509_new_null();
    sk_X509_push(certs, cert);
#endif

    // open input data stream
    in_bio = BIO_new(BIO_s_file());
    ret = BIO_read_filename(in_bio, in_file);
    if (!ret)
    {
        BIO_printf(err_bio, "failed open/create file %s\n", in_file);
        ERR_print_errors(err_bio);
        goto end;
    }

    // open output data stream
#if 1
    out_bio = BIO_new_file(out_file, "wb");
#else
    out_bio = BIO_new_fp(stdout, BIO_NOCLOSE);
#endif
    if (!out_bio)
    {
        BIO_printf(err_bio, "failed open/create file %s\n", out_file);
        ERR_print_errors(err_bio);
        goto end;
    }

    // read PKCS7 data from the input stream
    p7 = PEM_read_bio_PKCS7(in_bio, NULL, NULL, NULL);
    if (!p7)
    {
        BIO_printf(err_bio, "Bad input format for PKCS#7 file\n");
        ERR_print_errors(err_bio);
        goto end;
    }

    // load default certificates
    store = setup_verify(err_bio, NULL, NULL);

    // Отключаем проверку сертификата-подписанта. Для примера она не нужна
    flags = PKCS7_NOVERIFY;
    flags |= PKCS7_BINARY;

    // Verify
    if (PKCS7_verify(p7, certs, store, NULL, out_bio, flags))
    {
        BIO_printf(err_bio, "Verification successful\n");
    }
    else
    {
        BIO_printf(err_bio, "failed PKCS7_verify()\n");
        ERR_print_errors(err_bio);
        goto end;
    }
/*
     BIO *p7bio = PKCS7_dataInit(p7,NULL);
     for (;;)
     {
          char buf[1024];
          memset(buf, 0, sizeof(buf) );
          int in_idx = BIO_read(p7bio, buf, sizeof(buf) - 1 );
          if (in_idx <= 0)
               break;
          int out_idx = BIO_write(out_bio, buf, in_idx);
          if (out_idx <= 0)
               break;
     }
     PKCS7_dataFinal(p7, out_bio);
*/
    puts("verify Ok\n");

end:
    if (p7)
        PKCS7_free(p7);
    if (certs)
        sk_X509_pop_free(certs, X509_free);
    if (store)
        X509_STORE_free(store);

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
