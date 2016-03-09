/* serv.cpp  -  Minimal ssleay server for Unix
   30.9.1996, Sampo Kellomaki <sampo@iki.fi> */

/* mangled to work with SSLeay-0.9.0b and OpenSSL 0.9.2b
   Simplified to be even more minimal
   12/98 - 4/99 Wade Scholine <wades@mail.cybg.com> */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/bio.h>
#include <openssl/rsa.h>        /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

/* define HOME to be dir for key and cert files... */
#define HOME "../server_client/example1/certs/"
/* Make these what you want for cert & key files */
#define CAF HOME "rsa_ca_cert.pem"
#define CERTF HOME "rsa_server_cert.pem"
#define KEYF  HOME "rsa_server_prikey.pem"

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

static void msg_cb(int write_p, int version, int content_type, const void *buf, size_t len, SSL * ssl, void *arg)
{

    fprintf(stdout, "SSL_set_msg_callback called\n");

    BIO *bio = arg;

    const char *str_write_p, *str_version, *str_content_type = "", *str_details1 = "", *str_details2 = "";

    str_write_p = write_p ? ">>>" : "<<<";

    switch (version)
    {
        case SSL2_VERSION:
            str_version = "SSL 2.0";
            break;
        case SSL3_VERSION:
            str_version = "SSL 3.0 ";
            break;
        case TLS1_VERSION:
            str_version = "TLS 1.0 ";
            break;
        case TLS1_1_VERSION:
            str_version = "TLS 1.1 ";
            break;
        case TLS1_2_VERSION:
            str_version = "TLS 1.2 ";
            break;
        case DTLS1_VERSION:
            str_version = "DTLS 1.0 ";
            break;
        case DTLS1_BAD_VER:
            str_version = "DTLS 1.0 (bad) ";
            break;
        default:
            str_version = "???";
    }

    if (version == SSL2_VERSION)
    {
        str_details1 = "???";

        if (len > 0)
        {
            switch (((const unsigned char *) buf)[0])
            {
                case 0:
                    str_details1 = ", ERROR:";
                    str_details2 = " ???";
                    if (len >= 3)
                    {
                        unsigned err = (((const unsigned char *) buf)[1] << 8) + ((const unsigned char *) buf)[2];

                        switch (err)
                        {
                            case 0x0001:
                                str_details2 = " NO-CIPHER-ERROR";
                                break;
                            case 0x0002:
                                str_details2 = " NO-CERTIFICATE-ERROR";
                                break;
                            case 0x0004:
                                str_details2 = " BAD-CERTIFICATE-ERROR";
                                break;
                            case 0x0006:
                                str_details2 = " UNSUPPORTED-CERTIFICATE-TYPE-ERROR";
                                break;
                        }
                    }

                    break;
                case 1:
                    str_details1 = ", CLIENT-HELLO";
                    break;
                case 2:
                    str_details1 = ", CLIENT-MASTER-KEY";
                    break;
                case 3:
                    str_details1 = ", CLIENT-FINISHED";
                    break;
                case 4:
                    str_details1 = ", SERVER-HELLO";
                    break;
                case 5:
                    str_details1 = ", SERVER-VERIFY";
                    break;
                case 6:
                    str_details1 = ", SERVER-FINISHED";
                    break;
                case 7:
                    str_details1 = ", REQUEST-CERTIFICATE";
                    break;
                case 8:
                    str_details1 = ", CLIENT-CERTIFICATE";
                    break;
            }
        }
    }

    if (version == SSL3_VERSION || version == TLS1_VERSION || version == TLS1_1_VERSION || version == TLS1_2_VERSION || version == DTLS1_VERSION || version == DTLS1_BAD_VER)
    {
        switch (content_type)
        {
            case 20:
                str_content_type = "ChangeCipherSpec";
                break;
            case 21:
                str_content_type = "Alert";
                break;
            case 22:
                str_content_type = "Handshake";
                break;
        }

        if (content_type == 21)    /* Alert */
        {
            str_details1 = ", ???";

            if (len == 2)
            {
                switch (((const unsigned char *) buf)[0])
                {
                    case 1:
                        str_details1 = ", warning";
                        break;
                    case 2:
                        str_details1 = ", fatal";
                        break;
                }

                str_details2 = " ???";
                switch (((const unsigned char *) buf)[1])
                {
                    case 0:
                        str_details2 = " close_notify";
                        break;
                    case 10:
                        str_details2 = " unexpected_message";
                        break;
                    case 20:
                        str_details2 = " bad_record_mac";
                        break;
                    case 21:
                        str_details2 = " decryption_failed";
                        break;
                    case 22:
                        str_details2 = " record_overflow";
                        break;
                    case 30:
                        str_details2 = " decompression_failure";
                        break;
                    case 40:
                        str_details2 = " handshake_failure";
                        break;
                    case 42:
                        str_details2 = " bad_certificate";
                        break;
                    case 43:
                        str_details2 = " unsupported_certificate";
                        break;
                    case 44:
                        str_details2 = " certificate_revoked";
                        break;
                    case 45:
                        str_details2 = " certificate_expired";
                        break;
                    case 46:
                        str_details2 = " certificate_unknown";
                        break;
                    case 47:
                        str_details2 = " illegal_parameter";
                        break;
                    case 48:
                        str_details2 = " unknown_ca";
                        break;
                    case 49:
                        str_details2 = " access_denied";
                        break;
                    case 50:
                        str_details2 = " decode_error";
                        break;
                    case 51:
                        str_details2 = " decrypt_error";
                        break;
                    case 60:
                        str_details2 = " export_restriction";
                        break;
                    case 70:
                        str_details2 = " protocol_version";
                        break;
                    case 71:
                        str_details2 = " insufficient_security";
                        break;
                    case 80:
                        str_details2 = " internal_error";
                        break;
                    case 90:
                        str_details2 = " user_canceled";
                        break;
                    case 100:
                        str_details2 = " no_renegotiation";
                        break;
                    case 110:
                        str_details2 = " unsupported_extension";
                        break;
                    case 111:
                        str_details2 = " certificate_unobtainable";
                        break;
                    case 112:
                        str_details2 = " unrecognized_name";
                        break;
                    case 113:
                        str_details2 = " bad_certificate_status_response";
                        break;
                    case 114:
                        str_details2 = " bad_certificate_hash_value";
                        break;
                    case 115:
                        str_details2 = " unknown_psk_identity";
                        break;
                }
            }
        }

        if (content_type == 22)    /* Handshake */
        {
            str_details1 = "???";

            if (len > 0)
            {
                switch (((const unsigned char *) buf)[0])
                {
                    case 0:
                        str_details1 = ", HelloRequest";
                        break;
                    case 1:
                        str_details1 = ", ClientHello";
                        break;
                    case 2:
                        str_details1 = ", ServerHello";
                        break;
                    case 3:
                        str_details1 = ", HelloVerifyRequest";
                        break;
                    case 11:
                        str_details1 = ", Certificate";
                        break;
                    case 12:
                        str_details1 = ", ServerKeyExchange";
                        break;
                    case 13:
                        str_details1 = ", CertificateRequest";
                        break;
                    case 14:
                        str_details1 = ", ServerHelloDone";
                        break;
                    case 15:
                        str_details1 = ", CertificateVerify";
                        break;
                    case 16:
                        str_details1 = ", ClientKeyExchange";
                        break;
                    case 20:
                        str_details1 = ", Finished";
                        break;
                }
            }
        }

#ifndef OPENSSL_NO_HEARTBEATS
        if (content_type == 24)    /* Heartbeat */
        {
            str_details1 = ", Heartbeat";

            if (len > 0)
            {
                switch (((const unsigned char *) buf)[0])
                {
                    case 1:
                        str_details1 = ", HeartbeatRequest";
                        break;
                    case 2:
                        str_details1 = ", HeartbeatResponse";
                        break;
                }
            }
        }
#endif
    }

    BIO_printf(bio, "%s %s%s [length %04lx]%s%s\n", str_write_p, str_version, str_content_type, (unsigned long) len, str_details1, str_details2);

    if (len > 0)
    {
        size_t num, i;

        BIO_printf(bio, "   ");
        num = len;
#if 0
        if (num > 16)
            num = 16;
#endif
        for (i = 0; i < num; i++)
        {
            if (i % 16 == 0 && i > 0)
                BIO_printf(bio, "\n   ");
            BIO_printf(bio, " %02x", ((const unsigned char *) buf)[i]);
        }
        if (i < len)
            BIO_printf(bio, " ...");
        BIO_printf(bio, "\n");
    }
    (void) BIO_flush(bio);
}

