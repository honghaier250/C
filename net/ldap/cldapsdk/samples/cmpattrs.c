/* $Novell: cmpattrs.c,v 1.6 2003/05/12 12:15:12 $ */
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
    cpattrs.c
***************************************************************************
   Description: cpattrs.c does a compare operation to determine if 
                an entry contains particular objectClass and cn values.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   cmpattrs <host name> <port number> <login dn> <password>"
" <compare dn>" 
"\n Example: cmpattrs acme.com 389 cn=admin,o=acme secret"
"\n\t  cn=james,ou=sales,o=acme\n";

void printout( int rc, char *dn, char *attr, char *vals );

int main( int argc, char **argv)
{ 

    int         version, ldapPort, rc;
    char        *ldapHost, *loginDN, *password;    
    char        *compareDN, *compareAttribute, *compareValue;       
    LDAP        *ld;
    struct berval bvalue;
    struct timeval timeOut = {10,0};    /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
        printf("%s", usage);
        exit ( 0 );
    }

    ldapHost    = argv[1];
    ldapPort    = atoi(argv[2]);
    loginDN     = argv[3];
    password    = argv[4];
    compareDN   = argv[5];

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
    printf("\n\tLDAP session initialized\n");

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
     * Determine if this object contains the value "inetOrgPerson"
     * in its objectclass attribute. 
     */
    compareAttribute = "objectclass";
    compareValue = "inetOrgPerson";
    bvalue.bv_val = compareValue;
    bvalue.bv_len = strlen( compareValue );

    rc = ldap_compare_ext_s( ld,                /* LDAP session handle */
                             compareDN,         /* the object being compared */
                             compareAttribute,  /* the attribute to compare */
                             &bvalue,           /* the value to compare */
                             NULL,              /* server controls */
                             NULL);             /* client controls */

    printout( rc, compareDN, compareAttribute, compareValue );

    /* 
     * Determine if this object contains the value "Mary" in its cn attribute.
     */
    compareAttribute = "cn";
    compareValue = "Mary";
    bvalue.bv_val = compareValue;
    bvalue.bv_len = strlen( compareValue );

    rc = ldap_compare_ext_s( ld,                /* LDAP session handle */ 
                             compareDN,         /* the object being compared */
                             compareAttribute,  /* the attribute to compare */ 
                             &bvalue,           /* the value to compare */
                             NULL,              /* server controls */
                             NULL);             /* client controls */

    printout( rc, compareDN, compareAttribute, compareValue );

    ldap_unbind_s( ld );

    return( 0 );
}

void printout( int rc, char *dn, char *attr, char *vals )
{  
    switch ( rc )
    {
    case LDAP_COMPARE_TRUE:
        printf("\n\t%s has the value \"%s\"\n\t"
               "in the \"%s\" attribute.\n", dn, vals, attr );
        break;

    case LDAP_COMPARE_FALSE:
        printf("\n\t%s does NOT have the value \"%s\"\n\t"
               "in the \"%s\" attribute.\n", dn, vals, attr );
        break;

    default:
        printf("ldap_compare_ext_s: %s\n", ldap_err2string( rc ) );        
        break;
    }
}
