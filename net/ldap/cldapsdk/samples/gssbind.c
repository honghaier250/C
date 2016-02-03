/* $Novell: gssbind.c,v 1.8 2003/05/12 12:14:11 $ */
/**************************************************************************
*  Novell Software Developer Kit
*
*  Copyright (C) 2002-2004 Novell, Inc. All Rights Reserved.
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
    gssbind.c
***************************************************************************
   Description: 
   The gssbind.c sample demonstrates LDAP v3 SASL bind to an LDAP server 
   using GSSAPI authentication mechanism.

   GSSAPI internally uses kerberos TGT to authenticate. So you should 
   have got kerberos TGT on the client machine before doing GSSAPI bind. 
   You can use kinit to get TGT from KDC. 

   eDirectory 8.8 or newer is required.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#include <string.h>
#include <ldap_gss.h>

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

int isGSSAPISupported(LDAP *ld);

static char usage[] = "\n\tUsage:   gssbind <host name> <port number>"
                      "\n\tExample: gssbind Acme.com 389 \n";

int main( int argc, char **argv)
{ 
    int      rc = 0, ldapPort = 0, version = 0;
    char     *ldapHost;
    LDAP     *ld;
    struct timeval timeOut = {10,0};    /* 10 second connection timeout */
    gss_err_code gerr_code;

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 3)
    {
        printf("%s", usage);
        return (1);
    }
             
    ldapHost   =  argv[1];
    ldapPort   =  atoi(argv[2]);
 
    /* 
     * Set LDAP version to 3.
     * Using a NULL session handle sets the global options.
     * All subsequent LDAP sessions created in this process will be version 3.
     * Also set the connection timeout.
     */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* 
     * Initialize a new LDAP session 
     */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n\tLDAP session initialization failed\n");
        return( 1 );
    }

    if ( isGSSAPISupported(ld) )
    {
         /* GSS bind to the server */
         rc = ldap_gssbind( ld,
                            ldapHost,
                            "GSSAPI",	// Note: "GSSAPI" provided to ldap_gssbind is casesensitive
                            NULL, 
                            NULL,
                            &gerr_code);

         if (rc != LDAP_SUCCESS)
         {
             if(rc==LDAP_GSS_ERROR)
                  printf("\n\tldap_simple_bind_s: %s\n", ldap_gss_error(gerr_code));
             else 
                  printf("\n\tldap_simple_bind_s: %s\n", ldap_err2string( rc ));
             ldap_unbind_s( ld );
             return ( 1 );
         }

         printf("\n\tBind and authentication to the server successful\n");
    }
    else
        printf("\n\tNo GSS support from the LDAP server.\n");

    ldap_unbind_s( ld );    

    return ( rc );
}

int isGSSAPISupported(LDAP *ld)
{
    int         i, rc         = LDAP_SUCCESS;
    char        *attribute    = NULL;
	char        **values      = NULL;
    BerElement  *ber          = NULL;
    LDAPMessage *searchResult = NULL; 
	LDAPMessage *entry        = NULL;
    char        *attribs[]    = {"supportedsaslmechanisms",NULL};    

	/* Reading the root DSE */
    rc = ldap_search_ext_s(ld, "" , LDAP_SCOPE_BASE, "(objectClass=*)",
                           attribs, 0,
						   NULL, NULL, NULL, LDAP_NO_LIMIT, &searchResult);

    if( LDAP_SUCCESS != rc)
	{
        printf("ldap_search_ext_s: %s\n", ldap_err2string( rc ));
        ldap_msgfree( searchResult );
        ldap_unbind_s( ld );
        return ( 0 );
	}

    entry = ldap_first_entry( ld, searchResult ); 
            
    for (attribute = ldap_first_attribute( ld, entry, &ber );
           attribute != NULL; 
           attribute = ldap_next_attribute( ld, entry, ber )) 
    {   
        /* Get values and print.  Assumes all values are strings. */
        if (( values = ldap_get_values( ld, entry, attribute)) != NULL ) 
        {
		    for ( i = 0; values[i] != NULL; i++ )
                if (strcmp(values[i],"GSSAPI") == 0)
                {
                    rc = 1;
                    goto End;
                }
                    
            ldap_value_free( values );
        }
        ldap_memfree( attribute );
	}
    ber_free(ber, 0);

End:   
    ldap_msgfree( searchResult );
    return rc;
}

