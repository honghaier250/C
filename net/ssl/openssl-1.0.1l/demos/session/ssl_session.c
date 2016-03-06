#include <stdio.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

int ssl_session_parse(const char *filename)
{
    int ret = 0;
    BIO *in = NULL;
    BIO *out = NULL;
    X509 *peer = NULL;
    SSL_SESSION *sess = NULL;

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

    out = BIO_new(BIO_s_file());
    if (NULL == out)
    {
        ERR_print_errors_fp(stderr);
        goto err;
    }

    BIO_set_fp(out, stdout, BIO_NOCLOSE);

    sess = PEM_read_bio_SSL_SESSION(in, NULL, NULL, NULL);
    if (NULL == sess)
    {
        ERR_print_errors_fp(stderr);
        goto err;
    }
    else
    {
        SSL_SESSION_print(out, sess);
    }

    peer = SSL_SESSION_get0_peer(sess);
    if (NULL == peer)
    {
        BIO_puts(out, "No Certificate !");
    }
    else
    {
        X509_print(out, peer);
    }

    /* i = PEM_write_bio_SSL_SESSION (out, x); */
    /* if (!i) */
    /* { */
    /*     BIO_printf (bio_err, "unable to write SSL_SESSION\n"); */
    /*     goto end; */
    /* } */

    /* i = PEM_write_bio_X509 (out, peer); */
    /* if (!i) */
    /* { */
    /*     BIO_printf (bio_err, "unable to write X509\n"); */
    /*     goto end; */
    /* } */

    /* just testing for memory leaks :-) */
    /* { */
    /*     SSL_SESSION *s; */

    /*     char buf[1024 * 10], *p; */

    /*     int i; */

    /*     s = SSL_SESSION_new (); */

    /*     p = &buf; */
    /*     i = i2d_SSL_SESSION (x, &p); */
    /*     p = &buf; */
    /*     d2i_SSL_SESSION (&s, &p, (long) i); */
    /*     p = &buf; */
    /*     d2i_SSL_SESSION (&s, &p, (long) i); */
    /*     p = &buf; */
    /*     d2i_SSL_SESSION (&s, &p, (long) i); */
    /*     SSL_SESSION_free (s); */
    /* } */

    if (in)
        BIO_free(in);
    if (out)
        BIO_free_all(out);
    if (sess)
        SSL_SESSION_free(sess);

    return 0;

err:
    if (in)
        BIO_free(in);
    if (out)
        BIO_free_all(out);
    if (sess)
        SSL_SESSION_free(sess);

    return -1;
}

int main()
{
    int ret = 0;
    const char *ssl_session = "ssl_session.pem";

    ret = ssl_session_parse(ssl_session);
    if (0 > ret)
    {
        fprintf(stderr, "Something Wrong !\n");
        return -1;
    }

    return 0;
}
