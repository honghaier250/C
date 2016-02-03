/* $Novell: vlvcntl.c,v 1.14 2003/05/12 13:11:49 $ */
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
   vlvcntl.c 
***************************************************************************
   Description: The vlvcntl.c sets up a server-side sort control and 
                a virtual list view control, searches 
                the directory, and prints the results.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#include <string.h>

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

int isVLVSupported(LDAP *ld);

static char usage[] =
"\n Usage:   vlvcntl <host name> <port number> <login dn> <password>"
"\n          <search base>\n" 
"\n Example: vlvcntl acme.com 389 cn=admin,o=acme secret ou=sales,o=acme\n";

int main( int argc, char **argv)
{
    int   ldapPort;
    char  *ldapHost, *loginDN, *password, *searchBase;
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */
    LDAP  *ld;
    int   rc;
    int   version=3;
    int   err;
    unsigned long err1;
    char  *dn = NULL;
    char  **values = NULL;
    LDAPSortKey **sortkeylist = NULL;
    LDAPMessage *searchResult = NULL;
    LDAPMessage *entry = NULL;
    LDAPControl *serverctrls[3] = {NULL, NULL, NULL};  /* Null-terminated */
    LDAPControl **resultControls = NULL;
    LDAPVLVInfo vlvInfo;

    /*
     * Sort control parameters.
     */
    char  *sortAttr = "sn";                        /* Attribute to sort on */
    char  *attrs[] = {"givenname","sn", NULL };    /* Attributes to return */       
    int   attrsonly = 0;                           /* Return attrs and values */
    int   isCritical = 1;                          /* Control is critical */
    char  *searchFilter = "(sn=*)";                /* Search filter to use */
    /*
     * VLV control parameters.  If typedown is NULL, index mode is used.
     * The values below should return 4 entries, entry numbers 2 through 5, 
     * with the 3rd entry as the target.
     */
    unsigned long  target = 3;            /* Retrieve 3rd entry in list */
    unsigned long  before_count = 1;      /* Get one entry before target */
    unsigned long  after_count = 2;       /* Get two entries after the target */
    unsigned long  total_count = 0;       /* Estimate of list size. Init to 0 */
    struct berval  *pContext = NULL;      /* Initially set to NULL */

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
    
    /* Set LDAP version to 3 and set connection timeout. */
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);
     
    /* 
     * Initialize an LDAP session
     */
    ld = ldap_init( ldapHost, ldapPort );
    if (NULL == ld)
    {
        printf( "\n\tLDAP session initialization failed\n");
        return ( 1 );
    }        
    
    /* Retrieve the root DSE of the LDAP server; find out whether
       Virtual List View Control is supported.  Otherwise quit */

    if (!isVLVSupported(ld)) 
    {  
        printf("\nVirtual List View Control is not supported by the LDAP Server.\n");
		ldap_unbind_s(ld);
        return ( 1 );
    }

    printf("\nSort key:  %s\n", sortAttr);
    printf("Target index:  %d\nBefore count:  %d\nAfter count:   %d\n",
           target, before_count, after_count);
    printf("Client's total_count estimate:  %d\n", total_count);

    printf("\nVirtual List View Control is supported by the LDAP Server ... \n");

    /* 
     * Authenticate to the server 
     */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (LDAP_SUCCESS != rc)
    {
        printf("\n\tldap_simple_bind_s: %s\n", ldap_err2string(rc));
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
     * Create the sort control. sort control
     * must comes before vlv control
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
     * Create the VLV control.
     */
     vlvInfo.ldvlv_version = 1;                 /* Version of this structure */
     vlvInfo.ldvlv_attrvalue = NULL;            /* Typedown value*/
     vlvInfo.ldvlv_offset = target;             /* Index of desired entry */
     vlvInfo.ldvlv_before_count = before_count; /* # of entries before target */
     vlvInfo.ldvlv_after_count = after_count;   /* # of entries after target */
     vlvInfo.ldvlv_count = total_count;         /* Client's est. of list size */
     vlvInfo.ldvlv_context = pContext;          /* Result of previous control */
     vlvInfo.ldvlv_extradata = NULL;            /* Not used */
    
     rc = ldap_create_vlv_control(
            ld,                         /* Session handle */
            &vlvInfo,                   /* VLV information structure */
            &serverctrls[1]);           /* The returned control structure */
    
     if (LDAP_SUCCESS != rc)
    {
        printf("\n\tldap_create_vlv_control: %s\n",ldap_err2string(rc));
        goto cleanup;
    }

    /*
     * Perform the search with server-side sorting and vlv control.
     */
    printf("\nPerforming search with sort and VLV controls...\n");
    rc = ldap_search_ext_s( 
            ld,                            /* Session handle */
            searchBase,                    /* Container to search */
            LDAP_SCOPE_ONELEVEL,           /* Search this container only */
            searchFilter,                  /* Search filter to use */
            attrs,                         /* Attributes to return */
            attrsonly,                     /* Return attrs and values */
            serverctrls,                   /* Server controls */
            NULL,                          /* No client controls */
            &timeOut,                      /* Search timeout */
            LDAP_NO_LIMIT,                 /* No size limit */ 
            &searchResult);                /* Returned results */

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
            &err1,                   /* Error code for the vlv control */
            NULL);                  /* Attribute involved in the error */

    if (LDAP_SUCCESS != rc)
    {
        printf("\n\tldap_parse_sort_control: %s\n", ldap_err2string(rc));
        goto cleanup;
    }
    if (err)
        printf("*** Sort control error: %s\n\n", ldap_err2string((int)err1));


    /* 
     * Parse the returned VLV control.
     */
    rc = ldap_parse_vlv_control(
            ld,                     /* Session handle */
            resultControls,         /* Results from the search operation */
            &target,                /* Server's estimate of target position */
            &total_count,           /* Server's estimate of total count */
            &pContext,              /* Context returned for this search */
            &err);                  /* Error code for the vlv control */

    if (LDAP_SUCCESS != rc)
    {
        printf("\n\tldap_parse_vlv_control: %s\n", ldap_err2string(rc));
        goto cleanup;
    }
    if (err)
        printf("*** VLV control error: %s\n\n", ldap_err2string(err));
    else
    {
        printf("\nParse VLV control results:\n");
        printf("  Server's target index: %d\n", target);
        printf("  Server's total count: %d\n\n", total_count);
    }

    /* 
     * For each entry print out dn, sn, and givenname.
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
            printf("Object %s:\t",dn);
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
    ber_bvfree( pContext );
    ldap_msgfree( searchResult );
    ldap_control_free( serverctrls[0] );    /* Free the sort control */
    ldap_control_free( serverctrls[1] );    /* Free the vlv control */
    ldap_controls_free( resultControls );
    ldap_free_sort_keylist( sortkeylist );
    ldap_unbind_s( ld );
   
    return( rc );
}

int isVLVSupported(LDAP *ld)
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
       Virtaul List View's OID, 2.16.840.1.113730.3.4.9 
       Assume all values are strings. */
    if (( values = ldap_get_values( ld, entry, attribute)) != NULL )
    {
        for ( i = 0; values[i] != NULL; i++ )
            if (strcmp(values[i], "2.16.840.1.113730.3.4.9") == 0)
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

