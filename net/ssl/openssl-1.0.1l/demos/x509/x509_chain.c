#include <stdio.h>
#include <stdlib.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/err.h>

STACK_OF(X509) * load_certificate_chain(STACK_OF(X509) * certificate_chain)
{
    BIO *in, *errout;
    X509 *cert = NULL;
    X509_NAME *name = NULL;
    int i;
    FILE *fp;

    errout = BIO_new(BIO_s_file());
    BIO_set_fp(errout, stdout, BIO_NOCLOSE);

    // 读取证书
    in = BIO_new(BIO_s_file());
    BIO_read_filename(in, "./cacert.pem");
    cert = PEM_read_bio_X509(in, NULL, 0, NULL);
    if (NULL == cert)
    {
        unsigned long err;

        while ((err = ERR_peek_error()) != 0)
        {
            BIO_printf(errout, "PEM_read_bio_X509: %s\n", ERR_reason_error_string(err));
            ERR_get_error();    /* remove e from error stack */
        }

    }

    //证书数组的操作
    certificate_chain = sk_X509_new_null();
    if (!certificate_chain)
        exit(1);

    if (!sk_X509_push(certificate_chain, cert))
    {
        sk_X509_pop_free(certificate_chain, X509_free);
        exit(1);
    }

    return certificate_chain;
}

X509 *load_leaf_certificate_chain(X509 * certificate)
{
    FILE *fp;
    X509 *leaf_certificate = certificate;

    fp = fopen("ec_clientcert.pem", "rb");
    leaf_certificate = PEM_read_X509(fp, NULL, 0, NULL);

    return leaf_certificate;
}

int main(int argc, char *argv[])
{
    BIO *in = NULL, *errout = NULL, *tmp = NULL;
    X509 *cert = NULL, *leaf_certificate = NULL;
    STACK_OF(X509) * certificate_chain = NULL;
    X509_STORE *store = NULL;
    X509_STORE_CTX ctx;

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    //设置验证标记 都验证那些项 X509_V_FLAG_CRL_CHECK_ALL表示全部验证
    //X509_STORE_set_flags(store,X509_V_FLAG_CRL_CHECK_ALL);
    if (!X509_STORE_CTX_init(&ctx, store, load_leaf_certificate_chain(leaf_certificate), NULL))
    {

    }

//    if(NULL != certificate_chain)
    {
        X509_STORE_CTX_trusted_stack(&ctx, load_certificate_chain(certificate_chain));
    }

    if (1 == X509_verify_cert(&ctx))
    {
        puts("OK");
    }

    X509_free(cert);
    X509_free(leaf_certificate);
    sk_X509_free(certificate_chain);
    X509_STORE_CTX_cleanup(&ctx);
    if (store)
        X509_STORE_free(store);
    //X509_NAME_free(name);
    BIO_free(in);
    BIO_free(tmp);
    BIO_free(errout);

    return 0;
}
