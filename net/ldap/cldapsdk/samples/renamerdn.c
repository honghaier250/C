/* $Novell: renamerdn.c,v 1.8 2003/05/12 12:58:13 $ */
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
    renamerdn.c
***************************************************************************
   Description: The renamerdn.c sample modifies the relative distinguished 
                name of the specified entry. The user specifies the entry's 
                DN, a new RDN for the entry, and a new parent container. 
                The new parent container must already exist in the directory.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   renamerdn <host name> <port number> <login dn> <password>"
"\n          <DN> <new rdn> [<new parent dn>]\n" 
"\n Example: renamerdn acme.com 389 cn=admin,o=acme secret"
"\n          cn=james,ou=sales,o=acme cn=jimmy ou=products,o=acme\n";

int main( int argc, char **argv)
{ 

    int         version, ldapPort, rc;  
    char        *ldapHost, *loginDN, *password, *dn, *newRDN;
    char        *newParentDN = NULL;
    LDAP        *ld;    
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc < 7 )
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost          = argv[1];
    ldapPort          = atoi(argv[2]);
    loginDN           = argv[3];
    password          = argv[4];
    dn                = argv[5];
    newRDN            = argv[6];
    if (argc > 7)                      /* Last argument is optional */
        newParentDN   = argv[7];

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n\tLDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n\tLDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("\n\tBind successful\n");

    
    /* 
     * Perform the rename RDN operation and delete the old 
     * RDN value from the naming attribute.
     */
    rc = ldap_rename_s( ld,             /* LDAP session handle */ 
                        dn,             /* dn of the entry to be modified */
                        newRDN,         /* new rdn to be named as */                          
                        newParentDN,    /* dn of the new parent */
                        1,              /* delete old rdn */
                        NULL,           /* server controls */ 
                        NULL );         /* client controls */

    if ( rc != LDAP_SUCCESS )
    {
        printf("ldap_rename_s: %s\n", ldap_err2string( rc ) );
        ldap_unbind_s( ld );
        return(1);
    }
    printf("\n\t%s was renamed successfully.\n", dn );

    ldap_unbind_s( ld );

    return( 0 );
}

