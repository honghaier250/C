/* $Novell: utf8bind.c,v 1.8 2003/05/12 12:14:11 $ */
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
    utf8bind.c
***************************************************************************
   Description: The utf8bind.c sample demonstrates how to convert utf8 messages (returned from 
   ldap_err2string api) to local string and diplay it. 
***************************************************************************/
#if defined(N_PLAT_NLM) && defined(CLIB)
#define NWL_EXCLUDE_TIME  
#define NWL_EXCLUDE_FILE  
#include <string.h>  
#include <assert.h>  
#include <nwlocale.h>  
#include <unicode.h>  

#elif defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#include <unilib.h> 

#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <lm.h>
#include <stdio.h>
#include <ldap_utf8.h>
#endif

#include <stdio.h>
#include <limits.h>
#include <ldap.h>
#include <errno.h>
#include <string.h>

static char usage[] =
"\nUsage:   UTFbind <host name> <port number> <login dn> <password>" 
"\nExample: bind Acme.com 389 cn=admin,o=Acme secret\n";

char *utf8_to_local(char *utf8str);
extern int errno;

int main( int argc, char **argv)
{ 
    int      rc, ldapPort, version;
    char     *ldapHost, *loginDN, *password;
    LDAP     *ld;
    struct timeval timeOut = {10,0};    /* 10 second connection timeout */
    char 	*errorMesg;	    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 5)
    {
        printf("%s", usage);
        return (1);
    }
             
    ldapHost   =  argv[1];
    ldapPort   =  atoi(argv[2]);
    loginDN    =  argv[3];
    password   =  argv[4];
 
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
     * Initialize the LDAP session 
     */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n\tLDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n\tLDAP session initialized\n");
   
    /* 
     * Bind to the server anonymously 
     */
    if(( rc = ldap_simple_bind_s( ld, NULL, NULL )) != LDAP_SUCCESS )
    {
       errorMesg= ldap_err2string( rc );
	if (errno==0)
		printf("\n\tldap_simple_bind_s: %s\n", utf8_to_local(errorMesg));
	else
		printf("\n\tldap_simple_bind_s: %s\n", errorMesg);
       ldap_unbind_s( ld );
       return ( 1 );
    }
    printf("\n\tAnonymous bind successful\n");
    
    ldap_unbind_s( ld );
   
    
    /* 
     * Initialize a new LDAP session 
     */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n\tLDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n\tNew LDAP session initialized\n");

    /* 
     * Simple bind to the server 
     */
    if ((rc = ldap_simple_bind_s( ld, loginDN, password )) != LDAP_SUCCESS )
    {
	errorMesg= ldap_err2string( rc );
	if (errno==0)
		printf("\n\tldap_simple_bind_s: %s\n", utf8_to_local(errorMesg));
	else
		printf("\n\tldap_simple_bind_s: %s\n", errorMesg);
       ldap_unbind_s( ld );
       return ( 1 );
    }
    printf("\n\tBind and authentication to the server successful\n");

    ldap_unbind_s( ld );    

    return ( 0 );
}

#define MAX_UNICODE_CHAR 512
#define MAX_UTF8_CHAR 512
#define MAX_LOCAL_CHAR 512

/* This function actaully takes a utf8 string and converts it into local copde page string */

/****************************************************************************
							Netware CLIB Section
/****************************************************************************/

#if defined(N_PLAT_NLM) && defined(CLIB)
char *utf8_to_local(char *utf8str)
{
	LCONV lconv;  
	nint err;  
       nuint8 noMap = 0;   /* Use the default replacement char. */  
       nuint len;  
	nptr   handle;
	unicode src[MAX_UNICODE_CHAR * sizeof(unicode)];
	static char dest[MAX_LOCAL_CHAR * MB_CUR_MAX];
        NWLlocaleconv(&lconv);  
	 err = NWInitUnicodeTables(lconv.country_id, lconv.code_page);  
	 if (err)
	 	{
	 	printf("NWInitUnicodeTables failed: %d\n",err);
	 	goto Error;
	 	}
	 err=NWGetUnicodeToLocalHandle  (&handle);
	 if (err)
	 	{
	 	printf("NWGetUnicodeToLocalHandle failed: %d\n",err);
	 	goto Error;
	 	}
	 err = NWLUTF8ToUnicode((pnuint8) utf8str, sizeof(src), src, &len, NULL);
	 if (err)
	 	{
	 	printf("NWLUTF8ToUnicode failed: %d\n",err);
	 	goto Error;
	 	}
	 err = NWUnicodeToLocal(handle, (unsigned char *)dest, sizeof(dest), src, noMap, &len);  
	 if (err)
	 	{
	 	printf("NWUnicodeToLocal failed: %d\n",err);
	 	goto Error;
	 	}
 	 NWFreeUnicodeTables();
	 return dest;
	 Error:
 	 NWFreeUnicodeTables();
   	 printf("Unable to cinvert from utf-8 to local.. Returning utf-8 string\n");
   	 return utf8str;
}

