/* $Novell: rebind.c,v 1.18 2003/05/12 12:57:41 $ */
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
   rebind.c 
***************************************************************************
   Description: The rebind.c sample demonstrates the use of a rebind 
                procedure.  This procedure is needed when an application
                needs to bind to another server in order to follow a referral
                or search continuation reference.
***************************************************************************/

#include <stdio.h>
#include <ldap.h>
#include <string.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

#if defined(HAVE_WINSOCK)
#include <winsock.h>
#endif

static int dosearch( LDAP *ld, char *container);

/*
 * Structure to save user credentials for rebind operation
 */
typedef struct rebind_cred {
    char    *dn;
    char    *pw;
} rebind_cred_t;

rebind_cred_t   creds = { NULL, NULL};

/*
 * The rebind procedure - this gets called when the application must
 * perform a bind operation to follow a referral
 */
int LIBCALL dorebind(   LDAP *ld, LDAP_CONST char *url, int request, ber_int_t msgid)
{
    int     rc;
    LDAPURLDesc *srv;

    if( (rc = ldap_url_parse( url, &srv)) != LDAP_SUCCESS) {
        printf("rebind:     parse_url failed, %s\n", ldap_err2string(rc));
        return rc;
    }
    /* Not request is defined in ldap.h - one of LDAP_REQ_XXXXX */
    printf("rebind: in rebind proc: referral - host %s:%d, dn \"%s\", request %d, msgid %d\n",
            srv->lud_host, srv->lud_port, srv->lud_dn, request, msgid);
    if ( creds.pw == NULL) {
        printf("rebind:     simple bind as anonymous\n");
    } else {
        printf("rebind:     simple bind as %s/%s\n", creds.dn, creds.pw);
    }
    rc = ldap_simple_bind_s( ld, creds.dn, creds.pw );
    if ( rc != LDAP_SUCCESS ) {
        printf("rebind: ldap_simple_bind_s: %s\n",ldap_err2string(rc));
    }
    ldap_free_urldesc( srv);
    return rc;
}

void usage()
{
    printf("usage rebind [-f] [-r] [-t] <server> <container> [user_dn passwd]\n");
    printf("\t-f - disable rebind function (enabled if option absent)\n");
    printf("\t-r - disable automatic referral handling (enabled if option absent)\n");
    printf("\t-t - set ldap v2 bind semantics (V3 if option absent)\n");
    return;
}

int main( int argc, char **argv )
{
    LDAP    *ld;
    char    *myContainer;   /* starting container for search */
    int     rc;             /* return code */
    char    *host;          /* Host server */
    int     rfunc = 1;      /* 1 if rebind function enabled (-f option) */
    int     ref = 1;        /* 1 if referral following enabled (-r option) */
    int     ver = 3;        /* LDAP version (-t option sets ver = 2) */
    
    /* Connection timeout.  This applies to both the initial bind
       and the rebind. 
     */
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */
    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif

    /* Determine which options are set */
    if (argc > 1) {
        /* Interrogate options until we get something else */
        while( 1 ) {
            /* -f option disables rebind procedure */
            if( strcmp( argv[1],"-f") ==0) {
                rfunc = 0;
                argc--;argv++;
                continue;
            } else
            /* -r option disables automatic referral handling */
            if( strcmp( argv[1],"-r") ==0) {
                ref = 0;
                argc--;argv++;
                continue;
            } else
            /* -t option sets ldap version 2 bind */
            if( strcmp( argv[1],"-t") ==0) {
                ver = 2;
                argc--;argv++;
                continue;
            } else
            /* An option, but not -f, -r, or -t options */
            if( argv[1][0] == '-') {
                usage();
                return 0;
            }
            break;
        }
    }

    /* Get host and search base arguments */
    if (argc > 2) {
        host = argv[1];
        myContainer = argv[2];
    } else {
        usage();
        return 255;
    }

    /* Get optional userid & passwd arguments */
    if ( argc > 3) {
        creds.dn = argv[3];
        if ( argc > 4) {
            creds.pw = argv[4];
        }
    }

    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &ver);
    printf("rebind: LDAP protocol version %d set\n", ver);


    /* Set the connection timeout. */
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);
    
    /* Referral following is on by default in LDAP SDK.
     * This demonstrates disabling referrals
     */
    if( ! ref ) {
        ldap_set_option( NULL, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);
        printf("rebind: SDK referral following disabled\n");
    } else {
        printf("rebind: SDK referral following enabled\n");
    }

    if( rfunc) {
        /* Register the rebind procedure.
         * Registered for all ldap_init calls after this time
         * dorebind will be called if a bind is required to follow a referral.
         */
        ldap_set_rebind_proc( NULL, &dorebind);
        printf("rebind: rebind procedure enabled\n");
    }

    /* Get a handle to an LDAP session. */
    ld = ldap_init( host, LDAP_PORT );
    if (ld == NULL) {
        printf("ldap_init\n");
        return 255;
    }

    /* Bind to the LDAP server. */
    if( creds.dn == NULL) {
        printf("rebind: Simple bind to server %s as anonymous\n", host);
    } else {
        printf("rebind: Simple bind to server %s as %s/%s\n",
            host, creds.dn, creds.pw);
    }

    rc = ldap_simple_bind_s( ld, creds.dn, creds.pw );
    if ( rc != LDAP_SUCCESS ) {
        printf("rebind: ldap_simple_bind_s: %s\n", ldap_err2string(rc));
        ldap_unbind( ld );
        return 255;
    }

    if( (rc = dosearch( ld, myContainer)) != LDAP_SUCCESS) {
        printf("rebind: Search failed: %s\n", ldap_err2string(rc));
        ldap_unbind( ld );
        return 255;
    }

    ldap_unbind( ld );

    return 0;
}

