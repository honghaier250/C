/* $Novell: schema.c,v 1.15 2003/05/12 13:01:45 $ */
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
*   schema.c
***************************************************************************
*   Description:  schema.c demonstrates the following:
*                  - Fetching all schema element definitions
*                  - Reading and printing schema element definitions
*                  - Deleting schema element definitions
*                  - Adding (Extending) new schema element definitions
*
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ldap.h> 
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif
   
void print_schema(LDAPSchema *schema, int type);

int main(int argc, char *argv[])
{         
    char *ldapHost, *loginDN, *password; 
    LDAP *ld; 
    LDAPSchema *schema = NULL; 
    int rc, ldapPort, version;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

            
    /* Variables for adding a new addribute */
    LDAPSchemaMod m_oid, m_name, m_desc, m_syntax, m_x, *mod[6]; 

    /* Syntax for a Case Ignore String */
    char *syntax[]  = {"1.3.6.1.4.1.1466.115.121.1.15", NULL};  
    char *oid []    = {"2.16.840.1.113719.1.187.4.1", NULL};    
    char *name[]    = {"TestAttribute", NULL};                  
    char *desc[]    = {"Test attribute definition", NULL}; 

    /* This field specifies that the attribute doesn't syncronize 
       immediately.  Any X- Qualifier can be used */
    char *x_field   = "X-NDS_NOT_SCHED_SYNC_IMMEDIATE";
    char *x_values[]= {"1", NULL };
    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 5){
        printf("\n Usage: schema <host name> <port number>"
               " <login dn> <password>\n\n"
               " Example: schema Acme.com 389 cn=admin,o=Acme secret \n");
        return 1;
    }
    
    ldapHost = argv[1];
    ldapPort = atoi( argv[2] );
    loginDN  = argv[3];
    password = argv[4];

    /* Set LDAP version to 3, needed to read root DSE.
     * Also set connection timeout.
     */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* open an LDAP session */ 
    ld = ldap_init( ldapHost, ldapPort);
    if (ld == NULL ){
        printf ( "\n\tLDAP session initialization failed.\n");
        return 1; 
    }
    printf("\n\tLDAP session initialized successfully.\n");
         
    /* bind */ 
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS ){
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("\n\tBind successful.\n");
 
    /* Fetch all schema element definitions for the directory */ 
    rc = ldap_schema_fetch( ld, &schema, NULL );
    if (rc != LDAP_SUCCESS) {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
        
    /* Reading and printing schema element definitions: */
    print_schema(schema, LDAP_SCHEMA_ATTRIBUTE_TYPE);
    print_schema(schema, LDAP_SCHEMA_OBJECT_CLASS);
    print_schema(schema, LDAP_SCHEMA_MATCHING_RULE);
    print_schema(schema, LDAP_SCHEMA_MATCHING_RULE_USE);
    print_schema(schema, LDAP_SCHEMA_SYNTAX);
    print_schema(schema, LDAP_SCHEMA_DIT_CONTENT_RULE);
    print_schema(schema, LDAP_SCHEMA_DIT_STRUCTURE_RULE);
    
    
    /* Delete the definition if it already exists */
    rc = ldap_schema_delete(schema, LDAP_SCHEMA_ATTRIBUTE_TYPE, name[0]);

    /* If the definition already existed, commit the deletion */
    if (rc == LDAP_SUCCESS){
        rc = ldap_schema_save(ld, schema, NULL);
        if (rc != LDAP_SUCCESS)
            printf("Could not remove an existing attribute: %s(%d)",
                ldap_err2string(rc), rc);
    }  
    
        
    /*  Setting up add schema element definition: 
        The 'op' field of LDAP_SCHEMA_MOD is not used by 
        ldap_schema_add. 
     */
    m_oid.fieldName = LDAP_SCHEMA_OID;
    m_oid.values = oid;
    mod[0] = &m_oid;
    
    m_name.fieldName   = LDAP_SCHEMA_NAMES;
    m_name.values      = name;
    mod[1] = &m_name;

    m_desc.fieldName    = LDAP_SCHEMA_DESCRIPTION;
    m_desc.values       = desc;    
    mod[2] = &m_desc;

    m_syntax.fieldName  = LDAP_SCHEMA_SYNTAX_OID;
    m_syntax.values     = syntax;
    mod[3] = &m_syntax;
    
    m_x.fieldName   = x_field;
    m_x.values      = x_values;
    mod[4] = &m_x;

    mod[5] = NULL;

    /* Add new definition to the directory */
    rc = ldap_schema_add(schema, LDAP_SCHEMA_ATTRIBUTE_TYPE, mod);
    if (rc != LDAP_SUCCESS) {
        printf("Error in function ldap_schema_add: %s(%d)", 
            ldap_err2string(rc), rc);
    } 

    rc = ldap_schema_save(ld, schema, NULL);
    if (rc != LDAP_SUCCESS) {
        printf("Error in function ldap_schema_save: %s(%d)", 
            ldap_err2string(rc), rc);
    } 

    ldap_schema_free(schema);
    ldap_unbind(ld);
    return 1;
} 

/**********************************************************************
    print_schema:

    Reads schema element definitions from the local copy of schema, 
    according to the type passed in.  print_schema iterates through 
    definitions using ldap_schema_get_by_index. (If only one element 
    is needed ldap_schema_get_by_name will lookup the element by name.)

    Three nested loops are used:
    1) Loop through all schema element definitions
    2) Loop through the field names for an element
    3) Loop through the field values for an element                 */

void print_schema(LDAPSchema *schema, int type)
{
    int index,       /* iterates through schema elements     */
        field,       /* iterates through fields of a element */
        value,       /* iterates through values of a field   */
        count,       /* count of schema elements             */
        rc;          /* LDAP result code                     */

    LDAPSchemaElement *element;
    char** names;    /* holds the field names       */
    char** values;   /* holds the values of a field */

    /* Iterate through the definitions */
    count = ldap_schema_get_count(schema, type); 
    for (index=0; index < count; index++) 
    { 
        /* Retrieve the schema element definition */
        rc = ldap_schema_get_by_index( 
            schema, index, type, &element);
        if (rc != LDAP_SUCCESS ) {
            printf("Error in function ldap_schema_get_by_index: %s(%d)", 
                ldap_err2string(rc), rc);
            break;
        }
        
        /* Retrieve defined field names */
        rc = ldap_schema_get_field_names( element, &names );
        if (rc != LDAP_SUCCESS ) {
            printf("Error in function ldap_schema_get_by_index: %s(%d)", 
                ldap_err2string(rc), rc);
            break;
        }

        /* Iterate through each field name*/
        for (field = 0; names[field] != NULL; field++)
        {
            printf("%s\n", names[field]);
            
            /* Retrieve values of the field */
            rc = ldap_schema_get_field_values( 
                element, names[field], &values );

            if (rc != LDAP_SUCCESS ) {
                printf("Error in function ldap_schema_get_by_index: %s(%d)", 
                    ldap_err2string(rc), rc);
                break;
            }

            /* Iterate through each value */
            if (values[0] != NULL)
                printf("\t%s", values[0]);
            for(value = 1; values[value] != NULL; value++)
                printf(", %s", values[value]);  
            printf("\n");
            ldap_value_free ( values );
        }
        ldap_value_free( names );
    } 
}