/****************************************************************************
							Netware LIBC Section
/****************************************************************************/

#elif defined(N_PLAT_NLM) && defined(LIBC)
#define MB_CUR_MAX 2
char *utf8_to_local(char *utf8str)
{
	UniRuleTable_t   unirule_table=NULL;
	static char dest[MAX_LOCAL_CHAR * MB_CUR_MAX];
	int err;
	err=UniGetTable (  UniGetHostCodePage(),  &unirule_table);
	if(err)
		goto Error;
	if ((err = utf82loc(unirule_table, dest, utf8str, 0, UNI_MAP_NO_CHAR))!=0)
		goto Error;
	UniDisposeTable(unirule_table);
	return dest;
	Error:
   	 printf("Unable to convert from utf-8 to local.. Returning utf-8 string\n");
 	 return utf8str;
}

/****************************************************************************
							Win32 Section
/****************************************************************************/

#elif defined(_WIN32) || defined(_WIN64)
char *utf8_to_local(char *utf8str)
{
WCHAR dest[MAX_UNICODE_CHAR * sizeof(WCHAR)];          // Unicode user name
static char errorstr[MAX_LOCAL_CHAR * 2]=" ";
int err;
// _asm{int 3};
err = ldap_x_utf8s_to_wcs (dest, utf8str,sizeof(dest));
if (err<=0)
	{
	printf("ldap_x_utf8s_to_wcs failed \n",err);
	goto Error;
	}

err = WideCharToMultiByte( CP_ACP, 0, dest, -1, errorstr, sizeof(errorstr), NULL, NULL );
if (err==0)
	{
	printf("WideCharToMultiByte  failed: %d\n",GetLastError());
	goto Error;
	}
return errorstr;
Error: 
   	 printf("Unable to convert from utf-8 to local.. Returning utf-8 string\n");
   	 return utf8str;
}

/****************************************************************************
							UNIX Section
/****************************************************************************/

#elif defined(UNIX)
//#if defined(HAVE_ICONV_H) /* && !defined(__GLIBC__) */
#define USE_ICONV 

#include <langinfo.h>
#include <iconv.h>
static iconv_t   UTF8ToLoc = (iconv_t) -1;
#define MB_CUR_MAX 2
//#endif

char *utf8_to_local(char *utf8str)
{
#ifdef USE_ICONV
	char *oldlocale, *codeset, *tmp;
	const char *inbuf;
	char *outbuf;
	size_t inbytesleft, outbytesleft;
	static char errorstr[MAX_LOCAL_CHAR * MB_CUR_MAX]=" ";
	codeset = nl_langinfo(CODESET);
/* conversion to UTF-8 not needed in UTF-8 codeset locales (obviously!)*/
	if (strcmp(codeset, "UTF-8") != 0 && ((UTF8ToLoc = iconv_open(codeset, "UTF-8")) == (iconv_t) -1))
		{
			if (UTF8ToLoc != (iconv_t) -1)
			{
				iconv_close(UTF8ToLoc);
				UTF8ToLoc = (iconv_t) -1;
			}
		}
	inbuf = utf8str; inbytesleft = MAX_UTF8_CHAR * MB_LEN_MAX; 
	outbuf = errorstr; outbytesleft = sizeof(errorstr);
	if (iconv(UTF8ToLoc,(char**) &inbuf, &inbytesleft, &outbuf, 
					 &outbytesleft) == -1) 
		 {
		     iconv_close(UTF8ToLoc);
	      	     printf("Unable to convert from utf-8 to local.. Returning utf-8 string\n");
       	     return utf8str;
	        }
	else 
		{
 	   	    iconv_close(UTF8ToLoc);
		    return errorstr;
		}
#endif
}
#endif


