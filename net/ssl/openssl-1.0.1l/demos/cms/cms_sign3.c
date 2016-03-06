#include <stdio.h>
/* S/MIME signing example: 2 signers */
#include <openssl/pem.h>
#include <openssl/cms.h>
#include <openssl/err.h>

int main(int argc, char **argv)
{
    BIO *in = NULL, *out = NULL, *tbio = NULL, *errout = NULL;

    X509 *scert = NULL, *scert2 = NULL;

    EVP_PKEY *skey = NULL, *skey2 = NULL;

    CMS_ContentInfo *cms = NULL;

    FILE *fp;

    int ret = 1;

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    errout = BIO_new(BIO_s_file());
    BIO_set_fp(errout, stdout, BIO_NOCLOSE);

    fp = fopen("ec_clientcert.pem", "rb");

    if (!fp)
        goto err;

    scert = PEM_read_X509(fp, NULL, 0, NULL);
    if (NULL == scert)
    {
        unsigned long err;

        while ((err = ERR_peek_error()) != 0)
        {
            BIO_printf(errout, "PEM_read_X509: %s\n", ERR_reason_error_string(err));
            ERR_get_error();    /* remove e from error stack */
        }

        fclose(fp);
        goto err;

    }

    fclose(fp);

    fp = fopen("./ec_clientkey.pem", "rb");

    skey = PEM_read_PrivateKey(fp, NULL, 0, NULL);
    if (NULL == skey)
    {
        unsigned long err;

        while ((err = ERR_peek_error()) != 0)
        {
            BIO_printf(errout, "PEM_read_PrivateKey: %s\n", ERR_reason_error_string(err));
            ERR_get_error();    /* remove e from error stack */
        }

        fclose(fp);
        goto err;

    }

    fclose(fp);

    fp = fopen("ec_servercert.pem", "rb");

    if (!fp)
        goto err;

    scert2 = PEM_read_X509(fp, NULL, 0, NULL);

    fclose(fp);

    fp = fopen("./ec_serverkey.pem", "rb");

    if (!fp)
        goto err;
    skey2 = PEM_read_PrivateKey(fp, NULL, 0, NULL);

    fclose(fp);

    if (!scert2 || !skey2)
        goto err;

    in = BIO_new_file("sign.txt", "r");

    if (!in)
        goto err;

    cms = CMS_sign(NULL, NULL, NULL, in, CMS_STREAM | CMS_PARTIAL);

    if (!cms)
        goto err;

    /* Add each signer in turn */

    if (!CMS_add1_signer(cms, scert, skey, NULL, 0))
        goto err;

    if (!CMS_add1_signer(cms, scert2, skey2, NULL, 0))
        goto err;

    out = BIO_new_file("smout.txt", "w");
    if (!out)
        goto err;

    /* NB: content included and finalized by SMIME_write_CMS */

    if (!SMIME_write_CMS(out, cms, in, CMS_STREAM))
        goto err;

    ret = 0;

err:

    if (ret)
    {
        fprintf(stderr, "Error Signing Data\n");
        ERR_print_errors_fp(stderr);
    }

    if (cms)
        CMS_ContentInfo_free(cms);

    if (scert)
        X509_free(scert);
    if (skey)
        EVP_PKEY_free(skey);

    if (scert2)
        X509_free(scert2);
    if (skey)
        EVP_PKEY_free(skey2);

    if (in)
        BIO_free(in);
    if (out)
        BIO_free(out);
    if (tbio)
        BIO_free(tbio);
    if (errout)
        BIO_free(errout);

    return ret;

}
