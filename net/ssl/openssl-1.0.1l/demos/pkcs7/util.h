#ifndef __UTIL_H
#    define __UTIL_H

#    include <openssl/crypto.h>
#    include <openssl/pem.h>
#    include <openssl/err.h>
#    include <openssl/x509_vfy.h>
#    include <openssl/x509v3.h>
#    include <openssl/engine.h>
#    include <openssl/safestack.h>

ENGINE *load_engine(const char *id);
EVP_PKEY *load_key(const char *file);
X509 *load_certificate(const char *file);

X509_STORE *setup_verify(BIO * bp, char *CAfile, char *CApath);

#endif                            // __UTIL_H