int password_callback(char *buf, int size, int rwflag, void *userdata)
{
    /* For the purposes of this demonstration, the password is "123456" */

    fprintf(stdout, "SSL_CTX_set_default_passwd_cb called\n");
    strcpy(buf, "123456");

    return strlen(buf);
}

#if OPENSSL_VERSION_NUMBER >= 0x00907000L
static int verify_dont_fail_cb(X509_STORE_CTX * c, void *unused_arg)
#else
static int verify_dont_fail_cb(X509_STORE_CTX * c)
#endif
{
    int i;

    fprintf(stdout, "SSL_CTX_set_cert_verify_callback called\n");

    i = X509_verify_cert(c);    /* sets c->error */
#if OPENSSL_VERSION_NUMBER >= 0x00905000L    /* don't allow unverified
                                             * certificates -- they could
                                             * survive session reuse, but
                                             * OpenSSL < 0.9.5-dev does not
                                             * preserve their verify_result */
    if (i == 0)
        return 1;
    else
#endif
        return i;
}

int new_sess_cb(struct ssl_st *ssl, SSL_SESSION * sess)
{
    fprintf(stdout, "SSL_CTX_sess_set_new_cb called\n");

    return 1;
}

SSL_SESSION *get_sess_cb(struct ssl_st * ssl, unsigned char *data, int len, int *copy)
{
    fprintf(stdout, "SSL_CTX_sess_set_get_cb called\n");

    return SSL_get_session(ssl);
}

