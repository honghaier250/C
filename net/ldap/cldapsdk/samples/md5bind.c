/* $Novell: md5bind.c,v 1.10 2003/05/12 12:23:38 $ */
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
   md5bind.c 
***************************************************************************
   Description: 
    
   The md5bind.c sample shows how to authenticate to an LDAP server using 
   the DIGEST-MD5 SASL mechanism.
   
   The advantage of DIGEST-MD5 is that one can remain in clear text mode
   but not have to send the password in clear text, as in a simple bind.
   Nor does one need to deal with generating and managing certificates.
   The eDirectory server decodes this encrypted password and compares it
   with the simplePassword attribute of the object on the server.
   
   The LDAP server must support the DIGEST-MD5 mechanism. 
   The SASL mechanisms supported by a server can be determined by reading 
   the supportedSASLMechanisms attribute of the rootdse object. 
   There are three functions that support DIGEST-MD5 SASL authentication: 
   ldap_bind_digest_md5_start, ldap_get_digest_md5_realms, and 
   ldap_bind_digest_md5_finish.

   An LDAP client initiates a DIGEST-MD5 authentication by calling the 
   ldap_bind_digest_md5_start() function. This function sends a SASL bind 
   request to the server. The request specifies the DIGEST-MD5 mechanism 
   and contains no credential information. The server responds with a 
   digest-challenge. The ldap_bind_digest_md5_start() function is a 
   synchronous function. It returns after the digest-challenge has been 
   received.

   The digest-challenge is a string containing various data fields 
   including one or more realm values. A realm value is a character string 
   that indicates to the user what user name and password to supply. The 
   realm value names a collection of accounts to which the user may 
   belong. The exact meaning of the realm value is server specific. For an 
   eDirectory server the realm value is the name of the eDirectory tree to 
   which the server belongs. Realm values can be retrieved by calling the 
   ldap_get_digest_md5_realms() function. This step is optional.

   The DIGEST-MD5 authentication is completed by calling the 
   ldap_bind_digest_md5_finish() function. This function sends a digest-
   response to the server. The digest-response contains an MD5 hash of the 
   user's name and password and other information. When the server 
   receives the digest-response and successfully validates it, the server 
   sends a response-auth message to the client to complete the 
   authentication handshake. The ldap_bind_digest_md5_finish() function 
   returns after the response-auth message is received from the server.

   eDirectory uses the password stored in the simplePassword attribute to 
   perform a DIGEST-MD5 bind. This value must be stored as clear text in 
   order for the DIGEST-MD5 bind to succeed. The simplePassword attribute 
   can be set by using the ICE import-export tool or through the 
   SimplePassword ConsoleOne snap-in. The ConsoleOne snap-in always stores 
   the password as clear text.
     
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ldapx.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   md5bind <host name> <port number> dn:<login dn> <password>"
"\n Example: md5bind Acme.com 389 dn:cn=admin,o=Acme secret\n";

int doSimpleSearch(LDAP*);

int main( int argc, char **argv )
{   
    int                      version, ldapPort, rc;
    char                     *ldapHost, *loginDN, *password;
    char                     *errorMsg = NULL, **realms, *realm; 
    LDAP                     *ld;
    LDAPMessage              *result = NULL;    
    LDAP_DIGEST_MD5_CONTEXT  *md5ctx;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */
                         
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 5) {
        printf("%s", usage);
        return (1);
    }

    ldapHost = argv[1];
    ldapPort = atoi(argv[2]);
    loginDN  = argv[3];
    password = argv[4];

    if (strncmp(loginDN, "dn:", 3) != 0)    /* Check for "dn:" */
    {
        printf("%s", usage);
        printf("\n\n *** The login dn must be preceeded by \"dn:\" \n\n");
        return (1);
    }

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\nError in ldap_init\n");
        return( 1 );
    }

    /* Bind to the server */
    if ( LDAP_SUCCESS != (rc = ldap_bind_digest_md5_start(ld, &md5ctx)) ) {
        printf("\nError in ldap_bind_digest_md5_start: %d\n", rc);
    }
    else {
        /* print the realms */
        rc = ldap_get_digest_md5_realms(md5ctx, &realms);
        realm = *realms;
        while (realm != NULL) {
            printf("\n realm: %s\n", realm);
            realms++;
            realm = *realms;
        }

        rc = ldap_bind_digest_md5_finish(
            &md5ctx, 
            loginDN,
            password,
            -1, /*null terminated password */
            0,  /*first realm. */
            LDAP_DIGEST_MD5_FINISH);

        if (rc != LDAP_SUCCESS) {
            printf("\nError in ldap_bind_digest_md5_finish: %d\n", rc);
            ldap_unbind( ld );
            return(1);
        }
        
        printf("\nDIGEST-MD5 bind successful\n    - performing search\n\n");

        if ( (rc = doSimpleSearch(ld)) != LDAP_SUCCESS ) {
            printf("\n    - Seasrch failed: %d\n", rc);
            ldap_unbind( ld );
            return(1);
        }
        
        printf("\n    - Search successful\n");
    }

    ldap_unbind( ld );  
 
    return( 0 );
}

/* 
 * doSimpleSearch - just to verify things worked
 */
int doSimpleSearch(LDAP *ld)
{
    int         rc         = LDAP_SUCCESS;
    char        *dn        = NULL;
    LDAPMessage *resultBuf = NULL;
    LDAPMessage *entry     = NULL;
    char        *attrs[]   = { LDAP_NO_ATTRS, NULL };
    
    /* List objects at the top level */
    rc = ldap_search_ext_s(ld, "" , LDAP_SCOPE_ONELEVEL, "(objectClass=*)",
                           attrs, 0, NULL, NULL, NULL, 
                           LDAP_NO_LIMIT, &resultBuf);
        
    if( rc == LDAP_SUCCESS) {
       entry = ldap_first_entry(ld, resultBuf);

        while (entry != NULL) {
            dn = ldap_get_dn(ld, entry);
            if (dn) 
                printf("\tObject: %s\n", dn);
            ldap_memfree(dn);

            entry = ldap_next_entry(ld, entry);
        }

        ldap_msgfree( resultBuf );
    }
    
    return rc;
}
