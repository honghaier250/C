#include <stdio.h>
#include <stdlib.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/err.h>

int main(int argc, char *argv[])
{
    BIO *in, *errout, *tmp;
    X509 *cert=NULL, *cert2=NULL;
    X509_NAME *name = NULL;
    STACK_OF (X509) * certs = NULL;
    int i;
    FILE *fp;

    OpenSSL_add_all_algorithms ();
    ERR_load_crypto_strings ();

    errout = BIO_new (BIO_s_file ());
    BIO_set_fp (errout, stdout, BIO_NOCLOSE);

    // 读取证书
    in = BIO_new(BIO_s_file());
    BIO_read_filename(in, "./certself.pem");
    cert = PEM_read_bio_X509 (in, NULL, 0, NULL);
    if(NULL == cert)
    {
        unsigned long err;

        while ((err = ERR_peek_error ()) != 0)
        {
            BIO_printf (errout, "PEM_read_bio_X509: %s\n", ERR_reason_error_string (err));
            ERR_get_error ();    /* remove e from error stack */
        }

    }

    fp = fopen("ec_clientcert.pem", "rb");
    cert2 = PEM_read_X509 (fp, NULL, 0, NULL);

    //证书数组的操作
    certs = sk_X509_new_null ();
    if (!certs)
        exit(1);

    if (!sk_X509_push (certs, cert))
    {
        sk_X509_pop_free (certs, X509_free);
        exit(1);
    }

    if (!sk_X509_push (certs, cert2))
    {
        sk_X509_pop_free (certs, X509_free);
        exit(1);
    }

    tmp = BIO_new_file ("test.pem", "w");
    for (i = 0; i < sk_X509_num (certs); i++)
    {
        PEM_write_bio_X509 (tmp, sk_X509_value (certs, i));
    }

    X509_free(cert);
    X509_free(cert2);
    sk_X509_free (certs);
    //X509_NAME_free(name);
    BIO_free(in);
    BIO_free (tmp);
    BIO_free(errout);

    return 0;
}
