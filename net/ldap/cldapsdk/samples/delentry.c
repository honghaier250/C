/* $Novell: delentry.c,v 1.8 2003/05/12 12:17:41 $ */
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
   delentry.c
***************************************************************************
   Description: The delentry.c sample deletes the entry with the 
                distinguished name provided by the user.
***************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   delentry <host name> <port number> <login dn> <password>"
"\n\t  <deleteDN>" 
"\n Example: delentry acme.com 389 cn=admin,o=acme secret"
"\n\t  cn=james,ou=sales,o=acme\n";

int main( int argc, char **argv)
{ 

    int         version, ldapPort, rc; 
    char        *ldapHost, *loginDN, *password, *deleteDN;        
    LDAP        *ld;
    struct timeval timeOut = {10,0};    /* 10 second connection timeout */


    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost = argv[1];
    ldapPort = atoi(argv[2]);
    loginDN  = argv[3];
    password = argv[4];
    deleteDN = argv[5];

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


    /* Perform the delete operation. */
    rc = ldap_delete_ext_s( ld,         /* LDAP session handle */
                            deleteDN,   /* dn of the object to delete */
                            NULL,       /* server controls */
                            NULL );     /* client controls */

    if ( rc != LDAP_SUCCESS )
    {
        printf("\n\tldap_delete_ext_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return(1);
    }
    printf("\n\tEntry deleted successfully: %s\n", deleteDN );

    ldap_unbind_s( ld );

    return( 0 );
}

