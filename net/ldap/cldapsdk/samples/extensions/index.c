/* $Novell: index.c,v 1.9 2003/05/12 13:17:24 $ */
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
   index.c
***************************************************************************
   Description: The index.c sample demonstrates how to create, delete and list 
                eDirectory* database indexes.

   In general, people tend to want two things out of a Directory: fast search 
   speed and high scalability.  To quickly find the entries that match the
   search filter, eDirectory uses indexes.  Apart from performance reasons, an 
   index must exist if you want to use the Server Side Sorting (SSS) control or
   the Virtual List View (VLV) control when performing LDAP searches on the 
   eDirectory server. The eDirectory server object holds the index definitions.

   * All indexes are stored in a multi-valued attribute called indexDefinition
     on the eDirectory Server object.
   * Each value of the index attribute holds all information for a single index.
   * The string representation looks like:
     <version>$<indexName>$<state>$<rule>$<type>$<stateValue>$<attributeName>
   where
     <version>
         is a version value for indexes.  This must be set to 0 for eDirectory 
         8.5 and above
     <indexName>
         is any user-defined name for the index.  It should be unique and 
         relate to the attribute being indexed
     <state>
         is a read-only value and may be one of the following
         0 = online
             Online means that the index is up and working.     
         1 = suspend
             When an index is in the suspended state, it is not used in queries
             and is not updated. When an index is suspended, it will not be
             updated for the new records created in the database.  However, if
             the key field of an existing record is modified,the index will be
             updated even though it is marked suspended.
         2 = bringing online (low)
         3 = bringing online (high) 
             The bringing online low or high priority states indicate an index
             is waiting to be created the next time the background process runs.
             The difference between low and high priority is the wait time
             between indexing blocks of data.  Very little wait time is used
             in the high priority mode, so the CPU load increases dramatically
             while the index is being built.
         4 = creating
             Creating is an intermediate state used in the creation of a new
             index. When defining an index using LDAP, this field should
             normally be set to 2. Only set it to 3 if you absolutely require
             the high priority index build and can handle the CPU load this will
             create. The background process LIMBER automatically changes the
             state once index building has begun.
         * The Online and Creating states are reserved for internal use.
           Attempts to set these two state values in the attribute definition
           will be ignored.
         * The process of transitioning the suspended index to a bringing online
           state will index all records added to the database since the index
           was suspended.

     <rule>
         is the matching rule to be used by index and may be
         0 = value
             (equality or begins with substring) matching which optimizes
             queries that involve the entire value or the first part of the
             value, for example a query for all entries with a surname equal to
             Jensen or begin with Jen
         1 = presence
             matching which optimizes queries that involve only the presence of
             an attribute, for example, a query for all entries with a surname
             attribute
         2 = substring
             matching which optimizes queries that involve a match of a few
             characters, for example, a query for all entries with a surname
             containing "der".  This query returns entries with the surnames of
             Derington, Anderson, and Lauders
     <type>
         specifies source of the index. It may be:
         0 = user defined
         1 = added when attribute was created
         2 = needed for operation of system (system created)
         3 = other system-created index
         when creating a new index, this must be set to 0.
     <stateValue>
         specifies the source of index. It may be:
         0 = uninitialized
         1 = added from server
         2 = added from local DIB
         3 = deleted from local DIB
         4 = modified from local DIB
         should always set this value to 1
     <attributeName>
         is the name of the attribute as it appears in the schema.  Right now,
         this is the eDirectory name of the attribute if the eDirectory name 
         and the LDAP name are different.

    Sample LDIF script for index creation
         dn: cn=myserver,o=Acme
         changetype: modify
         add: indexDefinition
         indexDefinition: 0$indexName$2$2$0$1$attributeName

    Sample LDIF script to delete index
         dn: cn=myserver, o=Acme
         changetype: modify
         delete: indexDefinition
         indexDefinition: 0$indexName$2$2$0$1$attributeName
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#include <ldapx.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

