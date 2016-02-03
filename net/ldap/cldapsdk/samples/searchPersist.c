/* $Novell: searchPersist.c,v 1.9 2003/05/12 13:04:46 $ */
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
   persistsearch.c 
***************************************************************************
   Description: The searchPersist.c sample performs a persistent search
                on the directory and receives notification of any changes
                to entries within the scope of the search's result set.
***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ldap.h>

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

/* forward declarations of helper functions */
static char *
getChangeTypeString(
    int changeType);

/* global data */
static char usage[] =
"\n Usage:   searchPersist <host name> <port number> <login dn> <password>"
"\n          <search base>\n" 
"\n Example: searchPersist Acme.com 389 cn=admin,o=Acme secret"
"\n          ou=Sales,o=Acme\n";

#define EXECUTE_TIME   300  /* execute for 300 seconds (5 minutes) */

/* main entry point */
int main( int argc, char **argv )
{   
    int             version, ldapPort, rc;
    int             msgid;
    char            *ldapHost, *loginDN, *password, *searchBase;   
    char            *dn ;       
    LDAP            *ld;
    struct timeval  timeOut = {10,0};  /* 10 second connection timeout */
    LDAPMessage     *result, *entry;
    LDAPControl     *controls[2], *psControl, **ecControls;
    int             finished, errorCode;
    time_t          startTime;
    char            *attrs[2];  
    char            *errorMsg;

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    startTime = time(NULL); /* record the start time */
   
    if (argc != 6)
    {
        printf("%s", usage);
        return (1);
    }

    ldapHost          = argv[1];
    ldapPort          = atoi(argv[2]);
    loginDN           = argv[3];
    password          = argv[4];
    searchBase        = argv[5];    

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n    LDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n    LDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind( ld );
        return( 1 );
    }
    printf("\n    Bind successful\n");

    /* construct the Persistent Search control */
    rc = ldap_create_persistentsearch_control( 
          ld, 
          LDAP_CHANGETYPE_ANY,
          1,  /* changesOnly */
          1,  /* request entry change controls */
          1,  /* critical */
          &psControl );

    if (rc != LDAP_SUCCESS)
    {
        printf("ldap_create_persistentsearch_control: %s\n", 
               ldap_err2string( rc ));
        ldap_unbind( ld );
        return( 1 );
    }
    
    /* initialize controls array */
    controls[0] = psControl;
    controls[1] = NULL;  /* NULL signals the end of the array */

    /* initialize attributes array */
    attrs[0] = LDAP_NO_ATTRS; /* don't return attributes */
    attrs[1] = NULL;    /* NULL signals the end of the array */

    /* Search the directory */
    rc = ldap_search_ext(  
                    ld,                    /* LDAP session handle */
                    searchBase,            /* container to search */
                    LDAP_SCOPE_SUBTREE,    /* search scope */
                    "(objectclass=*)",     /* search filter, all objects */
                    attrs,                 /* attributes array */
                    0,                     /* ignored, not returning attrs */
                    controls,              /* server controls */
                    NULL,                  /* no client controls */
                    NULL,                  /* no search timeout */
                    LDAP_NO_LIMIT,         /* no size limit */
                    &msgid );              /* message ID */
    
    ldap_control_free( psControl );  /* no longer needed */
    
    if ( rc != LDAP_SUCCESS ) 
    {
        printf("ldap_search_ext: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return ( 1 );
    }                                    
     
    /*
     * Loop, polling for results until EXECUTE_TIME seconds have passed or
     * the server shuts down or we lose the connection for some other reason.
     *
     * We call ldap_result with the "all" argument
     * set to LDAP_MSG_ONE.  This causes ldap_result() to return exactly one
     * entry if at least one entry is available.  This allows us to
     * display the entries as they are received.
     *
     * A zero valued timeval struct causes the ldap_result function to not
     * block so it can be called in a polling loop. 
     */
    timeOut.tv_sec    = 0L;
    timeOut.tv_usec   = 0L;
    
    printf("Monitoring directory changes for %d minutes.\n", EXECUTE_TIME/60);
    finished = 0;
    while ( 0 == finished )
    {
        result = NULL;
        rc = ldap_result( ld, msgid, LDAP_MSG_ONE, &timeOut, &result );
        switch ( rc )
        {
        case -1:   /* some error occurred */
            ldap_get_option(ld, LDAP_OPT_ERROR_NUMBER, &errorCode);
            printf("ldap_result: %s\n", ldap_err2string( errorCode ));
            if ( result != NULL )
                ldap_msgfree( result );
            finished = 1;  /* terminate polling loop */
            break;

        case 0:    /* No entries are ready for retrieval. */
            if ( result != NULL )
                ldap_msgfree( result );
            break;

        case LDAP_RES_SEARCH_ENTRY: /* An entry is ready for retrieval */
            if (( entry = ldap_first_entry( ld, result )) == NULL )
            {  /* didn't get an entry, loop again */
                if ( result != NULL )
                    ldap_msgfree( result );
                continue;
            }

            printf( "\n" ); /* separate each entry's output with a blank line */
            
            /* for each entry print out name */
            if (( dn = ldap_get_dn( ld, entry )) != NULL ) 
            {
                printf("dn: %s\n", dn );
                ldap_memfree( dn );
            }

            /* print entry change info. if it was returned */
            if ( LDAP_SUCCESS == 
                  ldap_get_entry_controls( ld, entry, &ecControls ))
            {
                int chgtype, chgnumpresent;
                long    chgnum;
                char    *prevdn;

                if ( LDAP_SUCCESS == ldap_parse_entrychange_control( ld,
                    ecControls, &chgtype, &prevdn, &chgnumpresent, &chgnum ))
                {
                    printf("changeType: %s\n",
                           getChangeTypeString( chgtype ));
                    if ( prevdn != NULL )
                    {
                        printf("previousDN: %s\n", prevdn );
                        ldap_memfree( prevdn );
                    }
                    if ( chgnumpresent )
                    {
                        printf("changeNumber: %d\n", chgnum );
                    }
                    ldap_controls_free( ecControls );
                }
            }
            if (NULL != result)
                ldap_msgfree( result );
            break;

        case LDAP_RES_SEARCH_RESULT:   /* Got a final response */
            errorMsg = NULL;
            rc = ldap_parse_result(ld, 
                                   result, /* result to parse */
                                   &errorCode, /* get the errorCode */
                                   NULL,    /* don't need matched dn */ 
                                   &errorMsg, /* get server error msg*/
                                   NULL,    /* don't get referrals */
                                   NULL,    /* don't get server ctrls */
                                   1);      /* free the result */
            if (rc != LDAP_SUCCESS)
            {
                printf("Error parsing result: %d\n", rc);
            }
            else if (errorCode != LDAP_SUCCESS)
            {
                printf("Search result error: %d\n", errorCode);
                if (NULL != errorMsg)
                 {
                    ldap_memfree( errorMsg );
                    printf("Server Message: %s\n", errorMsg);
                 }
                finished = 1;  /* terminate polling loop */ 
            }
            break;

        default:
            break;
            
        } /* end switch */

        /* Is it time to exit? */
        if ( (time(NULL) - startTime) > EXECUTE_TIME )
        {
            finished = 1; /* terminate polling loop */
            rc = ldap_abandon(ld, msgid);
            if (LDAP_SUCCESS == rc)
                printf("\nSearch operation successfully abandoned.\n");
            else
                printf("\nldap_abandon: %s\n", ldap_err2string(rc));
        }
    } /* end polling loop */

    ldap_unbind( ld );  
 
    return( 0 );
}


 /*************************************************************************
   getChangeTypeString 

   Description: Returns a character string description of the specified
                change type.
  ***************************************************************************/
static char *
getChangeTypeString(
    int changeType)
{
    static char buffer[30];

    if (LDAP_CHANGETYPE_ADD == changeType)
        sprintf( buffer, "%s", "Entry Added" );
    else if (LDAP_CHANGETYPE_DELETE == changeType)
        sprintf( buffer, "%s", "Entry Deleted" );
    else if (LDAP_CHANGETYPE_MODIFY == changeType)
        sprintf( buffer, "%s", "Attribute Modified" );
    else if (LDAP_CHANGETYPE_MODDN == changeType)
        sprintf( buffer, "%s", "DN Changed" );
    else
        sprintf( buffer, "Change type '%d' unknown.", changeType );

    return buffer;
}
