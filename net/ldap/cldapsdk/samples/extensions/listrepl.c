/* $Novell: listrepl.c,v 1.13 2003/05/13 14:09:31 $ */
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
    listrepl.c
***************************************************************************
   Description: The listrepl.c sample lists all the replicas on the 
                specified LDAP server.
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
"\n Usage:   listrepl <host name> <port number> <login dn> <password>" 
"\n\t  <serverDN>\n"
"\n Example: listrepl acme.com 389 cn=admin,o=acme secret cn=myServer,o=acme\n";


int main(int argc, char **argv)
{
    int   i, rc, version;
    char  *ldapHost;         
    int   ldapPort;   
    char  *loginDN;
    char  *password;     
    char  *serverDN;
    char  *msg;
    char  **replicaList = NULL;
    LDAP  *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if ( argc != 6 )
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost      =  argv[1];         
    ldapPort      =  atoi(argv[2]);   
    loginDN       =  argv[3];
    password      =  argv[4];     
    serverDN      =  argv[5];   

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
     * Novell extended operation to list the replicas on this server.
     */
    printf( "    Calling ldap_list_replicas...\n" );
    rc = ldap_list_replicas(
                       ld,            /* LDAP session handle */
                       serverDN,      /* dn of the server */
                       &replicaList); /* char array to receive replica list */

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
        printf("    LDAP error  message: ldap_list_replicas:" 
                            " %s (%d)\n", ldap_err2string( rc ), rc);
        printf("\n    ldap_list_replicas failed\n\n");
        ldap_unbind_s( ld );
        return( 1 );
    }

    printf("\n    ldap_list_replicas succeeded\n\n");

    if(replicaList)
    {
		/* [Root] replica is returned as "" */
        printf("\n    replicas existing in %s: \n", serverDN);
        for ( i = 0; replicaList[i] != NULL; i++)
            if ( strcmp( replicaList[i], "") == 0)
                printf("        [Root]\n");
            else
                printf("        %s\n", replicaList[i] );
    }

    printf("\n\n");

    ldap_value_free( replicaList );
    
    ldap_unbind_s( ld );
    
    return 0;
}

