/* $Novell: addUserToGroup.c,v 1.8 2003/05/12 12:05:20 $ */
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
   addUserToGroup.c 
***************************************************************************
   Description: This sample shows how to add a user to a group on eDirectory. 
                This includes four steps of attribute modifications:
                  1. add group's dn to user's groupMemberShip attribute.
                  2. add group's dn to user's securityEquals attribute.
                  3. add user's dn to group's uniqueMember attribute.
                  4. add user's dn to group's equivalentToMe attribute.
                After the modification, the security privileges that are
                granted to the group are now inherited by the user.
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

int _addUserToGroup( LDAP*, char* , char* );

static char usage[] =
"\n Usage:   addUserToGroup <host name> <port number> <login dn> <password>"
"\n          <user dn> <group dn>\n"
"\n Example: addUserToGroup acme.com 389 cn=admin,o=acme secret"
"\n          cn=JSmith,ou=sales,o=acme cn=salesGroup,ou=sales,o=acme\n";

int main( int argc, char **argv) {

    int  version, ldapPort, rc;
    LDAP *ld;
    char *ldapHost, *loginDN, *password,  *userDN, *groupDN;
    struct timeval timeOut = {10,0};    /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 7) {
        printf("%s", usage);
        return(1);
    }

    ldapHost = argv[1];
    ldapPort = atoi(argv[2]);
    loginDN  = argv[3];
    password = argv[4];
    userDN   = argv[5];
    groupDN  = argv[6];

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n\tLDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n\tLDAP session initialized\n");

    /* Bind to the server */
    if ( (rc = ldap_simple_bind_s( ld, loginDN, password )) != LDAP_SUCCESS) {
        printf("\n\tldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return (1);
    }
    printf("\n\tBind successful\n");

    /* call addUserToGroup() to add the user to the group */
    rc = _addUserToGroup( ld, userDN, groupDN );

    if ( rc != LDAP_SUCCESS )
        printf("\n\tUser: %s could not be enrolled in group: %s.\n",
                                                          userDN, groupDN);
    else
        printf("\n\tUser: %s was enrolled in group: %s.\n", userDN, groupDN);

    ldap_unbind_s( ld );

    return (0);
}

int _addUserToGroup( LDAP *ld, char *userdn, char *groupdn ) {
    int         rc, rc1;
    char        *userValues[2], *groupValues[2] ;
    LDAPMod     security, membership, *modUser[3];
    LDAPMod     equivalent, member, *modGroup[3];
    
    /* NULL-terminated modify values for user's attributes */
    userValues[0] = groupdn;
    userValues[1] = NULL;
    
    /* add group's dn to user's 'groupMembership' attribute */
    membership.mod_op = LDAP_MOD_ADD;
    membership.mod_type = "groupMembership";
    membership.mod_values = userValues;

    /* add group's dn to user's 'securityEquals' attribute */
    security.mod_op = LDAP_MOD_ADD;
    security.mod_type = "securityEquals";
    security.mod_values = userValues;
            
    /* NULL-terminated array of pointers for user modify structures */
    modUser[0] = &security;
    modUser[1] = &membership;
    modUser[2] = NULL;

    /* NULL-terminated modify values for group's attributes */
    groupValues[0] = userdn;
    groupValues[1] = NULL;

    /* add user's dn to group's 'uniqueMember' attribute */
    member.mod_op = LDAP_MOD_ADD;
    member.mod_type = "uniqueMember";
    member.mod_values = groupValues;

    /* add user's dn to group's 'equivalentToMe' attribute */
    equivalent.mod_op = LDAP_MOD_ADD;
    equivalent.mod_type = "equivalentToMe";
    equivalent.mod_values = groupValues;

    /* NULL-terminated array of pointers for group modify structures */
    modGroup[0] = &equivalent;
    modGroup[1] = &member;
    modGroup[2] = NULL;
                                
    /* Modify the user's attributes */
    rc= ldap_modify_ext_s(
        ld,      /* LDAP session handle */
        userdn,  /* the user object to modify */
        modUser, /* mod user's securityEquals and groupMembership attributes */
        NULL,    /* server controls */
        NULL);   /* client controls */

    if ( rc == LDAP_SUCCESS )
        printf("\n\tModified %s's attributes.\n", userdn); 
    else {
        printf("\n\tFailed to modify %s's attributes. ldap_modify_ext_s: %s.\n",
                                                  userdn, ldap_err2string(rc));
        return rc;
    }
    
    /* Modify the group's attributes */
    rc= ldap_modify_ext_s(
        ld,      /* LDAP session handle */
        groupdn, /* the group object to modify */
        modGroup,/* mod group's equivalentToMe and uniqueMember attributes */
        NULL,    /* server controls */
        NULL);   /* client controls */

    if ( rc == LDAP_SUCCESS )
        printf("\n\tModified %s's attributes.\n", groupdn);
    else {
        /* since we have modified the user's attributes and failed to
           modify the group's attribute, we need to delete the modified
           user's attribute values.
         */
        printf("\n\tFailed to modify %s's attributes. ldap_modify_ext_s: %s.\n",
                                                  groupdn, ldap_err2string(rc));
        printf("\n\tDeleting modified %s's attribute values...\n", userdn);
        
        modUser[0]->mod_op = LDAP_MOD_DELETE;
        modUser[1]->mod_op = LDAP_MOD_DELETE;
        
        /* Delete the modified user's attribute values */
        rc1 = ldap_modify_ext_s(
            ld,      /* LDAP session handle */
            userdn,  /* the user object to modify */
            modUser, /* delete user's modified attribute values */
            NULL,    /* server controls */
            NULL);   /* client controls */

        if ( rc1 == LDAP_SUCCESS )
            printf("\n\tDeleted %s's modified attribute values.\n", userdn);
        else
            printf("\n\tCould not delete %s's modified attribute values. "
                "ldap_modify_ext_s: %s.\n", userdn, ldap_err2string(rc));
    }
    
    return rc;
}

