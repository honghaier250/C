/* $Novell: modifyACL.c,v 1.12 2003/05/12 12:26:02 $ */
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
   modifyACL.c 
*******************************************************************************
   Description: modifyACL.c first modifies entryDN's ACL values to grant 
                trusteeDN the read, write, and delete entry rights. It then 
                dispalys entryDN's ACL values. Finaly it removes entryDN's 
                modified ACL value.
                
                ACL (Access Control List) is a multivalued optional attribute.
                An entry's ACL values define which other entries (trustees) 
                have what kinds of access to the entry itself ([Entry Rights])
                and its attributes ([All Attributes Rights] or single attribute
                rights). 
                
                Each of ACL values is in the format of
                    "privileges#scope#subjectname#protectedattrname".
                privileges:        ORed bits to indicate the rights.
                scope:             either 'entry' or 'subtree'.
                subjectname:       trustee DN.
                protectedattrname: [Entry Rights], or [All Attributes Rights],
                                   or a single attribute name.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#include <ldap_ds_constants.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

int  findACLValues( LDAP* , char* );
void printACLValue( char* );


static char usage[] =
"\n Usage:   modifyACL <host name> <port number> <login dn> <password>"
"\n                    <entry dn> <trustee dn>"
"\n Example: modifyACL Acme.com 389 cn=Admin,o=Acme secret"
"\n                    cn=test,ou=Sales,o=Acme cn=trustee,o=Acme\n";

