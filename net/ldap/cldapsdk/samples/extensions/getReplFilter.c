/* $Novell: getReplFilter.c,v 1.11 2003/05/12 13:14:25 $ */
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
   getReplFilter.c 
***************************************************************************
   Description: The getReplFilter.c sample returns the replication filter
                set on the target Novell eDirectory LDAP server.

                See setReplFilter.c sample for more information about
                replication filter.
***************************************************************************/ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ldap.h>
#include <ldapx.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif


void printFilter(char*);

static char usage[] =
"\n Usage:   getReplFilter <host name> <port number> <login dn>"
"\n          <password> <server DN>\n" 
"\n Example: getReplFilter acme.com 389 cn=admin,o=acme secret "
"\n          cn=myServerName,o=acme\n\n";

int main(int argc, char **argv)
{
    
    int     rc, version, ldapPort;
    char    *ldapHost, *loginDN, *password, *serverDN, *filter, *msg;
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
   
    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n    LDAP session initialization failed");
        return (1);
    }
    printf ("\n    LDAP session initialized");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS ) {
        printf("\nldap_simple_bind_s: %s", ldap_err2string(rc));
        ldap_unbind_s (ld);
        return (1);
    }
    printf("\n    Bind successful");

    /* Call Novell extended operation to get replication filter. */
    printf( "\n    Calling ldap_get_replication_filter..." );
    rc = ldap_get_replication_filter( 
        ld,         /* LDAP session handle */ 
        serverDN,   /* where to get replication filter */
        &filter);   /* returned replication filter */

    if ( rc != LDAP_SUCCESS ) {
        printf("\n    ldap_get_replication_filter error %d: %s", 
                 rc, ldap_err2string(rc));
        /* Print out server error message */         
        if(ldap_get_option(ld, LDAP_OPT_ERROR_STRING, &msg ) == 
                                                      LDAP_OPT_SUCCESS ) {

        if (msg != NULL) {
            if (strlen(msg) != 0)
                printf("\n    LDAP server message: %s", msg);
                ldap_memfree(msg);
            }
        }
        ldap_unbind_s(ld);
        return (1);
    }
            
    printf("\n    ldap_get_replication_filter succeeded\n");

        if ( (strlen(filter) == 1) && (filter[0] == '$') ) {
            printf("\n    No filter returned.\n");
        }
        else {
            printFilter(filter);
        }

    ldapx_memfree(filter);
    ldap_unbind_s(ld);
    
    return (0);
}

/* printFilter parses and prints the filter */
void printFilter(char *filter) {

    /* The filter has a format of:
     *     "c1$a1$a2...$an$$c2$a1$a2...an$$...$$cn$a1$a2..$an$$$"
     * where '$$'s are used as delimiters of class/attrs names, 
     * while '$$$' indicates the end of the filter.
     */
    size_t  len = strlen(filter) - 3, i;
    char *names, cName[64], aName[64], *pdest;

    /* Remove the trailing '$$$' */ 
    filter[len] = '\0';

    names = (char*)malloc(len + 1);

    /* Replace each '$$' with '  ' */
    for ( i=0; i<len-1; i++) {    
        if ( (filter[i] == '$') && (filter[i+1] == '$') ) {
            filter[i] = ' ';
            filter[i+1] = ' ';
        }
    }

    printf("\n    Filter consists of:");

    /* There should be at least one class/attrs names as part of the filter */
    do {
        /* Get the first of the remaining class/attrs names combination(s) */ 
        sscanf(filter, "%s", names);

        len = strlen(names);
        /* Replace each '$' in names with ' ' */
        for ( i=0; i<len-1; i++) {    
            if ( names[i] == '$' )
                 names[i] = ' ';
        }
        
        /* Get and print the class name */
        sscanf(names, "%s", cName );
        printf("\n        Class name: %s", cName);

        /* Get and print the attribute names */
        /* dealing with attribute names, this includes three cases:
         *     1. There are no attribute names. 
         *        The DSA will determine what attributes to replicate. 
         *     2. There is only one '*' char, indicating all attributes
         *        of the class will be replicated.
         *     3. There are some attribute names.
         */ 
        
        pdest = (char *) strchr(names, ' ');
        if (pdest) {    
            /* Skip one char to point to the attribute names.
             * There will be at least one attribute name.
             */
            ++pdest;
            
            do {
                /* Get and print the first of the remaining attribute names. */
                sscanf(pdest, "%s", aName);
                if ( (strlen(aName)==1) && (aName[0]='*')) {
                    printf("\n            Attribute name: " 
                        "(All the attributes of the object class)");
                }
                else {
                    printf("\n            Attribute name: %s", aName );
                }
                
                /* Skip one char to point to the next attribute name. */
                pdest = (char *) strchr(pdest, ' ');
                if (pdest)
                    pdest++;
            } while (pdest!=NULL);
        }
        else {
            printf("\n            Attribute name:" 
                       " (no attribute name specified)");
        }
                
        pdest = (char *) strchr(filter, ' ');
        if (pdest)
            /* Skip two chars to point to the next class/attrs names. */
            filter = pdest + 2; 
    } while (pdest);

    printf("\n");
    free(names);
}
