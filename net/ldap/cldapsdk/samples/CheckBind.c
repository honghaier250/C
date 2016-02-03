/* $Novell: CheckBind.c,v 1.12 2003/05/12 12:03:45 $ */
/***************************************************************************
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
****************************************************************************
    checkBind.c
****************************************************************************
   Description: This sample verifies that a user has a valid password and
                is authorized to bind, without actually doing the bind.
                
                Even with a valid password, there are a number of other 
                restrictions that could prevent a user from binding.  
                This sample reads the user object and checks the password
                as well as these other restrictions.

                checkBind.c checks the attributes 'LoginDisabled', 
                'loginExpirationTime', 'passwordExpirationTime', 
                'loginAllowedTimeMap', and 'lockedByIntruder'
                to determine if the user can bind to the directory at 
                the current time.
                
                checkBind.c does not check 'networkAddressRestriction' and
                'loginMaximumSimultaneous' attributes. The settings of 
                those attributes can still prevent the user from binding
                to the directory.
                
                In order to access these attributes, the login DN should
                have admin or admin equivalent rights.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\nUsage:   checkBind <host name> <port number> <login dn> <password> <user DN>"
"\n                   <user password>" 
"\nExample: checkBind Acme.com 389 cn=admin,o=Acme secret cn=test,o=Sales"
"\n                   userpwd\n";

int  checkBindRestrictions( LDAP *, char *, char *, int * );
int  getTimeRestriction( unsigned char *, char * );
int  compareTime( char *, struct tm * );
void printUTC( char *);

#define LENGTH  42          /* Expected length of loginAllowedTimeMap data */ 

int main( int argc, char **argv) { 
    
    int      rc, ldapPort, version, cr = 0;
    char     *ldapHost, *loginDN, *password, *userDN, *userPWD;
    LDAP     *ld;
    struct timeval timeOut = {10,0};    /* 10 second connection timeout */
        
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 7) {
        printf("%s", usage);
        return (1);
    }

    ldapHost   =  argv[1];
    ldapPort   =  atoi(argv[2]);
    loginDN    =  argv[3];
    password   =  argv[4];
    userDN     =  argv[5];
    userPWD    =  argv[6];

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);
   
    /* initialize an LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n    LDAP session initialization failed.");
        return( 1 );
    }
    printf ( "\n    New LDAP session initialized.");    

    /* simple bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );

    if ( rc == LDAP_SUCCESS )
        printf("\n    Bind successful.");
    else {
        printf("\n    ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }

    printf("\n\n    Entry DN: %s", userDN );
    printf("\n    Checking bind restrictions...\n");
    if ( (rc = checkBindRestrictions( ld, userDN, userPWD, &cr) ) 
                                                    == LDAP_SUCCESS ) {
        if ( cr == 0 )   
            printf( "\n\n    User can login now.\n\n" );
        else
            printf( "\n\n    User cannot login now.\n\n" );
    }
    else {
        printf( "\n\n    Failed to check bind restrictions." );
        printf( "\n    Error: %s.\n", ldap_err2string( rc ));
    }

    ldap_unbind_s( ld ); 

    return ( 0 );
}

/* 
 * checkBindRestrictions() returns LDAP error code rc and check
 * result cr. If cr's value is '0' the user passed the test and
 * can login now. Otherwise the user cannot login currently.
 */
