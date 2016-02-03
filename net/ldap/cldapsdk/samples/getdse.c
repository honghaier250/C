/* $Novell: getdse.c,v 1.18 2003/05/12 12:20:26 $ */
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
   getdse.c 
***************************************************************************
   Description: The getdse.c sample retrieves the root DSE of the LDAP server
                specified by the LDAP session handle and displays the attribute
                values. For known extensions and controls, a lookup table is 
                used to print a descriptive name.
 
   Sample Run:   Root DSE
                 -----------------------------------------
                 supportedSASLMechanisms:
                     EXTERNAL
                 supportedLDAPVersion:
                     2
                     3
                 supportedExtension:
                     2.16.840.1.113719.1.27.100.55 (TRIGGER_PART_PURGE_REQ)
                     2.16.840.1.113719.1.27.100.56 (TRIGGER_PART_PURGE_RES)
                     2.16.840.1.113719.1.27.100.53 (TRIGGER_SCHEMA_SYNC_REQ)     
                     ...
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#include <ldap_oids.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   getdse <host name> <port number> <login dn> <password>"
"\n Example: getdse Acme.com 389 cn=admin,o=Acme secret\n";


int main( int argc, char **argv) { 

    int         version, ldapPort, i, j, rc; 
    char        *ldapHost, *loginDN, *password, *desc;    
    char        *attrName ;
    char        **values;       
    BerElement  *ber;
    LDAP        *ld;
    LDAPMessage *searchResult = NULL, *dse;    
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 5) {
        printf("%s", usage);
        return(1);
    }

    ldapHost          = argv[1];
    ldapPort          = atoi(argv[2]);
    loginDN           = argv[3];
    password          = argv[4];   

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n    LDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n    LDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS ) {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("\n    Bind successful\n");

    /*
     * To search for rootDSE,
     *  1. Must be set for LDAP v3 before binding.
     *  2. Set search base = NULL or empty string
     *  3. Set search filter = (objectclass=*)
     *  4. Set search scope = LDAP_SCOPE_BASE
     */
    rc = ldap_search_ext_s(
                ld,                 /* LDAP session handle */
                NULL,               /* search base */
                LDAP_SCOPE_BASE,    /* scope */ 
                "(objectclass=*)",  /* search filter */
                NULL,               /* return all attrs */
                0,                  /* return both attrs and values */
                NULL,               /* server controls */
                NULL,               /* client controls */
                &timeOut,           /* search timeout */
                LDAP_NO_LIMIT,      /* no limit on entries */
                &searchResult );    /* returned search results */

    if ( rc != LDAP_SUCCESS ) {
        printf(  "ldap_search_ext_s: %s\n", ldap_err2string( rc ));          
        ldap_msgfree( searchResult );
        ldap_unbind_s( ld );
        return( 1 );
    }


    /* We have only one entry - root dse in search result */
    dse = ldap_first_entry( ld, searchResult );  

    /* print out all the attributes and attribute values */
    printf("\n    Root DSE");
    printf("\n    -----------------------------------------\n");
    for (   attrName  = ldap_first_attribute( ld, searchResult, &ber );
            attrName  != NULL; 
            attrName  = ldap_next_attribute( ld, searchResult, ber )) {
        if (( values = ldap_get_values( ld, dse, attrName )) != NULL ) {
            
            printf("    %s:\n", attrName);
            
            for ( i = 0; values[ i ] != NULL; i++ ) {
                
                if ( strcmp(attrName, "supportedExtension") == 0
                    || strcmp(attrName, "supportedControl") == 0) {
                    printf("        %s", values[i]);                        
                    for ( j = 0; j < TOTAL_LEN; j++ ) {
                        desc = "";
						if ( strcmp( values[i], namesandoids[j][3]) == 0 ) {
                            desc = namesandoids[j][1];
							if( strlen(desc) )
                                printf("   (%s)\n", namesandoids[j][1]);                                
                            else                           
                                printf("\n");

                            break;
                        }						
                    }
                    if( !strlen(desc) ) {
						    printf("\n");						
					}
                }                
                else
                    printf("        %s\n", values[ i ] );
            }
            ldap_value_free( values );
        }
        ldap_memfree( attrName  );
    }                     

    ber_free(ber, 0);
    ldap_msgfree( searchResult );
    ldap_unbind_s( ld );   

    return( 0 );
}

