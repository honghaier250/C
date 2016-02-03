/* $Novell: sortcntl.c,v 1.9 2003/05/12 13:07:52 $ */
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
   sortcntl.c
***************************************************************************
   Description: The sorcntl.c sample sets up a server-side sort control, 
                searches the directory, and prints the entry name, sn, 
                and given name.
                
                This sample also demonstrates how to return only 
                specified attributes.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#include <string.h>

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

int isServerSortControlSupported(LDAP *ld);

static char usage[] =
"\n Usage:   sortcntl <host name> <port number> <login dn> <password>"
"\n\t  <search base>\n" 
"\n Example: sortcntl acme.com 389 cn=admin,o=acme secret ou=Sales,o=acme\n";

int main( int argc, char **argv)
{
    int   ldapPort;
    char  *ldapHost, *loginDN, *password, *searchBase;
    LDAP  *ld;
    int   rc;
    int   version;
    int   err;
    unsigned long err1;
    char  *dn = NULL;
    char  **values = NULL;
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */
    LDAPSortKey **sortkeylist = NULL;
    LDAPMessage *searchResult = NULL;
    LDAPMessage *entry = NULL;
    LDAPControl *serverctrls[2] = {NULL, NULL};    /* Ensure null-terminated */
    LDAPControl **resultControls = NULL;

    /*
     * Search parameters.
     */
    char  *attrs[] = {"givenname","sn", NULL };     /* Attributes to return */       
    char  *filter = "(objectclass=inetorgperson)";  /* Search filter */
    int   attrsonly = 0;                            /* Return attrs and values*/
    
    /*
     * Sort control parameters.
     */
    char  *sortAttr = "sn";                        /* Attribute to sort on */
    int   isCritical = 1;                          /* Control is critical */
    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
        printf("%s", usage);
        return ( 1 );
    }
    
    ldapHost   = argv[1];
    ldapPort   = atoi(argv[2]);
    loginDN    = argv[3];
    password   = argv[4];
    searchBase = argv[5];
    
    printf("\nSort key:  %s\n", sortAttr);
        printf("Control is CRITICAL\n\n");
    
    
     /* Set LDAP version to 3 and set connection timeout. */
     version = LDAP_VERSION3;
     ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
     ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* 
     * Initialize an LDAP session
     */
    ld = ldap_init( ldapHost, ldapPort );
    if (NULL == ld)
    {
        printf("\n\tLDAP session initialization failed\n");
        return ( 1 );
    }

    /* Read rootDSE of the LDAP server; find out whether Server-side
       Sort Control is supported.  Otherwise quit */
    if (!isServerSortControlSupported(ld))
    {
        printf("Server-side Sort Control is not supported by the LDAP Server.\n");
        return ( 1 );
    } 
    printf("Server-side Sort Control is supported by the LDAP Server ... \n");
    
    /* 
     * Authenticate to the server 
     */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (LDAP_SUCCESS != rc)
    {
        printf("\n\tldap_simple_bind_s: %s\n", ldap_err2string(rc));
        ldap_unbind_s( ld );
        return ( rc );
    }
        
    /* 
     * Create the sort key structure specifying the attribute to sort on. 
     */
    rc = ldap_create_sort_keylist(&sortkeylist, sortAttr);
    if (LDAP_SUCCESS != rc)
    {
        printf("\n\tldap_create_sort_keylist: %s\n", ldap_err2string(rc));
        goto cleanup;
    }

    /* 
     * Create the sort control. 
     */
    rc = ldap_create_sort_control(
            ld,                         /* Session handle */
            sortkeylist,                /* Array of sortkey structs */
            isCritical,                 /* Criticality of this control */
            &serverctrls[0]);           /* The returned control structure */                        
    if (LDAP_SUCCESS != rc)
    {
        printf("\n\tldap_create_sort_control: %s\n",ldap_err2string(rc));
        goto cleanup;
    }
    
    /*
     * Perform the search with server-side sorting.
     */
    rc = ldap_search_ext_s( 
            ld,                         /* Session handle */
            searchBase,                 /* Container to search */
            LDAP_SCOPE_ONELEVEL,        /* Search this container only */
            filter,                     /* Search filter to use */
            attrs,                      /* Attributes to return */
            attrsonly,                  /* Return just attr names? */
            serverctrls,                /* Server controls */
            NULL,                       /* No client controls */
            &timeOut,                   /* Search timeout */
            LDAP_NO_LIMIT,              /* No size limit */ 
            &searchResult);             /* Returned results */

    if (LDAP_SUCCESS != rc)
    {
        printf("\n\tldap_search_ext_s: %s\n", ldap_err2string(rc));
        goto cleanup;
    }

    /*
     * Parse the search results to obtain the return controls.
     */
    rc = ldap_parse_result(
            ld,                         /* Session handle */
            searchResult,               /* Results from the search operation */
            &err,                       /* Error code of the search operation */
            NULL,                       /* Matched DN */
            NULL,                       /* Error message */
            NULL,                       /* Referrals */ 
            &resultControls,            /* Returned controls */
            0);                         /* Don't free the searchResult */
    
    if (LDAP_SUCCESS != rc)
    {
        printf("\n\tldap_parse_result: %s\n", ldap_err2string(rc));
        goto cleanup;
    }

    /* 
     * Parse the returned sort control.
     */
    rc = ldap_parse_sort_control(
            ld,                     /* Session handle */                   
            resultControls,         /* Results from the search operation */
            &err1,                  /* Error code for the SSS control */   
            NULL);                  /* Attribute involved in the error */  

    if (LDAP_SUCCESS != rc)
    {
        printf("\n\tldap_parse_sort_control: %s\n", ldap_err2string(rc));
        goto cleanup;
    }
    if (err)
    {
        /* 
         * If sort control is not critical, the search continues.
         */
        printf("*** Sort control error: %s\n\n", ldap_err2string((int)err));
        printf("    The control was ignored.\n");
    }


    /* 
     * For each entry, print out dn, sn, and givenname.
     * Only print first value if there are multiple values.
     */
    printf("Entry <dn>:   <sn>, <given name>\n\n");
    for ( entry = ldap_first_entry( ld, searchResult ); 
          entry != NULL;
          entry = ldap_next_entry( ld, entry ) ) 
    {
        dn = ldap_get_dn( ld, entry );
        if (dn)
        {
            printf("Entry %s:\t",dn);
            ldap_memfree(dn);
        }

        if ((values = ldap_get_values( ld, entry, "sn")) != NULL ) 
        {
            if ( values[0] != NULL )    
                printf("%s,  ", values[0] );
            ldap_value_free( values );
        }
   
        if ((values = ldap_get_values( ld, entry, "givenname")) != NULL ) 
        {
           if ( values[0] != NULL ) 
               printf("%s", values[0] );
           ldap_value_free( values );
        }
        printf("\n" );
    }
            
