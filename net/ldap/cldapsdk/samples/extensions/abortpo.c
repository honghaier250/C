/* $Novell: abortpo.c,v 1.13 2003/05/12 13:12:24 $ */
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
   abortpo.c
***************************************************************************
   Description: The abortpo.c sample schedules a partition operation 
                and then aborts the pending operation on the specified 
                partition. Once eDirectory has started processing the 
                operation, it cannot be aborted.
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
"\n Usage:   abortpo <host name> <port number> <login dn> <password>" 
"\n\t  <partition dn>\n"
"\n Example: abortpo acme.com 389 cn=admin,o=acme secret ou=sales,o=acme\n";

int main(int argc, char **argv)
{
    int     rc, version;           
    int     ldapPort;
    char    *ldapHost;
    char    *loginDN;
    char    *password;
    char    *partitionDN;
    char    *msg;
    LDAP    *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */
      
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
        printf("%s", usage);
        return (1);
    }
       
    ldapHost        =   argv[1];         
    ldapPort        =   atoi(argv[2]);
    loginDN         =   argv[3];
    password        =   argv[4];
    partitionDN     =   argv[5];

    
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
     * Split a partition to give us a pending operation we can abort. 
     * Once DS starts the actual operation, it cannot be aborted.
     */
    printf( "    Calling ldap_split_partition...\n" );
    rc = ldap_split_partition( 
                ld,                         /* LDAP session handle */
                partitionDN,                /* dn of the partition */
                LDAP_ENSURE_SERVERS_UP );   
    printf("\n    ldap_split_partition is pending...");

    /*
     *  Novell extended operation to abort a pending eDirectory operation 
     */
    printf("\n\n    Attempting to abort the operation...\n");
    rc = ldap_abort_partition_operation( 
                ld,                 /* LDAP session handle */ 
                partitionDN,        /* dn of the partition */
                0 );                /* reserved */

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
        printf("    LDAP error  message: ldap_abort_partition_" 
                           "operation: %s (%d)\n", ldap_err2string( rc ), rc);
        
        printf("\n    ldap_abort_partition_operation failed\n\n");
        ldap_unbind_s( ld );
        return( 1 );
    }
            
    printf("\n    ldap_abort_partiton_operation succeeded\n\n");
  
    ldap_unbind_s( ld );
    return 0;
}

