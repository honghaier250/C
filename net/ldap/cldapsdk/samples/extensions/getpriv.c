/* $Novell: getpriv.c,v 1.16 2003/05/12 13:16:08 $ */
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
   getpriv.c 
***************************************************************************
   Description: The getpriv.c sample returns the effective rights of 
                the specified entry (trustee) to the specified attribute 
                of the specified subject (object).
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
"\n Usage:   getpriv <host name> <port number> <login dn> <password>" 
"\n\t  <object dn> <trustee dn> <attribute name>\n"
"\n Example: getpriv acme.com 389 cn=admin,o=acme secret"
"\n\t  cn=admin,o=acme cn=James,ou=sales,o=acme cn\n";

int main(int argc, char **argv)
{
    int   rc, version, privileges, ldapPort;        
    char  *ldapHost;        
    char  *loginDN; 
    char  *password; 
    char  *objectDN;
    char  *trusteeDN;
    char  *attrName;
    char  *msg;
    LDAP  *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 8)
    {
        printf("%s", usage);
        return(1);
    }


    ldapHost   =  argv[1];         
    ldapPort   =  atoi(argv[2]);
    loginDN    =  argv[3];
    password   =  argv[4];
    objectDN   =  argv[5];
    trusteeDN  =  argv[6];
    attrName   =  argv[7];   

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    printf( "    Getting the handle to the LDAP connection...\n" );
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
     * Novell extended operation to get the effective privileges 
     * of a trustee upon an entry's attribute.
     */
    printf( "    Calling ldap_get_effective_privileges...\n" );
    rc = ldap_get_effective_privileges(
                                      ld,            /* LDAP session handle */ 
                                      objectDN,      /* object dn */
                                      trusteeDN,     /* trustee dn */
                                      attrName,      /* attribute name */
                                      &privileges);  /* privileges */

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
        printf("    LDAP error  message: ldap_get_effective_" 
                   "privileges: %s (%d)\n", ldap_err2string( rc ), rc);
        
        printf("\n    ldap_get_effective_privileges failed\n\n");
        ldap_unbind_s( ld );
        return( 1 );
    }
            
    printf("\n    ldap_get_effective_privileges succeeded\n\n");

    printf("\tTrustee:   %s\n", trusteeDN);
    printf("\tObject:    %s\n", objectDN);
    printf("\tAttribute: %s\n", attrName);

    printf("\n\tPrivileges: 0x%08x\n", privileges);

    if ( privileges & LDAP_DS_ATTR_COMPARE )
        printf("\n\tTrustee has compare rights to the attribute.\n\n");
    else
        printf("\n\tTrustee has NO compare rights to the attribute.\n\n");

    if ( privileges & LDAP_DS_ATTR_READ )
        printf("\n\tTrustee has read rights to the attribute.\n\n"); 
    else
        printf("\n\tTrustee has NO read rights to the attribute.\n\n"); 

    if ( privileges & LDAP_DS_ATTR_WRITE )
        printf("\n\tTrustee has write rights to the attribute.\n\n"); 
    else
        printf("\n\tTrustee has NO write rights to the attribute.\n\n"); 
    
    if ( privileges & LDAP_DS_ATTR_SELF )
        printf("\n\tTrustee can add or delete its name as a value "
               "of the attribute.\n\n");
    else
        printf("\n\tTrustee CANNOT add or delete its name as a value "
               "of the attribute.\n\n");

    if ( privileges & LDAP_DS_ATTR_SUPERVISOR )
        printf("\n\tTrustee has all rights to the the object's attributes\n\n");
    else
        printf("\n\tTrustee does not have all rights to the the object's attributes\n\n");

    if ( privileges & LDAP_DS_ATTR_INHERIT_CTL )
        printf("\n\tTrustee inherits the rights granted in the ACL.\n\n");
    else
        printf("\n\tTrustee does not inherit the rights granted in the ACL.\n\n");


    ldap_unbind_s( ld );
    return 0;
}

