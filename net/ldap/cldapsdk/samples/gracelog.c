/* $Novell: gracelog.c,v 1.11 2003/05/12 12:21:21 $ */
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
    graceLog.c
***************************************************************************
   Description: If a password has expired and grace logins are enabled,
                the bind operation succeeds, but uses one grace login.
                An application may want to detect this situation and
                display a warning message to the user to change his or 
                her password.
                                
                In eDirectory 8.5, an interim solution was provided to
                detect expired passwords.  When a password is expired,
                a message is returned in the bind response of the form:
                    "NDS error: password expired (-223)"
                If desired, the application can read the user object to
                obtain the total and remaining grace logins.
                
                It is expected that a control will be added in a future
                release to allow detection of expired passwords.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#include <string.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */

static char usage[] =
"\nUsage:   gracelog <host name> <port number> <login dn> <password>" 
"\nExample: gracelog Acme.com 389 cn=admin,o=Acme secret\n";

int getGraceLoginInfo( LDAP* ld, char *loginDN);

int main( int argc, char **argv) { 
    
    int      rc, ldapPort, version;
    char     *ldapHost, *loginDN, *password, *errorString;    
    LDAP     *ld;
        
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 5) {
        printf("%s", usage);
        return (1);
    }
             
    ldapHost   =  argv[1];
    ldapPort   =  atoi(argv[2]);
    loginDN    =  argv[3];
    password   =  argv[4];
    
 
    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    
    /* 
     * Initialize an LDAP session 
     */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n\tLDAP session initialization failed.\n");
        return( 1 );
    }
    printf ( "\n\tNew LDAP session initialized.\n");    

    
    /* 
     * Simple bind to the server 
     */
    rc = ldap_simple_bind_s( ld, loginDN, password );

    if ( rc == LDAP_SUCCESS ) {
        printf("\n\tBind successful.\n");

        /*
         * Check the errorString returned from the eDirectory LDAP server.  
         * A NULL or an empty string indicates the password is valid.
         * If the password has expired, an error message string is returned 
         * from the server. The bind was successful but used a grace login.  
         * Display a warning message.
         *
         * Read the loginDN object and display the loginGraceLimit and 
         * loginGraceRemaining attributes.
         */
        if(ldap_get_option (ld, LDAP_OPT_ERROR_STRING, &errorString ) == 
                                                      LDAP_OPT_SUCCESS ) {
            
            if ( errorString != NULL ) {
                if (strlen(errorString) != 0) {
                    printf("\n\tPassword is expired for loginDN: %s.\n",loginDN);
                    printf("\tServer error string:  %s\n", errorString);
                    printf("\tGrace login used.\n");
                    getGraceLoginInfo(ld, loginDN);
                }
                ldap_memfree( errorString );
            }
        }        
    }    
    else {
        printf("\n\tldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }

    ldap_unbind_s( ld );    

    return ( 0 );
}

int getGraceLoginInfo( LDAP *ld, char *loginDN) {
    int         rc;
    char        **values, *attribute;
    char        *attrs[] = { "loginGraceRemaining","loginGraceLimit", NULL };
    BerElement  *ber;
    LDAPMessage *searchResult, *entry;

    /* Search the directory to get grace login attributes */
    rc = ldap_search_ext_s(  
        ld,                    /* LDAP session handle */
        loginDN,               /* object to read */
        LDAP_SCOPE_BASE,       /* search scope */
        "(objectclass=*)",     /* search filter */
        attrs,                 /* return grace login attributes */
        0,                     /* return both the attributes and the values */
        NULL,                  /* server controls */
        NULL,                  /* client controls */
        &timeOut,              /* search timeout */
        LDAP_NO_LIMIT,         /* no size limit */
        &searchResult );       /* returned results */

    if ( rc != LDAP_SUCCESS ) {
        printf("Unable to read loginDN object: %s\n", ldap_err2string( rc ));
        ldap_msgfree( searchResult );
        return ( rc );
    }                                    

    /* printout the grace login info */
    entry   =   ldap_first_entry( ld, searchResult );
    
    for (   attribute = ldap_first_attribute( ld, entry, &ber );
            attribute != NULL; 
            attribute = ldap_next_attribute( ld, entry, ber ) ) {   
            
        if( (values = ldap_get_values( ld, entry, attribute)) != NULL ) {             
            printf("\t\t%s: %s\n", attribute, values[0] );
            ldap_value_free( values );
        }
        ldap_memfree( attribute );
    }
    
    ber_free(ber, 0);
    ldap_msgfree( searchResult );

    return (0);
}
