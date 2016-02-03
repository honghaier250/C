/* $Novell:*/
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
   ldaptrigger.c
***************************************************************************
   Description: 
   eDirectory background processes run automatically at periodic intervals 
   to keep different replicas in an eDirectory tree synchronized.  
   Sometimes it may be desirable to initiate one or more of these processes 
   manually rather than waiting for the next scheduled execution.

   The ldaptrigger.c sample demonstrates how to schedule eDirectory
   background processes:


    Back Linker - Keeps the "backlink" attribute of objects synchronized
    between servers.  This attribute keeps track of external references
    to the object.

    Janitor - Cleans up bindery information, refreshes server status, and
    updates inherited ACLs.

    Limber - Maintains tree connectivity, making sure tree names are
    consistent among servers.  Also caches information from the NCPServer
    object and index information.  After creating a new index, an application
    may want to kick off the limber process to cause it to start creating the
    index immediately.

    Skulker -  Replicates and synchronizes directory data among replicas.
    A developer may want to initiate this process to start data synchronization
    immediately.

    Schema Sync - Replicates and synchronizes the schema.

    Partition Purge - Purges deleted entries and values that have been
    synchronized with all replicas, and synchronizes external references.

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#include <ldapx.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[]=
"\n Usage: ldaptrigger <host name> <port number> <login dn> <password> "
"\n\t<background trigger type>"
"\n Example: ldaptrigger Acme.com 389 cn=admin,o=novell secret 1"
"\n\t  <background trigger type>:  1 - back linker"
"\n\t                              2 - janitor"
"\n\t                              3 - limber"
"\n\t                              4 - skulker" 
"\n\t                              5 - schema sync"
"\n\t                              6 - partition purge\n";

int main (int argc, char **argv)
{
    int     rc, version;
    int     ldapPort;
    char    *ldapHost;
    char    *loginDN;
    char    *password;
    int     triggerType;
    char    *msg;
    LDAP    *ld;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
         printf("%s", usage);
         return (1);
    }

    ldapHost        = argv[1];
    ldapPort        = atoi(argv[2]);
    loginDN         = argv[3];
    password        = argv[4];
    triggerType     = atoi(argv[5]);

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    printf( "Initializing the LDAP session handle ... \n");
    if ((ld = ldap_init( ldapHost, ldapPort )) == NULL )
    {
         printf ( "\tLDAP session initialization failed\n");
         return ( 1 );
    }

    /* Authenticate to the server */
    printf("Binding to the directory ... \n");
    rc = ldap_simple_bind_s (ld, loginDN, password);
    if ( rc != LDAP_SUCCESS )
    {
         printf( "ldap_simple_bind_s: %s\n", ldap_err2string(rc));
         ldap_unbind_s (ld);
         return ( rc );
    }

    printf("Calling ldap_trigger_back_process ...\n");
    switch (triggerType)
    {
    case LDAP_BK_PROCESS_BKLINKER :
         printf("Initiating Back linker process ...\n");
         rc = ldap_trigger_back_process(ld, LDAP_BK_PROCESS_BKLINKER);
         break;
    case LDAP_BK_PROCESS_JANITOR :
         printf("Initiating Janitor process ...\n");
         rc = ldap_trigger_back_process(ld, LDAP_BK_PROCESS_JANITOR);
         break;
    case LDAP_BK_PROCESS_LIMBER :
         printf("Initiating Limber process ...\n");
         rc = ldap_trigger_back_process(ld, LDAP_BK_PROCESS_LIMBER);
         break;
    case LDAP_BK_PROCESS_SKULKER : 
         printf("Initiating Skulker process ...\n");
         rc = ldap_trigger_back_process(ld, LDAP_BK_PROCESS_SKULKER);
         break;
    case LDAP_BK_PROCESS_SCHEMA_SYNC : 
         printf("Initiating Schema Sync process ... \n");
         rc = ldap_trigger_back_process(ld, LDAP_BK_PROCESS_SCHEMA_SYNC);
         break;
    case LDAP_BK_PROCESS_PART_PURGE :
         printf("Initiating Purge process ... \n");
         rc = ldap_trigger_back_process(ld, LDAP_BK_PROCESS_PART_PURGE);
         break;
    default :
         printf("Invalid trigger type.\n");
         ldap_unbind_s(ld);
         return LDAP_PARAM_ERROR;
    }

    if ( rc != LDAP_SUCCESS )
    {
        /* print out LDAP server message */         
        if(ldap_get_option (ld, LDAP_OPT_ERROR_STRING, &msg ) == 
                                                      LDAP_OPT_SUCCESS ) {
            if ( (msg != NULL) && (strlen(msg) != 0) )
             {
                printf("\n    LDAP server message: %s", msg);
                ldap_memfree(msg);
             }
        }
        /* print out LDAP error message */
        printf("    LDAP error  message: ldap_trigger_back_process:" 
                                     " %s (%d)\n", ldap_err2string( rc ), rc);
        
        printf("\n    ldap_trigger_back_process failed\n\n");        
        ldap_unbind_s(ld);
        return (rc);
    }

    printf("The background process was successfully initiated.\n\n");
    ldap_unbind_s(ld);
    return 0;
}
