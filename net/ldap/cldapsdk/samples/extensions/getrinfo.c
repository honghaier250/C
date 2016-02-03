/* $Novell: getrinfo.c,v 1.13 2003/05/12 13:16:43 $ */
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
   getrinfo.c 
***************************************************************************
   Description: The getrinfo.c returns information about a replica. 
                The replica is identified by specifying the distinguished 
                name of the replica's partition and the server it is on.
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
"\n Usage:   getrinfo <host name> <port number> <login dn> <password> " 
"\n\t  <partition dn> <serverDN>\n"
"\n Example: getrinfo acme.com 389 cn=admin,o=acme secret ou=sales,o=acme"
"\n\t  cn=myServer,o=acme\n";

int main(int argc, char **argv)
{      
    int   rc, version, ldapPort;
    char  *ldapHost;             
    char  *loginDN;
    char  *password;
    char  *partitionDN;
    char  *serverDN;
    char  *msg;
    LDAPReplicaInfo replicaInfo;
    LDAP  *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if ( argc != 7 )
    {
        printf("%s", usage);
        return (1);
    }
       
    ldapHost    =  argv[1];         
    ldapPort    =  atoi(argv[2]);   
    loginDN     =  argv[3];
    password    =  argv[4];
    partitionDN =  argv[5];
    serverDN    =  argv[6];
          
    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    printf( "    Getting the handle to the LDAP connection...\n" );
    if ( (ld = ldap_init( ldapHost, ldapPort )) == NULL ) 
    {
        perror( "ldap_init" );
        return ( 1 );   
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
     * Novell extended operation to get replica information.
     */
    printf( "    Calling ldap_get_replica_info...\n" );
    rc = ldap_get_replica_info(
                        ld,            /* LDAP session handle */
                        partitionDN,   /* Partition dn */
                        serverDN,      /* server dn */
                        &replicaInfo); /* struct to receive replica info */

    if ( rc != LDAP_SUCCESS ) 
    {
        /* print out LDAP server message */         
        if(ldap_get_option (ld, LDAP_OPT_ERROR_STRING, &msg ) == 
                                                      LDAP_OPT_SUCCESS ) {
            
            if ( (msg != NULL) ) {
                printf("\n    LDAP server message: ");
                if ( strlen(msg) != 0 )
                    printf("%s\n", msg);
                else
                    printf("none\n");
                ldap_memfree( msg );
            }
        }
        /* print out LDAP error message */
        printf("    LDAP error  message: ldap_get_replica_info:" 
                            " %s (%d)\n", ldap_err2string( rc ), rc);
        
        printf("\n    ldap_get_replica_info failed\n\n");
        ldap_unbind_s( ld );
        return( 1 );
    }

    printf("\n    ldap_get_replica_info succeeded\n\n");

    printf("\n    Replica info: ");

    printf("\n        Partition root ID: %d", replicaInfo.rootID);
    printf("\n        Replica state:     %d", replicaInfo.state);
    printf("\n        Modification time: %d", replicaInfo.modificationTime);
    printf("\n        Purge time:        %d", replicaInfo.purgeTime);
    printf("\n        Local replica ID:  %d", replicaInfo.localReplicaID);
    printf("\n        Replica type:      %d", replicaInfo.replicaType);
    printf("\n        partition DN:      %s", replicaInfo.namingContextDN);
    printf("\n        Flag:              %d", replicaInfo.flags);

    printf("\n\n");

    ldap_unbind_s( ld );
    return 0;
}

