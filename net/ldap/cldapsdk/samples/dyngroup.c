/* $Novell: dyngroup.c,v 1.11 2003/05/12 12:18:46 $ */
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
   dyngroup.c
***************************************************************************
   Description:

   The dyngroup.c sample demonstrates how to:
   1) create a Dynamic Group object (cn=myDynamicGroup) in a specified 
      container with a specified memberQueryURL
   2) read the "member" attribute of myDynamicGroup and print out values
   3) delete the dynamic group 

   
   "Dynamic groups" are like normal groups, but they let one specify 
   criteria to be used for evaluating memberships to a group; the 
   membership of the group is determined dynamically by the directory 
   servers involved.  This lets the group administrator define the 
   membership in terms of attributes, and let the DSA worry about what is
   an actual member. 

   In other words, a dynamic group is similar to a group object.  But 
   in a dynamic group, a search filter is specified.  Any object passing the 
   search filter is considered a member of the group and will be returned
   when reading the "member" attribute.

   Dynamic groups are supported in Novell's eDirectory version 8.6.1 or later.

   A dynamic group is created with object class = "dynamicGroup" or by adding
   the auxilliary class "dynamicGroupAux" to an existing group object.

   The search filter is specified in "memberQuery" attribute.  The value
   of "memberQuery" is encoded as an "LDAP URL".

   A dynamic group is a subtype of a static group.  Hence members can be
   included statically using "uniqueMember" attribute.

   Similarly members can be excluded statically from a dynamic group using
   "excludedMember" attribute.

   In order to provide consistent results when processing the search criteria,
   the server uses a single authorization entry.  The "dgIdentity" attribute
   holds this authorization entry.  The server decides the entry to be used 
   based on the follwing conditions:
   1) If the dynamic group object contains a public/private key, then the group
      object will be used as the identity.
   2) If no public/private key exists for the group, then the "dgIdentity" 
      attribute will be used as the identity.
   3) If no dgIdentity attribute is present on the group, then the implied
      search is done using the anonymous identity.
   
   The "dgAllowDuplicates" attribute enables/disables duplicate value presence
   in the Dynamic Group entry.  This is a boolean attribute and can be set to
   true or false.  The default is false.  Setting this attribute to true results
   in a faster search, but some values may be duplicates.

   The "dgAllowUnknown" boolean attribute determines the behaviour when the
   Dynamic Group members are not fully expandable for some reason, such as the
   referred server is not accessible. By default it is false.

   The "dgTimeout" attribute determines how long to wait to get results from
   another server during Dynamic Group member search, when the search spans
   across servers.
   
   The search filter is specified in LDAP URL form and is contained in
   the "memberQuery" attribute.  The format of this URL is:
   ldap:///<base dn>??<scope>?<filter>[?[!]x-chain]
 
   The optional extension "x-chain" causes the server to chain to
   other servers if necessary to complete the search.  In its absence,
   the search will be limited to the host server.  The exclamation
   indicates it is a critical extension, and the server will return
   an error if chaining is not supported.
 
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   dyngroup <host name> <port number> <login dn> <password>"
"\n          <container name> <queryURL> \n" 
"\n Example: dyngroup Acme.com 389 cn=admin,o=Acme secret"
"\n          o=Acme ldap:///ou=Sales,o=Acme??sub?(title=*Manager*)\n";

