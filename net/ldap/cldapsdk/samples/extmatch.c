/* $Novell: extmatch.c,v 1.10 2003/05/12 12:19:53 $ */
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
   extMatch.c 
***************************************************************************
   Description: The extMatch.c sample constructs the extensible match 
                filter with the given inputs,  searches the directory and 
                prints the results.  Extensible match requires eDirectory
                version 8.7 or higher.
   
   The LDAP v3 Core protocol specification requires LDAP servers to 
   recognize a search element called an extensible match. Extensible
   match provides additional features which can be expressed in the
   search filter.
   
   Feature 1. Allow matching to components of the object's Distinguished Name 
              as well as attributes of the object.
        
   Feature 2. Allow a specific matching rule to be used for an attribute-value 
              comparison, such as a sounds-alike match.
        
   Feature 3. Allow a match to any attribute of an object supporting a 
              particular matching rule.  For example, one could search for 
              objects with any string attribute containing "Dino".
   
   These features may be used independently or in combination.

   All the supported matching rules are advertised through LDAP subschema
   subentry object using the standard matchingRules and matchingRuleUse
   schema attributes.

   It is mandatory that either dnAttribute or matching rule be present
   as part of extensible match filter. 
   
   NOTE:  In eDirectory 8.7, only the DN feature of extensible match 
   is supported (feature 1).  
   
   The following examples illustrate the use of extensible match filters
   and correspond to the features listed above.

   Example #1: (ou:dn:=Sales)
   Same as filter "(ou=Sales)" except that DN components should be 
   considered part of the entry when doing the match.  Any object 
   with "ou=Sales" as part of its DN will match.

   Example #2: (cn:2.4.6.8:=Barney Rubble)
   Specifies matching rule "2.4.6.8" to be used when making comparisons
   for (cn="Barney Rubble").

   Example #2a: (cn:dn:2.4.6.8:=Barney Rubble)
   Combines examples #1 and #2.  Specifies the matching rule to use and 
   indicates that components of entry's DN should be considered attributes of 
   the entry when evaluating the match.

   Example #3: (:2.4.6.8:=Dino)
   Indicates this filter should be applied to all attributes supporting 
   this matching rule.
   
   Example #3a: (:dn:2.4.6.8:=Dino)
   Same as example #3, but also includes components of the DN in the match.
   
   For further details, refer to IETF RFC 2251 and RFC 2254.

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\nextMatch [-d] <host name> <port number> <login dn> <password>"
"\n         <search base> <attribute name> <matching rule> <match value>"
"\n where:"
"\n    -d means consider DN components as attributes of the object"
"\n    <search base> is the starting container for a subtree search"
"\n    <attribute name> is the attribute to match against, or \"\" for all attrs"
"\n    <matching rule> is an OID specifying the matching rule to use, or \"\""
"\n    <match value> is the value to compare against in the search filter\n"
"\nExamples:"
"\n #1. extMatch -d Acme.com 389 cn=admin,o=Acme secret o=Acme ou \"\" Sales"
"\n #2. extMatch Acme.com 389 cn=admin,o=Acme secret"
"\n              ou=Sales,o=Acme cn 2.4.6.8 \"Barney Rubble\""
"\n #3. extMatch Acme.com 389 cn=admin,o=Acme secret o=Acme \"\" 2.4.6.8 Dino"
"\n              NOTE: Only example 1 is supported by eDirectory 8.7\n";

int main( int argc, char **argv )
{   
    int         version = 0, ldapPort = 0, rc = 0;
    size_t      len;
    char        *ldapHost, *loginDN, *password, *searchBase, *dn;
    char        *matchAttribute, *matchingRule, *matchValue,  *extMatchFilter;
    LDAP        *ld;
    LDAPMessage *searchResult, *entry;
    char        *attrs[] = { LDAP_NO_ATTRS, NULL };   /* Return DN names only */
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */

    int         useDN = 0;         /* 1 if to use dnAttribute */
    int         useRule = 0;       /* 1 if matching rule is given */
    int         useAttribute = 0;  /* 1 if attribute value is given */
   
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    /* Determine which options are set */
    if (argc > 1) {
        /* Interrogate options until we get something else */
        for( ;; ) {
            /* -d option inserts ":dn" in the filter */
            if( strcmp( argv[1],"-d") == 0 ) {
                useDN = 1;
                argc--; argv++;
                continue;
            }
            /* An option, but not -d option */
            if( argv[1][0] == '-') {
                printf("%s", usage);
                return -1;
            }
            break;
        }
    }

    /* Validate number of arguments against options chosen. */
    if (argc == 9) {
        ldapHost        = argv[1];
        ldapPort        = atoi(argv[2]);
        loginDN         = argv[3];
        password        = argv[4];
        searchBase      = argv[5];
        matchAttribute  = argv[6];
        matchingRule    = argv[7];
        matchValue      = argv[8];
    } else {
        printf("%s", usage);
        return -1;
    }

    if (*matchAttribute) 
       useAttribute = 1;
 
    if (*matchingRule)
       useRule = 1;

    /* Must have dn option or matching rule. */
    if (useDN==0 && useRule==0) {
        printf("\n Error: Either <attribute name> or <matching rule> must be\n"
               " present as part of an extensible match filter.\n\n");
        printf("%s", usage);
        return -1;
    }
    
    /* Allocate enough memory for "(attr:dn:rule:=value)" */
    len = 8 + strlen(matchValue);     /* "(:dn:=value)<null>" */
    if (useAttribute)
        len += strlen(matchAttribute);
    if (useRule)
        len += strlen(matchingRule);

    extMatchFilter = (char*)malloc(len);
    if (extMatchFilter == NULL) {
        printf("Memory allocation error\n");
        return -1;
    }

    /* Construct extensible matching filter*/
    strcpy(extMatchFilter, "(");
    if (useAttribute)
         strcat(extMatchFilter, matchAttribute);
    if (useDN) 
         strcat(extMatchFilter, ":dn");
    if (useRule) {
        strcat(extMatchFilter, ":");
        strcat(extMatchFilter, matchingRule);
    }
    strcat(extMatchFilter, ":=");
    strcat(extMatchFilter, matchValue);
    strcat(extMatchFilter, ")");

    printf("\n    Extensible match filter:  %s\n", extMatchFilter);

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n    LDAP session initialization failed\n");
        goto cleanup;
    }
    printf("\n    LDAP session initialized\n");

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
                    LDAP_SCOPE_SUBTREE,    /* search scope */
                    extMatchFilter,        /* extensible match search filter */
                    attrs,                 /* attributes to return */
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
     
    /* Read and print matched entry names. */
    for (   entry   =   ldap_first_entry( ld, searchResult );
            entry   !=  NULL; 
            entry   =   ldap_next_entry( ld, entry ) ) 
    {
        if (( dn = ldap_get_dn( ld, entry )) != NULL )
        {
            printf("\n   dn: %s\n", dn );
            ldap_memfree( dn );
        }
    }

    printf("\n    Extensible match search completed successfully.\n");
   
    ldap_msgfree( searchResult );

cleanup:
    free( extMatchFilter );
    if(ld)
	ldap_unbind_s( ld );

    return rc;
}

