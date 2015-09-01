#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

int main(void)
{

    SSL_SESSION *x = NULL;

    X509 *peer = NULL;

    int ret = 1, i, num, badops = 0;

    BIO *out = NULL;
    BIO *in = NULL;
    BIO *bio_err = NULL;

    int informat, outformat;

    char *infile = NULL, *outfile = NULL, *context = NULL;

    int cert = 0, noout = 0, text = 0;

    const char **pp;

    ERR_load_crypto_strings ();

    if (bio_err == NULL)
        if ((bio_err = BIO_new (BIO_s_file ())) != NULL)
            BIO_set_fp (bio_err, stderr, BIO_NOCLOSE | BIO_FP_TEXT);

    out = BIO_new (BIO_s_file ());
    if (out == NULL)
    {
        ERR_print_errors (bio_err);
        goto end;
    }
    BIO_set_fp (out, stdout, BIO_NOCLOSE);

    in = BIO_new (BIO_s_file ());
    if (in == NULL)
    {
        ERR_print_errors (bio_err);
        goto end;
    }

    if (BIO_read_filename (in, "./sslsession.pem") <= 0)
    {
        perror (infile);
        goto end;
    }

    x = PEM_read_bio_SSL_SESSION (in, NULL, NULL, NULL);
    if (x == NULL)
    {
        BIO_printf (bio_err, "unable to load SSL_SESSION\n");
        ERR_print_errors (bio_err);
        goto end;
    }
    else
    {
        SSL_SESSION_print (out, x);
    }

    peer = SSL_SESSION_get0_peer (x);
    if (peer == NULL)
        BIO_puts (out, "No certificate present\n");
    else
        X509_print (out, peer);

    i = PEM_write_bio_SSL_SESSION (out, x);
    if (!i)
    {
        BIO_printf (bio_err, "unable to write SSL_SESSION\n");
        goto end;
    }

    i = PEM_write_bio_X509 (out, peer);
    if (!i)
    {
        BIO_printf (bio_err, "unable to write X509\n");
        goto end;
    }

    /* just testing for memory leaks :-) */
    {
        SSL_SESSION *s;

        char buf[1024 * 10], *p;

        int i;

        s = SSL_SESSION_new ();

        p = &buf;
        i = i2d_SSL_SESSION (x, &p);
        p = &buf;
        d2i_SSL_SESSION (&s, &p, (long) i);
        p = &buf;
        d2i_SSL_SESSION (&s, &p, (long) i);
        p = &buf;
        d2i_SSL_SESSION (&s, &p, (long) i);
        SSL_SESSION_free (s);
    }
end:
    if (in != NULL)
        BIO_free (in);
    if (out != NULL)
        BIO_free_all (out);
    if (bio_err != NULL)
        BIO_free_all (bio_err);
    if (x != NULL)
        SSL_SESSION_free (x);
    if (peer != NULL)
        X509_free (peer);

    return 0;
}

