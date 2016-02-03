/* $Novell: addentry1.c,v 1.10 2003/05/12 12:10:40 $ */
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
    addentry1.c
***************************************************************************
   Description: The addentry1.c sample adds an entry ( cn=james smith ) 
                with its mandatory attributes to a specified container.
                This version dynamically allocates the memory used for 
                input data.  
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static void mods_free( LDAPMod **mods, int freemods );
static void memvfree( void **vec );

static char usage[] =
"\n Usage:   addentry1 <host name> <port number> <login dn> <password>"
"\n\t  <container name> \n" 
"\n Example: addentry1 Acme.com 389 cn=admin,o=Acme secret"
" ou=Sales,o=Acme\n";

int main( int argc, char **argv)
{
    int      i, version, ldapPort, rc, modNumber = 7; 
    char     *ldapHost;
    char     *loginDN;
    char     *password; 
    char     *containerName;
    char     *dn;        
    LDAP     *ld;
    LDAPMod  **mods; 
    struct timeval timeOut = {10,0};    /* 10 second connection timeout */



    char **objectclassValues = NULL;     
    char **commonnameValues  = NULL;
    char **givennameValues   = NULL;
    char **surnameValues     = NULL;
    char **phonenumberValues = NULL;
    char **mailValues        = NULL;
    char **passValues        = NULL;

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
        printf("%s", usage);
        return(1);
    }

    ldapHost          = argv[1];
    ldapPort          = atoi(argv[2]);
    loginDN           = argv[3];
    password          = argv[4];
    containerName     = argv[5];

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);

    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL)
    {
        printf ( "\n\tLDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n\tLDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS )
    {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("\n\tBind successful\n");

    /*
    * To add an entry to the directory:
    *   1. Create the array of LDAPMod structures representing the
    *      attributes of the newentry 
    *   2. Specify the dn of the entry to create
    *   3. Call ldap_add_ext_s to add the entry to the directory
    */
    mods = ( LDAPMod ** ) malloc(( modNumber + 1 ) * sizeof( LDAPMod * ));

    if ( mods == NULL )
    {
        printf("No memory for array of mods\n" );
        ldap_unbind_s( ld );
        return( 1 );
    }

    for ( i = 0; i < modNumber; i++ )
    {
        if (( mods[ i ] = ( LDAPMod * ) malloc( sizeof( LDAPMod ))) == NULL )
        {
            printf("No memory for mods element\n" );
            ldap_unbind_s( ld );
            return( 1 );
        }
    }

    /*
    * This example shows complete dynamic allocation of the LDAPMod
    * structures, as may be the case when the application is receiving
    * entry data from an external source.
    *
    * Note: data allocated by the application must be freed
    * by application functions, not LDAP library functions.
    *
    * Note: data allocated by the LDAP library must be freed
    * by LDAP library functions.
    */
    objectclassValues = (char**)malloc(sizeof(char*)*(1+1)); 
    commonnameValues  = (char**)malloc(sizeof(char*)*(3+1));
    givennameValues   = (char**)malloc(sizeof(char*)*(3+1));
    surnameValues     = (char**)malloc(sizeof(char*)*(1+1));
    phonenumberValues = (char**)malloc(sizeof(char*)*(1+1));
    mailValues        = (char**)malloc(sizeof(char*)*(1+1));
    passValues        = (char**)malloc(sizeof(char*)*(1+1));

    objectclassValues[0] = strdup("inetOrgPerson");
    objectclassValues[1] = NULL;
    commonnameValues[0]  = strdup("james");
    commonnameValues[1]  = strdup("James Smith");
    commonnameValues[2]  = strdup("Jimmy Smith");
    commonnameValues[3]  = NULL;
    givennameValues[0]   = strdup("James");
    givennameValues[1]   = strdup("Jim");
    givennameValues[2]   = strdup("Jimmy");
    givennameValues[3]   = NULL;
    surnameValues[0]     = strdup("Smith");
    surnameValues[1]     = NULL;
    phonenumberValues[0] = strdup("1 801 555 1212");
    phonenumberValues[1] = NULL;
    mailValues[0]        = strdup("JSmith@Acme.com");
    mailValues[1]        = NULL;
    passValues[0]        = strdup("newpassword");
    passValues[1]        = NULL;

    mods[0]->mod_op       = LDAP_MOD_ADD;
    mods[0]->mod_type     = strdup("objectclass");
    mods[0]->mod_values   = objectclassValues;
    mods[1]->mod_op       = LDAP_MOD_ADD;
    mods[1]->mod_type     = strdup("cn");
    mods[1]->mod_values   = commonnameValues;
    mods[2]->mod_op       = LDAP_MOD_ADD;
    mods[2]->mod_type     = strdup("givenname");
    mods[2]->mod_values   = givennameValues;
    mods[3]->mod_op       = LDAP_MOD_ADD;
    mods[3]->mod_type     = strdup("sn");
    mods[3]->mod_values   = surnameValues;
    mods[4]->mod_op       = LDAP_MOD_ADD;
    mods[4]->mod_type     = strdup("telephonenumber");
    mods[4]->mod_values   = phonenumberValues;
    mods[5]->mod_op       = LDAP_MOD_ADD;
    mods[5]->mod_type     = strdup("mail");
    mods[5]->mod_values   = mailValues;
    mods[6]->mod_op       = LDAP_MOD_ADD;
    mods[6]->mod_type     = strdup("userPassword");
    mods[6]->mod_values   = passValues;
    mods[7]               = NULL;

    dn=(char*)malloc(strlen("cn=james,") + strlen(containerName));
    strcpy( dn, "cn=james," );
    strcat( dn, containerName );

    /*
     * Now add the object
     */
    rc = ldap_add_ext_s( ld,         /* LDAP session handle */
                         dn,         /* dn of the object to be added */
                         mods,       /* array of add options */
                         NULL,       /* server controls */
                         NULL );     /* client controls */

    if ( rc != LDAP_SUCCESS )
    {
        printf("ldap_add_ext_s: %s\n", ldap_err2string( rc ));
        mods_free( mods, 1 );
        free( dn );
        ldap_unbind_s ( ld );
        return(1);
    }
    printf("\n\tEntry added successfully: %s\n", dn );

    mods_free( mods, 1 );
    free( dn );
    ldap_unbind_s( ld );

    return(0);
}

/*
 * free a null-terminated array of pointers to mod structures. the
 * structures are freed, not the array itself, unless the freemods
 * flag is set.
 */

static void
mods_free( LDAPMod **mods, int freemods )
{
    int i;

    if ( mods == NULL )
        return;

    for ( i = 0; mods[i] != NULL; i++ )
    {
        /*
         * Free is the same whether bvalues or values,
         * because all buffers were alloced by the app, not by sdk.
         */
        if ( mods[i]->mod_values != NULL )
            memvfree( (void**) (mods[i]->mod_values) );
        
        if ( mods[i]->mod_type != NULL )
            free( mods[i]->mod_type );
        
        free( (char *) mods[i] );
    }
    
    if ( freemods )
        free( (char *) mods );
    
    return;
}

/*
 * free a null-terminated array of mod values
 */
static void
memvfree( void **vec )
{
    int i;

    if ( vec == NULL )
        return;
    
    for ( i = 0; vec[i] != NULL; i++ )
        free( vec[i] );
    
    free( vec);
    
    return;
}
