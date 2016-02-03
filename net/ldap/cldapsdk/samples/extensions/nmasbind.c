/* $Novell: nmasbind.c,v 1.4 2003/05/12 13:21:01 $ */
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
   nmasbind.c 
***************************************************************************
   Description: 
    
  The nmasbind.c sample shows how to authenticate to an LDAP server using a 
  NetIQ Modular Authentication Service (NMAS) login sequence. NMAS provides 
  an infrastructure for adding new authentication methods for authenticating
  a user to eDirectory and/or Netware. New authentication methods can include 
  alternate passwords, smart cards, tokens, or biometric measures such as 
  finger prints or retina scans. LDAP client support for NMAS is only 
  available on the Microsoft Windows platforms.

  eDirectory 8.7 ships with standard NMAS software that supports three basic
  login methods provided by NetIQ: 

    - Simple Password: authenticates the user using the user's Simple Password.
         The simple password can be set by using the ICE import-export tool, 
         the SimplePassword ConsoleOne snap-in (available while editing a user 
         object's properties), or iManager's modify object task under 
         eDirectory Administration.

    - X509 Certificate: authenticates the user using the user's exported X509
         Certificate.

    - NDS: authenticates the user using the user's NDS password. The NDS
         method provides the same password security as authenticating with
         the NetWare client but does not require the NetWare client.

  To use more advanced authentication methods, including those provided by 
  other vendors, you must purchase the Enterprise Edition of NMAS.
 
  To use NMAS with an LDAP client application, the modules nmas.dll and 
  nmasmsg.dll must be available on the client machine. These modules are 
  not included in the SDK. Hence, the user has to download the latest version 
  from developer.novell.com to use the nmas related functionalities with ldapsdk.
  Additionally, both server support and client support for the 
  method you wish to use must be installed. Server support for methods and 
  any associated ConsoleOne snap-ins can be installed using the method 
  installer found on the eDirectory installation CD at 
  \nmas\NmasMethods\MethodInstaller.exe. Client support for individual 
  methods can be installed by running the install program on the eDirectory 
  installation CD at 
  \nt\I386\NDSonNT\ndsnt\nmas\nmasmthd\<method name>\client\clientsetup.exe

  After installation, a method must be associated with an NMAS sequence object.
  By default, the server side installation of a method creates a sequence with 
  the same name as the method. This sequence name is the <sequence> required 
  on the command line when you run this sample.
 
  More information on NMAS, including documentation, can be obtained at: 
  http://www.novell.com/products/nmas
     
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ldapx.h> /* needed for ldap_get_bind_dn function, includes ldap.h */
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   nmasbind <host> <port> <login dn> <sequence> [<password>]"
"\n Example: nmasbind Acme.com 389 cn=admin,o=Acme \"Simple Password\"\n";

int main( int argc, char **argv )
{   
    int          version, ldapPort, rc;
    char         *ldapHost, *loginDN, *sequence, *bindDN, *password = NULL;
    LDAP         *ld;
    LDAPMessage  *result = NULL;    
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */
	int nmasError;
                         
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if ((argc != 5) && (argc != 6)) {
        printf("%s", usage);
        return (1);
    }

    ldapHost = argv[1];
    ldapPort = atoi(argv[2]);
    loginDN  = argv[3];
    sequence = argv[4];
    if (6 == argc)
        password = argv[5];

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init(ldapHost, ldapPort)) == NULL) {
        printf ( "\nError in ldap_init\n");
        return( 1 );
    }

    /* Bind to the server */
    rc = ldap_bind_nmas_s(ld,       /* ldap context */
                          loginDN,  /* user dn */
		 	              password, /* optional password */
                          sequence, /* optional requested sequence */
                          NULL);    /* optional requested clearance */

	if ( LDAP_SUCCESS != rc ) {
		printf("LDAP Error Code : %d\n", rc);
		nmasError = ldap_nmas_get_errcode();
		if (nmasError != 0)
			printf("NMAS Error: %d, %s\n",nmasError , ldap_nmas_err2string(nmasError));
		else
			 printf("Unexpected Error in ldap_bind_nmas_s: %d, %s\n", 
               rc, ldap_err2string(rc));
	}
    else {
        printf("NMAS Bind was successful\n");
    }

    /* Get the bind DN. If the bind failed, bind DN will be [public] */
    rc = ldap_get_bind_dn(ld, &bindDN);
    if (LDAP_SUCCESS == rc) {
        printf("Bind DN: %s", bindDN);
        ldapx_memfree(bindDN);
    }
    else {
        printf("Unexpected error in ldap_get_bind_dn: %d, %s\n", 
               rc, ldap_err2string(rc));
    }

    ldap_unbind( ld );  

    return( 0 );
}
