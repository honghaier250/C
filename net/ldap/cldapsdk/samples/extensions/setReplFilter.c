/* $Novell: setReplFilter.c,v 1.7 2003/05/12 13:24:02 $ */
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
   setReplFilter.c 
***************************************************************************
   Description: The setReplFilter.c sample shows how to set replication 
                filter on a NetIQ eDirectory LDAP server.

                A server replication filter contains a set of Novell 
                eDirectory classes and attributes you want to replicate
                on the server. Then the filter applies to all the 
                replicas on the server.

                The filter parameter passed to ldap_set_replication_filter()
                in this sample is:
                     "inetorgperson$cn$givenName$$top$*$$computer$$$"
                This means that the replication filter consists of:
                1. inetOrgPerson class and it's cn, givenName attributes;
                2. top class and all the attributes of it;
                3. computer class;
                4. certain required attributes that are always 
                replicated, such as ACL, objeceClass, etc.
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
"\n Usage:   setReplFilter <host name> <port number> <login dn>"
"\n          <password> <serverDN>" 
"\n Example: setReplFilter acme.com 389 cn=admin,o=acme secret"
"\n          cn=myServerName,o=acme\n";

int main(int argc, char **argv)
{
    
    int     rc, version, ldapPort;
    char    *ldapHost, *loginDN, *password, *serverDN, *msg;
    char    *filter="inetorgperson$cn$givenName$$top$*$$computer$$$";
    LDAP    *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6) {
        printf("%s", usage);
        return (1);
    }

    ldapHost    =   argv[1];         
    ldapPort    =   atoi(argv[2]);
    loginDN     =   argv[3];
    password    =   argv[4];
    serverDN    =   argv[5];


    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);
   
    /* initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n    LDAP session initialization failed");
        return (1);
    }
    printf ( "\n    LDAP session initialized");

    /* bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS ) {
        printf("\nldap_simple_bind_s: %s", ldap_err2string(rc));
        ldap_unbind_s (ld);
        return (1);
    }
    printf("\n    Bind successful");

    /* 
     * call Novell extended operation to set replication filter.
     */
    printf( "\n    Calling ldap_set_replication_filter..." );
    rc = ldap_set_replication_filter( 
        ld,         /* LDAP session handle */ 
        serverDN,   /* where to set replication filter */
        filter);    /* replication filter */

    if ( rc != LDAP_SUCCESS ) {
        /* print out server error message */
        if(ldap_get_option(ld, LDAP_OPT_ERROR_STRING, &msg ) == 
                                           LDAP_OPT_SUCCESS ) {
            
            if ( (msg != NULL) && (strlen(msg) != 0) )
             {
                printf("\n    LDAP server message: %s", msg);
                ldap_memfree(msg);
             }
        }
        /* print out LDAP error message */
        printf("\n    LDAP error  message: ldap_set_replication_filter:"
                                " %s (%d)", ldap_err2string(rc), rc);
        
        printf("\n    ldap_get_replication_filter failed\n");
        ldap_unbind_s( ld );
        return (1);
    }
    printf("\n    ldap_get_replication_filter succeeded\n");

    ldap_unbind_s( ld );
    
    return (0);
}

