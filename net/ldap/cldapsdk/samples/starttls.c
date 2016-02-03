/* $Novell: starttls.c,v 1.7 2003/05/12 13:10:19 $ */
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
    starttls.c
***************************************************************************
   Description: strt_tls.c demonstrates how to use the ldapssl_start_tls
   and ldapssl_stop_tls functions in the ldapssl library. The use of
   ldapssl_set_verify_mode(LDAPSSL_VERIFY_NONE) is for simplicity. It 
   allows this sample code to accept any certificate from an LDAP server.
   
   NOTE:  This sample requires the server to support Start/Stop TLS.
          Start/Stop TLS support was introduced in eDirectory 8.7.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap_ssl.h>

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   starttls <host name> [<port number> [<login dn> <password>]]\n"
"  host name      = ldap server name or IP address\n"
"  port number    = port to use - 389 default (clear text)\n" 
"  login dn       = user name to login as\n"
"  password       = user password\n\n"
"Examples:\n"
"          starttls www.openldap.org\n"
"          starttls www.openldap.org 389\n"
"          starttls Acme.com 389 cn=admin,o=Acme secret\n";

int doSimpleSearch
(
   LDAP *ld
);

int main(int argc, char *argv[])
{
    int   rc       = 0;
    int   version  = LDAP_VERSION3;
    int   ldapPort = 389;
    char *ldapHost = NULL;
    char *loginDN  = NULL;
    char *password = NULL;
    LDAP *ld       = NULL;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    /*
     * Check for proper parameters
     */
    if (2 != argc && 3 != argc && 5 != argc)
    {
        printf("%s", usage);
        exit(1);
    }

    ldapHost = argv[1];
    
    /* If there are 3 or more arguments, get the port number.
     */
    if (3 <= argc)
    {
        ldapPort = atoi(argv[2]);
    }
    /* If there are 5 arguments, get the login dn and password.
     */
    if (5 == argc)
    {
        loginDN  = argv[3];
        password = argv[4];
    }

    /* Set LDAP version to 3 and set connection timeout. */
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);
    
    /* 
     * Initialize the ssl library.  The first parameter of 
     * ldapssl_client_init is a certificate file.  However, when used
     * the file must be a DER encoded file.  A the API, 
     * ldapssl_add_trusted_cert accepts both DER and B64 encoded files.
     *
     * ldapssl_client_init is an application level initialization not a 
     * thread level initilization and should be done once.
     */
    rc = ldapssl_client_init(NULL,   /* Certificate File */ 
                              NULL); /* reserved, just use NULL */

    if (LDAP_SUCCESS != rc)
    {
        printf("ldapssl_client_init error: %d\n", rc);
        exit(1);
    }
    
    /*
     * Configure the LDAP SSL library to not verify the server certificate.  
     * The default is LDAPSSL_VERIFY_SERVER which validates all servers 
     * against the trusted certificates normally passed in 
     * during ldapssl_client_init or ldapssl_add_trusted_cert.
     *
     * WARNING:  Setting the verify mode to LDAPSSL_VERIFY_NONE turns off
     * server certificate verification.  This means all servers are 
     * considered trusted.  This should be used only in controlled 
     * environments where encrypted communication between servers and 
     * clients is desired but server verification is not necessary.
     */   
    rc = ldapssl_set_verify_mode(LDAPSSL_VERIFY_NONE);
    if (LDAP_SUCCESS != rc)
    {
        printf("ldapssl_set_verify_mode error: %d\n", rc);
        ldapssl_client_deinit();
        exit(1);
    }

    /* 
     * Create an SSL enabled LDAP handle
     */
    ld = ldapssl_init(ldapHost,       /* host name */
                      ldapPort,       /* port number */
                      0);             /* 0- clear text, 1-enable for ssl */

    if (NULL == ld)
    {
        printf("ldapssl_init error\n");
        ldapssl_client_deinit();
        exit(1);
    }

    /*
     * Bind (connect) to the LDAP server.
     */
    rc = ldap_simple_bind_s(ld, loginDN, password);
    if (LDAP_SUCCESS != rc)
    {
        printf("ldap_simple_bind_s error: %d, %s\n",
               rc, ldap_err2string(rc));
        ldap_unbind_s(ld);
        ldapssl_client_deinit();
        exit(1);
    }
    
    printf("Bind successful.  Do a simple search.\n");
      
    
    rc = doSimpleSearch(ld);
    if (LDAP_SUCCESS != rc)
    {
        printf("doSimpleSearch error: %d, %s\n",
               rc, ldap_err2string(rc));
        ldap_unbind_s(ld);
        ldapssl_client_deinit();
        exit(1);
    }

    /*
     * Start TLS.
     */
    printf("\nNow start TLS.\n");
    rc = ldapssl_start_tls(ld);

    if (LDAP_SUCCESS != rc)
    {
       printf("ldapssl_start_tls error: %d, %s\n",
              rc, ldap_err2string(rc));
       ldap_unbind_s(ld);
       ldapssl_client_deinit();
       exit(1);
    }
    printf("Start TLS successful\n");
    printf("Performing search\n");
    rc = doSimpleSearch(ld);
    if (LDAP_SUCCESS != rc)
    {
        printf("doSimpleSearch error: %d, %s\n", rc, ldap_err2string(rc));
        ldap_unbind_s(ld);
        ldapssl_client_deinit();
        exit(1);
    }
    /* Now lets stop tls.
     */
    rc = ldapssl_stop_tls(ld);
    if (LDAP_SUCCESS != rc)
    {
       printf("ldapssl_stop_tls error: %d, %s\n", rc, ldap_err2string(rc));
       ldap_unbind_s(ld);
       ldapssl_client_deinit();
       exit(1);
    }
    printf("\nStop TLS successful\n");
    printf("Performing search\n");
    rc = doSimpleSearch(ld);
    if (LDAP_SUCCESS != rc)
    {
        printf("doSimpleSearch error: %d, %s\n", rc, ldap_err2string(rc));
        ldap_unbind_s(ld);
        ldapssl_client_deinit();
        exit(1);
    }
       
    /*
     * Unbind
     */
    printf("\nUnbind and deinit\n");
    ldap_unbind_s(ld);

    /* 
     * Uninitialize the LDAP ssl library
     */
    ldapssl_client_deinit();
    return 0;
}

