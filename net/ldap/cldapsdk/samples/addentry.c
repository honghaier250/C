/* $Novell: addentry.c,v 1.11 2003/05/12 13:26:24 $ */
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
    addentry.c
***************************************************************************
   Description: The addentry.c sample adds an entry ( cn=james ) with
                its mandatory attributes to a specified container.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   addentry <host name> <port number> <login dn> <password>"
"\n\t  <continer name> \n" 
"\n Example: addentry Acme.com 389 cn=admin,o=Acme secret"
" ou=Sales,o=Acme\n";

int main( int argc, char **argv)
{
    int         version, ldapPort, rc; 
    char        *ldapHost, *loginDN, *password, *containerName, *dn = NULL;     
    char        *vclass[5], *vcn[4],*vgn[4], *vsn[2];
    char        *vphone[2], *vmail[2], *vpass[2];
    LDAP        *ld;
    LDAPMod     modClass, modCN, modGN, modSN, modPhone, modMail, modPass;
    LDAPMod     *mods[8];
    struct timeval timeOut = {10,0};    /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif

    if (argc != 6)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost        =   argv[1];
    ldapPort        =   atoi(argv[2]);
    loginDN         =   argv[3];
    password        =   argv[4];
    containerName   =   argv[5];

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf("\n\tLDAP session initialization failed\n");
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
     * To add an entry to the directory:
     *   1. Create the array of LDAPMod structures representing the
     *      attributes of the new entry 
     *   2. Specify the DN of the entry to create
     *   3. Call ldap_add_ext_s to add the entry to the directory
     */                     
    modClass.mod_op       =   LDAP_MOD_ADD;
    modClass.mod_type     =   "objectclass";
    vclass[0]             =   "top";
    vclass[1]             =   "person";
    vclass[2]             =   "organizationalPerson";
    vclass[3]             =   "inetOrgPerson";
    vclass[4]             =   NULL;
    modClass.mod_values   =   vclass;

    modCN.mod_op          =   LDAP_MOD_ADD;
    modCN.mod_type        =   "cn";
    vcn[0]                =   "james";
    vcn[1]                =   "James Smith";
    vcn[2]                =   "Jimmy Smith";
    vcn[3]                =   NULL;
    modCN.mod_values      =   vcn;

    modGN.mod_op          =   LDAP_MOD_ADD;
    modGN.mod_type        =   "givenname";
    vgn[0]                =   "James";
    vgn[1]                =   "Jim";
    vgn[2]                =   "Jimmy";
    vgn[3]                =   NULL;
    modGN.mod_values      =   vgn;

    modSN.mod_op          =   LDAP_MOD_ADD;
    modSN.mod_type        =   "sn";
    vsn[0]                =   "Smith";
    vsn[1]                =   NULL;
    modSN.mod_values      =   vsn;

    modPhone.mod_op       =   LDAP_MOD_ADD;
    modPhone.mod_type     =   "telephonenumber";
    vphone[0]             =   "1 801 555 1212";
    vphone[1]             =   NULL;
    modPhone.mod_values   =   vphone;

    modMail.mod_op        =   LDAP_MOD_ADD;
    modMail.mod_type      =   "mail";
    vmail[0]              =   "JSmith@Acme.com";
    vmail[1]              =   NULL;
    modMail.mod_values    =   vmail;
    
    /* The userPassword attribute must be present to allow the
       user to log in.  Setting the value of UserPassword to an 
       empty string would allow the user to login without a password.
    */
    modPass.mod_op        =   LDAP_MOD_ADD;
    modPass.mod_type      =   "userPassword";
    vpass[0]              =   "newpassword";
    vpass[1]              =   NULL;
    modPass.mod_values    =   vpass;

    mods[0] = &modClass;
    mods[1] = &modCN;
    mods[2] = &modGN;
    mods[3] = &modSN;
    mods[4] = &modPhone;
    mods[5] = &modMail;
    mods[6] = &modPass;
    mods[7] = NULL;

    /*
     * Construct the dn "cn=james" with the container name from the command line
     */
    dn=(char*)malloc(strlen("cn=james,") + strlen(containerName) + 1 );
    strcpy( dn, "cn=james," );
    strcat( dn, containerName );

    /*
     * Now add the object
     */
    rc = ldap_add_ext_s( ld,     /* LDAP session handle */ 
                         dn,     /* dn of the object to be created */
                         mods,   /* array of add options and values */
                         NULL,   /* server control */
                         NULL ); /* client control */

    if ( rc != LDAP_SUCCESS )
    {
        printf("\n\tldap_add_ext_s: %s\n", ldap_err2string( rc ));
        free( dn );
        ldap_unbind_s ( ld );
        return(1);
    }
    
    printf("\n\tEntry successfully added: %s\n", dn );

    free( dn );
    ldap_unbind_s( ld );

    return( 0 );
}

