/* $Novell*/
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
   simplepass.c
***************************************************************************
   Description: Shows how to set the simple password of an entry.  
                
                The simple password is set by modifying the 'userpassword'
                attribute and attaching the Simple Password control to
                the LDAP add or modify operation.
                
                The purpose of the Simple Password is to allow migration 
                of an object with a hashed password into eDirectory.
                The object may then be accessed using the same password as 
                in the orginal system.
                
                Simple Passwords are also used when binding with the
                DIGEST-MD5 SASL mechanism.
                
                The simple password value may be specified as the original 
                clear text password, or as the result of hashing the password
                with the Secure Hash (SHA), Salted Secure Hash (SSHA), 
                digest-md5 (MD5) or Unix Crypt (CRYPT) algorithm.  
                A hashed password is represented as a text string consisting
                of a prefix indicating the hashing algorithm used, followed
                by the base-64 encoding the the binary hash result.
                
                Valid prefixes are  {SHA}, {SSHA}, {MD5}, or {CYRPT}. 
                Example:  "{SHA}qUqP5cyxm6YcTAhz05Hph5gvu9M="
                
                Note:  For simplicity this sample uses a clear text connection 
                to the server.  Real applications should use a TLS/SSL 
                (encrypted) connection when setting a password, so that
                the password or its hash value is not sent on the wire
                as clear text.
                
                If the DIGEST-MDG SASL mechanism is to be used to authenticate
                users, the simple password must be stored as clear text (not
                hashed.)
                
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ldap.h"

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   simplepass <host name> <port number> <login dn> <password>"
"\n          <modify dn> <new simple password>\n" 
"\n Example: simplepass Acme.com 389 cn=admin,o=Acme secret"
"\n          cn=james,ou=Sales,o=Acme simple\n";

int main( int argc, char **argv)
{ 

    LDAP        *ld;
    int         version, ldapPort, rc; 
    char        *ldapHost, *loginDN, *password, *modifyDN, *simplePassword;
    char        *passValues[2];
    LDAPMod     setpass, *modify[2];
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    /* Statically create the Simple Password control.  There is no value 
     * associated with this control, just an OID and criticality.
     *
     * Setting the criticality to TRUE means the server will return an error
     * if it does not recognize or is unable to perform the control. 
     */
    static LDAPControl simplePasswordControl = {
               LDAP_CONTROL_SIMPLE_PASS,          /* OID */
               {0, NULL},                         /* No value */
               1 };                               /* Criticality = TRUE */
    LDAPControl *controls[2] = { &simplePasswordControl, NULL };
    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 7)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost       = argv[1];
    ldapPort       = atoi(argv[2]);
    loginDN        = argv[3];
    password       = argv[4];
    modifyDN       = argv[5];
    simplePassword = argv[6];

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
     * LDAP_MOD_REPLACE replaces any existing values with the new value.
     */
    setpass.mod_op     = LDAP_MOD_REPLACE;
    setpass.mod_type   = "userPassword";
    passValues[0]      = simplePassword;
    passValues[1]      = NULL;
    setpass.mod_values = passValues;

    modify[0] = &setpass;
    modify[1] = NULL;

    /* Modify the 'userpassword' attribute, with the Simple Password control. */
    rc= ldap_modify_ext_s( ld,         /* LDAP session handle */
                           modifyDN,   /* the object to modify */
                           modify,     /* array of LDAPMod structures */
                           controls,   /* Simple Password control */
                           NULL);      /* client controls */

    if ( rc != LDAP_SUCCESS )
    {
        printf("\n\tldap_modify_ext_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return(1);
    }
    printf("\n\tSimple password modified for: %s\n", modifyDN );

    ldap_unbind_s( ld );

    return( 0 );
}

