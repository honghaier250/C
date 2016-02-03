/* $Novell: searchURL.c,v 1.6 2003/05/12 13:05:24 $ */
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
   searchURL.c
***************************************************************************
   Description:
   The searchURL.c sample demonstrates how to call functions to parse an 
   LDAP URL into its components and to process a search request specified by 
   an LDAP URL. In addtion, the results are sorted by the sn attribute and 
   printed.

   LDAP URLs provide a uniform method to access information on an LDAP
   server.  Details can be found in RFC 2255. LDAP URLs have the following 
   syntax:

   ldap[s]://<hostname>:<port>/
   <base_dn>?<attributes>?<scope>?<filter>?<extension>

***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ldap.h>
#include <ldap_ssl.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   searchURL <search URL>\n"
"\n Examples: "
"\n 1. searchURL ldap://Acme.com:389/ou=Sales,o=Acme?sn,mail?sub?(objectclass=*)"
"\n 2. searchURL ldap:///ou=Sales,o=Acme?sn?one"
"\n 3. searchURL ldaps://Acme.com:636/o=Acme?objectclass?one\n";

int main( int argc, char **argv )
{
    int         version, i, rc, entryCount, isSecure=0;
    char        *ldapURL;
    char        *attribute, *dn, **values,  *sortAttribute = "sn";
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */
    BerElement  *ber;
    LDAP        *ld;
    LDAPMessage *searchResult, *entry;
    LDAPURLDesc *ludpp;


    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if ( argc != 2 )
    {
        printf("%s", usage);
        return (1);
    }

    ldapURL           = argv[1];

    /* 
     * Determining if given URL is an LDAP URL 
     */
    if ( ldap_is_ldap_url( ldapURL ))
        printf( "\n    %s is a valid URL\n", ldapURL);
    else
    {
        printf( "\n    %s is not a valid LDAP URL.", ldapURL);
        printf( "\n    URL had bad prefix.  Should be ldap:// or ldaps://\n");
        return ( 1 );
    }

    /* Parse the specified URL into its components */
    rc = ldap_url_parse(ldapURL, &ludpp);
    if ( rc != LDAP_SUCCESS )
    {
        switch ( rc ) 
        {
            case  LDAP_URL_ERR_MEM :
                      printf("\n    Cannot allocate memory space.\n");
                      break;

            case  LDAP_URL_ERR_PARAM :
                      printf("\n     Invalid parameter.\n");
                      break;

            case  LDAP_URL_ERR_BADSCHEME :
                      printf("\n     URL doesnt begin with \"ldap[s]://\".\n");
                      break;

            case  LDAP_URL_ERR_BADENCLOSURE :
                      printf("\n     URL is missing trailing \">\".\n");
                      break;

            case  LDAP_URL_ERR_BADURL :
                      printf("\n     Invalid URL.\n");
                      break;

            case  LDAP_URL_ERR_BADHOST :
                      printf("\n     Host port is invalid.\n");
                      break;

            case  LDAP_URL_ERR_BADATTRS :
                      printf("\n     Invalid or missing attributes.\n");
                      break;

            case  LDAP_URL_ERR_BADSCOPE :
                      printf("\n     Invalid or missing scope string.\n");
                      break;

            case  LDAP_URL_ERR_BADFILTER :
                      printf("\n     Invalid or missing filter.\n");
                      break;

            case  LDAP_URL_ERR_BADEXTS :
                      printf("\n     Invalid or missing extensions.\n");
                      break;
        }
        return ( 1 );
    }

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Get the URL scheme ( either ldap or ldaps ) */
    if ( 0 == strcmp ( ludpp->lud_scheme, "ldaps" ) )
        isSecure = 1;

    /* Initialize the LDAP session */
    if ( !isSecure ) 
    {
        printf("\n    Making clear-text connection\n");
        /* Making clear text connection */
        if (( ld = ldap_init( ludpp->lud_host, 
                   ludpp->lud_port?ludpp->lud_port:LDAP_PORT )) == NULL)
        {
            printf ( "\n    LDAP session initialization failed\n");
            ldap_free_urldesc( ludpp );
            return( 1 );
        }
        printf ( "\n    LDAP session initialized\n");
    }
    else  /* Making encrypted connection */
    {
        printf("\n    Making encrypted connection\n");
        /*
         * Initialize the ssl library.  The first parameter of
         * ldapssl_client_init is a certificate file.  However, when used
         * the file must be a DER encoded file.  NULL is passed in for the
         * certificate file because ldapssl_set_verify_mode will be used
         * to specify no server certificate verification.
         *
         * ldapssl_client_init is an application level initialization not a
         * thread level initilization and should be done once.
         */
        rc = ldapssl_client_init(   NULL,       /* DER encoded cert file  */
                                    NULL );     /* reserved, use NULL */

        if (rc != LDAP_SUCCESS)
        {
            printf("ldapssl_client_init error: %d\n", rc);
            ldap_free_urldesc( ludpp );
            return (1);
        }

        /* Configure the LDAP SSL library to not verify the server certificate.
         * The default is LDAPSSL_VERIFY_SERVER which validates all servers
         * against the trusted certificates normally passed in
         * during ldapssl_client_init or ldapssl_add_trusted_cert.
         *
         * WARNING:  Setting the verify mode to LDAPSSL_VERIFY_NONE turns off
         * server certificate verification.  This means all servers are
         * considered trusted.  This should be used only in controlled
         * environments where encrypted communication between servers and
         * clients is desired but server verification is not necessary.
         */
        rc = ldapssl_set_verify_mode(LDAPSSL_VERIFY_NONE);
        if (rc != LDAP_SUCCESS)
        {
            printf("ldapssl_set_verify_mode error: %d\n", rc);
            ldapssl_client_deinit();
            ldap_free_urldesc( ludpp );
            return (1);
        }

        /*
         * create a LDAP session handle that is enabled for ssl connection
         */
        ld = ldapssl_init( ludpp->lud_host, /* host name */
                           ludpp->lud_port?ludpp->lud_port:LDAPS_PORT,
                                            /* port number */
                           1 );             /* 0-clear text,1-enable for ssl */

        if (ld == NULL )
        {
            printf("ldapssl_init error\n" );
            ldapssl_client_deinit();
            ldap_free_urldesc( ludpp );
            return (1);
        }
    }

    /* Bind to the server  - Anonymous bind */
    rc = ldap_simple_bind_s( ld, NULL, NULL);

    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s error: %d, %s\n", rc, ldap_err2string( rc ));
        ldap_unbind_s( ld );
        if (isSecure) ldapssl_client_deinit();
        ldap_free_urldesc( ludpp );
        return (1);
    }

    printf("\n    Bind successful - performing search\n");

    /* Search the directory */
    rc = ldap_url_search_st(
                    ld,             /* LDAP session handle */
                    ldapURL,        /* LDAP URL to use in the search operation*/
                    0,              /* return attributes and values */
                    &timeOut,       /* search timeout */
                    &searchResult );/* returned results */
    
    if ( rc != LDAP_SUCCESS ) 
    {
        printf("ldap_search_ext_s: %s\n", ldap_err2string( rc ));
        ldap_msgfree( searchResult );
        ldap_unbind_s( ld );
        if (isSecure) ldapssl_client_deinit();
        ldap_free_urldesc( ludpp );
        return ( 1 );
    }

    /* client-sort */
    ldap_sort_entries( ld, &searchResult, sortAttribute, strcmp );

    /* Go through the search results by checking entries */
    for (   entry   =   ldap_first_entry( ld, searchResult ); 
            entry   !=  NULL; 
            entry   =   ldap_next_entry( ld, entry ) )
    {
        if (( dn = ldap_get_dn( ld, entry )) != NULL )            
        {
            printf("\n    dn: %s\n", dn );
            ldap_memfree( dn );
        }

        for (   attribute = ldap_first_attribute( ld, entry, &ber );
                attribute != NULL; 
                attribute = ldap_next_attribute( ld, entry, ber ) ) 
        {
            /* Get values and print.  Assumes all values are strings. */
            if (( values = ldap_get_values( ld, entry, attribute)) != NULL )
            {
                for ( i = 0; values[i] != NULL; i++ )
                    printf("        %s: %s\n", attribute, values[i] );
                ldap_value_free( values );
            }
            ldap_memfree( attribute );
        }

        ber_free(ber, 0);

    }

    entryCount = ldap_count_entries( ld, searchResult );

    printf("\n    Search completed successfully.\n    Entries  returned: %d\n",
             entryCount);

    ldap_msgfree( searchResult );

    ldap_unbind_s( ld );  

    if (isSecure) ldapssl_client_deinit();

    ldap_free_urldesc( ludpp );
 
    return( 0 );
}

