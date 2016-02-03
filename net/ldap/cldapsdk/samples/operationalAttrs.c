/* $Novell: operationalAttrs.c,v 1.10 2003/05/12 12:57:03 $ */
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
    operationalAttrs.c
***************************************************************************
   Description: operationalAttrs.c shows LDAP operational attributes 
                of an entry.
                
                LDAP operational attributes are not returned in search 
                results unless explicitly requested by name.
                
                NOTE:  In eDirectory 8.7 and later, all operational
                       attributes may be read by specifying "+" in
                       the list of requested attributes.
                       For example:  char *attrs[] = { "+", NULL };
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
"\nUsage:   operationalAttrs <host name> <port number> <login dn> <password>" 
"\n                          <entryDN>"
"\nExample: operationalAttrs Acme.com 389 cn=admin,o=Acme secret"
"\n                          cn=James,o=Acme\n";

int  printOperationalAttrs( LDAP* , char* );
void printTime( char * );
void displayEntryFlags( int );

#define DS_ALIAS_ENTRY           0x0001
#define DS_PARTITION_ROOT        0x0002
#define DS_CONTAINER_ENTRY       0x0004
#define DS_CONTAINER_ALIAS       0x0008
#define DS_MATCHES_LIST_FILTER   0x0010
#define DS_REFERENCE_ENTRY       0x0020
#define DS_40X_REFERENCE_ENTRY   0x0040
#define DS_BACKLINKED            0x0080
#define DS_NEW_ENTRY             0x0100
#define DS_TEMPORARY_REFERENCE   0x0200
#define DS_AUDITED               0x0400
#define DS_ENTRY_NOT_PRESENT     0x0800
#define DS_ENTRY_VERIFY_CTS      0x1000
#define DS_ENTRY_DAMAGED         0x2000

int main( int argc, char **argv) {
    
    int      rc, ldapPort, version;
    char     *ldapHost, *loginDN, *password, *testObjDN;
    LDAP     *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6) {
        printf("%s", usage);
        return (1);
    }

    ldapHost   =  argv[1];
    ldapPort   =  atoi(argv[2]);
    loginDN    =  argv[3];
    password   =  argv[4];
    testObjDN  =  argv[5];

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);
    
    /* Initialize an LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n    LDAP session initialization failed.\n");
        return( 1 );
    }
    printf ( "\n    New LDAP session initialized.\n");
    
    /* Simple bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );

    if ( rc == LDAP_SUCCESS ) {
        printf("\n    Bind successful.\n");
    }
    else {
        printf("\n    ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }

    /* Get LDAP operational attributes of the entry */
    if ( (rc = printOperationalAttrs( ld, testObjDN )) != LDAP_SUCCESS )
        printf("\n    Unable to read entryDN object: %s\n", 
                                                  ldap_err2string( rc ));
    
    ldap_unbind_s( ld );

    return ( 0 );
}

/*
 * printOperationalAttrs() searches the directory to get LDAP 
 * operational attributes of the entry specified by dn, and then 
 * prints them out.
 */
