/* $Novell: saslExternal.c,v 1.6 2003/05/12 12:58:48 $ */
/**************************************************************************
*  Novell Software Developer Kit
*
*  Copyright (C) 2002-2003 Novell, Inc. All Rights Reserved.
*
*  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND TREATIES.
*  USE AND REDISTRIBUTION OF THIS WORK IS SUBJECT TO THE LICENSE AGREEMENT
*  ACCOMPANYING THE SOFTWARE DEVELOPER KIT (SDK) THAT CONTAINS THIS WORK.
*  PURSUANT TO THE SDK LICENSE AGREEMENT, NOVELL HEREBY GRANTS TO DEVELOPER A
*  ROYALTY-FREE, NON-EXCLUSIVE LICENSE TO INCLUDE NOVELL'S SAMPLE CODE IN ITS
*  PRODUCT. NOVELL GRANTS DEVELOPER WORLDWIDE DISTRIBUTION RIGHTS TO MARKET,
*  DISTRIBUTE, OR SELL NOVELL'S SAMPLE CODE AS A COMPONENT OF DEVELOPER'S
*  PRODUCTS. NOVELL SHALL HAVE NO OBLIGATIONS TO DEVELOPER OR DEVELOPER'S
*  CUSTOMERS WITH RESPECT TO THIS CODE.
*
***************************************************************************
    saslExternal.c
***************************************************************************
   
   Description: Demonstrates how to do a SASL External Bind with an LDAP 
   server. No login dn is needed because this information is passed to the
   LDAP server via a client public key certificate. In order for this to work
   the LDAP server must require mutual authentication (client public key
   certificate required). This is similar to the mutual.c sample. Three files
   are used in this sample to do SASL External Bind with an LDAP server.
   They are:
      
   1. server trusted root certificate
   2. client public key certificate
   3. client private key
   
   The server trusted root and client public key certificate files are used
   to authenticate the server and client respectively. The ldapssl api uses
   the server trusted root certificate to validate the server it is trying to
   connect to. The client public key certificate is sent to the server to
   authenticate the client and it contains the clients public key information.
   This client public key certificate is used by the LDAP server SASL External
   mechanism (Cert Mutual) during a saslbind(EXTERNAL). Before this sample can
   run properly the "Cert Mutual" External Method needs to be installed into
   eDirectory. The client private key contains the key used by the ldapssl api to
   encrypt data to send the server. The server un-encrypts data from the
   client via the client's public key.
   
   The client public key certificate file may contain the clients private key
   if created properly. This file can be used as the client private key and 
   public key certificate file (The private key information is never sent to
   the server).
   
   In order to run this sample some configuring needs to be done on the LDAP
   server that you are testing against. The following are steps needed to
   configure an eDirectory LDAP server to do SASL External Bind. It is
   assumed that you are using ConsoleOne with admin rights to configure
   eDirectory. eDirectory 8.7 or newer is required.
   
   1. To ensure that the client certificate is being sent to the server,
      require client certificates for the LDAP Server. This is done by setting 
      "Client Certificate:" to "Required" in the "LDAP Server" under the
      "TLS Configuration" tab in ConsoleOne.
      (LDAP Server|TLS Configuration|Client Certificate:|Required)
      
   2. Export the SSL CertificateDNS object trusted root certificate.
      (This is your server trusted root certificate file.) For this example
      name it "server.der"
      (SSL CertificateDNS|Certificates|Trusted Root Certificate|Export)
      
   3. Create a client in ConsoleOne. Lets call the client "dave"
      with a password of "foo" in the organization "Acme".
      (File|New|User...)
      
   4. Create a certificate for "dave".
      (dave|Security|Certificate|Create)
      
   5. Refresh the NLDAP server now.
      (LDAP Server|General|Refresh NLDAP Server Now)
      
   6. Authenticate into ConsoleOne as "dave".(This allows you to export "dave"
      certificate that contains both private and public keys.)
      (File|Authenticate)
      
   7. Export dave's certificate with the private key. (This certificate 
      can be used for both the client certificate and private key file.)
      For this example name the certificate file "dave.pfx" with a password
      of "evad".
      (dave|Security|Certificate|Export|Yes export the private key)
   
   At this point you should be able to run this sample as follows. For this
   example assume the server name is "Acme.com" and "dave" was created in
   an organization called "Acme"
   
   saslExternal Acme.com 636 server.der DER dave.pfx PFX evad dave.pfx PFX evad

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap_ssl.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n"
"Usage:\n"
"saslExternal <host name> <port number>\n"
" <server trusted root> <server trusted root type>\n"
" <client private key> <client private key type> <client private key password>\n"
" <client public key> <client public key type> [<client public key password>]\n"
"\n"
"Where:\n"
" host name                   = ldap server name or IP address\n"
" port number                 = port to use - 636 is ldap ssl default\n" 

" server trusted root         = server trusted root certificate file\n"
" server trusted root type    = DER=der encoded file, B64=b64 encoded file\n"
" client private key          = client private key certificate file\n"
" client private key type     = DER=der encoded file, B64=b64 encoded file\n"
"                               PFX=pkcs12 encoded file\n"
" client private key password = client private key certificate file password.\n"
" client public key           = client public key certificate file\n"
" client public key type      = DER=der encoded file, B64=b64 encoded file\n"
"                               PFX=pkcs12 encoded file\n"
" client public key password  = client public key certificate file password.\n"
"                               [Optional]\n\n"
"Example:\n"
"saslExternal Acme.com 636 server.der DER privatekey.pfx PFX secret1\n"
" publickey.pfx PFX secret2\n";

/* Helper Functions
 */
int getFileEncoding(char *fileType);

