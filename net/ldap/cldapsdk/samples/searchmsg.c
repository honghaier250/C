/* $Novell: searchmsg.c,v 1.11 2003/05/12 13:05:58 $ */
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
   searchMsg.c
***************************************************************************
   Description: The searchMsg.c sample performs a search and parses 
                all search messages, not just entries.  This sample
                will cause referrals and search references to be returned 
                and printed instead of automatically following them. 
                
                The server must also be set up to return referrals
                ("always refer") as opposed the the default of chaining.
                
                Note:  For simplicity, this sample prints all values as 
                if they were strings.  See the searchBinary.c sample program
                for an example of handling both string and binary values.
***************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   searchMsg <host name> <port number> <login dn> <password>"
"\n          <search base>\n" 
"\n Example: searchMsg acme.com 389 cn=admin,o=acme secret"
"\n          ou=Sales,o=Acme\n\n";

int main( int argc, char **argv)
{ 

    int         version, ldapPort, i, rc, messageID, parseResult;
    int         messageType, entryCount, referenceCount, messageCount;  
    char        *ldapHost, *loginDN, *password, *searchBase;    
    char        *attribute, *dn, **values, **referrals;       
    LDAP        *ld;
    BerElement  *ber;
    LDAPMessage *searchResult, *message;
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif

    if (argc != 6)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost          =   argv[1];
    ldapPort          =   atoi(argv[2]);
    loginDN           =   argv[3];
    password          =   argv[4];
    searchBase        =   argv[5];


    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Don't automatically follow referrals.  Return them to us. */
    ldap_set_option( NULL, LDAP_OPT_REFERRALS, LDAP_OPT_OFF );

    /* Initialize an LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf("\n\tLDAP session initialization failed\n");
        return( 1 );
    }

    /* Perform a simple bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return( 1 );
    }

    /* Search the directory */
    rc = ldap_search_ext_s(
                ld,                  /* LDAP session handle */ 
                searchBase,          /* container to search */
                LDAP_SCOPE_SUBTREE,  /* search entire subtree */
                "(objectclass=*)",   /* search filter */
                NULL,                /* return all attributes */
                0,                   /* return both attributes and values */
                NULL,                /* search controls */
                NULL,                /* client controls */
                &timeOut,            /* search timeout */ 
                LDAP_NO_LIMIT,       /* no size limit */
                &searchResult );     /* returned results */

    if ( rc != LDAP_SUCCESS )
    {
        printf("ldap_search_ext_s: %s\n", ldap_err2string( rc ));
        
        /* If the server returns referrals, we get an LDAP_REFERRAL error
         * from the operation.  We can then read the list of referrals.
         */
        if (rc == LDAP_REFERRAL)
        {
            char **reflist = NULL;
            rc = ldap_get_option(ld, LDAP_OPT_REFERRAL_LIST, &reflist);
            if (rc == LDAP_SUCCESS)
            {
                printf(" Referrals:\n");
                for (i=0; reflist[i]; i++)
                    printf("    %s\n",reflist[i]);
                ldap_value_free(reflist);
            }
        }

        ldap_unbind_s( ld );
        ldap_msgfree( searchResult );
        return( 1 );
    }

    /* Go through the search results by checking message types */
    for (   message = ldap_first_message( ld, searchResult ); 
        message != NULL; 
        message = ldap_next_message( ld, message ) )
    {
        messageType = ldap_msgtype( message );         
        printf("\nMessage type: %d\n", messageType );

        messageID = ldap_msgid( message );        
        printf("Message ID: %d\n", messageID );

        switch ( messageType )
        {
            /*
             * For an entry, print out its DN, attributes,
             * and attribute values  
             */
            case LDAP_RES_SEARCH_ENTRY:                 
                if (( dn = ldap_get_dn( ld, message )) != NULL )
                {
                    printf("\tdn: %s\n", dn );
                    ldap_memfree( dn );
                }

                for ( attribute = ldap_first_attribute( ld,searchResult,&ber );
                    attribute != NULL; 
                    attribute = ldap_next_attribute( ld, searchResult,ber))
                {
                    /* Get values and print.  Assumes all values are strings. */
                    if (( values = ldap_get_values( ld, message, attribute)) 
                        != NULL )
                    {
                        for ( i = 0; values[ i ] != NULL; i++ )
                            printf("\t\t%s: %s\n", attribute, values[ i ] );

                        ldap_value_free( values );
                    }

                    ldap_memfree( attribute );
                }

                ber_free(ber, 0);
                break;

            /* Print out all the URLs */
            case LDAP_RES_SEARCH_REFERENCE:                   
                parseResult = ldap_parse_reference(
                                    ld,             /* LDAP session handle */
                                    message,        /* message to be parsed */
                                    &referrals,     /* where to put referrals */
                                    NULL,           /* server controls */
                                    0);             /* not free the resource */ 
                
                if ( parseResult != LDAP_SUCCESS )
                {
                    printf("ldap_parse_result: %s\n",
                           ldap_err2string( parseResult ) );
                    ldap_msgfree( searchResult );
                    ldap_unbind_s( ld );
                    return( 1 );
                }

                if ( referrals != NULL )
                {
                    for (   i = 0; referrals[ i ] != NULL; i++ )
                        printf("Search reference: %s\n\n", referrals[ i ] );
                    ldap_value_free( referrals );
                }
                break; 

            /* This is the last message be returned */
            case LDAP_RES_SEARCH_RESULT:       
                break;

            default:
                break;
        }            

    }
        
    /* Statistics of returned message(s) */
    messageCount   = ldap_count_messages( ld, searchResult );
    entryCount     = ldap_count_entries( ld, searchResult );
    referenceCount = ldap_count_references( ld, searchResult ); 

    printf("\nSearch completed successfully.\n"
           "\nMessages returned: %d\n"
           "Entries  returned: %d\n"
           "Search references returned: %d\n",
           messageCount, entryCount, referenceCount );

    ldap_msgfree( searchResult );
    ldap_unbind_s( ld );   

    return( 0 );
}