int main( int argc, char **argv)
{
    int         version, ldapPort, i, rc; 
    char        *ldapHost, *loginDN, *password, *containerName, *queryURL;
    char        *vclass[2], *vURL[2], *vdgIdentity[2], *requiredAttributes[2];
    char        *attribute, **values, *dn=NULL;
    BerElement  *ber;
    LDAP        *ld;
    LDAPMod     modClass, modMemberQuery, modDgIdentity; 
    LDAPMod     *mods[4];
    LDAPMessage *searchResult=NULL, *entry;
    
    /* Since reading members of a dynamic group could potentially involve
     * a significant directory search, we use a timeout.
     */
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */

    /* Name of the new dynamic group object to create. (RDN)
     * Includes a trailing comma.
     */
    char        *newGroupRDN = "cn=myDynamicGroup,";

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 7)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost        =   argv[1];
    ldapPort        =   atoi(argv[2]);
    loginDN         =   argv[3];
    password        =   argv[4];
    containerName   =   argv[5];
    queryURL        =   argv[6];

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf("\n   LDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n   LDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("\n   Bind successful\n");

    /*
     * To add an entry to the directory:
     *   1. Create the array of LDAPMod structures representing the
     *      attributes of the new entry 
     *   2. Specify the DN of the entry to create
     *   3. Call ldap_add_ext_s to add the entry to the directory
     */      

    /* The objectclass "dynamicGroup" is used to create dynamic group objects */
    modClass.mod_op               =   LDAP_MOD_ADD;
    modClass.mod_type             =   "objectClass";
    vclass[0]                     =   "dynamicGroup";
    vclass[1]                     =   NULL;
    modClass.mod_values           =   vclass;
    
    /* The memberQuery attribute describes the membership of the list using
     * an LDAPURL, which is defined in RFC2255. 
     */
    modMemberQuery.mod_op      =   LDAP_MOD_ADD;
    modMemberQuery.mod_type    =   "memberQuery";
    vURL[0]                       =   queryURL;
    vURL[1]                       =   NULL;
    modMemberQuery.mod_values  =   vURL;


    /* Set the identity to use for the implied search.  
     * loginDN is used as the dgIdentity in this sample.
     */
    modDgIdentity.mod_op          =   LDAP_MOD_ADD;
    modDgIdentity.mod_type        =   "dgIdentity";
    vdgIdentity[0]                =   loginDN;
    vdgIdentity[1]                =   NULL;
    modDgIdentity.mod_values      =   vdgIdentity;

    mods[0] = &modClass;
    mods[1] = &modMemberQuery;
    mods[2] = &modDgIdentity;
    mods[3] = NULL;

    /*
     * Construct the object's dn using the container name from the
     * command line
     */
    dn=(char*)malloc(strlen(newGroupRDN) + strlen(containerName) + 1);
    strcpy( dn, newGroupRDN );
    strcat( dn, containerName);

    /*
     * Now add the object
     */
    rc = ldap_add_ext_s( ld,     /* LDAP session handle */ 
                         dn,     /* dn of the object to be created */
                         mods,   /* array of add options and values */
                         NULL,   /* server control */
                         NULL ); /* client control */

    if ( rc != LDAP_SUCCESS )
    {
        printf("\n   ldap_add_ext_s: %s\n", ldap_err2string( rc ));
        if (rc == LDAP_UNDEFINED_TYPE)
            printf("      Dynamic Groups require eDirectory 8.6.1 or later.\n");
        free( dn );
        ldap_unbind_s ( ld );
        return(1);
    }
    
    printf("\n   Dynamic Group entry successfully added: %s\n", dn);

    printf("\n   Reading the member attribute of dynamic group object:\n");

   /* The "member" attribute is used to interrogate both the static and dynamic
    * member values of a dynamic group object. 
    * 
    * Querying for "member" attribute.
    */
    requiredAttributes[0]="member";
    requiredAttributes[1]=NULL;

   /* Search the directory */
    rc = ldap_search_ext_s(
                    ld,                    /* LDAP session handle */
                    dn,                    /* entry to read */
                    LDAP_SCOPE_BASE,       /* search scope */
                    "(objectclass=*)",     /* search filter */
                    requiredAttributes,    /* attributes to return */
                    0,                     /* return attributes and values */
                    NULL,                  /* server controls */
                    NULL,                  /* client controls */
                    &timeOut,              /* search timeout */
                    LDAP_NO_LIMIT,         /* no size limit */
                    &searchResult );       /* returned results */

    if ( rc != LDAP_SUCCESS )
    {
        printf("ldap_search_ext_s: %s\n", ldap_err2string( rc ));
        free( dn );
        ldap_msgfree( searchResult );
        ldap_unbind_s( ld );
        return ( 1 );
    }

    /* Read and print search results.  We expect only one entry and attr. */
    if ( (entry = ldap_first_entry( ld, searchResult )) != NULL)
    {
        if ( (attribute = ldap_first_attribute( ld, entry, &ber )) != NULL)
        {
            /* Get values and print. */
            values = ldap_get_values( ld, entry, attribute);
            if (values != NULL)                
            {
                for ( i = 0; values[i] != NULL; i++ )
                    printf("        %s: %s\n", attribute, values[i] );
                ldap_value_free( values );
            }
            ldap_memfree( attribute );
        }
		else
		{
		    printf("     No objects passed the memberQuery filter.\n"
                   "     Hence, there are no members in this group.\n");
		}
        ber_free(ber, 0);
    }
    ldap_msgfree( searchResult );

    /* Delete the dynamic group */
    rc = ldap_delete_ext_s( ld,         /* LDAP session handle */
                            dn,         /* dn of the object to delete */
                            NULL,       /* server controls */
                            NULL );     /* client controls */

    if ( rc != LDAP_SUCCESS )
        printf("\n   ldap_delete_ext_s: %s\n", ldap_err2string( rc ));
    else
        printf("\n   Dynamic Group entry successfully deleted: %s\n\n", dn);

    free ( dn );
    ldap_unbind_s( ld );

    return( rc );
}