int main( int argc, char **argv)
{ 
    int      version, ldapPort, rc, privileges; 
    char     *ldapHost, *loginDN, *password, *entryDN, *trusteeDN;
    char     *modValues[2], aclValue[128];
    LDAPMod  modACL, *modify[2];
    LDAP     *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 7) {
        printf("%s", usage);
        return(1);
    }

    ldapHost   = argv[1];
    ldapPort   = atoi(argv[2]);
    loginDN    = argv[3];
    password   = argv[4];
    entryDN    = argv[5];
    trusteeDN  = argv[6]; 

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n    LDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n    LDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS ) {
        printf("\n    ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("\n    Bind successful\n");

    /* Encode ACL value */ 
    privileges = LDAP_DS_ENTRY_BROWSE;
    privileges |= LDAP_DS_ENTRY_ADD;
    privileges |= LDAP_DS_ENTRY_DELETE;
    sprintf(aclValue,"%d", privileges);
    strcat(aclValue, "#entry#");
    strcat(aclValue, trusteeDN);
    strcat(aclValue, "#[Entry Rights]");

    /* Populate LDAPMod structures with the values */
    modACL.mod_op     = LDAP_MOD_ADD;
    modACL.mod_type   = "acl";
    modValues[0]      = aclValue;
    modValues[1]      = NULL;
    modACL.mod_values = modValues;
    modify[0]         = &modACL;   
    modify[1]         = NULL;

    /* Modify entryDN's ACL value */ 
    printf( "\n    Entry DN: %s", entryDN );
    printf( "\n    Trustee DN: %s\n", trusteeDN );
    printf( "\n    Modifying entry's ACL values ..." );
    rc = ldap_modify_ext_s( ld,         /* LDAP session handle */
                            entryDN,    /* the object to modify */
                            modify,     /* array of LDAPMod structures */
                            NULL,       /* server controls */
                            NULL);      /* client controls */

    if ( rc != LDAP_SUCCESS ) {
        printf("\n    ldap_modify_ext_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return(1);
    }
    
    printf("\n    Modified  entry's ACL values to grant the trustee the 'read', "
           "\n    'write', and 'delete' entry rights.\n");

    /* List entryDN's ACL attribute values */
    rc = findACLValues( ld, entryDN);

    if ( rc != LDAP_SUCCESS ) {
        printf("\n    Failed to read the entry: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return(1);
    }
    
    /* Remove modified entryDN's ACL value */
    printf( "\n\n    Removing entry's modified ACL value ...\n" );
    modify[0]->mod_op = LDAP_MOD_DELETE;
    rc= ldap_modify_ext_s( ld,         /* LDAP session handle */
                           entryDN,    /* the object to modify */
                           modify,     /* array of LDAPMod structures */
                           NULL,       /* server controls */
                           NULL);      /* client controls */
    if ( rc != LDAP_SUCCESS ) {
        printf("\n    ldap_modify_ext_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return(1);
    }
    printf( "    Removed entry's modified ACL value.\n" );

    ldap_unbind_s( ld );
    return( 0 );
}

/* 
 * listACLValues() searches the directory for the entry to 
 * get the entry's acl attribute 
 */
int findACLValues( LDAP *ld, char *entryDN) 
{    
    int         rc, i;
    char        **values, *attribute, *attrs[] = { "acl", NULL };
    BerElement  *ber;
    LDAPMessage *searchResult, *entry;
    struct timeval timeOut = {10,0};    /* Search timeout */

    /* Search the directory to get the entry's ACL attribute */
    rc = ldap_search_ext_s(  
        ld,                    /* LDAP session handle */
        entryDN,               /* object to read */
        LDAP_SCOPE_BASE,       /* search scope */
        "(objectclass=*)",     /* search filter */
        attrs,                 /* return acl attribute only */
        0,                     /* return both the ACL and its values */
        NULL,                  /* server controls */
        NULL,                  /* client controls */
        &timeOut,              /* search timeout */
        LDAP_NO_LIMIT,         /* no size limit */
        &searchResult );       /* returned results */

    if ( rc != LDAP_SUCCESS ){
        ldap_msgfree( searchResult ); 
 
        return rc;
     }                                   

    /* Search returns entryDN's ACL, print its values out */
    entry   =   ldap_first_entry( ld, searchResult );
    attribute = ldap_first_attribute( ld, entry, &ber );               
        
    printf("\n    ===========================================================");
    printf("\n     entry's ACL values after modification: " );
    printf("\n    ===========================================================");
                
    if (( values = ldap_get_values( ld, entry, attribute)) != NULL )
        for ( i = 0; values[ i ] != NULL; i++ )
            printACLValue( values[i] );                  
           
    ldap_value_free( values );
    ldap_memfree( attribute );
    ber_free(ber, 0);
    ldap_msgfree( searchResult );

    return rc;
}

/*
 * printACLValue() takes aclValue as input. It parses
 * and prints the ACL value.
 */
void printACLValue( char* aclValue ) 
{        
    int  privileges;
    char *scope, *trusteeDN, *protectedAttrName, privs[128];                                   	
	
	protectedAttrName = strrchr(aclValue, '#') + 1;
	aclValue[protectedAttrName-aclValue-1] = '\0';
	
	trusteeDN = strrchr(aclValue, '#') + 1;
	aclValue[trusteeDN-aclValue-1] = '\0';

	scope = strrchr(aclValue, '#') + 1;
	aclValue[scope-aclValue-1] = '\0';

	sscanf(aclValue, "%d", &privileges);

    if (strcmp( protectedAttrName, "[Entry Rights]") == 0) {
        /* Decode the [Entry Rights]   */
        strcpy(privs,"");
        if ( privileges & LDAP_DS_ENTRY_BROWSE )
            strcat(privs, "BrowseEntry ");
        if ( privileges & LDAP_DS_ENTRY_ADD )
            strcat(privs, "AddEntry ");
        if ( privileges & LDAP_DS_ENTRY_DELETE )
            strcat(privs, "DeleteEntry ");
        if ( privileges & LDAP_DS_ENTRY_RENAME )
            strcat(privs, "RenameEntry ");
        if ( privileges & LDAP_DS_ENTRY_SUPERVISOR )
            strcat(privs, "Supervisor");            
    }
    else {
        /* Decode [All Attributes Rights] or attribute rights */
        strcpy(privs,"");
        if ( privileges & LDAP_DS_ATTR_COMPARE )
            strcat(privs, "CompareAttributes ");        
        if ( privileges & LDAP_DS_ATTR_READ )
            strcat(privs, "ReadAttributes ");        
        if ( privileges & LDAP_DS_ATTR_WRITE )
            strcat(privs, "Write/Add/DeleteAttributes ");
        if ( privileges & LDAP_DS_ATTR_SELF )
            strcat(privs, "Add/DeleteSelf ");
        if ( privileges & LDAP_DS_ATTR_SUPERVISOR )
            strcat(privs, "Supervisor");
    }    
             
    /* Print out each ACL value */
    printf("\n    Trustee DN: %s", trusteeDN);
    printf("\n    Scope: %s", scope);
    printf("\n    Protected attribute name: %s", protectedAttrName);
    printf("\n    Privileges: %s", &privs);
    printf("\n    ------------------------------------------------------");
}

