/* $Novell: modattrs.c,v 1.8 2003/05/12 12:25:02 $ */
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
   modattrs.c 
***************************************************************************
   Description: The modattrs.c sample shows how to modify the attributes 
                of the entry created with the addentry.c sample. 
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   modattrs <host name> <port number> <login dn> <password>"
"\n\t <modify dn> \n" 
"\n Example: modattrs acme.com 389 cn=admin,o=acme secret"
"\n\t cn=james,ou=sales,o=acme\n";

int main( int argc, char **argv)
{ 

    int         version, ldapPort, rc; 
    char        *ldapHost, *loginDN, *password, *modifyDN;
    char        *mailValues[2], *phoneValues1[2], *phoneValues2[2] ;
    LDAP        *ld;
    LDAPMod     modMail, modPhone1, modPhone2, *modify[4];
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
    modifyDN    =   argv[5];

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
     *    1  Specify the modify actions    
     *    2. Specify the attribute name to be modified 
     *    3. Specify the value of the attribute
     *    4. Add to an array of LDAPMod structures
     *    5. Call ldap_modify_ext_s
     */


    /* 
     * LDAP_MOD_REPLACE succeeds whether the value already exists or not.
     */
    modMail.mod_op = LDAP_MOD_REPLACE;
    modMail.mod_type = "mail";
    mailValues[0] = "jimmy@acme.com";
    mailValues[1] = NULL;
    modMail.mod_values = mailValues;

    /* 
     * LDAP_MOD_ADD gives an error if the value already exists.
     */
    modPhone1.mod_op = LDAP_MOD_ADD;
    modPhone1.mod_type = "telephoneNumber";
    phoneValues1[0] = "1 234 567 8910";
    phoneValues1[1] = NULL;
    modPhone1.mod_values = phoneValues1;

    /* 
     * LDAP_MOD_DELETE will give an error if the value does not exist.
     * In this example, we delete the telephone number we just added.
     */
    modPhone2.mod_op = LDAP_MOD_DELETE;
    modPhone2.mod_type = "telephoneNumber";
    phoneValues2[0] = "1 234 567 8910";
    phoneValues2[1] = NULL;
    modPhone2.mod_values = phoneValues2;

    modify[0] = &modMail;
    modify[1] = &modPhone1;
    modify[2] = &modPhone2;
    modify[3] = NULL;

    /* Modify the attributes */
    rc= ldap_modify_ext_s( ld,         /* LDAP session handle */
                           modifyDN,   /* the object to modify */
                           modify,     /* array of LDAPMod structures */
                           NULL,       /* server controls */
                           NULL);      /* client controls */

    if ( rc != LDAP_SUCCESS )
    {
        printf("ldap_modify_ext_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s( ld );
        return(1);
    }
    printf("\n\t%s modified successfully.\n", modifyDN );

    ldap_unbind_s( ld );

    return( 0 );
}