/*
 * Main - Demonstrates mutual authentication with an LDAP server
 */
int main(int argc, char *argv[])
{       
    /*
     * Variables
     */
    int          ldapPort                  = 0;
    char        *ldapHost                  = NULL;
    
    char        *serverTrustedRoot         = NULL;
    int          serverTrustedRootEncoding = 0;
    
    char        *clientPrivateKey          = NULL;
    int          clientPrivateKeyEncoding  = 0;
    char        *clientPrivateKeyPassword  = NULL;
   
    char        *clientPublicKey           = NULL;
    int          clientPublicKeyEncoding   = 0;
    char        *clientPublicKeyPassword   = NULL;
    
    int          version                   = 0;
    int          rc                        = 0;
    LDAP        *ld                        = NULL;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */
   
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    /*
     * Check for valid number of arguments.
     */
    if (10 != argc && 11 != argc)
    {
        printf("%s", usage);
        exit(1);
    }
   
    /*
     * Parse arguments
     */
    ldapHost                  = argv[1];
    ldapPort                  = atoi(argv[2]);
    serverTrustedRoot         = argv[3];
    serverTrustedRootEncoding = getFileEncoding(argv[4]);
    if((0 == serverTrustedRootEncoding) ||
       (LDAPSSL_FILETYPE_P12 == serverTrustedRootEncoding)) 
    {
       printf("Invalid server trusted root certificate file type.\n");
       printf("%s", usage);
       exit(1);
    }    
    clientPrivateKey          = argv[5];
    clientPrivateKeyEncoding  = getFileEncoding(argv[6]);
    if(0 == clientPrivateKeyEncoding) 
    {
       printf("Invalid client private key certificate file type.\n");
       printf("%s", usage);
       exit(1);
    }    
    clientPrivateKeyPassword  = argv[7];
    clientPublicKey           = argv[8];
    clientPublicKeyEncoding   = getFileEncoding(argv[9]);
    if(0 == clientPublicKeyEncoding) 
    {
       printf("Invalid client public key certificate file type.\n");
       printf("%s", usage);
       exit(1);
    }    
    if (11 == argc)
       clientPublicKeyPassword = argv[10];
    
    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* 
     * Initialize the ssl library.
     * ldapssl_client_init is an application level initialization not a 
     * thread level initialization and should be done once.
     */
    rc = ldapssl_client_init(   NULL,       /* NULL means no cert file*/ 
                                NULL );     /* reserved, use NULL */

    if (rc != LDAP_SUCCESS)
    {
        printf("ldapssl_client_init error: %d\n", rc);
        exit(1);
    }

    /* 
     * Add the server trusted root certificate file.  Multiple trusted root 
     * certificates can be added with ldapssl_add_trusted_cert.  Applications
     * can use this functionality to specify groups of trusted servers.
     */   
    rc = ldapssl_add_trusted_cert(serverTrustedRoot, serverTrustedRootEncoding);
    if (rc != LDAP_SUCCESS)
    {
        printf("ldapssl_add_trusted_cert error: %d\n", rc);
        ldapssl_client_deinit();
        exit(1);
    }

    /*
     * Add the client private key.
     */
    rc = ldapssl_set_client_private_key(clientPrivateKey,
                                        clientPrivateKeyEncoding,
                                        clientPrivateKeyPassword);
    
    if (rc != LDAP_SUCCESS)
    {
        printf("ldapssl_set_client_private_key error: %d\n", rc);
        ldapssl_client_deinit();
        exit(1);
    }
    
    /*
     * Add the client certificate.
     */
    rc = ldapssl_set_client_cert(clientPublicKey,
                                 clientPublicKeyEncoding,
                                 clientPublicKeyPassword);
    
    if (rc != LDAP_SUCCESS)
    {
        printf("ldapssl_set_client_cert error: %d\n", rc);
        ldapssl_client_deinit();
        exit(1);
    }
    

    /* 
     * Create an LDAP session handle that is enabled for ssl connection
     */
    ld = ldapssl_init(  ldapHost,       /* host name */
                        ldapPort,       /* port number */
                        1 );            /* 0- clear text, 1-enable for ssl */

    if (ld == NULL )
    {
        printf("ldapssl_init error\n" );
        ldapssl_client_deinit();
        exit(1);
    }

    
    /* Do external sasl bind
     */
    rc = ldap_sasl_bind_s(ld,          /* LDAP session handle */
                          NULL,        /* Authenticating dn */
                          "EXTERNAL",  /* Method used for authentication */
                          NULL,        /* Credentials to authenticate with */
                          NULL,        /* Server controls */
                          NULL,        /* Client controls */
                          NULL);       /* Credentials passed by the server */

    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_sasl_bind_s error: %d, %s\n", rc, ldap_err2string( rc ));
        ldap_unbind_s( ld );
        ldapssl_client_deinit();
        exit(1);
    }    
    
    printf("SSL External bind successful\n");
      
    ldap_unbind_s( ld );

    /* 
     * Uninitialize the LDAP ssl library
     */
    ldapssl_client_deinit();

    return 0;
}

/* 
 * getFileEncoding - Return the file encoding type
 */
int getFileEncoding(char *fileType)
{
   if ((0 == strcmp(fileType, "DER")) ||
       (0 == strcmp(fileType, "der")))
      return LDAPSSL_FILETYPE_DER;
   else if ((0 == strcmp(fileType, "B64")) ||
            (0 == strcmp(fileType, "b64")))
      return LDAPSSL_FILETYPE_B64;
   else if ((0 == strcmp(fileType, "PFX")) ||
            (0 == strcmp(fileType, "pfx")))
      return LDAPSSL_FILETYPE_P12;
   else
      return 0;
}
