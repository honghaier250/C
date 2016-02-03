/* $Novell: monitorevents.c,v 1.11 2003/05/12 13:20:30 $ */
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
   monitorevents.c 
***************************************************************************
   Description: The monitorevents.c sample uses the monitor events
                extension to receive notifications when a directory entry 
                is created or deleted. The progam monitors events for 
                approximately five minutes and then abandons the operation,
                unbinds and exits. A notice is displayed each time an object
                is added to or deleted from the directory. This sample
                requires eDirectory version 8.7 or greater.
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
"\n Usage:   monitorevents <host name> <port number> <login dn> <password>"
"\n Example: monitorevents Acme.com 389 cn=admin,o=Acme secret\n";

#define EXECUTE_TIME   1800  /* execute for 300 seconds (5 minutes) */

void PrintIPAddress(void *pIP)
{

	EVT_NetAddress* Ip = NULL;
	Ip = (EVT_NetAddress*) pIP;
	int i=0, p = 0;	
	char* buffer= (char*)malloc(256);
	char *ep = buffer;
	static const char *lcafmt[] = {"%02x%02x:", ":%02x%02x"};
	const char **afmt = lcafmt;

	p=Ip->data[0] & 0xff;
	p<<=8;
	p|=(Ip->data[1] & 0xff);
	if (Ip->length != 0) 
	{
		if(Ip->length == 6)
		{
			printf("length = %d   ",Ip->length);	
			printf("Connection Type = %d   ",Ip->type);	
			sprintf(buffer, "%u.%u.%u.%u:%d", (Ip->data[2]) & 0xff, (Ip->data[3]) & 0xff, (Ip->data[4]) & 0xff, (Ip->data[5]) & 0xff,p);
			printf("IP Address=%s\n",buffer);
		}
		else if(Ip->length == 18) 
		{

			*ep++ = '[';
			for (i = 2; i < 18 && (Ip->data[i] || Ip->data[i+1]); i += 2)
				ep += sprintf(ep, afmt[0], Ip->data[i], Ip->data[i+1]);
			if (i == 2)
				*ep++ = ':';
			for ( ; i < 18 && !Ip->data[i] && !Ip->data[i+1]; i += 2)
				;       // skip all zeros, but only once
			for ( ; i < 18; i += 2)
				ep += sprintf(ep, afmt[1], Ip->data[i], Ip->data[i+1]);
			if (ep[-1] == ':')
				ep--;           // in case we didn't take the shortcut, overwrite the ':'
			*ep++ = ']';
			*ep = '\0';
			printf("length = %d   ",Ip->length);	
			printf("Connection Type = %d   ",Ip->type);	
			printf("IP Address=%s",buffer);
			printf(":%d\n",p);

		}
	}
	if( buffer!= NULL)
		free(buffer);
}
/* main entry point */
int main( int argc, char **argv )
{   
    int                version, ldapPort, rc, prc;
    int                msgID;
    char               *ldapHost, *loginDN, *password;       
    LDAP               *ld;
    LDAPMessage        *result;
    int                finished, errorCode, resultCode;
    time_t             startTime;
    char               *errorMsg = NULL;
    int                eventType, eventResult;
    EVT_EntryInfo      *entryInfo;
    int                badEventCount;
    EVT_EventSpecifier *badEvents = NULL;
    void               *eventData;
    int                i;
    struct timeval     timeOut = {10,0};   /* 10 second connection timeout */
	 void          *pIP = NULL; 
	 EVT_NetAddress          *IP = NULL; 

    /* Specify events to be monitored */
    EVT_EventSpecifier  events[]  = {
        {EVT_CREATE_ENTRY, EVT_STATUS_ALL},       /* All create entry events */
        {EVT_LDAP_ADD, EVT_STATUS_SUCCESS},       /* All create entry events */
        {EVT_DELETE_ENTRY, EVT_STATUS_SUCCESS} }; /* only successful deletes */

    int eventCount = sizeof(events)/sizeof(EVT_EventSpecifier);

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

    /* Perform the MonitorEvent extended operation */
    rc =  ldap_monitor_events(ld, eventCount, events, &msgID);

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
            prc = ldap_parse_ds_event_IP(ld,               
                                      result,           
                                      &eventType,
                                      &eventResult,
                                      &eventData,
                                      0, &pIP );  /* don't free result */
            if ( prc != LDAP_SUCCESS )
            {
                printf("Error in ldap_parse_ds_event: %s\n", 
                       ldap_err2string( prc ));
            }
	    else
	    {
		    printf("Event notification: %d\t", eventType);
		    IP = (EVT_NetAddress*) pIP;
		    if(pIP !=NULL)
			    PrintIPAddress(pIP);
		    if (EVT_CREATE_ENTRY == eventType)
		    {
			    entryInfo = (EVT_EntryInfo *)eventData;
			    IP = (EVT_NetAddress*) pIP;
		    }
		    else if (EVT_DELETE_ENTRY == eventType)
		    {
			    entryInfo = (EVT_EntryInfo *)eventData;
			    printf("Deleted entry: %s\n", entryInfo->entryDN);
		    }
		    if( eventData!=NULL )
		    {
			    ldap_event_free(eventData);
			    eventData = NULL;
		    }
		    if( pIP!=NULL )
		    {
			    ldap_event_free(pIP);
			    pIP = NULL;
		    }
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
