/* $Novell: debug.c $ */
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
    debug.c 
***************************************************************************
   Description: This sample performs an asynchronous search operaton by 
                reading and displaying entries as they are sent back
                from the server, by enabling various debugging modes.

                Several debugging flags have been provided listed below

                    LDAP_DEBUG_ANY      -1
                    LDAP_DEBUG_TRACE    0x0001
                    LDAP_DEBUG_PACKETS  0x0002
                    LDAP_DEBUG_ARGS     0x0004
                    LDAP_DEBUG_CONNS    0x0008
                    LDAP_DEBUG_BER      0x0010
                    LDAP_DEBUG_FILTER   0x0020
                    LDAP_DEBUG_CONFIG   0x0040
                    LDAP_DEBUG_ACL      0x0080
                    LDAP_DEBUG_STATS    0x0100
                    LDAP_DEBUG_STATS2   0x0200
                    LDAP_DEBUG_SHELL    0x0400
                    LDAP_DEBUG_PARSE    0x0800
                    LDAP_DEBUG_NONE     0x8000

                By default, LDAP_DEBUG_NONE is enabled.

                The debug messages are displayed in different ways on 
                different platforms:

                Unix    - on the terminal itself

                Windows - in the WinDbg debugger. WinDbg debugger is a 
                          windows debugging graphical tool

                Netware - on the Logger Screen

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

#define LDAP_DEBUG_ANY      -1
#define LDAP_DEBUG_TRACE    0x0001
#define LDAP_DEBUG_PACKETS  0x0002
#define LDAP_DEBUG_ARGS     0x0004
#define LDAP_DEBUG_CONNS    0x0008
#define LDAP_DEBUG_BER      0x0010
#define LDAP_DEBUG_FILTER   0x0020
#define LDAP_DEBUG_CONFIG   0x0040
#define LDAP_DEBUG_ACL      0x0080
#define LDAP_DEBUG_STATS    0x0100
#define LDAP_DEBUG_STATS2   0x0200
#define LDAP_DEBUG_SHELL    0x0400
#define LDAP_DEBUG_PARSE    0x0800
#define LDAP_DEBUG_NONE     0x8000

static char usage[] =
"\n Usage:   debug <host name> <port number> <login dn> <password>"
"\n          <search base> <debug option>\n" 
"\n Example: debug Acme.com 389 cn=admin,o=Acme secret ou=Sales,o=Acme 1\n"
"\n\t  <debug option type>:  -1 - LDAP_DEBUG_ANY"
"\n\t                         1 - LDAP_DEBUG_TRACE"
"\n\t                         2 - LDAP_DEBUG_PACKETS"
"\n\t                         3 - LDAP_DEBUG_ARGS"
"\n\t                         4 - LDAP_DEBUG_CONNS"
"\n\t                         5 - LDAP_DEBUG_BER"
"\n\t                         6 - LDAP_DEBUG_FILTER" 
"\n\t                         7 - LDAP_DEBUG_CONFIG"
"\n\t                         8 - LDAP_DEBUG_ACL"
"\n\t                         9 - LDAP_DEBUG_STATS"
"\n\t                        10 - LDAP_DEBUG_STATS2"
"\n\t                        11 - LDAP_DEBUG_SHELL"
"\n\t                        12 - LDAP_DEBUG_PARSE"
"\n\t                        13 - LDAP_DEBUG_NONE\n";