int listIndexes (LDAP *ld, char *serverDN );
int addIndex (LDAP *ld, char *serverDN, char *indexName, int rule, 
              char *attribute );
int deleteIndex (LDAP *ld, char *serverDN, char *indexName );

static char usage[] =
"Usage: index <operation type> <host name> <port number> <loginDN> <password>"
"\n             <server DN> <index name> <eDirectory attribute> <rule>"
"\nwhere"
"\n<operation type>:   1 - list all indexes"
"\n                    2 - add index"
"\n                    3 - delete index"
"\n<rule>: (valid only while creating the index)"
"\n                    0 - value"
"\n                    1 - presence"
"\n                    2 - substring"
"\nExamples:"
"\nindex 1 Acme.com 389 cn=admin,o=Acme secret cn=myserver,o=Acme"
"\nindex 2 Acme.com 389 cn=admin,o=Acme secret cn=myserver,o=Acme myIndex city 0"
"\nindex 3 Acme.com 389 cn=admin,o=Acme secret cn=myserver,o=Acme myIndex\n";

int main( int argc, char **argv)
{
    int         version, ldapPort, operationType, rule, rc; 
    char        *ldapHost, *loginDN, *password, *dn = NULL;
    char        *serverDN, *indexName, *attribute;
    LDAP        *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if ( argc < 7 ) 
    {
        printf("%s", usage);
        return ( 1 );
    }

    switch ( atoi(argv[1]) ) 
    {
        case 1:
              if (argc != 7 ) 
              {
                  printf("%s", usage); 
                  return ( 1 );
              }
              break;
        case 2:
              if (argc != 10 ) 
              {
                  printf("%s", usage);
                  return ( 1 );
              }
              indexName = argv[7];
              attribute = argv[8];
              rule      = atoi(argv[9]);
              break;
        case 3:
              if (argc != 8 ) 
              {
                  printf("%s", usage);
                  return ( 1 );
              }
              indexName = argv[7];
              break;
        default:
            printf("%s", usage);
            return ( 1 );
    };

    operationType     = atoi(argv[1]);
    ldapHost          = argv[2];
    ldapPort          = atoi(argv[3]);
    loginDN           = argv[4];
    password          = argv[5];
    serverDN          = argv[6];
 
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

    switch (operationType) 
    {
    case 1 :
           rc = listIndexes ( ld, serverDN );
           break;
    case 2 :
           rc = addIndex (ld, serverDN, indexName, rule, attribute);
           break;
    case 3 : 
           rc = deleteIndex (ld, serverDN, indexName );
           break;
    }

    ldap_unbind_s( ld );

    return( rc );
}