static int dosearch( LDAP *ld, char *container)
{
    LDAPMessage   *result = NULL;       /* Place to store search result */
    LDAPMessage   *msg = NULL;          /* The message being processed */
    struct timeval timeout = {20,0};    /* 20 second search timeout */

    int       rc;                       /* return code */
    int       counte, countr, countm;   /* Counters to report search results */
    int       i = 0;
    char    * ref;
    char    * dn;
    int       inentry = 0;              /* flag to control printing */
    int       inref = 0;                /* flag to control printing */

    /* The following are used to save the results of a search result entry */
    int            errcode = 0;
    char        *  matcheddn = NULL;
    char        *  errmsg = NULL;
    char        ** referrals = NULL;
    LDAPControl ** servercontrols = NULL;

    /* Now do a search */
    printf("rebind: Start the search beginning with \"%s\"\n", container);
    rc = ldap_search_ext_s(   ld,
                              container,
                              LDAP_SCOPE_SUBTREE,
                              "(objectclass=*)",
                              NULL,
                              0,
                              NULL,
                              NULL,
                              &timeout,
                              LDAP_NO_LIMIT,
                              &result );

    counte = -1;
    countr = -1;
    countm = -1;

    if( result == NULL) {
        /* No result returned, most likely a timeout */
        printf("rebind: ldap_search_ext_s: result is NULL, status = 0x%x\n", rc);
        printf("rebind: ldap_search_ext_s: %s\n", ldap_err2string( rc));
        return rc;
    }

    /* We got something, count what we have */
    counte = ldap_count_entries(ld, result);
    countr = ldap_count_references(ld, result);
    countm = ldap_count_messages(ld, result);

    if ( (rc != LDAP_SUCCESS) && (rc != LDAP_REFERRAL) ) {
        /* Search failed, report error and any entries received */
        if( counte != -1) {
            printf("rebind: ldap_search_ext_s: %s\n", ldap_err2string(rc));
            printf("rebind: Return from search: # entries:  %d,  ", counte);
            printf("# references:  %d,  ", countr);
            printf("# messages:  %d\n", countm);
        }
    }

    /* Note: referral is a status returned on the operation */
    /* You only get the referral status if you disable referral following */
    if( rc == LDAP_REFERRAL) {
        /* We got a referral status, include that in the output */
        printf("rebind: Referral status in result\n" );
    }

    /* Display the number of entries returned from search. */
    printf("rebind: # entries:  %d,  ", counte);
    printf("# references:  %d,  ", countr);
    printf("# messages:  %d\n", countm);

    /* Report the results of the search */
    /* Note: Printf statements only work if object names are ASCII characters */
    for( msg=ldap_first_message(ld, result); msg!=NULL; msg=ldap_next_message(ld, msg)) {
        int type = ldap_msgtype(msg);
        switch( type) {
        case LDAP_RES_SEARCH_REFERENCE:
            /* Note Search Reference is a type of search result */
            /* You only get search references if you disable referral following */
            if ( inref == 0) {
                printf("rebind: Search reference\n");
            }
            inentry = 0;
            inref = 1;
            rc = ldap_parse_reference( ld, msg, &referrals, NULL, 0);
            if( rc != 0) {
                printf("rebind: parse_reference returned error: %s\n",
                    ldap_err2string( rc));
                return rc;
            }
            /* Print out this list of search references */
            if( referrals != NULL) {
                for( i = 0, ref = referrals[0]; ref != NULL;i++) {
                    printf("rebind:     Search reference %s\n", ref);
                    ref = referrals[i+1];
                }
                ldap_value_free( referrals);
            }
            break;
        case LDAP_RES_SEARCH_ENTRY:
            if( inentry == 0) {
                printf("rebind: Search entry\n");
            }
            inref = 0;
            inentry = 1;
            dn = ldap_get_dn(ld, msg);
            if (dn) {
                printf("rebind:     Object %s:\n", dn);
                ldap_memfree(dn);
            }
            break;
        case LDAP_RES_SEARCH_RESULT:
            inentry = 0;
            printf("rebind: Search result\n");
            rc = ldap_parse_result( ld, msg, &errcode, &matcheddn,
                            &errmsg, &referrals, &servercontrols, 0);
            printf("rebind:     Operation errcode = %d: %s\n",
                errcode, ldap_err2string( errcode));
            if( matcheddn != NULL) {
                printf("rebind:     matcheddn = \"%s\"\n", matcheddn);
                ldap_memfree( matcheddn);
            }
            if( errmsg != NULL) {
                printf("rebind:     errmsg = \"%s\"\n", errmsg);
                ldap_memfree( errmsg);
            }
            if( referrals != NULL) {
                while( referrals[i] != NULL) {
                    printf("rebind:     referral[%d]:\"%s\"\n",
                        i, referrals[i]);
                    i++;
                }
                ldap_value_free( referrals);
            }
            if( servercontrols != NULL) {
                printf("rebind:     Server controls = 0x%X\n", servercontrols);
                ldap_controls_free( servercontrols);
            }
            break;
        default:
            printf("rebind: Unknown message type %d\n", type);
            break;
        }

    }  /* Next element */

    ldap_msgfree( result );
    return LDAP_SUCCESS;
}
