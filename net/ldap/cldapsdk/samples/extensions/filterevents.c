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
   filterevents.c 
***************************************************************************
   Description:

   The filterevents.c sample uses the filtered monitor events extension to 
   receive filtered notifications when a directory entry attribute is added
   or deleted to an object.  The progam monitors events for approximately 
   five minutes and then abandons the operation, unbinds and exits. 
   
   A notice is displayed each time an object attribute is  added or deleted
   ot an object. This sample requires eDirectory version 8.7 or greater.

   An event filter is patterned after the string representation of an LDAP 
   search filter. An event filter is contained in parenthesis "( )", and can 
   filter events based on one or more values returned by an event.

   For example, a value event (a change to an attribute value) returns the 
   following nine parameters in an EVT_ValueInfo structure:
     - verb 
     - perpetratorDN 
     - entryDN 
     - attributeName 
     - syntaxOID 
     - className 
     - timeStamp 
     - size 
     - value

   When monitoring a value event, you can specify a filter based on one or 
   more of the the nine values returned by this event.

   For example, the following event filter causes the server to return only 
   value events where the acted upon attribute is a title:

       (attributeName=title)

   More complex event filters can be created using the same syntax as LDAP 
   search filters.  Here are two examples:

   (&(entryDN=cn=user1,o=system)(perpetratorDN=cn=admin,o=system)
      (attributeName=fullName))

   (|(attributeName=modifiersName)(&(entryDN=cn=user1,o=system)
      (perpetratorDN=cn=admin,o=system)(attributeName=fullName)))

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ldapx.h>
#include <ldap_events.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

/* global data */
static char usage[] =
"\n Usage:   filterevents <host name> <port number> <login dn> <password>"
"\n Example: filterevents Acme.com 389 cn=admin,o=Acme secret \n";

#define EXECUTE_TIME   300  /* execute for 300 seconds (5 minutes) */