int listIndexes (LDAP *ld, char *serverDN )
{

    char *index_state[] = { "Online",
                            "Suspended",
                            "Bringing online(low)",
                            "Bringing online(high)",
                            "Creating" };
    char *index_rule[]  = { "Value",
                            "Presence",
                            "Substring" };

    char *index_type[]  = { "User defined",
                            "Added on attribute creation",
                            "required for operation",
                            "Syntax index" };

    char *index_value[] = { "Uninitialized",
                            "Added from server",
                            "Added from local DIB",
                            "Deleted from local DIB",
                            "Modified from local DIB" };
    int         rc, i ;
    char        *attribute, **values; 
    char        *requiredAttributes[2];
    BerElement  *ber;
    LDAPMessage *searchResult, *entry;

    /*
     * Read the indexDefinition attribute from the server object.
     */
    requiredAttributes[0]="indexDefinition";
    requiredAttributes[1]=NULL;

    rc = ldap_search_ext_s(
                    ld,                    /* LDAP session handle */
                    serverDN,              /* container to search */
                    LDAP_SCOPE_BASE,       /* search scope */
                    "(objectClass=*)",     /* search filter */
                    requiredAttributes,    /* return all attributes */
                    0,                     /* return attributes and values */
                    NULL,                  /* server controls */
                    NULL,                  /* client controls */
                    NULL,                  /* search timeout */
                    LDAP_NO_LIMIT,         /* no size limit */
                    &searchResult );       /* returned results */

    if ( rc != LDAP_SUCCESS )
    {
        printf("ldap_search_ext_s: %s\n", ldap_err2string( rc ));
        ldap_msgfree( searchResult );
        return ( rc );
    }

    /* Read and print search results.  We expect only one entry. */
    if ( (entry = ldap_first_entry( ld, searchResult )) != NULL)
    {
        for (   attribute = ldap_first_attribute( ld, entry, &ber );
                attribute != NULL;
                attribute = ldap_next_attribute( ld, entry, ber ) )
        {
            /* Get values and print.  Assumes all values are strings. */
            if (( values = ldap_get_values( ld, entry, attribute)) != NULL )
            {
                for ( i = 0; values[i] != NULL; i++ )
                {
                    printf("\n   Index Version: %s\n", strtok(values[i] ,"$"));
                    printf("   Index Name: %s\n", strtok(NULL,"$"));
                    printf("   Index State: %s\n", 
                           index_state[atoi(strtok(NULL,"$"))]);
                    printf("   Index Rule: %s\n", 
                           index_rule[atoi(strtok(NULL,"$"))]);
                    printf("   Index Type: %s\n", 
                           index_type[atoi(strtok(NULL,"$"))]);
                    printf("   Index State Value: %s\n", 
                           index_value[atoi(strtok(NULL,"$"))]);
                    printf("   eDirectory Attribute: %s\n", strtok(NULL,"$"));
                }
                ldap_value_free( values );
            }
            ldap_memfree( attribute );
        }
        ber_free(ber, 0);
    }

    ldap_msgfree( searchResult );
    return LDAP_SUCCESS;
}

int addIndex (LDAP *ld, char *serverDN, char *indexName, int rule, char *attribute) 
{
    int     rc;
    char    *definitions[2];
    LDAPMod modIndex, *modify[2];

    /*
     * To add an entry to the directory:
     *   1. Create the array of LDAPMod structures representing the
     *      attributes of the new entry
     *   2. Specify the DN of the entry to create
     *   3. Call ldap_add_ext_s to add the entry to the directory
     */
    modIndex.mod_op = LDAP_MOD_ADD;
    modIndex.mod_type = "indexDefinition";

    /*
     * Construct index value according to format:
     * <version>$<indexName>$<state>$<rule>$<type>$<stateValue>$<attributeName>
     */

    definitions[0]=(char *) malloc( sizeof( "0$$2$$0$1$" ) + strlen( indexName )
                                  + sizeof ( rule ) + strlen( attribute ) + 1 );
    sprintf(definitions[0],"0$%s$2$%d$0$1$%s",indexName,rule,attribute);

    definitions[1] = NULL;
    modIndex.mod_values = definitions;

    modify[ 0 ] = &modIndex;
    modify[ 1 ] = NULL;

    rc = ldap_modify_ext_s( ld,         /* LDAP session handle */
                            serverDN,   /* the object to modify */
                            modify,     /* array of LDAPMod structures */
                            NULL,       /* server controls */
                            NULL);      /* client controls */

    free( definitions[0] );
    if ( rc != LDAP_SUCCESS )
    {
        printf("\n   ldap_add_ext_s: %s\n", ldap_err2string( rc ));
        return( rc );
    }
    printf("\n   Index successfully added: %s", indexName);

    /*
     * Trigger eDirectory Limber background process start building the index.
     */
    printf("\n   Initiating Limber Process.\n");
    rc = ldap_trigger_back_process(ld, LDAP_BK_PROCESS_LIMBER);

    if ( rc != LDAP_SUCCESS ) {
        printf("\n    ldap_trigger_back_process: %s\n", ldap_err2string( rc ));
        return( rc );
    }
    printf("\n   Limber process successfully initiated.\n"); 

    return LDAP_SUCCESS;
}


