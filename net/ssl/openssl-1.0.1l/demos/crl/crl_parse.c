#include <stdio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/err.h>

int do_crl_parse(char *filename)
{
    int ret = 0;
    BIO *in = NULL;
    BIO *out = NULL;
    X509_CRL *crl = NULL;

    ERR_load_crypto_strings();

    in = BIO_new(BIO_s_file());
    if (NULL == in)
    {
        ERR_print_errors_fp(stderr);
        goto err;
    }

    ret = BIO_read_filename(in, filename);
    if (0 >= ret)
    {
        ERR_print_errors_fp(stderr);
        goto err;
    }

    crl = PEM_read_bio_X509_CRL(in, NULL, NULL, NULL);
    if (NULL == crl)
    {
        ERR_print_errors_fp(stderr);
        goto err;
    }

    out = BIO_new(BIO_s_file());
    if (NULL == out)
    {
        ERR_print_errors_fp(stderr);
        goto err;
    }

    BIO_set_fp(out, stdout, BIO_NOCLOSE);
    X509_CRL_print(out, crl);

    return 0;

err:
    if (in)
        BIO_free(in);

    if (out)
        BIO_free(out);

    if (crl)
        X509_CRL_free(crl);

    return -1;
}

int main()
{
    int ret = 0;
    char *crl = "crl.pem";

    OpenSSL_add_all_algorithms();
    ret = do_crl_parse(crl);
    if (0 > ret)
    {
        fprintf(stderr, "Something Wrong !\n");
        return -1;
    }

    return 0;
}
