#include <stdio.h>
#include <stdlib.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/err.h>

int main(int argc, char *argv[])
{
    BIO *in, *errout;
    X509 *cert=NULL;
    X509_NAME *name = NULL;
    long l;
    ASN1_INTEGER *bs;
    EVP_PKEY *pkey = NULL;

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

    //输出证书内容
    BIO_printf(errout, "PEM_read_bio_X509 OK\n");
    if (cert)
    {
        X509_print(errout, cert);
    }
    else
    {
        BIO_puts(errout, "No certificate present\n");
    }

    //输出证书版本号
    l=X509_get_version (cert);
    if(BIO_printf (errout, "\nCertificate Version: %lu (0x%lx)", l + 1, l)<=0)
    {

    }
    //输出证书序列号
    BIO_puts(errout, "\nCertificate Serial Number:");
    l=ASN1_INTEGER_get(X509_get_serialNumber(cert));
    if (BIO_printf (errout, " %lu (0x%lx)\n", l, l) <= 0)
    {

    }
    //输出证书颁发者项
    name = X509_get_issuer_name (cert);
    if(NULL != name)
    {
        BIO_puts(errout, "\nCertificate Issuer:");
        X509_NAME_print(errout, name, 0);
    }

    //输出证书有效期
    BIO_write (errout, "\n            Not Before: ", 24);
    ASN1_TIME_print (errout, X509_get_notBefore (cert));

    BIO_write (errout, "\n            Not After : ", 25);
    ASN1_TIME_print (errout, X509_get_notAfter (cert));


    //输出证书主题项
    name = X509_get_subject_name (cert);
    if(NULL != name)
    {
        BIO_puts(errout, "\nCertificate Subject:");
        X509_NAME_print(errout, name, 0);
    }

    //输出证书公钥
    pkey = X509_get_pubkey (cert);
    if (pkey == NULL)
    {
        BIO_printf (errout, "%12sUnable to load Public Key\n", "");
        ERR_print_errors (errout);
    }
    else
    {
        BIO_puts(errout,"\n");
        EVP_PKEY_print_public (errout, pkey, 16, NULL);
        EVP_PKEY_free (pkey);
    }

    //输出证书扩展项
    if(X509_get_ext_count (cert) > 0)
    {
         BIO_printf(errout,X509_get_ex_data (cert, X509_get_ext_count (cert)));
    }

    //输出证书签名算法与签名值
    X509_signature_print (errout, cert->sig_alg, cert->signature);

    X509_free(cert);
    //X509_NAME_free(name);
    BIO_free(in);
    BIO_free(errout);

    return 0;
}
