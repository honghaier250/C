#include <stdlib.h>
#include <string.h>

#include "util.h"

static EVP_PKEY *load_key_PEM(const char *file);
static EVP_PKEY *load_key_DER(const char *file);
static X509 *load_cert_PEM(const char *file);
static X509 *load_cert_DER(const char *file);

ENGINE *load_engine(const char *id)
{
    ENGINE *engine = NULL;

    ENGINE_load_builtin_engines();
    engine = ENGINE_by_id(id);
    if (!engine)
    {
        engine = ENGINE_by_id("dynamic");
        if (!engine)
            return NULL;

        if (!ENGINE_ctrl_cmd_string(engine, "SO_PATH", id, 0) || !ENGINE_ctrl_cmd_string(engine, "LOAD", NULL, 0))
        {
            ENGINE_free(engine);
            engine = NULL;
        }
    }

    if (!ENGINE_set_default(engine, ENGINE_METHOD_ALL))
    {
        ENGINE_free(engine);
        engine = NULL;
    }

    return engine;
}

EVP_PKEY *load_key_PEM(const char *file)
{
    EVP_PKEY *key = NULL;
    BIO *key_bio = NULL;

    if ((NULL == (key_bio = BIO_new_file(file, "rb")) || NULL == (key = PEM_read_bio_PrivateKey(key_bio, NULL, NULL, NULL))))
    {
        if (key_bio)
            BIO_free(key_bio);
    }

    return key;
}

EVP_PKEY *load_key_DER(const char *file)
{
    EVP_PKEY *key = NULL;
    BIO *key_bio = NULL;

    if ((NULL == (key_bio = BIO_new_file(file, "rb")) || NULL == (key = d2i_PrivateKey_bio(key_bio, NULL))))
    {
        if (key_bio)
            BIO_free(key_bio);
    }

    return key;
}

EVP_PKEY *load_key(const char *file)
{
    EVP_PKEY *key = NULL;
    // BIO *key_bio = NULL;

    if (!file)
        return NULL;

    key = load_key_PEM(file);
    if (!key)
        key = load_key_DER(file);

    return key;
}

X509 *load_cert_PEM(const char *file)
{
    X509 *cert = NULL;
    BIO *cert_bio = NULL;

    if ((NULL == (cert_bio = BIO_new_file(file, "rb")) || NULL == (cert = PEM_read_bio_X509_AUX(cert_bio, NULL, NULL, NULL))))
    {
        if (cert_bio)
            BIO_free(cert_bio);
    }

    return cert;
}

X509 *load_cert_DER(const char *file)
{
    X509 *cert = NULL;
    BIO *cert_bio = NULL;

    if ((NULL == (cert_bio = BIO_new_file(file, "rb")) || NULL == (cert = PEM_read_bio_X509_AUX(cert_bio, NULL, NULL, NULL))))
    {
        if (cert_bio)
            BIO_free(cert_bio);
    }

    return cert;
}

X509 *load_certificate(const char *file)
{
    X509 *cert = NULL;
    // BIO *cert_bio = NULL;

    if (!file)
        return NULL;

    cert = load_cert_PEM(file);
    if (!cert)
        cert = load_cert_DER(file);

    return cert;
}

X509_STORE *setup_verify(BIO * bp, char *CAfile, char *CApath)
{
    X509_STORE *store;
    X509_LOOKUP *lookup;
    if (!(store = X509_STORE_new()))
        goto end;
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    if (lookup == NULL)
        goto end;

    if (CAfile)
    {
        if (!X509_LOOKUP_load_file(lookup, CAfile, X509_FILETYPE_PEM))
        {
            BIO_printf(bp, "Error loading file %s\n", CAfile);
            goto end;
        }
    }
    else
        X509_LOOKUP_load_file(lookup, NULL, X509_FILETYPE_DEFAULT);

    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
    if (lookup == NULL)
        goto end;

    if (CApath)
    {
        if (!X509_LOOKUP_add_dir(lookup, CApath, X509_FILETYPE_PEM))
        {
            BIO_printf(bp, "Error loading directory %s\n", CApath);
            goto end;
        }
    }
    else
        X509_LOOKUP_add_dir(lookup, NULL, X509_FILETYPE_DEFAULT);

    ERR_clear_error();
    return store;

end:
    X509_STORE_free(store);
    return NULL;
}