int checkBindRestrictions( LDAP *ld, char *userDN, char *userPWD, int *cr) {        

    int           rc;
    ber_len_t     len;
    unsigned char *bv;
    char          **values, *attribute, *localTime, *currentUTC;
    time_t        ltime;
    struct        timeval timeOut;
    struct        tm *gmTime;
    BerElement    *ber;
    BerValue      **berValue = NULL;
    LDAPMessage   *searchResult, *entry;
    char          *attrs[] = { "LoginDisabled",     /* Attributes to return */
                               "loginExpirationTime",
                               "passwordExpirationTime",
                               "loginAllowedTimeMap",
                               "lockedByIntruder",
                               NULL };
    
    timeOut.tv_sec    = 10L;
    timeOut.tv_usec   = 0L;

    time( &ltime );
    localTime = strdup( ctime( &ltime ) );      /* Save local time */
    localTime[strlen(localTime)-1] = '\0';      /* Remove newline char */
    gmTime = gmtime( &ltime );                  /* Save UTC time */
    currentUTC = strdup(asctime(gmTime));
    currentUTC[strlen(currentUTC)-1] = '\0';    /* Remove newline char */

    /* check user's password */
    printf("\n        check user's password: ");
    rc = ldap_compare_s( ld,              /* LDAP session handle */
                         userDN,          /* the object to verify password on */
                         "userPassword",  /* attribute to compare */
                         userPWD);        /* the password to verify */
    if ( rc == LDAP_COMPARE_TRUE)
        printf(" OK.\n");    
    else if ( rc == LDAP_COMPARE_FALSE ) {
        printf(" User's password is not correct.\n");
        *cr = -1;
    }
    else if ( rc == LDAP_NO_SUCH_ATTRIBUTE ) {
        printf(" User has no password.\n");
        *cr = -1;
    }
    else if ( rc == LDAP_NO_SUCH_OBJECT ) {
        printf(" User does not exist.\n");
        *cr = -1;
    }
    else {
        free( localTime );
        free( currentUTC );
        return rc;
    }

    /* search to get the entry's login attributes */
    rc = ldap_search_ext_s(  
        ld,                    /* LDAP session handle */
        userDN,                /* object to read */
        LDAP_SCOPE_BASE,       /* search scope */
        "(objectclass=*)",     /* search filter */
        attrs,                 /* return login attributes */
        0,                     /* return both the attributes and the values */
        NULL,                  /* server controls */
        NULL,                  /* client controls */
        &timeOut,              /* time out */
        LDAP_NO_LIMIT,         /* no size limit */
        &searchResult );       /* returned results */

    if ( rc != LDAP_SUCCESS )
       {
        free( localTime );
        free( currentUTC );
        ldap_msgfree( searchResult );    
        return ( rc );
       }

    entry = ldap_first_entry( ld, searchResult );    
    for ( attribute = ldap_first_attribute( ld, entry, &ber );
          attribute != NULL; 
          attribute = ldap_next_attribute( ld, entry, ber ) ) {
        
        if ( strcmp( attribute, "loginAllowedTimeMap") == 0 ) {
            berValue = ldap_get_values_len(ld, entry, attribute);
            if ( berValue != NULL ) {
                len = berValue[0]->bv_len;
                /* check the data returned */
                printf("\n        check 'LoginAllowedTimeMap': ");
                if ( (len == LENGTH) && 
                     ((bv = (unsigned char*)berValue[0]->bv_val) != NULL) ) {                    
                    if ( getTimeRestriction( bv, localTime) == 0)
                        printf(" OK.\n");
                    else {
                        printf(" has time restriction\n");
                        *cr = -1;
                    }
                }
                else
                    printf(" 'LoginAllowedTimeMap' not initialized\n");
            }
        }
        else {           
            if ( ((values = ldap_get_values( ld, entry, attribute)) != NULL) ) {
                /* Check 'LoginDisabled' */
                if ( strcmp( attribute, "LoginDisabled") == 0 ) {
                    printf("\n        check 'Logindisabled': ");
                    if ( strcmp( values[0], "FALSE" ) == 0 )
                        printf(" OK.\n");            
                    else {
                        printf(" the user's account is disabled\n");
                        *cr = -1;
                    }        
                }
                /* Check 'loginExpirationTime' */
                else if ( strcmp( attribute, "loginExpirationTime") == 0 ) {
                    printf("\n        check 'loginExpirationTime': ");
                    if ( compareTime( values[0], gmTime)==0)
                        printf(" OK.\n");
                    else {            
                        printf(" exceeded login expiration time.");
                        printf("\n            loginExpirationTime:");
                        /* print loginExpirationTime in LDAP Timestamp format */
                        printf(" %s          (UTC)", values[0]);                        
                        /* print loginExpirationTime in UTC text format */
                        printf("\n                                 ");
                        printUTC( values[0] );
                        /* print current UTC in text format */
                        printf("\n                   Current Time:");
                        printf(" %s (UTC)", currentUTC);
                        /* print current local time in text format */
                        printf("                                  ");
                        printf("                %s (Local)\n", localTime);
                        *cr = -1;
                    }
                }
                /* Check 'passwordExpirationTime' */
                else if ( strcmp( attribute, "passwordExpirationTime") == 0 ) {
                    printf("\n        check 'passwordExpirationTime': ");    
                    if ( compareTime( values[0], gmTime ) ==0)
                        printf(" OK.\n");
                    else {
                        printf("exceeded password expiration time");
                        printf("\n            passwordExpirationTime:");
                        /* print loginExpirationTime in LDAP Timestamp format */
                        printf(" %s          (UTC)", values[0]);                        
                        /* print loginExpirationTime in UTC text format */
                        printf("\n                                    ");
                        printUTC( values[0] );
                        /* print current UTC in text format */
                        printf("\n                      Current Time:");
                        printf(" %s (UTC)", currentUTC);
                        /* print current local time in text format */
                        printf("\n                                  ");
                        printf("  %s (Local)", localTime);
                        *cr = -1;
                    }         
                }
                /* Check 'lockedByIntruder' */
                else if ( strcmp( attribute, "lockedByIntruder") == 0 ) {
                    printf("\n        check 'lockedByIntruder': ");
                    if ( strcmp( values[0], "FALSE" ) == 0 )
                        printf(" OK.\n");
                    else {
                        printf(" intruder detection locked the account\n");
                        *cr = -1;
                    }
                }
                ldap_value_free( values );
            }
        }        
        ldap_memfree( attribute );
    }
    
    free( localTime );
    free( currentUTC );
    ber_free( ber, 0);
    ldap_msgfree( searchResult );    

    return rc;
}

