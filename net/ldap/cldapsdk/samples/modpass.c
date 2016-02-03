/* $Novell: modpass.c,v 1.9 2003/05/12 12:54:14 $ */
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
   modpass.c     (For users - does not require admin privilege)
***************************************************************************
   
   Description: The modpass.c sample shows how to modify your own 
                password, giving both the old and new password.
                
                Unless the caller has admin privilegs, eDirectory requires both
                the old and new passwords in order to change a password.
                
                NOTE:  This method requires the Novell Server to be running
                eDirectory version 8.3x or later.  See the setpass.c sample 
                which works on any Novell LDAP server but requires that the 
                caller have admin privilege.

                Two LDAPMod structures are created.
                - The first deletes the value of the userPassword attribute with
                  an LDAP_MOD_DELETE operation, specifying the old password.
                - The second one adds the new password to the userPassword
                  attribute using an LDAP_MOD_ADD operation.
                  
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   modpass <host name> <port number> <login dn> <old password>"
"\n          <new password>\n" 
"\n Example: modpass Acme.com 389 cn=james,ou=Sales,o=Acme oldpass"
"\n          newpass\n";

int main( int argc, char **argv)
{ 

    LDAP        *ld;
    int         version, ldapPort, rc; 
    char        *ldapHost, *loginDN, *password, *newPassword;
    char        *oldValues[2], *newValues[2];
    LDAPMod     delmod, addmod, *modify[3];
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost    =   argv[1];
    ldapPort    =   atoi(argv[2]);
    loginDN     =   argv[3];
    password    =   argv[4];
    newPassword =   argv[5];

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
     * Delete the old password, add the new password.
     * eDirectory requires both passwords unless caller has admin privilege.
     */
    delmod.mod_op   = LDAP_MOD_DELETE;
    delmod.mod_type = "userPassword";
    /* "Invalid credentials" error is returned if old password is incorrect. */
    oldValues[0] = password;
    oldValues[1] = NULL;
    delmod.mod_values = oldValues;

    addmod.mod_op   = LDAP_MOD_ADD;
    addmod.mod_type = "userPassword";
    newValues[0] = newPassword;
    newValues[1] = NULL;
    addmod.mod_values = newValues;

    modify[0] = &delmod;
    modify[1] = &addmod;
    modify[2] = NULL;

    /* Modify the attribute */
    rc= ldap_modify_ext_s( ld,         /* LDAP session handle */
                           loginDN,    /* the object to modify */
                           modify,     /* array of LDAPMod structures */
                           NULL,       /* server controls */
                           NULL);      /* client controls */

    if ( rc != LDAP_SUCCESS )
    {
        printf("\n\tldap_modify_ext_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return(1);
    }
    printf("\n\tPassword modified for: %s\n", loginDN );

    ldap_unbind_s( ld );

    return( 0 );
}

