/* $Novell: chgrepl.c,v 1.15 2003/05/12 13:13:44 $ */
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
   chrepl.c 
***************************************************************************
   Description: The chgrepl.c sample changes the type of the specified 
                replica on the specified server. You cannot directly
                change the type of a master replica. eDirectory 
                automatically changes the master replica's type when a 
                subordinate replica is changed to a master replica.

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
"\n Usage:   chgrepl <host name> <port number> <login dn> <password>" 
"\n          <replica dn> <new replica type> <server entry dn>\n"
"\n Example: chgrepl acme.com 389 cn=admin,o=acme secret"
"\n          ou=sales,o=acme 1 cn=myServer,o=acme\n"
"\n          <new replica type>: 0-master, 1-read/write, 2-read only\n";

int main(int argc, char **argv)
{
     
    int      rc, version; 
    int      ldapPort;
    char     *ldapHost;       
    char     *loginDN; 
    char     *password; 
    char     *replicaDN; 
    int      newReplicaType; 
    char     *serverName;
    char     *msg;
    LDAP     *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 8)
    {
        printf("%s", usage);
        return (1);
    }
    
    ldapHost        =  argv[1];
    ldapPort        =  atoi(argv[2]);
    loginDN         =  argv[3];
    password        =  argv[4];
    replicaDN       =  argv[5];
    newReplicaType  =  atoi(argv[6]);  /* 0-master, 1-read/write, 2-read only */
    serverName      =  argv[7];

      
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
     *  Novell extended operation to change a replica type. 
     */
    printf( "    Calling ldap_change_replica_type...\n" );
    rc = ldap_change_replica_type(   
                ld,                     /* LDAP session handle */
                replicaDN,              /* dn of the replica */
                serverName,             /* server entry dn */                          
                (LDAP_REPLICA_TYPE)newReplicaType,
                LDAP_ENSURE_SERVERS_UP );           

    if ( rc != LDAP_SUCCESS )
    {
        /*  print out LDAP server message */
        if(ldap_get_option (ld, LDAP_OPT_ERROR_STRING, &msg ) == 
                                                      LDAP_OPT_SUCCESS ) {
            
            if ( (msg != NULL) && (strlen(msg) != 0) )
             {
                printf("\n    LDAP server message: %s", msg);
                ldap_memfree(msg);
             }
        }
        /* print out LDAP error message */
        printf("    LDAP error  message: ldap_change_replica_type:" 
                            " %s (%d)\n", ldap_err2string( rc ), rc);        
        
        printf("\n    ldap_change_replica_type failed\n\n");
        ldap_unbind_s( ld );
        return( 1 );
    }
             
    printf("\t\tldap_change_replica_type succeeded\n\n");

    ldap_unbind_s( ld );
    return 0;
}
