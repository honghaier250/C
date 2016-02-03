 /* $Novell: list.c,v 1.7 2003/05/12 12:22:09 $ */
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
   list.c 
***************************************************************************
   Description: The list.c sample lists all objects in a container.
                No attributes are read.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   list <host name> <port number> <login dn> <password>"
"\n          <search base>\n" 
"\n Example: list Acme.com 389 cn=admin,o=Acme secret ou=Sales,o=Acme\n";

int main( int argc, char **argv)
{   
    int         version, ldapPort, rc;
    char        *ldapHost, *loginDN, *password, *searchBase;   
    char        *dn;       
    LDAP        *ld;
    LDAPMessage *searchResult, *entry;
    char        *attrs[] = { LDAP_NO_ATTRS, NULL };
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */
   
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
        printf("%s", usage);
        return (1);
    }
           
    ldapHost          = argv[1];
    ldapPort          = atoi(argv[2]);
    loginDN           = argv[3];
    password          = argv[4];
    searchBase        = argv[5];    

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n  LDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n  LDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("  Bind successful\n\n");


    /* Search the directory */
    rc = ldap_search_ext_s(  
                    ld,                    /* LDAP session handle */
                    searchBase,            /* container to search */
                    LDAP_SCOPE_ONELEVEL,   /* search scope */
                    "(objectclass=*)",     /* search filter */
                    attrs,                 /* "1.1" returns entry names only*/
                    0,                     /* no attributes are returned */
                    NULL,                  /* server controls */
                    NULL,                  /* client controls */
                    &timeOut,              /* search timeout */
                    LDAP_NO_LIMIT,         /* no size limit */
                    &searchResult );       /* returned results */
    
    if ( rc != LDAP_SUCCESS ) 
    {
        printf("ldap_search_ext_s: %s\n", ldap_err2string( rc ));
        ldap_msgfree( searchResult );
        ldap_unbind_s( ld );
        return ( 1 );
    }   
     
    /* Go through the search results by checking entries */
    for (   entry   =   ldap_first_entry( ld, searchResult ); 
            entry   !=  NULL; 
            entry   =   ldap_next_entry( ld, entry ) ) 
    {
        if (( dn = ldap_get_dn( ld, entry )) != NULL )            
        {
            printf("  dn: %s\n", dn );
            ldap_memfree( dn );
        }
    }

    ldap_msgfree( searchResult );
    
    ldap_unbind_s( ld );  
 
    return( 0 );
}