cleanup:
    ldap_msgfree( searchResult );
    ldap_control_free( serverctrls[0] );
    ldap_controls_free( resultControls );
    ldap_free_sort_keylist( sortkeylist );
    ldap_unbind_s( ld );
   
    return( rc );
}

int isServerSortControlSupported(LDAP *ld)
{
    int         i, rc         = LDAP_SUCCESS;
    char        *attribute    = NULL;
    char        **values      = NULL;
    BerElement  *ber          = NULL;
    LDAPMessage *searchResult = NULL;
    LDAPMessage *entry        = NULL;
    char        *attribs[]    = {"supportedControl",NULL};

    /* Reading the root DSE */
    rc = ldap_search_ext_s(ld, "" , LDAP_SCOPE_BASE, "(objectClass=*)",
                             attribs, 0, NULL, NULL, NULL, 
                             LDAP_NO_LIMIT, &searchResult);

    if( LDAP_SUCCESS != rc)
    {
        printf("ldap_search_ext_s: %s\n", ldap_err2string( rc ));
        ldap_msgfree( searchResult );
        ldap_unbind_s( ld );
        return ( 0 );
    }

    entry = ldap_first_entry( ld, searchResult );

    attribute = ldap_first_attribute( ld, entry, &ber );
    if (attribute == NULL)
    {
        printf("ldap_first_attribute returned no results.\n");
        ldap_msgfree( searchResult );
        ldap_unbind_s( ld );
        return ( 0 );
    }

    /* Get supportedControl attribute.  Check for the presence of 
       Server-side Sort Control's OID, 1.2.840.113556.1.4.473
       Assume all values are strings. */
    if (( values = ldap_get_values( ld, entry, attribute)) != NULL )
    {
        for ( i = 0; values[i] != NULL; i++ )
            if (strcmp(values[i], "1.2.840.113556.1.4.473") == 0)
            {
                rc = 1;
                break;
            }
        ldap_value_free( values );
    }

    ldap_memfree( attribute );

    ldap_msgfree( searchResult );

    ber_free(ber, 0);

    return rc;
}