/* main entry point */
int main( int argc, char **argv )
{   
    int                version, ldapPort, rc, prc, msgID, badEventCount, i;
    int                eventCount, eventType, eventResult;
    int                finished, errorCode, resultCode;
    char               *ldapHost, *loginDN, *password, *errorMsg = NULL;
    LDAP               *ld;
    LDAPMessage        *result;
    time_t             startTime;
    EVT_ValueInfo      *valueInfo;
    EVT_EventSpecifier *badEvents = NULL;
    void               *eventData;
    struct timeval     timeOut = {10,0};   /* 10 second connection timeout */


    /* Specify events to be monitored along with filters - registering for 
     * EVT_ADD_VALUE : A value has been added to an object email attribute and
     * EVT_DEL_VALUE : A value has been deleted from an object telephone number
     * attribute */
    EVT_FilteredEventSpecifier  
        events[]  = { { EVT_ADD_VALUE, 
                        EVT_STATUS_ALL, 
                        (char *)"(attributeName=mail)"},
                      { EVT_DELETE_VALUE, 
                        EVT_STATUS_ALL, 
                        (char *)"(attributeName=telephoneNumber)"}};

    eventCount = sizeof(events)/sizeof(EVT_FilteredEventSpecifier);

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 5)
    {
        printf("%s", usage);
        return (1);
    }

    ldapHost = argv[1];
    ldapPort = atoi(argv[2]);
    loginDN  = argv[3];
    password = argv[4];

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\nLDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\nLDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind( ld );
        return( 1 );
    }
    printf("\nBind successful\n");

    printf("\nRegistering for events -");
    printf("\n\tEVT_ADD_VALUE    :filter (attributeName=mail) and ");
    printf("\n\tEVT_DELETE_VALUE :filter (attributeName=telephoneNumber)\n\n");

    /* Perform the MonitorEvent extended operation */
    rc =  ldap_monitor_events_filtered(ld, eventCount, events, &msgID);

    if ( rc != LDAP_SUCCESS )
    {
        printf("ldap_monitor_event: %s",  ldap_err2string( rc ) );
        ldap_unbind_s( ld );
        return( 1 );
    }

    /*
     * Loop, polling for results until EXECUTE_TIME seconds have passed,
     * the server shuts down or we lose the connection for some other reason.
     *
     * We call ldap_result with the "all" argument
     * set to LDAP_MSG_ONE.  This causes ldap_result() to return exactly one
     * result if at least one result is available.
     *
     * A timeout value of 5 seconds will cause ldap_result to block for 5 
     * seconds before returning if there are no results. This keeps the polling
     * loop from hogging the CPU. 
     */
    timeOut.tv_sec    = 5L;
    timeOut.tv_usec   = 0L;

    startTime = time(NULL); /* record the start time */
    
    printf("Monitoring events for %d minutes.\n", EXECUTE_TIME/60);
    finished = 0;
    while ( 0 == finished )
    {
        result = NULL;
        rc = ldap_result( ld, msgID, LDAP_MSG_ONE, &timeOut, &result );
        switch ( rc )
        {
        case -1:   /* some error occurred */
            ldap_get_option(ld, LDAP_OPT_ERROR_NUMBER, &errorCode);
            printf("Error in ldap_result: %s\n", ldap_err2string( errorCode ));
            finished = 1;  /* terminate polling loop */
            break;

        case 0:    /* Timed out, no result yet. */
            break;

        case LDAP_RES_EXTENDED: /* A Monitor Events extension failure */
            prc = ldap_parse_monitor_events_response(ld, result, 
                                                     &resultCode, 
                                                     &errorMsg, 
                                                     &badEventCount, 
                                                     &badEvents, 
                                                     0); /* don't free result */
            if (prc != LDAP_SUCCESS)
            {
                printf("Error in ldap_parse_monitor_events_response: %d", prc);
            }
            else
            {
                switch (resultCode)
                {
                case LDAP_OPERATIONS_ERROR:
                    printf("Server operations error: %s\n", errorMsg);
                    break;

                case LDAP_PROTOCOL_ERROR:
                    printf("Protocol error: %s\n", errorMsg);
                    if (NULL != badEvents)
                    {
                        for (i=0; i<badEventCount; i++)
                        {
                            printf("Bad Event ID: %d\n", 
                                   badEvents[i].eventType);
                        }
                    }
                    break;

                case LDAP_ADMINLIMIT_EXCEEDED:
                    printf(
                     "Maximum number of active event monitors exceeded.\n");
                    break;

                case LDAP_UNWILLING_TO_PERFORM:
                    printf("Extension is currently disabled\n");
                    break;

                default:
                    printf("Unexpected result: %d, %s\n", resultCode, errorMsg);
                }

            }
            finished = 1;
            break;


        case LDAP_RES_INTERMEDIATE: /* An event notification */
            prc = ldap_parse_ds_event(ld,               
                                      result,           
                                      &eventType,
                                      &eventResult,
                                      &eventData,
                                      0 );  /* don't free result */
            if ( prc != LDAP_SUCCESS )
            {
                printf("Error in ldap_parse_ds_event: %s\n", 
                       ldap_err2string( prc ));
            }
            else
            {
                if (EVT_ADD_VALUE == eventType)
                {
                    valueInfo = (EVT_ValueInfo *)eventData;
                    printf("Added new attribute: %s, value: %s to entry:%s\n", 
                          valueInfo->attributeName, valueInfo->value, 
                          valueInfo->entryDN);
                }
                else if (EVT_DELETE_VALUE == eventType)
                {
                    valueInfo = (EVT_ValueInfo *)eventData;
                    printf("Deleted attribute:%s with value: %s of entry:%s \n", 
                          valueInfo->attributeName, valueInfo->value, 
                          valueInfo->entryDN);
                }
                else
                {
                    printf("Unexpected event notification: %d\n", eventType);
                } 
                ldap_event_free(eventData);
            }
            break;

        default:
            break;
            
        } /* end switch on rc */

        if (NULL != result)
            ldap_msgfree(result);
        
        if (NULL != errorMsg)
            ldap_memfree(errorMsg);

        if (NULL != badEvents)
            ldapx_memfree(badEvents);

        /* Is it time to exit? */
        if ( (time(NULL) - startTime) > EXECUTE_TIME )
        {
            finished = 1; /* terminate polling loop */
            rc = ldap_abandon(ld, msgID);
            if (LDAP_SUCCESS == rc)
                printf("Event monitoring successfully abandoned.\n");
            else
                printf("Error in nldap_abandon: %s\n", ldap_err2string(rc));
        }
    } /* end polling loop */

    ldap_unbind( ld );  
 
    return( 0 );
}
