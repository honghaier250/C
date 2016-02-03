/* $Novell: searchBinary.c,v 1.11 2003/05/12 13:04:09 $ */
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
   searchBinary.c 
***************************************************************************
   Description: searchBinary.c is similar to search.c, but handles binary 
                values appropriately.  All values are read in binary form.  
                Any value with non-ASCII bytes is printed in hex, 
                while ASCII strings are printed normally.
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   searchBinary <host name> <port number> <login dn> <password>"
"\n          <search base>\n" 
"\n Example: searchBinary Acme.com 389 cn=admin,o=Acme secret"
"\n          ou=Sales,o=Acme\n";

/* Prototypes for internal functions */
int  value_is_printable( struct berval *bval );
void binary_print( char *attr, struct berval *bval );


int main( int argc, char **argv )
{   
    int           version, ldapPort, i, rc;
    char          *ldapHost, *loginDN, *password, *searchBase;   
    char          *attribute, *dn, *sortAttribute;       
    BerElement    *ber;
    LDAP          *ld;
    LDAPMessage   *searchResult, *entry;
    struct berval **bvalues;
    struct timeval timeOut = {10,0};   /* 10 second connection/search timeout */
    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
   
    if (argc != 6)
    {
        printf("%s", usage);
        return (1);
    }
           
    ldapHost          = argv[1];
    ldapPort          = atoi(argv[2]);
    loginDN           = argv[3];
    password          = argv[4];
    searchBase        = argv[5];    

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n    LDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n    LDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("\n    Bind successful\n");


    /* Search the directory */
    rc = ldap_search_ext_s(  
                    ld,                    /* LDAP session handle */
                    searchBase,            /* container to search */
                    LDAP_SCOPE_ONELEVEL,   /* search scope */
                    "(objectclass=*)",     /* search filter */
                    NULL,                  /* return all attributes */
                    0,                     /* return attributes and values */
                    NULL,                  /* server controls */
                    NULL,                  /* client controls */
                    &timeOut,              /* search timeout */
                    LDAP_NO_LIMIT,         /* no size limit */
                    &searchResult );       /* returned results */
    
    if ( rc != LDAP_SUCCESS ) 
    {
        printf("ldap_search_ext_s: %s\n", ldap_err2string( rc ));
        ldap_msgfree( searchResult );
        ldap_unbind_s( ld );
        return ( 1 );
    }   
     
    /* Sort the entries by surname (client-side sort) */
    sortAttribute = "sn";
    ldap_sort_entries( ld,                  /* LDAP session handle */
                       &searchResult,       /* results to sort */
                       sortAttribute,       /* attribute on which to sort */
                       strcmp );            /* comparison function to use */

    /* Go through the search results by checking entries */
    for (   entry   =   ldap_first_entry( ld, searchResult ); 
            entry   !=  NULL; 
            entry   =   ldap_next_entry( ld, entry ) ) 
    {
        if (( dn = ldap_get_dn( ld, entry )) != NULL )            
        {
            printf("\n--- dn: %s\n", dn );
            ldap_memfree( dn );
        }
            
        for (   attribute = ldap_first_attribute( ld, entry, &ber );
                attribute != NULL; 
                attribute = ldap_next_attribute( ld, entry, ber ) ) 
        {   
            /* Get array of pointers to berval structures */
            if ((bvalues = ldap_get_values_len( ld, entry, attribute)) != NULL) 
            {
                for ( i = 0; bvalues[i] != NULL; i++ ) 
                {
                    printf("\n");

                    if (value_is_printable( bvalues[i] ) )
                        printf("      %s: %s\n", 
                               attribute, bvalues[i]->bv_val );
                    else
                        binary_print( attribute, bvalues[i] );
                }
                ldap_value_free_len( bvalues );
            }
            ldap_memfree( attribute );
        }
       
        ber_free(ber, 0);

    }

    printf("\n    Search completed successfully.\n");
   
    ldap_msgfree( searchResult );
    
    ldap_unbind_s( ld );  
 
    return( 0 );
}


/* 
   Return 1 if all bytes in the value are printable, otherwise return 0. 
*/
int  value_is_printable( struct berval *bval )
{
    unsigned int i;
    for ( i=0; i<bval->bv_len; i++)
    {
        if ( !isprint( (bval->bv_val)[i] ) )
            return 0;   /* At least one non-printable byte in value */
    }

    return 1;   /* All bytes are printable. */
}


/* 
   Print the binary value in hex 
*/
void binary_print( char *attr, struct berval *bval )
{
   int  i, x, len, lines, m=0;
   char szChar[3], szHex[4];
   char HexLine[(3*16)+1], CharLine[16+1];
   unsigned char *buf;

   len = (int)bval->bv_len;
   buf = (unsigned char*)bval->bv_val;

   printf("      %s:\n", attr );

   lines = len / 16;

   if ( len % 16 )
      lines++;


   for (i=0; i<lines; i++)
   {
      HexLine[0] = '\0';
      CharLine[0] = '\0';
      for (x=0; x<16; x++)
      {
         if (m < len)
         {
            sprintf(szHex, "%.2X ", buf[m]);
            if (isprint(buf[m]))
               sprintf(szChar, "%c", buf[m]);
            else
               sprintf(szChar, ".");
            m++;
         }
         else
         {
            sprintf(szHex, "   ");
            sprintf(szChar, " ");
         }
         strcat(HexLine, szHex);
         strcat(CharLine, szChar);
      }
      printf("      %s  %s\n", HexLine, CharLine);
   }
}

