/* $Novell: parsync.c,v 1.12 2003/05/12 13:21:36 $ */
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
    parsync.c
***************************************************************************
   Description: The parsync.c sample schedules synchronization of the 
                specified partition after the specified number of seconds. 
***************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#include <ldapx.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   parsync <host name> <port number> <login dn> <password> " 
"\n\t  <server dn> <partition root> <delay seconds>\n"
"\n Example: parsync acme.com 389 cn=admin,o=acme secret cn=myServer,o=acme"
"\n\t  ou=sales,o=acme 1\n";

int main(int argc, char **argv)
{
   
    int   rc, version, ldapPort, delay;
    char  *ldapHost;        
    char  *loginDN;
    char  *password;
    char  *serverDN;
    char  *partitionRoot;
    char  *msg;
    LDAP  *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 8)
    {
        printf("%s", usage);
        return (1);
    }
      
    ldapHost =argv[1];  
    ldapPort = atoi(argv[2]);       
    loginDN = argv[3];
    password = argv[4];
    serverDN=argv[5];
    partitionRoot = argv[6];
    delay =atoi(argv[7]);

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);
   
    /* Initialize the LDAP session */
    printf( "    Initializing the LDAP session handle ...\n" );
    if ( (ld = ldap_init( ldapHost, ldapPort )) == NULL )
    {
        perror( "ldap_init" );
        return( 1 );  
    }
    
    /* Authenticate to the server */
    printf( "    Binding to the directory...\n" );
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if ( rc != LDAP_SUCCESS )
    {
        printf( "ldap_simple_bind_s: %s\n", ldap_err2string(rc));
        ldap_unbind_s( ld );
        return( 1 );
    }

    /* 
     * Novell extended operation to synchronize a partition.
     */
    printf( "    Calling ldap_request_partition_sync...\n" );
    rc = ldap_request_partition_sync(
                         ld,            /* LDAP session handle */ 
                         serverDN,    /* server name */
                         partitionRoot, /* partition root */
                         delay);        /* delay time */

    if ( rc != LDAP_SUCCESS )
    {         
        /* print out LDAP server message */         
        if(ldap_get_option (ld, LDAP_OPT_ERROR_STRING, &msg ) == 
                                                      LDAP_OPT_SUCCESS ) {
            
            if ( (msg != NULL) && (strlen(msg) != 0) )
             {
                printf("\n    LDAP server message: %s", msg);
                ldap_memfree(msg);
             }
        }
        /* print out LDAP error message */
        printf("    LDAP error  message: ldap_request_partition" 
                                "_sync: %s (%d)\n", ldap_err2string( rc ), rc);
        
        printf("\n    ldap_request_partition_sync failed\n\n");
        ldap_unbind_s( ld );
        return( 1 );
    }
      
    printf("\n    ldap_request_partition_sync scheduled\n\n");

    ldap_unbind_s( ld );
    return 0;
}