int main( int argc, char **argv)
{ 
    int         i, version, ldapPort, prc, messageID, level, debugLevel;
    int         rc, done = 0, entryCount = 0, errorCode; 
    char        *ldapHost, *loginDN, *password, *searchBase,*attribute;    
    char        *dn, *errorMessage = NULL, *matchedDN, **values;
    LDAP        *ld;
    BerElement  *ber;
    LDAPMessage *searchResult, *entry;
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif

    if (argc != 7)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost          = argv[1];
    ldapPort          = atoi(argv[2]);
    loginDN           = argv[3];
    password          = argv[4];
    searchBase        = argv[5];
    level             = atoi(argv[6]);

    switch ( level ) 
    {
        case  -1 : debugLevel = LDAP_DEBUG_TRACE; break;
        case   1 : debugLevel = LDAP_DEBUG_PACKETS; break;
        case   2 : debugLevel = LDAP_DEBUG_ARGS; break;
        case   3 : debugLevel = LDAP_DEBUG_CONNS; break;
        case   4 : debugLevel = LDAP_DEBUG_BER; break;
        case   5 : debugLevel = LDAP_DEBUG_FILTER; break;
        case   6 : debugLevel = LDAP_DEBUG_CONFIG; break;
        case   7 : debugLevel = LDAP_DEBUG_ACL; break;
        case   8 : debugLevel = LDAP_DEBUG_STATS; break;
        case   9 : debugLevel = LDAP_DEBUG_STATS2; break;
        case  10 : debugLevel = LDAP_DEBUG_SHELL; break;
        case  11 : debugLevel = LDAP_DEBUG_PARSE; break;
        case  12 : debugLevel = LDAP_DEBUG_NONE; break;
        default  : puts("Invalid option");
     }


    /* Set LDAP version to 3, set connection timeout and debug level. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);
    ldap_set_option( NULL, LDAP_OPT_DEBUG_LEVEL, &debugLevel);  


    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n\tLDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n\tLDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("\n\tBind successful\n");

    /* Perform the search operation. */
    rc = ldap_search_ext( ld,                     /* LDAP session handle */
                          searchBase,             /* container to search */
                          LDAP_SCOPE_ONELEVEL,    /* search scope */
                          "(objectclass=*)",      /* search filter */
                          NULL,                   /* return all attributes */
                          0,                      /* attributes and values */
                          NULL,                   /* server controls */ 
                          NULL,                   /* client controls */
                          &timeOut,               /* search timeout */
                          LDAP_NO_LIMIT,          /* no size limit */
                          &messageID );           /* ID of this operation */

    if ( rc != LDAP_SUCCESS )
    {
        printf("\n\tldap_search_ext: %s\n", ldap_err2string( rc ) );
        ldap_unbind_s( ld );
        return( 1 );
    }
    printf("\n\tAsynchronous search initiated ... \n");

    /* Poll the server for results */
    while ( !done )
    {
        rc = ldap_result( ld,                 /* LDAP session handle */
                          LDAP_RES_ANY,       /* any result is to be returned */
                          LDAP_MSG_ONE,       /* one message at a time */
                          &timeOut,           /* time out */  
                          &searchResult );

        switch ( rc )
        {
        case -1:
            /* An error occur */
            ldap_get_option( ld, LDAP_OPT_RESULT_CODE, &errorCode);
            printf("\n\tldap_result: %s\n", 
                    ldap_err2string( errorCode ) );
            ldap_unbind_s( ld );
            return( 1 );

        case 0:
            /* Time out */
            printf("\n\tTimed out.");
            ldap_unbind_s( ld );
            return(1);

        case LDAP_RES_SEARCH_ENTRY:
            /* Its an entry, print out it's dn, attributes ans values */ 
            entryCount++;

            if (( dn = ldap_get_dn( ld, searchResult )) != NULL )
            {
                printf("\n\tdn: %s\n", dn );
                ldap_memfree( dn );
            }

            entry = ldap_first_entry( ld, searchResult );
            for (   attribute = ldap_first_attribute( ld,entry,&ber );
                    attribute != NULL; 
                    attribute = ldap_next_attribute( ld, entry,ber))
            {
                /* Get values and print.  Assumes all values are strings. */
                if (( values = ldap_get_values( ld, entry, attribute)) 
                    != NULL )
                {
                    for ( i = 0; values[ i ] != NULL; i++ )
                        printf("\t\t%s: %s\n", attribute, values[ i ] );

                    ldap_value_free( values );
                }
                ldap_memfree( attribute );
            }               

            ber_free(ber, 0);
            ldap_msgfree( searchResult );
            break;


            /* It's the final result, check status and print it out */
        case LDAP_RES_SEARCH_RESULT:
            done = 1;
            prc = ldap_parse_result(ld,               /* LDAP session handle */
                                    searchResult,     /* search result */
                                    &errorCode,       /* error code */
                                    &matchedDN,       /* matched dn */
                                    &errorMessage,    /* error message */
                                    NULL,             /* referrals */
                                    NULL,             /* server controls */
                                    1 );              /* free search result */

            if ( prc != LDAP_SUCCESS )
                printf("\n\tUnknown error");
            else
                printf("\n\tldap_search_ext: %s\n", ldap_err2string(errorCode));
            
            if ( matchedDN != NULL && *matchedDN != 0)
            {
                printf("\n\tMatched dn: %s\n", matchedDN );
                ldap_memfree( matchedDN );
            }

            break;

        default:
            printf("\n\tReturn code : %d\n", rc );
            break;
        }

        /* Print a message while waiting. */
        if ( !done )
            printf("\n\tWaiting for the next search result ...\n");

    }

    printf("\n\tSearch completed.");
    printf("\n\tEntries found: %d\n", entryCount);     

    ldap_memfree( matchedDN );
    ldap_memfree( errorMessage );
    ldap_unbind_s( ld );

    return( 0 );
}