void remove_sess_cb(struct ssl_ctx_st *ctx, SSL_SESSION * sess)
{
    fprintf(stdout, "SSL_CTX_sess_set_remove_cb called\n");
}

RSA *tmp_rsa_cb(SSL * s, int is_export, int keylength)
{
    BIO *bio_err = NULL;
    BIGNUM *bn = NULL;
    static int s_quiet = 0;
    static RSA *rsa_tmp = NULL;

    fprintf(stdout, "SSL_CTX_set_tmp_rsa_callback called\n");
    bio_err = BIO_new_fp(stderr, BIO_NOCLOSE | BIO_FP_TEXT);

    if (!rsa_tmp && ((bn = BN_new()) == NULL))
        BIO_printf(bio_err, "Allocation error in generating RSA key\n");
    if (!rsa_tmp && bn)
    {
        if (!s_quiet)
        {
            BIO_printf(bio_err, "Generating temp (%d bit) RSA key...", keylength);
            (void) BIO_flush(bio_err);
        }
        if (!BN_set_word(bn, RSA_F4) || ((rsa_tmp = RSA_new()) == NULL) || !RSA_generate_key_ex(rsa_tmp, keylength, bn, NULL))
        {
            if (rsa_tmp)
                RSA_free(rsa_tmp);
            rsa_tmp = NULL;
        }
        if (!s_quiet)
        {
            BIO_printf(bio_err, "\n");
            (void) BIO_flush(bio_err);
        }
        BN_free(bn);
    }

    if (bio_err)
        BIO_free(bio_err);

    return (rsa_tmp);
}

