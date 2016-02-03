/* $Novell: getprivlist.c,v 1.16 2009/03/31 13:16:08 $ */
/**************************************************************************
*  Novell Software Developer Kit
*
*  Copyright (C) 2008-2009 Novell, Inc. All Rights Reserved.
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
   getprivlist.c 
***************************************************************************
   Description: The getprivlist.c sample returns the effective rights for
                the specified entry (subjectDN) on the specified object
		(targetDN) for the specified list of attributes.
***************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#include <ldapx.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

#define MAX_SCHEMA_NAME_CHARS   32
#define MAX_SCHEMA_NAME_BYTES   (2*(MAX_SCHEMA_NAME_CHARS + 1))

static char usage[] =
"\n Usage:   getprivlist <host name> <port number> <login dn> <password>" 
"\n\t  <target dn> <subject dn> <target attribute names list>\n"
"\n Example: getprivlist acme.com 389 cn=admin,o=acme secret"
"\n\t  ou=sales,o=acme cn=James,ou=sales,o=acme cn sn loginScript groupMembership \n";

int main(int argc, char **argv)
{
	int	rc, version, ldapPort = 0, iLoop = 0, numAttrsRequested, numAttrsReturned = 0;
	char	*ldapHost = NULL;
	char	*loginDN = NULL;
	char	*password = NULL;
	char	*targetDN = NULL;
	char	*subjectDN = NULL;
	char	**targetAttrs = NULL;
	int	*privileges = NULL;
	char	*msg = NULL;
	LDAP	*ld = NULL;
	struct timeval timeOut = {10,0};   /* 10 second connection timeout */

	#if defined(N_PLAT_NLM) && defined(LIBC)
	setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
	#endif
    
	if (argc < 8) {
		printf("%s", usage);
		return(1);
	}

	numAttrsRequested = argc - 7;

	ldapHost   =  argv[1];         
	ldapPort   =  atoi(argv[2]);
	loginDN    =  argv[3];
	password   =  argv[4];
	targetDN   =  argv[5];
	subjectDN  =  argv[6];

	targetAttrs = (char **) malloc(((argc - 7) + 1) * (sizeof(char *)));
	if(targetAttrs == NULL)
	{
		printf( "ERROR: Insufficient memory. Exiting...\n" );
		rc = LDAP_NO_MEMORY;
		goto cleanup;
	}

	for( iLoop = 7; iLoop < argc; iLoop++)
		targetAttrs[iLoop - 7] = argv[iLoop];
	targetAttrs[numAttrsRequested] = NULL;

	privileges = (int *) malloc((numAttrsRequested + 1) * (sizeof(int)));
	if (privileges == NULL) {
		rc = LDAP_NO_MEMORY;
		goto cleanup;
	}

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
	* of a subject upon an entry's attributes.
	*/
	printf( "    Calling ldap_get_effective_privileges_list...\n" );
	rc = ldap_get_effective_privileges_list(
						ld,				/* LDAP session handle */
						subjectDN,			/* subject dn */
						targetDN, 			/* object dn */
						targetAttrs,			/* attribute names */
						privileges);			/* privileges */

	if ( rc != LDAP_SUCCESS ) {
		/* print out LDAP server message */         
		if(ldap_get_option(ld, LDAP_OPT_ERROR_STRING, &msg ) == LDAP_OPT_SUCCESS ) {
			if ( (msg != NULL) && (strlen(msg) != 0) ) {
				printf("\n    LDAP server message: %s", msg);                
				ldap_memfree(msg);
			}
		}

		/* print out LDAP error message */
		printf("    LDAP error  message: ldap_get_effective_" 
			"privileges: %s (%d)\n", ldap_err2string( rc ), rc);
		printf("\n    ldap_get_effective_privileges_list failed\n\n");
		ldap_unbind_s( ld );
		return( 1 );
	}
            
	printf("\n    ldap_get_effective_privileges_list succeeded\n\n");

	printf("\tSubject:				%s\n", subjectDN);
	printf("\tTarget:    				%s\n", targetDN);

	for(iLoop = 0; iLoop < numAttrsRequested; iLoop++) {
		printf("\n\tAttribute: %s", targetAttrs[iLoop]);

		printf("\n\tPrivileges: 0x%08x", privileges[iLoop]);
	
		if ( privileges[iLoop] & LDAP_DS_ATTR_COMPARE )
			printf("\n\tSubject has compare rights to the attribute.");
		else
			printf("\n\tSubject has NO compare rights to the attribute.");
	
		if ( privileges[iLoop]& LDAP_DS_ATTR_READ )
			printf("\n\tSubject has read rights to the attribute."); 
		else
			printf("\n\tSubject has NO read rights to the attribute."); 

		if ( privileges[iLoop] & LDAP_DS_ATTR_WRITE )
			printf("\n\tSubject has write rights to the attribute."); 
		else
			printf("\n\tSubject has NO write rights to the attribute."); 
   	 
		if ( privileges[iLoop] & LDAP_DS_ATTR_SELF )
			printf("\n\tSubject can add or delete its name as a value "
				"of the attribute.");
		else
			printf("\n\tSubject CANNOT add or delete its name as a value "
				"of the attribute.");

		if ( privileges[iLoop] & LDAP_DS_ATTR_SUPERVISOR )
			printf("\n\tSubject has all rights to the the object's attributes");
		else
			printf("\n\tSubject does not have all rights to the the object's attributes");

		if ( privileges[iLoop] & LDAP_DS_ATTR_INHERIT_CTL )
			printf("\n\tSubject inherits the rights granted in the ACL.");
		else
			printf("\n\tSubject does not inherit the rights granted in the ACL.\n");
	}

cleanup:
	if(targetAttrs)
		free(targetAttrs);

	if(privileges)
		free(privileges);
	if(ld)
		ldap_unbind_s( ld );
	return 0;
}