/* 
 * getTimeRestriction() checks the localTime flagged by a
 * specific bit in bytes. It returns '0' if the bit value
 * is '1'. Otherwise '-1' is returned to indicate time 
 * restriction. 
 */
int getTimeRestriction( unsigned char *bytes, char *localTime ) {
        
    int           i, index, day = 0, hour, minute;   
    char          weekday[4];
    unsigned char byte0, byte1, temp;

    /* loginAllowedTimeMap has 42 bytes ( e.g. 336 bits) to flag 336 
     * thirty-minute time intervals in a week. We make some transformations
     * to put the bits into an order that's easier to use:
     *     1. reverse bit order in all the bytes.       
     *     2. shift the first 14 bits to the end of byteValue
     * Then the 336 intervals are in the order of 
     * Sun 12:00AM-12:30AM,  Sun 12:30AM-1:00Am, ..., 
     * Sat 11:00PM-11:30PM, Sat 11:30PM-12:00AM 
     */
               
    /* reverse the bit order in all the bytes */
    for ( i = 0, temp = 0; i < LENGTH; i ++, temp = 0 ) {
        if ( bytes[i] & 0x80 )
             temp |= 0x01;                 
        if ( bytes[i] & 0x40 )
             temp |= 0x02;                
        if ( bytes[i] & 0x20 )
             temp |= 0x04;                
        if ( bytes[i] & 0x10 )
             temp |= 0x08;                
        if ( bytes[i] & 0x08 )
             temp |= 0x10;                 
        if ( bytes[i] & 0x04 )
             temp |= 0x20;                 
        if ( bytes[i] & 0x02 )
             temp |= 0x40;                 
        if ( bytes[i] & 0x01 )
             temp |= 0x80;                                  
        bytes[i] = temp;
    }
    
    /* Shift the first 16 bits (first 2 bytes) to the end of bytes */
    byte0 = bytes[0];
    byte1 = bytes[1];
    for ( i = 2; i < LENGTH; i++ )
        bytes[i-2] = bytes[i];
    
    bytes[LENGTH-2] = byte0;
    bytes[LENGTH-1] = byte1;
    
    /* Shift all the bits in bytes right two times,
     * and move the last 2 bits in bytes to the 
     * first 2 bits of bytes
     */
    temp = bytes[LENGTH-1] & 0x03; /* save last 2 bits */
    
    for ( i = LENGTH - 1; i > 0; i -- ) { /* do the shift */
        bytes[i] = bytes[i] >> 2;
        if ( bytes[i-1] & 0x01 )
            bytes[i] |= 0x40;
        if ( bytes[i-1] & 0x02 )
            bytes[i] |= 0x80;
    }
    /* Shift the first byte */
    bytes[0] = bytes[0] >> 2; 
    /* The last bit to be the second */
    if ( temp & 0x01 ) 
        bytes[0] |= 0x40;
     /* The second to last bit to be the first */
    if ( (temp & 0x02) != 0 ) 
        bytes[0] |= 0x80;   
    
    
    /* Get weekday, hour, and minute from localTime */
    sscanf( localTime +  0, "%s", weekday );
    sscanf( localTime + 11, "%d", &hour );
    sscanf( localTime + 14, "%d", &minute );

     if (     strcmp( weekday, "Sun") == 0 )
        day = 0;
    else if ( strcmp( weekday, "Mon") == 0 )
        day = 1;
    else if ( strcmp( weekday, "Tue") == 0 )
        day = 2;
    else if ( strcmp( weekday, "Wed") == 0 )
        day = 3;
    else if ( strcmp( weekday, "Thu") == 0 )
        day = 4;
    else if ( strcmp( weekday, "Fri") == 0 )
        day = 5;
    else if ( strcmp( weekday, "Sat") == 0 )
        day = 6;

    /* Calculate total index */    
    index = ( day * 24 + hour ) * 2 + minute / 30;    

    /* Check time restriction */
    if ( !(bytes[index / 8] & 1<<(7 - index % 8) ))
        return -1;
 
    return ( 0 );
}

