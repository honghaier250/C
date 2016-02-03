/* $Novell: mkcontainer.c,v 1.10 2003/05/12 12:24:18 $ */
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
   mkcontainer.c 
***************************************************************************
   Description:  mkcontainer.c shows how to modify the 'inetOrgPerson' object
                 class definition. By default, in NetIQ eDirectory schema,
                 'inetOrgPerson' is a leaf object class with the containment
                 flag of "X-NDS_NOT_CONTAINER '1'". Running this sample 
                 removes this flag from the 'inetOrgPerson' object class 
                 schema definition, and thus changes all user objects of 
                 'inetOrgPerson' object class from leaf objects to container
                 objects.
 
                 WARNING: THIS MODIFICATION IS NOT REVERSIBLE!
                 
                 Just because you can do something, does not always mean you
                 should. Keep in mind when making this change that there 
                 could be unforeseen issues caused by this action. You should
                 not make this change in an mixed-tree environment that has 
                 NDS versions prior to NDS8. Also, running the option in 
                 DSRepair (to restore the operational schema) once this change
                 is made, could cause problems. Once you make this change, it
                 is NOT reversible.
   
                 Please look at TID 102504 on http://developer.novell.com 
                 before attempting to use this sample code.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ldap.h> 
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif
 
static char usage[] =
"\n Usage:   mkcontainer <host name> <port number> <login dn> <password>"
"\n Example: mkcontainer Acme.com 389 cn=admin,o=Acme secret\n";


int main(int argc, char *argv[])
{         
    int   version, ldapPort, rc; 
    char  *ldapHost, *loginDN, *password, ch;
    char  *flagValue[2] = {"0", NULL};
    LDAP  *ld;
    LDAPSchema    *schema = NULL;
    LDAPSchemaMod containerFlag, *mod[2]; 
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */
    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 5){
        printf("%s", usage);
        return(1);
    }
    
    ldapHost = argv[1];
    ldapPort = atoi( argv[2] );
    loginDN  = argv[3];
    password = argv[4];


    printf("    WARNING: THIS MODIFICATION IS NOT REVERSIBLE!\n" 
                            "Do you want to continue(N/Y)?");
    scanf("%c", &ch);

    if ( (ch != 'y') && (ch != 'Y'))
        return (0);

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n    LDAP session initialization failed\n");
        return (1);
    }
    printf ( "\n    LDAP session initialized\n");       
     
    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS ) {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s (ld);
        return( 1 );
    }
    printf("\n    Bind successful\n");        
    
    /* populate LDAPSchemaMod structure */
    containerFlag.fieldName = "X-NDS_NOT_CONTAINER";
    containerFlag.values = flagValue;
    containerFlag.op = LDAP_MOD_REPLACE;
    mod[0] = &containerFlag;        
    mod[1] = NULL;             
   
    rc = ldap_schema_fetch(ld, &schema, NULL);
    if (rc != LDAP_SUCCESS ) {
        printf("ldap_schema_fetch: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }

    rc = ldap_schema_modify( 
                  schema,                   /* LDAP schema handle */
                  "inetOrgPerson",          /* name to modify */ 
                  LDAP_SCHEMA_OBJECT_CLASS, /* modification type */  
                  mod);                     /* LDAPSchemaMod structure */ 
    
    if (rc != LDAP_SUCCESS ) {
        printf("ldap_schema_modify: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        ldap_schema_free(schema);
        return( 1 );
    }

    rc = ldap_schema_save(ld, schema, NULL);
    if (rc != LDAP_SUCCESS ) {
        printf("ldap_schema_save: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        ldap_schema_free(schema);
        return( 1 );
    }

    printf("'inetOrgPerson' object class was modified to be a" 
                                 " container object class\n");
    ldap_unbind(ld);
    return 0;
} 



