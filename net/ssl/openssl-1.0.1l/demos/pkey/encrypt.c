#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/err.h>

int do_decrypt(char *out, size_t *outlen, char *in, size_t *inlen)
{
    BIO *keyin, *errout;
    RSA *rsa=NULL;
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *pkey_ctx = NULL;

    OpenSSL_add_all_algorithms ();
    ERR_load_crypto_strings ();

    errout = BIO_new (BIO_s_file ());
    BIO_set_fp (errout, stdout, BIO_NOCLOSE);

    // 读取证书
    keyin = BIO_new(BIO_s_file());
    BIO_read_filename(keyin, "./prikey.pem");
    rsa = PEM_read_bio_RSAPrivateKey(keyin, &rsa, NULL, "123456");
    if(NULL == rsa)
    {
        unsigned long err;

        while ((err = ERR_peek_error ()) != 0)
        {
            BIO_printf (errout, "PEM_read_bio_RSAPrivateKey: %s\n", ERR_reason_error_string (err));
            ERR_get_error ();    /* remove e from error stack */
        }

    }

    pkey = EVP_PKEY_new();
    if(NULL == pkey)
    {
        BIO_puts(errout,"EVP_PKEY_new failed\n");
        goto err;
    }

    EVP_PKEY_assign_RSA(pkey, rsa);

    pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (pkey_ctx == NULL)
    {
        BIO_printf (errout, "%s\n", "EVP_PKEY_CTX_new failed");
        ERR_print_errors (errout);
    }

    if(EVP_PKEY_decrypt_init(pkey_ctx) <= 0)
    {
        BIO_puts(errout,"EVP_PKEY_decrypt_init failed\n");
        goto err;
    }

    if(EVP_PKEY_decrypt(pkey_ctx, out, outlen, in, *inlen) <= 0)
    {
        BIO_puts(errout,"EVP_PKEY_decrypt failed\n");
        goto err;
    }
    else
    {
        BIO_puts(errout,"Prikey decrypt successed\n");
    }

err:

    if(NULL != rsa)
        RSA_free(rsa);

    if(NULL != pkey)
        EVP_PKEY_free(pkey);

    if(NULL != pkey_ctx)
        //EVP_PKEY_CTX_free(pkey_ctx);

    if(NULL != keyin)
        BIO_free(keyin);

    if(NULL!= errout)
        BIO_free(errout);

    return 0;
}

int do_encrypt(char *out, size_t *outlen, char *in, size_t *inlen)
{
    BIO *cerin, *errout;
    X509 *cert=NULL;
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *pkey_ctx = NULL;

    OpenSSL_add_all_algorithms ();
    ERR_load_crypto_strings ();

    errout = BIO_new (BIO_s_file ());
    BIO_set_fp (errout, stdout, BIO_NOCLOSE);

    // 读取证书
    cerin = BIO_new(BIO_s_file());
    BIO_read_filename(cerin, "./certself.pem");
    cert = PEM_read_bio_X509 (cerin, NULL, 0, NULL);
    if(NULL == cert)
    {
        unsigned long err;

        while ((err = ERR_peek_error ()) != 0)
        {
            BIO_printf (errout, "PEM_read_bio_X509: %s\n", ERR_reason_error_string (err));
            ERR_get_error ();    /* remove e from error stack */
        }

    }

    //取得证书公钥
    pkey = X509_get_pubkey (cert);
    if (pkey == NULL)
    {
        BIO_printf (errout, "%s\n", "X509_get_pubkey failed");
        ERR_print_errors (errout);
    }

    pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (pkey_ctx == NULL)
    {
        BIO_printf (errout, "%s\n", "EVP_PKEY_CTX_new failed");
        ERR_print_errors (errout);
    }

    if(EVP_PKEY_encrypt_init(pkey_ctx) <= 0)
    {
        BIO_puts(errout,"EVP_PKEY_encrypt_init failed\n");
        goto err;
    }

    if(EVP_PKEY_encrypt(pkey_ctx, out, outlen, in, *inlen) < 0)
    {
        BIO_puts(errout,"EVP_PKEY_encrypt failed\n");
        goto err;
    }
    else
    {
        BIO_puts(errout,"pubkey encrypt successed\n");
    }

err:

    if(NULL != cert)
        X509_free(cert);

    if(NULL != pkey)
        EVP_PKEY_free(pkey);

    if(NULL != pkey_ctx)
        EVP_PKEY_CTX_free(pkey_ctx);

    if(NULL != cerin)
        BIO_free(cerin);

    if(NULL!= errout)
        BIO_free(errout);

    return 0;
}

int main(void)
{
    char out[1024],in[1024]="abcd";
    size_t outlen=1024;
    size_t inlen=strlen(in);

    do_encrypt(out,&outlen,in,&inlen);
    puts(in);

    inlen = 1024;
    memset(in,0,1024);

    do_decrypt(in,&inlen,out,&outlen);
    puts(in);

    return 0;
}
