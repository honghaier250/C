/* $Novell: verpass.c,v 1.9 2003/05/12 13:11:16 $ */
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
   verpass.c   
***************************************************************************
   Description: The verpass.c sample verifies that a password is correct 
                for the given object.
                
                We simply compare the password with the "userPassword"
                attribute of the object using the LDAP compare function.
                
                Verifying the password of an object is a much faster
                operation than binding to the object.  However, it
                does not give you the rights or identity of that
                object like the bind does.
                
                Note also that successfully comparing a password does not 
                necessarily mean that the object could successfully bind
                with that password.  The password may be expired, or the 
                account disabled, or time restrictions may prevent the user 
                from binding.
                
                eDirectory version 8.5 or higher is required to run this sample.
                                   
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   verpass <host name> <port number> <login dn> <password>"
"\n          <object dn> <test password>\n" 
"\n Example: verpass Acme.com 389 cn=admin,o=Acme secret"
"\n          cn=james,ou=Sales,o=Acme testpass\n";

int main( int argc, char **argv)
{ 

    LDAP        *ld;
    int         version, ldapPort, rc; 
    char        *ldapHost, *loginDN, *password, *objectDN, *testPassword;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 7)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost     =   argv[1];
    ldapPort     =   atoi(argv[2]);
    loginDN      =   argv[3];
    password     =   argv[4];
    objectDN     =   argv[5];
    testPassword =   argv[6];

    /* Set LDAP version to 3 and set connection timeout. */
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n\tLDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n\tLDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("\n\tBind successful\n");


    /* 
     * Compare the test password with the userPassword attribute 
     * of the object.
     */
    rc= ldap_compare_s( ld,              /* LDAP session handle */
                        objectDN,        /* the object to verify password on */
                        "userPassword",  /* attribute to compare */
                        testPassword);   /* the password to verify */

    if ( rc == LDAP_COMPARE_TRUE)
        printf("\n\tThe password is correct.\n");
    else if ( rc == LDAP_COMPARE_FALSE )
        printf("\n\tThe password is not correct.\n");
    else
    {
        printf("\n\tldap_compare_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return(1);
    }

    ldap_unbind_s( ld );

    return( 0 );
}

