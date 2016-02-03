/* $Novell: setpass.c,v 1.7 2003/05/12 13:06:33 $ */
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
   setpass.c   (Requires admin privilege) 
***************************************************************************
   Description: The setpass.c sample shows how to set the password 
                of an entry.  We simply set the userPassword attribute 
                of the object.
                
                In eDirectory only an admin can set a password without 
                supplying the old password.  Consequently this method 
                works on any Novell LDAP server, but only when the caller 
                has admin privileges.  Users cannot change their own 
                password with this method.
                
                Also see the modpass.c sample which allows a user to 
                specify the old and new password.
                                
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   setpass <host name> <port number> <login dn> <password>"
"\n          <modify dn> <new password>\n" 
"\n Example: setpass Acme.com 389 cn=admin,o=Acme secret"
"\n          cn=james,ou=Sales,o=Acme newpass\n";

int main( int argc, char **argv)
{ 

    LDAP        *ld;
    int         version, ldapPort, rc; 
    char        *ldapHost, *loginDN, *password, *modifyDN, *newPassword;
    char        *passValues[2];
    LDAPMod     setpass, *modify[2];
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 7)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost    =   argv[1];
    ldapPort    =   atoi(argv[2]);
    loginDN     =   argv[3];
    password    =   argv[4];
    modifyDN    =   argv[5];
    newPassword =   argv[6];

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
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
     * To modify the attributes of an entry
     *    1  Specify the modify action    
     *    2. Specify the attribute name to be modified 
     *    3. Specify the value of the attribute
     *    4. Add to an array of LDAPMod structures
     *    5. Call ldap_modify_ext_s
     */


    /* 
     * LDAP_MOD_REPLACE replaces any existing values with the new value.
     */
    setpass.mod_op     = LDAP_MOD_REPLACE;
    setpass.mod_type   = "userPassword";
    passValues[0]      = newPassword;
    passValues[1]      = NULL;
    setpass.mod_values = passValues;

    modify[0] = &setpass;
    modify[1] = NULL;

    /* Modify the attribute */
    rc= ldap_modify_ext_s( ld,         /* LDAP session handle */
                           modifyDN,   /* the object to modify */
                           modify,     /* array of LDAPMod structures */
                           NULL,       /* server controls */
                           NULL);      /* client controls */

    if ( rc != LDAP_SUCCESS )
    {
        printf("\n\tldap_modify_ext_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return(1);
    }
    printf("\n\tPassword modified for: %s\n", modifyDN );

    ldap_unbind_s( ld );

    return( 0 );
}