int printOperationalAttrs( LDAP *ld, char * dn ) {
        
    int         rc, flags;    
    char        **values, *attribute;
    struct timeval timeOut = {10,0};   /* 10 second search timeout */
    BerElement  *ber;
    LDAPMessage *searchResult, *entry;
    char        *attrs[] = { "createTimeStamp",
                             "creatorsName",
                             "entryFlags",
                             "federationBoundary",
                             "localEntryID",
                             "modifiersName",
                             "modifyTimeStamp",
                             "structuralObjectClass",
                             "subordinateCount",
                             "subschemaSubentry",
                              NULL };

    /* Get the entry's LDAP operational attributes */
    rc = ldap_search_ext_s(  
        ld,                    /* LDAP session handle */
        dn,                    /* object to read */
        LDAP_SCOPE_BASE,       /* search scope */
        "(objectclass=*)",     /* search filter */
        attrs,                 /* return the LDAP operational attributes */
        0,                     /* return both the attributes and the values */
        NULL,                  /* server controls */
        NULL,                  /* client controls */
        &timeOut,              /* search timeout */
        LDAP_NO_LIMIT,         /* no size limit */
        &searchResult );       /* returned results */

    if ( rc != LDAP_SUCCESS ){
        ldap_msgfree( searchResult );
        return ( rc );
       }

    /* Print out the LDAP operational attributes and values */
    entry   =   ldap_first_entry( ld, searchResult );

    printf("\n    Entry: %s\n", dn);
    printf("\n    LDAP Operational Attributes: ");
    
    for (   attribute = ldap_first_attribute( ld, entry, &ber );
            attribute != NULL; 
            attribute = ldap_next_attribute( ld, entry, ber ) ) {
        
        if( (values = ldap_get_values( ld, entry, attribute)) != NULL ) {
            if ( (strcmp( attribute, "modifyTimeStamp" ) == 0) 
                                                && (values[0] != NULL)) {
                printf("\n        %s:", attribute );
                printf("\n                %s (UTC)", values[0] );
                printTime( values[0] );
            }
            else if ( (strcmp( attribute, "createTimeStamp" ) == 0)
                                                && (values[0] != NULL)) {
                printf("\n        %s:", attribute );
                printf("\n                %s (UTC)", values[0] );
                printTime( values[0] );
            }
            else if ( (strcmp( attribute, "entryFlags" ) == 0)
                                                && (values[0] != NULL)) {
                sscanf( values[0], "%d", &flags);
                if ( flags  != 0 )    
                    displayEntryFlags( flags );
            }
            else if ( values[0] != NULL )
                printf("\n        %s: %s", attribute, values[0] );
            
            ldap_value_free( values );
            ldap_memfree( attribute );
        }        
    }

    printf("\n\n");    
    ber_free(ber, 0);
    ldap_msgfree( searchResult );

    return rc;
}

/* displayEntryFlags() decodes and displays OR'ed entry flags */
void displayEntryFlags( int flags ) 
{    
    char entryFlags[128];    
                    
    /* Start from the beginning of the buffer */
    strcpy( entryFlags, "" );
        
    if ( flags & DS_ALIAS_ENTRY )
        strcat( entryFlags, "AliasEntry ");
    if ( flags & DS_PARTITION_ROOT )
        strcat( entryFlags, "PartionRoot ");
    if ( flags & DS_CONTAINER_ENTRY )
        strcat( entryFlags, "ContainerEntry ");
    if ( flags & DS_CONTAINER_ALIAS )
        strcat( entryFlags, "ContainerAlias ");
    if ( flags & DS_MATCHES_LIST_FILTER )
        strcat( entryFlags, "MatchesListFilter ");
    if ( flags & DS_REFERENCE_ENTRY )
        strcat( entryFlags, "ReferenceEntry ");
    if ( flags & DS_40X_REFERENCE_ENTRY )
        strcat( entryFlags, "40XReferenceEntry ");
    if ( flags & DS_BACKLINKED )
        strcat( entryFlags, "Backlinked ");
    if ( flags & DS_NEW_ENTRY )
        strcat( entryFlags, "NewEntry ");
    if ( flags & DS_TEMPORARY_REFERENCE )
        strcat( entryFlags, "TemporaryEntry ");
    if ( flags & DS_AUDITED )
        strcat( entryFlags, "Audited ");
    if ( flags & DS_ENTRY_NOT_PRESENT )
        strcat( entryFlags, "EntryNotPresent ");
    if ( flags & DS_ENTRY_VERIFY_CTS )
        strcat( entryFlags, "EntryVarifyCTS ");
    if ( flags & DS_ENTRY_DAMAGED )
        strcat( entryFlags, "EntryDamaged");
 
    printf("\n        entryFlags: %s", &entryFlags);
}

/* printTime() prints a Generalized Time string in a more readable format */
void printTime( char *utc ) {
    
    char   *time;
    struct tm UTC;

    /* Initialize UTC fields with year, mon, day, hour, min, and sec 
     * in utc. utc is in the form of "yyyymmddhhmmssZ"
     */
    sscanf( utc, "%4d%02d%02d%02d%02d%02d", &UTC.tm_year, &UTC.tm_mon,
        &UTC.tm_mday, &UTC.tm_hour, &UTC.tm_min, &UTC.tm_sec );

    /* Adjust year and month field */
    UTC.tm_year -= 1900;
    UTC.tm_mon  -= 1;
    UTC.tm_isdst = 0;

    if ( mktime(&UTC) != (time_t)-1 ) {
        time = asctime(&UTC);
        /* Remove trailing new line char */
        time[strlen(time)-1] = '\0';
        printf( "\n                %s (UTC)", time );
    }
}
