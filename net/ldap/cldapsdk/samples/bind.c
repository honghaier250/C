/* $Novell: bind.c,v 1.8 2003/05/12 12:14:11 $ */
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
    bind.c
***************************************************************************
   Description: The bind.c sample demonstrates both an anonymous and simple
                LDAP v3 bind to an LDAP server.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\nUsage:   bind <host name> <port number> <login dn> <password>" 
"\nExample: bind Acme.com 389 cn=admin,o=Acme secret\n";

int main( int argc, char **argv)
{ 
    int      rc, ldapPort, version;
    char     *ldapHost, *loginDN, *password;
    LDAP     *ld;
    struct timeval timeOut = {10,0};    /* 10 second connection timeout */
    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 5)
    {
        printf("%s", usage);
        return (1);
    }
             
    ldapHost   =  argv[1];
    ldapPort   =  atoi(argv[2]);
    loginDN    =  argv[3];
    password   =  argv[4];
 
    /* 
     * Set LDAP version to 3.
     * Using a NULL session handle sets the global options.
     * All subsequent LDAP sessions created in this process will be version 3.
     * Also set the connection timeout.
     */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* 
     * Initialize the LDAP session 
     */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n\tLDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n\tLDAP session initialized\n");
   
    /* 
     * Bind to the server anonymously 
     */
    if(( rc = ldap_simple_bind_s( ld, NULL, NULL )) != LDAP_SUCCESS )
    {
        printf("\n\tldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return ( 1 );
    }
    printf("\n\tAnonymous bind successful\n");
    
    ldap_unbind_s( ld );
   
    
    /* 
     * Initialize a new LDAP session 
     */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n\tLDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n\tNew LDAP session initialized\n");

    /* 
     * Simple bind to the server 
     */
    if ((rc = ldap_simple_bind_s( ld, loginDN, password )) != LDAP_SUCCESS )
    {
        printf("\n\tldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return ( 1 );
    }
    printf("\n\tBind and authentication to the server successful\n");

    ldap_unbind_s( ld );    

    return ( 0 );
}

