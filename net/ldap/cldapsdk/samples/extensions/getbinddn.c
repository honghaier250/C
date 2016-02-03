/* $Novell: getbinddn.c,v 1.8 2003/05/12 13:15:02 $ */
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
   getbinddn.c 
***************************************************************************
   Description: The getbinddn.c sample returns the distinguished name of 
                the client associated with the LDAP session handle, or 
                the "bind dn".
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
"\n Usage:   getbinddn <host name> <port number> <login dn> <password>\n" 
"\n Example: getbinddn acme.com 389 cn=admin,o=acme secret\n";

int main(int argc, char **argv)
{
    
    int     rc, version, ldapPort;              
    char    *ldapHost;        
    char    *loginDN;  
    char    *password; 
    char    *bindDN = NULL;
    char    *msg;
    LDAP    *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */
    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 5)
    {
        printf("%s", usage);
        return (1);
    }
                     
    ldapHost    =   argv[1];         
    ldapPort    =   atoi(argv[2]);
    loginDN     =   argv[3];
    password    =   argv[4];
        

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
     * Novell extended operation to return the dn of the entry we
     * are logged in as.
     */
    printf( "    Calling ldap_get_bind_dn...\n" );
    rc = ldap_get_bind_dn( ld,         /* LDAP session handle */ 
                           &bindDN);   /* returned identity name */

    if ( rc != LDAP_SUCCESS )
    {
        /* print out LDAP server message */         
        if(ldap_get_option(ld, LDAP_OPT_ERROR_STRING, &msg ) == 
                                                      LDAP_OPT_SUCCESS ) {
            
            if ( (msg != NULL) && (strlen(msg) != 0) )
             {   
                 printf("\n    LDAP server message: %s", msg);
                 ldap_memfree(msg);
             }
        }
        /* print out LDAP error message */
        printf("    LDAP error  message: ldap_get_bind_dn:"
                                " %s (%d)\n", ldap_err2string( rc ), rc);
        
        printf("\n    ldap_get_bind_dn failed\n\n");
        ldap_unbind_s( ld );
        return( 1 );
    }
            
    printf("\n    ldap_get_bind_dn succeeded\n");
    printf("\n    bind dn: %s\n\n", bindDN);
        
    ldapx_memfree( bindDN );
    ldap_unbind_s( ld );
    return 0;
}