/* 
 * compareTime() returns 0 if utc1 is later than utc2.
 * Otherwise -1 is returned.
 */
int compareTime( char *utc1, struct tm *utc2 ) {        
    
    char   currentTime[32];
    
    /* create LDAP Timestamp from utc2 */         
    sprintf( currentTime, "%4d%02d%02d%02d%02d%02d%c", utc2->tm_year+1900, 
             utc2->tm_mon+1, utc2->tm_mday, utc2->tm_hour, utc2->tm_min, 
             utc2->tm_sec, 'Z' );
    
    if ( strcmp( utc1, currentTime) <= 0 )
        return -1;
    
    return ( 0 );
}

/* printUTC() prints UTC in text format */
void printUTC( char *utc ) {
    
    char   *time;
    struct tm UTC;
    
    /* Initialize UTC fields with UTC's year, month, day, hour, minute, 
     * and second. UTC is in the form of "yyyymmddhhmmssZ"
     */
    sscanf( utc, "%4d%02d%02d%02d%02d%02d", &UTC.tm_year, &UTC.tm_mon,
                    &UTC.tm_mday, &UTC.tm_hour, &UTC.tm_min, &UTC.tm_sec );   

    /* Adjust year and month field in UTC */
    UTC.tm_year -= 1900;
    UTC.tm_mon  -= 1;
    UTC.tm_isdst = 0;

    if ( mktime(&UTC) != (time_t)-1 ) {
        time = asctime(&UTC);
        time[strlen(time)-1] = '\0';     /* Get rid of new line char */
        printf( "%s (UTC)", time );
    }
}