int main()
{
    int err;
    int sd;
    int listen_sd;
    socklen_t client_len;
    struct sockaddr_in sa_serv;
    struct sockaddr_in sa_cli;
    X509 *peer;
    char *str;
    char buf[4096];
    SSL *ssl = NULL;
    SSL_CTX *ctx = NULL;
    SSL_SESSION *sess = NULL;
    const SSL_METHOD *meth = NULL;
    int (*callback) (char *, int, int, void *) = &password_callback;

    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    meth = TLSv1_2_server_method();
    ctx = SSL_CTX_new(meth);
    if (!ctx)
    {
        ERR_print_errors_fp(stderr);
        exit(2);
    }

    SSL_CTX_set_default_passwd_cb(ctx, callback);

    /* SSL preliminaries. We keep the certificate and key with the context. */
    if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(3);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(4);
    }

    if (!SSL_CTX_check_private_key(ctx))
    {
        fprintf(stderr, "Private key does not match the certificate public key\n");
        exit(5);
    }

    SSL_CTX_set_cipher_list(ctx, "EXP-RC4-MD5");
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, 0);
    SSL_CTX_set_cert_verify_callback(ctx, verify_dont_fail_cb, NULL);
    SSL_CTX_load_verify_locations(ctx, CAF, NULL);
    SSL_CTX_set_msg_callback(ctx, msg_cb);
    SSL_CTX_sess_set_new_cb(ctx, new_sess_cb);
    SSL_CTX_sess_set_get_cb(ctx, get_sess_cb);
    SSL_CTX_sess_set_remove_cb(ctx, remove_sess_cb);
    SSL_CTX_set_tmp_rsa_callback(ctx, tmp_rsa_cb);

    /* ----------------------------------------------- */
    /* Prepare TCP socket for receiving connections */

    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    CHK_ERR(listen_sd, "socket");

    memset(&sa_serv, '\0', sizeof(sa_serv));
    sa_serv.sin_family = AF_INET;
    sa_serv.sin_addr.s_addr = INADDR_ANY;
    sa_serv.sin_port = htons(1111);    /* Server Port number */

    err = bind(listen_sd, (struct sockaddr *) &sa_serv, sizeof(sa_serv));
    CHK_ERR(err, "bind");

    /* Receive a TCP connection. */

    err = listen(listen_sd, 5);
    CHK_ERR(err, "listen");

    client_len = sizeof(sa_cli);
    sd = accept(listen_sd, (struct sockaddr *) &sa_cli, &client_len);
    CHK_ERR(sd, "accept");
    close(listen_sd);

    printf("Connection from %u, port %d\n", sa_cli.sin_addr.s_addr, sa_cli.sin_port);

    /* ----------------------------------------------- */
    /* TCP connection is ready. Do server side SSL. */

    ssl = SSL_new(ctx);
    CHK_NULL(ssl);
    SSL_set_fd(ssl, sd);
    err = SSL_accept(ssl);
    CHK_SSL(err);

    /* Get the cipher - opt */

    printf("SSL connection using %s\n", SSL_get_cipher(ssl));

    /* Get client's certificate (note: beware of dynamic allocation) - opt */

    /* peer = SSL_get_peer_certificate(ssl); */
    /* if (peer != NULL) */
    /* { */
    /*     printf("Client certificate:\n"); */

    /*     str = X509_NAME_oneline(X509_get_subject_name(peer), 0, 0); */
    /*     CHK_NULL(str); */
    /*     printf("\t subject: %s\n", str); */
    /*     OPENSSL_free(str); */

    /*     str = X509_NAME_oneline(X509_get_issuer_name(peer), 0, 0); */
    /*     CHK_NULL(str); */
    /*     printf("\t issuer: %s\n", str); */
    /*     OPENSSL_free(str); */

    /*     /1* We could do all sorts of certificate verification stuff here before */
    /*        deallocating the certificate. *1/ */

    /*     X509_free(peer); */
    /* } */
    /* else */
    /*     printf("Client does not have certificate.\n"); */

    sess = SSL_get_session(ssl);
    if (NULL == sess)
    {
        ERR_print_errors_fp(stderr);
    }
    else
    {
        SSL_SESSION_print_fp(stdout, sess);

        /* Get client's certificate from SSL_SESSION(note: beware of dynamic allocation) - opt */
        peer = SSL_SESSION_get0_peer(sess);
        if (peer != NULL)
        {
            printf("Client certificate:\n");

            str = X509_NAME_oneline(X509_get_subject_name(peer), 0, 0);
            CHK_NULL(str);
            printf("\t subject: %s\n", str);
            OPENSSL_free(str);

            str = X509_NAME_oneline(X509_get_issuer_name(peer), 0, 0);
            CHK_NULL(str);
            printf("\t issuer: %s\n", str);
            OPENSSL_free(str);

            /* We could do all sorts of certificate verification stuff here before
               deallocating the certificate. */
        }
        else
            printf("Client does not have certificate.\n");
    }

    /* DATA EXCHANGE - Receive message and send reply. */
    err = SSL_read(ssl, buf, sizeof(buf) - 1);
    CHK_SSL(err);
    buf[err] = '\0';
    printf("Got %d chars:'%s'\n", err, buf);

    err = SSL_write(ssl, "I hear you.", strlen("I hear you."));
    CHK_SSL(err);

    /* Clean up. */

    close(sd);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    return 0;
}

/* EOF - serv.cpp */
