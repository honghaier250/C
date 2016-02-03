/* $Novell: modifyTimeStamp.c,v 1.10 2003/05/12 12:26:34 $ */
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
    modifyTimeStamp.c
***************************************************************************
   Description: modifyTimeStamp.c shows how to modify an attribute in 
                LDAP Timestamp format. 

                LDAP Timestamp is in the form of "yyyymmddhhmmssZ". The 
                trailing 'Z' indicates the time is expressed in
                Coordinated Universal Time (UTC). The date and time
                "Mon Jul 30 17:42:00 2001"  is represented in LDAP Time-
                stamp as "20010730174200Z".

                modifyTimeStamp.c modifies an entry's passwordExpirationTime 
                attribute. It reads and displays the attribute, modifies
                the attribute, then reads and displays it again.
                
                The last parameter to this sample is an integer number of
                days used to extend the user's passwordExpirationTime. 
                It can be a positive, a zero, or a negative value.                
                
                Before running this sample, the passwordExpirationTime
                attribute should be initialized.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#include <time.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\nUsage:   modifyTimeStamp <host name> <port number> <login dn> <password>"
"\n         <user DN> <day extension>" 
"\nExample: modifyTimeStamp Acme.com 389 cn=admin,o=Acme secret" 
"\n         cn=James,o=Sales 90\n";

int  getTime( LDAP *, char *, char *);
void printTime( char * );
int  extendTime( char * , int );

