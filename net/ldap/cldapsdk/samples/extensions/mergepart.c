/* $Novell: mergepart.c,v 1.7 2003/05/12 13:19:59 $ */
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
   mergepart.c
***************************************************************************
   Description: The mergepart.c sample merges a child partition with its 
                parent partition. The child partition is specified by the
                distinguished name of the partition root. When the flag 
                parameter is set to 1, the function verifies that the 
                affected servers are up.
**************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ldap.h"
#include "ldapx.h"
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   mergepart <host name> <port number> <login dn> <password> " 
"\n\t  <partition dn>\n"
"\n Example: mergepart acme.com 389 cn=admin,o=acme secret ou=sales,o=acme\n";

int main(int argc, char **argv)
{

    int   rc, version, ldapPort;  
    char  *ldapHost;         
    char  *loginDN;
    char  *password;
    char  *partitionDN;
    char  *msg;
    LDAP  *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost    =  argv[1];
    ldapPort    =  atoi(argv[2]); 
    loginDN     =  argv[3];
    password    =  argv[4];
    partitionDN =  argv[5];   

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
     * Novell extended operation to merge this partition with its parent. 
     */
    printf( "    Calling ldap_merge_partitions...\n" );
    rc = ldap_merge_partitions(
                   ld,                          /* LDAP session handle */ 
                   partitionDN,                 /* dn of the partition */ 
                   LDAP_ENSURE_SERVERS_UP);     /* flag */ 

    if ( rc != LDAP_SUCCESS ) {
        /* print out LDAP server message */         
        if(ldap_get_option (ld, LDAP_OPT_ERROR_STRING, &msg ) == 
                                                      LDAP_OPT_SUCCESS ) {
            
            if ( (msg != NULL) && (strlen(msg) != 0) )
                printf("\n    LDAP server message: %s\n", msg);
 
            ldap_memfree(msg);
        }
        /* print out LDAP error message */
        printf("    LDAP error  message: ldap_merge_partitions:" 
                                      " %s (%d)\n", ldap_err2string( rc ), rc);
        printf("\n    ldap_merge_partitions failed\n\n");
        ldap_unbind_s( ld );
        return( 1 );
    }

    printf("    ldap_merge_partitions succeeded\n\n");

    ldap_unbind_s( ld );
    return 0;
}

