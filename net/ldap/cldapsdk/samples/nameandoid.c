/* $Novell: nameandoid.c,v 1.13 2003/05/12 12:56:33 $ */
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
   nameandoid.c 
***************************************************************************
   Description:  nameandoid.c shows how to get an oid from its name or get 
                 a name from its oid. A single NetIQ eDirectory syntax may
                 have multiple matches of LDAP syntaxes. nameandoid.c does
                 a table lookup. Extensions and controls supported by Novell
                 eDirectory are also included in the table.
 
                 The names and oids are defined in:
                   1. LDAP RFC 2252: Lightweight Directory Access Protocol(v3)
                   2. Internet Draft: draft-sermersheim-nds-ldap-schema-**.txt
                   3. Internet Draft: draft-khan-ldapext-replica-mgmt-**.txt
                   4. Internet Draft: draft-rharrision-lburp-**.txt
                   5. Internet Draft: draft-smith-psearch-ldap-**.txt
                   6. Internet Draft: draft-ietf-ldapext-ldapv3-vlv-**.txt


   sample runs:  $ nameandoid SYN_STREAM
                 
                   oid: 1.3.6.1.4.1.1466.115.121.1.5
                   Descriptive name: Binary
                   NetIQ eDirectory syntax name: SYN_STREAM
                   Reference: RFC 2252: Lightweight Directory Access Protocol(v3)                  

                 $ nameandoid "Directory String"
 
                   oid: 1.3.6.1.4.1.1466.115.121.1.15
                   Descriptive name: Directory String
                   NetIQ eDirectory syntax name: SYN_CI_STRING
                   Reference: RFC 2252: Lightweight Directory Access Protocol(v3)

                 $ nameandoid "Add Replica Request"

                   oid: 2.16.840.1.113719.1.27.100.7
                   Descriptive name: Add Replica Request
                   Reference: Internet Draft: draft-khan-ldapext-replica-mgmt-**.txt

                 $ nameandoid "Persistent Search"

                   oid: 2.16.840.1.113730.3.4.3
                   Descriptive name: Persistent Search
                   Reference: Internet Draft: draft-ietf-ldapext-psearch-**.txt

                 $ nameandoid "1.3.6.1.4.1.1466.115.121.1.1"

                   oid: 1.3.6.1.4.1.1466.115.121.1.1
                   Descriptive name: ACI Item
                   NetIQ eDirectory syntax name: SYN_OCTET_STRING
                   Reference: RFC 2252: Lightweight Directory Access Protocol(v3)

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ldap_oids.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

void printNameAndOID(int);

static char usage[] =
"\n Usage:   nameandoid <name/OID> "
"\n Example: nameandoid \"SYN_STREAM\""
"\n or"
"\n Example: nameandoid \"Directory String\""
"\n or"
"\n Example: nameandoid \"ADD_REPLICA_REQ\""
"\n or"
"\n Example: nameandoid \"Persistent Search\""
"\n or"
"\n Example: nameandoid \"1.3.6.1.4.1.1466.115.121.1.1\""
"\n"
"\n Note:    Name can be a descriptive name, a NetIQ eDirectory"
"\n          syntax name, a control name, or a extension name"
"\n          supported by NetIQ eDirectory\n";

int main( int argc, char **argv)
{
    int  i, type, found = 0;
    size_t len;
    char *value;
    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif

    if (argc != 2) {
        printf("%s", usage);
        return(1);
    }

    value = argv[1];
    len = strlen(value);

    if (len == 0) {
        printf("%s", usage);
        return(1);
    }
    else {      
        for (i = 0; i < (int)len; i++) {
            if ( !isdigit(value[i]) && (value[i]!='.')) {
                type = 0;
                break;
            }
            else
                type = 1;                       
        }
        
        if ( type == 0) {     /* from name to oid */
            for ( i = 0; i < TOTAL_LEN; i++ ) {
                if (  (strcmp( value, namesandoids[i][1]) == 0) 
                    ||(strcmp( value, namesandoids[i][2]) == 0)) {
                        found = 1;
                        printNameAndOID(i);
                }
            }
        }
        else { /* from oid to name */
            for ( i = 0; i < TOTAL_LEN; i++ ) {
                if ( strcmp( value, namesandoids[i][3]) == 0 ) {
                    found = 1;
                    printNameAndOID(i);
                }
            }
        }            

        if(!found)      
            printf("Unknown %s%s\n", type? "oid: ":"name: ", value);
    }   

    return( 0 );
}

void printNameAndOID( int index) {        
    char *dName = namesandoids[index][1];
    char *sName = namesandoids[index][2];    
    char *oid   = namesandoids[index][3];
    char *ref   = namesandoids[index][4];
        
    if (*oid)   printf("oid: %s\n", oid);
    if (*dName) printf("Descriptive name: %s\n", dName);
    if (*sName)  printf("NetIQ eDirectory syntax name: %s\n", sName );           
    if (*ref)   printf("Reference: %s\n", ref );    
    printf("\n");
}