/* 
 * doSimpleSearch - just to verify things worked
 */
int doSimpleSearch(LDAP *ld)
{
    int          rc        = LDAP_SUCCESS;
    LDAPMessage *resultBuf = NULL;
    LDAPMessage *entry     = NULL;
    BerElement  *ber       = NULL;   
    char        *attribute = NULL;
    char        **values   = NULL;

    /* Reading the root DSE
     */
    rc = ldap_search_ext_s(ld, "" , LDAP_SCOPE_BASE, "(objectClass=*)",
                           NULL, 0,
                           NULL, NULL, NULL, LDAP_NO_LIMIT, &resultBuf);

    if(LDAP_SUCCESS == rc)
    {
       entry = ldap_first_entry(ld, resultBuf);

       if(entry != NULL)
       {          
          for(attribute = ldap_first_attribute(ld, entry, &ber);
              attribute != NULL; 
              attribute = ldap_next_attribute(ld, entry, ber))
          {
              /* Print only the first value
               */              
              if((values = ldap_get_values(ld, entry, attribute)) != NULL)
              {
                 printf("  %s: %s\n", attribute, values[0]);
                 ldap_value_free(values);
                 ldap_memfree(attribute);
              }        
          }
          /* Zero means free memory
           */
          ber_free(ber, 0);
       }

       ldap_msgfree(resultBuf);
    }
    else
    {
       printf("ldap_search_ext_s error: %d, %s\n", rc, ldap_err2string(rc));
    }
    return rc;
}
