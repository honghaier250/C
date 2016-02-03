/* $Novell: listgroup.c,v 1.16 2003/05/12 12:22:50 $ */
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
   listgroup.c 
***************************************************************************
   Description: The listgroup.c sample lists the members of the specified
                group.  If the entry is a dynamic group, the other 
                dynamic group attributes are also displayed.

                See the dyngroup.c sample for more information on
                dynamic groups.
                
                NOTE:  Dynamic groups are supported in eDirectory 
                       version 8.6.1 or later.

   Group Attributes:

        member (or it's synonym uniqueMember):  A list of the DNs of the 
            members of the group.   For dynamic groups, DN's added to this 
            attribute will be automatically included in the group regardless 
            of the search filter.
            
   Dynamic Group attributes:
           
        memberQueryURL:  Specifies parameters for the implied search when
            reading members of the group.  The query is in URL form:
                ldap:///<base dn>??<scope>?<filter>[?[!]x-chain]

            where:
                <base dn> is the starting point of the search
                <scope> is "one" for one-level, or "sub" for subtree search
                <filter> is the search filter
                x-chain indicates that the search will chain if necessary.
                    (Use with care, since it degrades performance.)

        excludedMember:  A list of the DN's specifically excluded from the 
            group regardless of the search filter.
            
        dgIdentity:  specifies the identity to use for the implicit search.
            If this attribute is not present, it uses the public/private key 
            of the group object, if present.
            Otherwise use the anonymous identity.

        dgAllowDuplicates:  Boolean attribute.  "true" speeds up the
            search, but the members list may have duplicates.
            
        dgTimeout:  Number of seconds to wait to get results from 
            another server when chaining.

***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   listgroup <host name> <port number> <login dn> <password>"
"\n          <group dn>\n" 
"\n Example: listgroup Acme.com 389 cn=admin,o=Acme secret"
"\n          cn=salesGroup,ou=sales,o=acme\n";

int main( int argc, char **argv )
{   
    int         version, ldapPort, i, rc;
    LDAP        *ld;
    char        *ldapHost, *loginDN, *password, *groupDN;   
    char        *attrs[] = { "objectClass", "memberQueryURL", "dgIdentity",
                             "dgAllowDuplicates", "dgTimeout", 
                             "excludedMember", "member", NULL };
    LDAPMessage *searchResult;
    char        *attribute;
    BerElement  *ber;
    char        **objClass=NULL, **identity=NULL, **queryURL=NULL;
    char        **allowDuplicates=NULL, **dgTimeout=NULL;
    char        **excludedMember=NULL, **member=NULL;    
    int         isGroup=0, isDynamicGroup=0;
    int         retStatus = 1;          /* Return status.  Assume error. */
    
    /* Since reading members of a dynamic group could potentially involve
       a significant directory search, we use a timeout. */
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
        printf( "%s", usage );
        return( 1 );
    }
           
    ldapHost          = argv[1];
    ldapPort          = atoi(argv[2]);
    loginDN           = argv[3];
    password          = argv[4];
    groupDN           = argv[5];    
    
    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf( "\n    LDAP session initialization failed\n" );
        return( 1 );
    }

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf( "ldap_simple_bind_s: %s\n", ldap_err2string( rc ) );
        goto cleanup;
    }

    printf( "\n    Reading object:  %s\n", groupDN );

    /* Read the specified attributes from the group object.  */
    rc = ldap_search_ext_s(  
                    ld,                    /* LDAP session handle */
                    groupDN,               /* object to read */
                    LDAP_SCOPE_BASE,       /* scope - read single object */
                    NULL,                  /* search filter */
                    attrs,                 /* attributes to read */
                    0,                     /* return attributes and values */
                    NULL,                  /* server controls */
                    NULL,                  /* client controls */
                    &timeOut,              /* search timeout */
                    LDAP_NO_LIMIT,         /* no size limit */
                    &searchResult );       /* returned results */
    
    if ( rc != LDAP_SUCCESS ) 
    {
        printf( "ldap_search_ext_s: %s\n", ldap_err2string( rc ) );
        ldap_msgfree( searchResult );
        goto cleanup;
    }   
     
    
    /* Examine the attributes that were returned and extract the data. */

    for (   attribute = ldap_first_attribute( ld, searchResult, &ber );
            attribute != NULL; 
            attribute = ldap_next_attribute( ld, searchResult, ber ) ) 
    {   
        
        /* Save objectClass values. Some vendors return "objectclass". */
        if (strcmp( attribute, "objectClass" ) == 0 ||
            strcmp( attribute, "objectclass" ) == 0 )
        {
            objClass = ldap_get_values( ld, searchResult, attribute );
        }

        /* Save the memberQueryURL attribute if present. */
        else if (strcmp( attribute, "memberQueryURL" ) == 0 )
        {
            queryURL = ldap_get_values( ld, searchResult, attribute );
        }

        /* Save the dgIdentity attribute if present. */
        else if (strcmp( attribute, "dgIdentity" ) == 0 )
        {
            identity = ldap_get_values( ld, searchResult, attribute );
        }

        /* Save the dgAllowDuplicates attribute if present. */
        else if (strcmp( attribute, "dgAllowDuplicates" ) == 0 )
        {
            allowDuplicates = ldap_get_values( ld, searchResult, attribute );
        }

        /* Save the dgTimeout attribute if present. */
        else if (strcmp( attribute, "dgTImeout" ) == 0 )
        {
            dgTimeout = ldap_get_values( ld, searchResult, attribute );
        }

        /* Save the excludedMember attribute if present. */
        else if (strcmp( attribute, "excludedMember" ) == 0)
        {
            excludedMember = ldap_get_values( ld, searchResult, attribute );
        }

        /* Save the member attribute.  This may also show up as uniqueMember. */
        else if (strcmp(attribute, "member") == 0 ||
                 strcmp(attribute, "uniqueMember") == 0)
        {
            member = ldap_get_values( ld, searchResult, attribute );
        }

        ldap_memfree( attribute );
    }
    ber_free( ber, 0 );
    ldap_msgfree( searchResult );
    
    
    /* Verify that this is a group object  (i.e. objectClass contains the 
       value "group", "groupOfNames", or "groupOfUniqueNames").
       Also determine if this is a dynamic group object (i.e. objectClass
       contains the value "dynamicGroup" or "dynamicGroupAux").
    */
    if (!objClass)
    {
        printf( "\n   Could not read objectClass of this object.  Exiting.\n");
        goto cleanup;
    }

    for ( i = 0; objClass[i] != NULL; i++ )
    {
        if ( strcmp( objClass[i], "group" ) == 0 ||
             strcmp( objClass[i], "groupOfNames" ) == 0 ||
             strcmp( objClass[i], "groupOfUniqueNames" ) == 0 )
            isGroup=1;
        else if ( strcmp(objClass[i], "dynamicGroup" ) == 0 ||
                  strcmp(objClass[i], "dynamicGroupAux" ) == 0 )
            isDynamicGroup = isGroup = 1;
    }
    
    if (!isGroup)
    {
        printf( "\n    This object is NOT a group object.  Exiting.\n" );
        goto cleanup;
    }
    
    /* If this is a dynamic group, display its attributes. */
    if (isDynamicGroup)
    {
        printf( "\n    This object is a dynamic group.\n" );

        if (queryURL)
        {
            printf( "\n        Member Query URL:\n" );
            for (i=0; queryURL[i]; i++)
                printf( "            %s\n", queryURL[i] );
        }

        if (identity && identity[0])
        {
            printf( "\n        Identity for search:\n            %s\n", 
                    identity[0] );
        }

        if (allowDuplicates && allowDuplicates[0])
        {
            printf( "\n        allowDuplicates:    %s\n", allowDuplicates[0] );
        }
        
        if (dgTimeout && dgTimeout[0])
        {
            printf( "\n        dgTimeout:   %s\n", dgTimeout[0] );
        }
        
        if (excludedMember)
        {
            printf( "\n        Excluded member list:\n" );
            for (i=0; excludedMember[i]; i++)
                printf( "            %s\n", excludedMember[i] );
        }
    }

    /* Print the group's member list */
    if (member!=NULL) {
        printf( "\n    Member list:\n" );
        for (i=0; member[i]; i++)
            printf( "         %s\n", member[i] );
    }
    else {
        printf("\n    %s has no members.\n\n", groupDN);
    }

    retStatus = 0;      /* success */


cleanup:
    ldap_value_free( objClass );
    ldap_value_free( queryURL );
    ldap_value_free( identity );
    ldap_value_free( allowDuplicates );
    ldap_value_free( dgTimeout );
    ldap_value_free( excludedMember );
    ldap_value_free( member );
    ldap_unbind_s( ld );  
 
    return( retStatus );
}

