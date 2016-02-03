/* $Novell: utf8towc.c,v 1.6 2003/05/12 13:10:49 $ */
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
   utf8towc.c 
***************************************************************************
   Description: Converts UTF-8 characters to or from wide characters.
   
   UTF-8 is an algorithmic encoding of Unicode.
   One 16-bit Unicode character converts to 1, 2, or 3 UTF-8 bytes.
   One 32-bit UCS-4 (basically 32-bit Unicode) character converts into 
   1 to 6 UTF-8 bytes.
   
   An important property of UTF-8 is that ASCII characters (value less than 
   128) have an identical encoding when converted to UTF-8.
   Therefore, local English strings and UTF-8 strings are identical.
   However, to correctly handle accented characters, Japanese characters, etc.,
   LDAP strings should always be encoded in UTF-8.  Generally strings in the
   local code page must be converted to Unicode, then to UTF-8.
       
   On Windows and NetWare wchar_t is 16 bits, equivalent to Unicode encoding.
   On Unix platforms, wchar_t is 32 bits.
    
   The Unicode "Smiley Face" character used in this example has
   the following encoding:
       Wide Char:       0x263A
       UTF-8 encoding:  0xE2 0x98 0xBA
        
   A -1 return value from a function indicates the conversion failed.        
*/

#include <stdio.h>
#include <stdlib.h>             /* For wchar_t definition */
#include <ldap.h>
#include <ldap_utf8.h>

int main(void)
{
  int i,n;
	
  /*---------------- UTF-8 to Wide Character conversions -------------------*/

  {
#define WCSTRLEN    5       /* Size of output buffer in wide chars */
    unsigned char utchr_in[] = { 0xE2, 0x98, 0xBA };
    unsigned char utstr_in[] = { 0xE2, 0x98, 0xBA, 'a', 'b', 'c', 0 };
    wchar_t wcchr_out;
    wchar_t wcstr_out[WCSTRLEN];

    /* Convert a UTF-8 character to a wide character. 
       Returns wcchr_out = 0x263A.
       Returns n = 3.  (Byte length of utchr_in)
    */
    n = ldap_x_utf8_to_wc(&wcchr_out, (char *)utchr_in);   

	printf("\nldap_x_utf8_to_wc.     UTF-8 char in:     %02X  %02X  %02X\n", 
		   utchr_in[0], utchr_in[1], utchr_in[2]);
	printf("Return value = %d.      Wide char out:     %04X\n",n, wcchr_out);
    
	
	/* Convert a UTF-8 string to a wide char string.
       The 3rd arg, WCSTRLEN, is the size of the output buffer in wide chars.
       Returns wcstr_out = { 0x263A, 'a', 'b', 'c', 0 }
       Returns n = 4.  ( # wide chars written, excl null )
       If n==WCSTRLEN, the string could not completely fit in the given buffer
       and may not be null-terminated.
    */
    n = ldap_x_utf8s_to_wcs(wcstr_out, (char *)utstr_in, WCSTRLEN);

	printf("\nldap_x_utf8s_to_wcs.   UTF-8 string in: ");
	for(i=0; i<sizeof(utstr_in); i++)
		printf("  %02X", utstr_in[i]);
	printf("\nReturn value = %d.      Wide string out: ",n);
	for(i=0; i<=n; i++)
		printf("  %04X", wcstr_out[i]);
  }



  /*---------------- Wide Character to UTF-8 conversions -------------------*/

  {
#define UTFSTRLEN   10       /* Size of output buffer in bytes. */
    wchar_t wc_in = 0x263A;
    wchar_t wcstr_in[] = { 0x263A, 'a', 'b', 'c', 0 };
    unsigned char utchr_out[LDAP_MAX_UTF8_LEN];      /* Either 3 or 6 bytes */
    unsigned char utstr_out[UTFSTRLEN];

    /* Convert a wide character to a UTF-8 character.
       The 3rd argument, LDAP_MAX_UTF8_LEN, is defined in ldap_utf8.h as 
         either 3 or 6 bytes depending on the platform.
         It's the largest possible size for a single UTF-8 character in bytes. 
       Returns utchr_out[] = { 0xE2, 0x98, 0xBA }
       Returns n = 3.  (Byte length of utchr_out)
    */
    n = ldap_x_wc_to_utf8((char*)utchr_out, wc_in, LDAP_MAX_UTF8_LEN); 

	printf("\n\nldap_x_wc_to_utf8.     Wide char in:      %04X\n", wc_in);
	printf("Return value = %d.      UTF-8 char out:  ",n);
	for(i=0; i<n; i++)
		printf("  %02X", utchr_out[i]);

    
	/* Convert a wide char string to a UTF-8 string.
       The 3rd arg, UTFSTRLEN, is the size of the output buffer in bytes.
       Returns utstr = { 0xE2, 0x98, 0xBA, 'a', 'b', 'c', 0 }
       Returns n = 6.  ( # bytes written, excl null )
       If n==UTFSTRLEN, the string could not completely fit in the given buffer
       and may not be null-terminated.
    */
    n = ldap_x_wcs_to_utf8s((char*)utstr_out, wcstr_in, UTFSTRLEN);

	printf("\n\nldap_x_wcs_to_utf8s.   Wide string in:  ");
	for(i=0; i<sizeof(wcstr_in)/sizeof(wchar_t); i++)
		printf("  %04X", wcstr_in[i]);
	printf("\nReturn value = %d.      UTF-8 string out:",n);
	for(i=0; i<=n; i++)
		printf("  %02X", utstr_out[i]);
  }
 	return 0;
}
