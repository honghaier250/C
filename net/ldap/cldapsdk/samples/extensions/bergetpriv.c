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
   bergetpriv.c
***************************************************************************
   Description: Demonstrates how to encode/decode the get effective rights
                extended request/response to get the effective rights of the
                specified entry (trustee) to the specified attribute of the 
                specified subject (object).
                
                The request is of the following form 

                RequestBer
                    dn          OCTET STRING
                    trusteeDN   OCTET STRING
                    attrName    OCTET STRING

                The response is of the following form 

                ResponseBer
                    privileges  INTEGER
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

    BerElement*    requestber = NULL;
    BerElement*    returnedber = NULL;
    int            err = LDAP_SUCCESS;
    char*       returnedOID = NULL;
    struct berval*  requestbv = NULL;
    struct berval* returnedbv = NULL;


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
    if ( (ld = ldap_init( ldapHost, ldapPort )) == NULL ) {
        perror( "ldap_init" );
        return( 1 );
    }

    /* Authenticate to the server */
    printf( "    Binding to the directory...\n" );
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if ( rc != LDAP_SUCCESS ) {
        printf( "ldap_simple_bind_s: %s\n", ldap_err2string(rc));
        ldap_unbind_s( ld );
        return( 1 );
    }

    /*
     * Novell extended operation to get the effective privileges
     * of a trustee upon an entry's attribute.
     *
     * Allocate a BerElement for stuffing in the function IN parameters 
     */
    requestber = ber_alloc();
    if(requestber == NULL)
         return LDAP_NO_MEMORY;

    /* The Request is of the following form 
     *  RequestBer
     *      dn          OCTET STRING
     *      trusteeDN   OCTET STRING
     *      attribute   OCTET STRING
     */

    /* stuff in the object dn, the trusteeDN and attrname parameters */
    rc = ber_printf(requestber, "sss", objectDN, trusteeDN, attrName);
    if (rc == LBER_ERROR) {
        rc = LDAP_ENCODING_ERROR;
        goto done;
    }

    /* Converts the BER we just built to a berval that we will send out
    in the extended request */
    if (ber_flatten(requestber, &requestbv) == LBER_ERROR) {
        rc = LDAP_NO_MEMORY;
        goto done;
    }

    printf("Calling ldap_extended_opetation to get effective privileges...\n");
    /* Call the ldap_extended_operation (synchronously) */
    rc = ldap_extended_operation_s(ld, 
                                NLDAP_GET_EFFECTIVE_PRIVILEGES_EXTENDED_REQUEST,
                                requestbv, 
                                NULL, 
                                NULL, 
                                &returnedOID, 
                                &returnedbv);

    if (rc  != LDAP_SUCCESS) {
        printf("ldap_extended_operation_s failed.\n");
        goto done;
    }

    /* Make sure there is a returned OID */
    if (!returnedOID) {
        rc = LDAP_NOT_SUPPORTED;
        goto done;
    }

    /* Is this what we were expecting to get back */
    if (strcmp(returnedOID, NLDAP_GET_EFFECTIVE_PRIVILEGES_EXTENDED_REPLY)) {
        rc = LDAP_NOT_SUPPORTED;
        goto done;
    }

    /* Do we have a good returned berval? */
    if (!returnedbv) {
        /* No returned berval means rather drastic error .... operations error*/
        rc = LDAP_OPERATIONS_ERROR;
        goto done;
    }

    /* Build a berElement from the returned berval */
    returnedber = ber_init(returnedbv);

    if (returnedber == NULL) {
        rc = LDAP_NO_MEMORY;
        goto done;
    }

    /* The returned data contains the privilege flag - scan it */
    rc = ber_scanf(returnedber, "i", &privileges);
    if (rc == LBER_ERROR) {
        rc = LDAP_ENCODING_ERROR;
        goto done;
    }
    else
        /* Extended request completed successfully */
        rc = LDAP_SUCCESS;

    if ( rc != LDAP_SUCCESS )
    {
         /* print out LDAP server message */
         if(ldap_get_option(ld, LDAP_OPT_ERROR_STRING, &msg)==LDAP_OPT_SUCCESS)
         {
             if ( (msg != NULL) && (strlen(msg) != 0) )
                  printf("\n    LDAP server message: %s", msg);
         }

         /* print out LDAP error message */
         printf("    LDAP error  message: ldap_get_effective_"
                "privileges: %s (%d)\n", ldap_err2string( rc ), rc); 
         printf("\n    ldap_get_effective_privileges failed\n\n");
         goto done;
    }

    printf("\nldap_get_effective_privileges extended operation succeeded\n\n");
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
        printf("\n\tTrustee CANNOT add or delete its name as a value of"
                 " the attribute.\n\n");

     if ( privileges & LDAP_DS_ATTR_SUPERVISOR )
        printf("\n\tTrustee has all rights to the the object's attributes\n\n");
     else
        printf("\n\tTrustee does not have all rights to the the object's "
                 "attributes\n\n");

     if ( privileges & LDAP_DS_ATTR_INHERIT_CTL )
         printf("\n\tTrustee inherits the rights granted in the ACL.\n\n");
     else
         printf("\n\tTrustee does not inherit the rights granted in "
                  "the ACL.\n\n");

     ldap_unbind_s( ld );


done:
    if (returnedber)
        ber_free(returnedber, 1);

    /* Free the returned OID string if one was returned */
    if (returnedOID)
        ldap_memfree(returnedOID);

    /* free any memory the extended_operation API must have allocated */
    if (returnedbv)
        ber_bvfree(returnedbv);

    /* Free memory that we allocated when building the request ber and berval */
    if (requestbv)
        ber_bvfree(requestbv);

    if (requestber)
        ber_free(requestber, 1);

    /* return the appropriate error/success code */
    return (rc);
}