deleteIndex (LDAP *ld, char *serverDN, char *indexName )
{
    int            i, rc=0;
    char           *attribute, *actualIndex=NULL;
    char           **values, *requiredAttributes[2], *definitions[2];
    LDAPMessage    *searchResult, *entry;
    LDAPMod        modIndex, *modify[2];
    BerElement     *ber;

    requiredAttributes[0] = "indexDefinition";
    requiredAttributes[1] = NULL;

    /* Read the eDirectory server object to retrieve the indexes */
    rc = ldap_search_ext_s(
                    ld,                    /* LDAP session handle */
                    serverDN,              /* entry to read */
                    LDAP_SCOPE_BASE,       /* search scope */
                    "(objectClass=*)",     /* search filter */
                    requiredAttributes,    /* return all attributes */
                    0,                     /* return attributes and values */
                    NULL,                  /* server controls */
                    NULL,                  /* client controls */
                    NULL,                  /* no time out */
                    LDAP_NO_LIMIT,         /* no size limit */
                    &searchResult );       /* returned results */

    if ( rc !=  LDAP_SUCCESS )
    {
        printf("ldap_search_ext_s: %s\n", ldap_err2string( rc ));
        ldap_msgfree( searchResult );
        return ( rc );
    }

    /* Read search results.  We expect only one entry and one attribute. */
    if ( (entry = ldap_first_entry( ld, searchResult )) != NULL)
    {
        if ( (attribute = ldap_first_attribute( ld, entry, &ber )) != NULL )
        {
            /* Get values and locate the correct index definition value.
             * This is required to delete the index.
             */
            if (( values = ldap_get_values( ld, entry, attribute)) != NULL )
            {
                for ( i = 0; values[i] != NULL; i++ )
                {
                    char *tmpValue, *token;
                    tmpValue = strdup(values[i]);
                    token = strtok( tmpValue,"$" );
                    token = strtok( NULL , "$" );
                    if (token && (strcmp(token,indexName))==0)
                    {
                        actualIndex = strdup( values[i] );
                        free( tmpValue );
                        break;
                    }
                    free( tmpValue );
                }
                ldap_value_free( values );
            }
            ldap_memfree( attribute );
        }
        ber_free(ber, 0);
    }
    ldap_msgfree( searchResult );
    
    if (!actualIndex) 
    {
         printf("\n   No such index: %s\n",indexName);
         return ( 1 );
    }

    /*
     * To modify the attributes of an entry
     *    1  Specify the modify actions
     *    2. Specify the attribute name to be modified
     *    3. Specify the value of the attribute
     *    4. Add to an array of LDAPMod structures
     *    5. Call ldap_modify_ext_s
     */

    modIndex.mod_op = LDAP_MOD_DELETE;
    modIndex.mod_type = "indexDefinition";

    definitions[0] = actualIndex;
    definitions[1] = NULL;
    modIndex.mod_values = definitions;

    modify[ 0 ] = &modIndex;
    modify[ 1 ] = NULL;

    rc = ldap_modify_ext_s( ld,         /* LDAP session handle */
                            serverDN,   /* the object to modify */
                            modify,     /* array of LDAPMod structures */
                            NULL,       /* server controls */
                            NULL);      /* client controls */

    free( actualIndex );
    if ( rc != LDAP_SUCCESS ) {
        printf("\n    ldap_modify_ext_s: %s\n", ldap_err2string( rc ));
        return( rc );
    }
    printf("\n   Index successfully deleted: %s\n", indexName);

    /*
     * Trigger eDirectory Limber process to remove the index.
     */
    printf("\n   Initiating Limber Process.\n");
    rc = ldap_trigger_back_process(ld, LDAP_BK_PROCESS_LIMBER);

    if ( rc != LDAP_SUCCESS ) {
        printf("\n    ldap_trigger_back_process: %s\n", ldap_err2string( rc ));
        return( rc );
    }
    printf("\n   Limber process successfully initiated.\n"); 

    return LDAP_SUCCESS;
}
