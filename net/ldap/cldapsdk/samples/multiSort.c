/* $Novell: multiSort.c,v 1.6 2003/05/12 12:55:08 $ */ 
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
   multiSort.c 
***************************************************************************
   Description: The multiSort.c sample searches the directory, sorts the 
                results by the given attribute list, and prints the results.

   NOTE:        For simplicity, this sample prints only values of attributes
                that are part of sort list. 

                If no sort attribute is supplied, all attributes values will 
                be printed.
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage: multiSort <host name> <port number> <login dn> <password>"
"\n        <search base> [ <sort attribute list> ]\n\n"
" Where <sort attribute list> is a set of attributes to use when \n"
" sorting the results. If none supplied, sorting is performed on \n"
" distinguished name\n"
"\n Examples:\n"
" #1. multiSort Acme.com 389 cn=admin,o=Acme secret ou=Sales,o=Acme\n"
"               sn title city\n"
" #2. multisort Acme.com 389 cn=admin,o=Acme secret ou=Sales,o=Acme\n";

int main( int argc, char **argv )
{
    int         version, ldapPort, i, rc, entryCount, sortAttrCount;
    char        *ldapHost, *loginDN, *password, *searchBase;
    char        *attribute, *dn, **values, **sortAttrs=NULL;
    BerElement  *ber;
    LDAP        *ld;
    LDAPMessage *searchResult, *entry;
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc < 5)
    {
        printf("%s", usage);
        return (1);
    }

    ldapHost      = argv[1];
    ldapPort      = atoi(argv[2]);
    loginDN       = argv[3];
    password      = argv[4];
    searchBase    = argv[5];
    sortAttrCount = argc-6;

    /* Create an array of entries specifying the attribute(s) to sort on.
       If no entry specified, sort is performed on distinguished name */
    if ( sortAttrCount > 0 )
    {
        sortAttrs = (char**)malloc(sizeof(char*)*(sortAttrCount+1));
        for (i=0; i<sortAttrCount;i++)
            sortAttrs[i]  = argv[i+6];

        sortAttrs[i] = NULL;
    }

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n    LDAP session initialization failed\n");
        rc = 1 ;
        goto cleanup;
    }
    printf ( "\n    LDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        goto cleanup;
    }
    printf("\n    Bind successful\n");

    /* Search the directory */
    rc = ldap_search_ext_s(
                    ld,                    /* LDAP session handle */
                    searchBase,            /* container to search */
                    LDAP_SCOPE_ONELEVEL,   /* search scope */
                    "(objectclass=*)",     /* search filter */
                    sortAttrs,             /* return only sort attributes */
                    0,                     /* return attributes and values */
                    NULL,                  /* server controls */
                    NULL,                  /* client controls */
                    &timeOut,              /* search timeout */
                    LDAP_NO_LIMIT,         /* no size limit */
                    &searchResult );       /* returned results */

    if ( rc != LDAP_SUCCESS )
    {
        printf("ldap_search_ext_s: %s\n", ldap_err2string( rc ));
        ldap_msgfree( searchResult );
        goto cleanup;
    }

    printf("\n    Sort key(s):\n");
    if ( sortAttrs )
        for (i=0; i < sortAttrCount; i++)
            printf("        %s\n", sortAttrs[i]); 
    else
        printf("        NULL\n");

    /* client-multisort */
    rc = ldap_multisort_entries( ld, &searchResult, (char const **) sortAttrs, strcmp );
    if ( rc != LDAP_SUCCESS )
    {
        printf("ldap_multisort_entries: %s\n", ldap_err2string( rc ));
        goto cleanup;
    }

    /* Go through the search results by checking entries */
    for (   entry   =   ldap_first_entry( ld, searchResult );
            entry   !=  NULL;
            entry   =   ldap_next_entry( ld, entry ) )
    {
        if (( dn = ldap_get_dn( ld, entry )) != NULL )            
        {
            printf("\n    dn: %s\n", dn );
            ldap_memfree( dn );
        }

        for (   attribute = ldap_first_attribute( ld, entry, &ber );
                attribute != NULL;
                attribute = ldap_next_attribute( ld, entry, ber ) )
        {
            /* Get values and print.  Assumes all values are strings. */
            if (( values = ldap_get_values( ld, entry, attribute)) != NULL )
            {
                for ( i = 0; values[i] != NULL; i++ )
                    printf("        %s: %s\n", attribute, values[i] );
                ldap_value_free( values );
            }
            ldap_memfree( attribute );
        }

        ber_free(ber, 0);

    }

    entryCount = ldap_count_entries( ld, searchResult );

    printf("\n    Sort completed successfully.\n    Entries  returned: %d\n",
             entryCount);

    ldap_msgfree( searchResult );

cleanup:
    if ( sortAttrs ) free ( sortAttrs );
    if(ld)
	ldap_unbind_s( ld );
    return( rc );
}