int main( int argc, char **argv) { 
    
    int      rc, ldapPort, version, days;
    char     *ldapHost, *loginDN, *password, *userDN, time[32], *timeValues[2];
    LDAP     *ld;
    LDAPMod  modTime, *modify[2];
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 7) {
        printf("%s", usage);
        return 1;
    }

    ldapHost   =  argv[1];
    ldapPort   =  atoi(argv[2]);
    loginDN    =  argv[3];
    password   =  argv[4];
    userDN     =  argv[5];
    days       =  atoi(argv[6]);
    
    if ( days == 0 ) {
        printf("\n    day extension is zero. "
               "Timestamp was not modified.\n\n");
        return (0);
    }

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize an LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n    LDAP session initialization failed.\n");
        return 1;
    }
    printf ( "\n    LDAP session initialized.\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );

    if ( rc == LDAP_SUCCESS )
        printf("\n    Bind successful.\n");   
    else {
        printf("\n    ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return 1;
    }    
    
    /* Read the user's passwordExpirationTime */
    printf("\n    Entry DN: %s", userDN);
    printf("\n    Day Extension: %d\n", days);

    if ( ( rc = getTime( ld, userDN, time )) != LDAP_SUCCESS ) {
        printf("\n    Unable to read the entry: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return 1;
    }

    if ( strlen(time) == 0 ) {
        printf("\n    passwordExpirationTime was not initialized.\n\n");
        ldap_unbind_s( ld );
        return 1;
    }
    else {
    
        /* Display user's passwordExpirationTime */
        printf( "\n        passwordExpirationTime before modification");
        printf( "\n            passwordExpirationTime: %s (UTC)", time );
        printTime( time );
      
        /* Update user's passwordExpirationTime */
        if ( (rc = extendTime( time, days )) != LDAP_SUCCESS ) {
            printf( "\n\n    Failed to extend time.\n" );
            ldap_unbind_s( ld );
            return 1;
        }

        /* Prepare to modify user's passwordExpirationTime */
        modTime.mod_op     = LDAP_MOD_REPLACE;
        modTime.mod_type   = "passwordExpirationTime";
        timeValues[0]      = time;
        timeValues[1]      = NULL;
        modTime.mod_values = timeValues;
        modify[0]          = &modTime;
        modify[1]          = NULL;

        /* Modify the user's passwordExpirationTime */
        printf("\n        Modifying passwordExpirationtime..." );
        rc = ldap_modify_ext_s( ld,         /* LDAP session handle */
                                userDN,     /* the object to modify */
                                modify,     /* array of LDAPMod structures */
                                NULL,       /* server controls */
                                NULL);      /* client controls */

        if ( rc != LDAP_SUCCESS ) {
            printf("ldap_modify_ext_s: %s\n", ldap_err2string( rc ));
            ldap_unbind_s( ld );
            return 1;
        }
        printf("\n        passwordExpirationTime was modified successfully.\n");
    
        /* Do search again to get user's new passwordExpirationTime value */
        if ( ( rc = getTime( ld, userDN, time )) != LDAP_SUCCESS ) {
            printf( "\n    Failed to get passwordExpirationTime attribute." );
            ldap_unbind_s( ld );
            return 1;
        }

        if ( strlen(time) == 0 ) {
            printf("\n    Failed to get passwordExpirationTime.\n\n");
            ldap_unbind_s( ld );
            return 1;
        }
        else {
            /* Display user's new passwordExpirationTime value */
            printf( "\n        passwordExpirationTime after modification" );
            printf( "\n            passwordExpirationTime: %s (UTC)", time );
            printTime( time );
        }
    }    
    
    printf("\n");

    ldap_unbind_s( ld );

    return 0;
}

/* 
 * getTime() reads the directory to get passwordExpirationTime.
 * It returns the passwordExpirationTime value and LDAP error code.
 */
int getTime( LDAP *ld, char *dn, char *utc) {
    
    int         rc;
    char        **values, *attr;
    char        *attrs[] = { "passwordExpirationTime", NULL };
    struct timeval timeOut = {10,0};   /* 10 second search timeout */
    BerElement  *ber;
    LDAPMessage *searchResult, *entry;

    /* Search the directory to get passwordExpirationTime attribute */
    rc = ldap_search_ext_s(  
         ld,                    /* LDAP session handle */
         dn,                    /* object to read */
         LDAP_SCOPE_BASE,       /* search scope */
         "(objectclass=*)",     /* search filter */
         attrs,                 /* return passwordExpirationTime attributes */
         0,                     /* return both the attributes and the values */
         NULL,                  /* server controls */
         NULL,                  /* client controls */
         &timeOut,              /* search timeout */
         LDAP_NO_LIMIT,         /* no size limit */
         &searchResult );       /* returned results */

    if ( rc != LDAP_SUCCESS )        
        return rc ;

    if (    (entry = ldap_first_entry( ld, searchResult ))    != NULL
         && (attr  = ldap_first_attribute( ld, entry, &ber )) != NULL
         && (values = ldap_get_values( ld, entry, attr))      != NULL) {
        strcpy( utc, values[0] );
        ldap_value_free( values );         
        ldap_memfree( attr );       
    }
    else
        strcpy( utc, "");
    
    ber_free(ber, 0);
    ldap_msgfree( searchResult );

    return 0;
}

/* printTime() takes prints an LDAP timestamp in a more readable form */ 
void printTime( char * utc ) {
    
    char   *time;
    struct tm UTC;
            
    /* Initialize UTC fields with utc year, mon, day, hour, minute, 
     * and second. utc is in the form of "yyyymmddhhmmssZ"
     */
    sscanf( utc, "%4d%2d%2d%2d%2d%2d", &UTC.tm_year, &UTC.tm_mon, 
             &UTC.tm_mday, &UTC.tm_hour, &UTC.tm_min, &UTC.tm_sec );

    /* Adjust year and month fields in UTC */
    UTC.tm_mon  -= 1;               /* Month 0-11 */
    UTC.tm_year -= 1900;            /* Year since 1900 */
    UTC.tm_isdst = 0;

    /* Convert to readable text string */
    if( mktime( &UTC ) != (time_t)-1 )  {
        time = asctime(&UTC);
        /* Remove the trailing new line char */
        time[strlen(time)-1] = '\0';
        printf("\n%60s (UTC)", time);
    }
}

/* extendTime() takes utc and days as input and returns updated utc */
int extendTime( char * utc, int days) {
    
    struct tm UTC;

    /* Convert LDAP timestamp format (yyyymmddhhmmssZ) into a struct tm,
       containing separate fields for year, month, day, hour, minute, second.
     */
    sscanf( utc, "%4d%2d%2d%2d%2d%2d", &UTC.tm_year, &UTC.tm_mon, 
             &UTC.tm_mday, &UTC.tm_hour, &UTC.tm_min, &UTC.tm_sec );
        
    /* Adjust year and month fields in UTC */
    UTC.tm_mon  -= 1;              /* Month 0-11 */      
    UTC.tm_year -= 1900;           /* Year since 1900 */ 
    UTC.tm_isdst = 0;              /* Let library determine if daylight time */

    /* Add (or subtract) day extension here */
    UTC.tm_mday += days;
        
    /* mktime normalizes each field in the struct tm */
    if ( mktime( &UTC ) == (time_t)-1 )
        return -1;    
        
    /* Adjust year and month fields in UTC */    
    UTC.tm_year += 1900;
    UTC.tm_mon  += 1;    
            
    /* Convert back to LDAP timestamp form */    
    sprintf( utc,  "%4d%02d%02d%02d%02d%02d%c", UTC.tm_year, UTC.tm_mon,
               UTC.tm_mday,UTC.tm_hour, UTC.tm_min, UTC.tm_sec, 'Z');

    return 0;
}
