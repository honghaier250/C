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
   lburp.c 
***************************************************************************
   Description: The lburp.c sample program demonstrates LBURP (LDAP 
                BULK Update/Replication Protocol functinality provided
                by eDirectory. 

                The LDAP Bulk Update/Replication Protocol (LBURP) allows an 
                LDAP client to perform a bulk update to a replica on an LDAP 
                server. The protocol frames a sequenced set of update 
                operations within a pair of LDAPv3 extended operation to 
                notify the client that the update operations in the framed 
                set are related.  The update operations within the framed 
                set are LDAPv3 extended operations each encapsulating a 
                sequence number and one or more LDAPv3 update operations.  
                The sequence number allows the server to process the
                update operations in the proper order even when they are sent
                asynchronously by the client, and the update operations can be
                grouped within the extended request to maximize the efficiency 
                of client-server communication.

                This protocol arose from the need to allow LDAP clients to
                efficiently present large quantities of updates to an LDAP 
                server and have the LDAP server efficiently process them. 
                LBURP introduces a minimum of new operational functionality to 
                the LDAP protocol since the update requests sent by the client 
                encapsulate standard LDAP [LDAPv3] update operations. However, 
                this protocol greatly facilitates bulk updates by allowing the 
                supplier to present the update operations asynchronously and 
                still allow the consumer to maintain proper ordering of the 
                operations.

                Refer IETF draft, draft-rharrison-lburp-03.txt
                to understand LBURP Protocol and its functionaltiy.
***************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ldap.h>
#include <ldapx.h>
#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static int lburp_start(LDAP *ld, int *transactionSize);
static int lburp_operation (LDAP *ld, int seqno,
                           LBURPUpdateOperationList **op, int *count);
static int lburp_end(LDAP *ld, int sequenceNumber);


static char usage[] =
"\n Usage:   lburp <host name> <port number> <login dn> <password> "
"<container name>\n"
"\n Example: lburp acme.com 389 cn=admin,o=acme secret ou=lburp,o=acme\n";

int main(int argc, char **argv)
{
    
    int     i, rc, version, count, ldapPort, transactionSize, sequenceNumber=0;
    char    *ldapHost, *loginDN, *password, *containerName, *dn = NULL; 
    char        *vclass[3], *vcn[2], *vsn[2];
    LDAP        *ld;
    LDAPMod     modClass, modCN, modSN, *mods[4];
    LBURPUpdateOperationList **adds = NULL, **deletes = NULL;

    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 6)
    {
        printf("%s", usage);
        return (1);
    }
                     
    ldapHost      =   argv[1];         
    ldapPort      =   atoi(argv[2]);
    loginDN       =   argv[3];
    password      =   argv[4];
    containerName =   argv[5];
        
    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);
   
    /* Initialize the LDAP session */
    if (( ld = ldap_init( ldapHost, ldapPort )) == NULL) {
        printf ( "\n    LDAP session initialization failed\n");
        return( 1 );
    }
    printf ( "\n    LDAP session initialized\n");

    /* Bind to the server */
    rc = ldap_simple_bind_s( ld, loginDN, password );
    if (rc != LDAP_SUCCESS ) {
        printf("ldap_simple_bind_s: %s\n", ldap_err2string( rc ));
        ldap_unbind_s ( ld );
        return( 1 );
    }
    printf("\n    Bind successful\n\n");

    /* call helper function to initiate LBURP Start extended request 
     * operation.  The helper function even parses the LBURP Start 
     * extended response.
     */
    printf("    Initiating LBURP Start extended request ... \n");
    rc = lburp_start(ld, &transactionSize);
    if ( rc != LDAP_SUCCESS )
    {
        printf("LBURP Start failed: %s\n", ldap_err2string ( rc ));
        goto cleanup;
    }
    printf("    LBURP Start extended request successfull.\n\n");
    printf("    LBURP transaction size: %d\n\n", transactionSize);
    
    /* Construct add objects of transaction size to perform LBURP 
     * operation of type add. 
     *
     * To add an entry to the directory:
     *   1. Create the array of LDAPMod structures representing the
     *      attributes of the new entry
     *   2. Specify the DN of the entry to create
     *   3. Call appropirate API to add the entry to the directory
     */

    modClass.mod_op       =   LDAP_MOD_ADD;
    modClass.mod_type     =   "objectclass";
    vclass[0]             =   "top";
    vclass[1]             =   "inetOrgPerson";
    vclass[2]             =   NULL;
    modClass.mod_values   =   vclass;
    modCN.mod_op          =   LDAP_MOD_ADD;
    modCN.mod_type        =   "cn";
    vcn[0]                =   "james";
    vcn[1]                =   NULL;
    modCN.mod_values      =   vcn;
    modSN.mod_op          =   LDAP_MOD_ADD;
    modSN.mod_type        =   "sn";
    vsn[0]                =   "Smith";
    vsn[1]                =   NULL;
    modSN.mod_values      =   vsn;
    mods[0] = &modClass;
    mods[1] = &modCN;
    mods[2] = &modSN;
    mods[3] = NULL;

    adds = (LBURPUpdateOperationList **) 
             malloc(sizeof(LBURPUpdateOperationList)*(transactionSize+1));
    if ( adds == NULL )
    {
        printf("No memory for array of adds\n" );
        goto cleanup;
    }

    for (i=0; i<transactionSize; i++) {
        adds[i] = (LBURPUpdateOperationList *)
                    malloc (sizeof (LBURPUpdateOperationList));
        if ( adds[i] == NULL )
        {
            printf("No memory for array or adds\n");
            goto cleanup;
        }

        /* Construct the dn "cn=lburpuser" with the container name from the
         * command line
         */
        dn=(char*)malloc(strlen("cn=lburpuser,")+2+strlen(containerName)+1);
        sprintf(dn, "cn=lburpuser%d,%s", i, containerName);

        adds[i]->operation = LDAP_REQ_ADD;
        adds[i]->dn = strdup(dn);
        adds[i]->value.attrs = mods;
        adds[i]->ServerControls=NULL;
        adds[i]->ClientControls= NULL;
        free (dn);
    }
    adds[i] = NULL;

    /* 
    * Now add the objects using LBURP Operation request extended call using the
    * helper function. The helper function process LBURP Operation response
    * also.
    */
    printf("    Adding entries using LBURP Operation extended request ...\n");
    /* Assign the sequence number as 1.  First LBUP Operation   */
    ++sequenceNumber;
    rc = lburp_operation(ld, sequenceNumber, adds, &count);
    if ((rc != 80) && ( rc != LDAP_SUCCESS ))
    {
        goto cleanup;
    }

    if ( count == 0)
        printf("    Successfully added entries.\n\n");
    else 
    {
        printf("\n\n    LBURP Addition failed.\n\n");
        goto cleanup;
    }

    /* Construct Delete elements and delete using LBURP Operation request
     * extended call using the helper function.  The function processes the
     * LBURP operation response also. 
     */
    deletes = (LBURPUpdateOperationList **) 
                malloc(sizeof(LBURPUpdateOperationList)*(transactionSize+1));
    if ( deletes == NULL )
    {
        printf("No memory for array of deletes\n" );
        goto cleanup;
    }

    for (i=0; i<transactionSize; i++) {
        deletes[i] = (LBURPUpdateOperationList *) 
                       malloc (sizeof (LBURPUpdateOperationList));
        if ( deletes[i] == NULL )
        {
            printf("No memory for array of deletes\n" );
            goto cleanup;
        }

        dn=(char*)malloc(strlen("cn=lburpuser,") +2+ strlen(containerName) + 1);
        sprintf(dn, "cn=lburpuser%d,%s", i, containerName);

        deletes[i]->operation = LDAP_REQ_DELETE;
        deletes[i]->dn = strdup(dn); 
        deletes[i]->ServerControls=NULL;
        deletes[i]->ClientControls= NULL; 
        free(dn);
    }
    deletes[i] = NULL;

    printf("    Deleting entries using LBURP Operation extended request ...\n");
    /* Assign the sequence number as 2.  Second LBUP Operation   */
    ++sequenceNumber;
    rc = lburp_operation(ld, sequenceNumber, deletes, &count);
    if ( rc != 80 ) {
        printf("ldap_update_entries failed.  Error code = %d\n", rc);
        goto cleanup;
    }
    printf("    Successfully deleted entries.\n\n");

cleanup:

    printf("    Calling LBURP End extended request ...\n");
    /* call LBURP End extended operation */
    /* Assign the sequence number as 3.  Final LBUP Operation, LBURP end  */
    ++sequenceNumber;
    rc = lburp_end(ld, sequenceNumber);
    printf("    LBURP End extended request successfull.\n\n");

    if (adds != NULL) {
        for (i=0; i<transactionSize; i++) { 
            free(adds[i]->dn);
            free(adds[i]);
        }
        free (adds);
    }

    if (deletes != NULL) {
        for (i=0; i<transactionSize; i++) {
            free(deletes[i]->dn);
            free(deletes[i]);
        }
        free (deletes);
    }
   
    ldap_unbind_s(ld);
    return 0;
}


static int lburp_start(LDAP *ld, int *transactionSize)
{
    int msgID, rc, resultCode = 0, done=0;
    char *errorMsg=NULL; 
    LDAPMessage *startResult=NULL;

    struct timeval timeOut = {10,0};   /* 10 second connection timeout */

    /* Perform the LBURP start extended operation */
    rc = ldap_lburp_start_request (ld, &msgID);

    if ( rc != LDAP_SUCCESS ) 
    {
        printf("ldap_lburp_start_request: %s\n", ldap_err2string ( rc ));
        goto exit0;
    }

    /*
     * We call ldap_result with the "all" argument  set to LDAP_MSG_ONE.  
     * This causes ldap_result() to return exactly one  result if at least one 
     * result is available.
     *
     * A timeout value of 10 seconds will cause ldap_result to block for 10 
     * seconds before returning if there are no results.
     */

    while ( !done )
    {
        rc = ldap_result( ld, msgID, LDAP_MSG_ONE, &timeOut, &startResult);

        switch ( rc )
        {
        case -1:   /* some error occurred */
            ldap_get_option(ld, LDAP_OPT_ERROR_NUMBER, &resultCode);
            printf("Error in ldap_result: %s\n", ldap_err2string( resultCode ));
            break;

        case 0:    /* Timed out, no result yet. */
            break;

        case LDAP_RES_EXTENDED: /* A Monitor Events extension failure */
            done = 1;
            rc = ldap_parse_lburp_start_response(ld, 
                                                 startResult, 
                                                 &resultCode, 
                                                 &errorMsg,
                                                 transactionSize,
                                                 0); /* don't free result */
            if (rc != LDAP_SUCCESS)
            {
                printf("Error in ldap_parse_lburp_start_response: %d", rc);
            }
            else
            {
                switch (resultCode)
                {
                case LDAP_OPERATIONS_ERROR:
                    printf("Server operations error: %s\n", errorMsg);
                    break;

                case LDAP_PROTOCOL_ERROR:
                    printf("Protocol error: %s\n", errorMsg);
                    break;

                case LDAP_UNWILLING_TO_PERFORM:
                    printf("Extension is currently disabled\n");
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        } /* end switch on rc */
    }

exit0:
    if ( NULL != startResult )
        ldap_msgfree(startResult);

    if (NULL != errorMsg)
        ldap_memfree(errorMsg);

    if (resultCode != 80)
        return (resultCode);
    else 
        return ( rc  );
}

static int lburp_operation( LDAP *ld, 
             int seqno, 
             LBURPUpdateOperationList **op,
             int *count)
{
    int rc, i, msgID, resultCode = 0, done=0;
    char *errorMsg=NULL;
    LDAPMessage *result=NULL;
    LBURPUpdateResult *lburpResult=NULL;
    struct timeval timeOut = {10,0};   

    rc = ldap_lburp_operation_request(ld, seqno, op, &msgID);

    if (rc != LDAP_SUCCESS) {
        printf("ldap_lburp_operation_request %s\n", ldap_err2string ( rc ));
        goto exit0;
    }

    /*
     * We call ldap_result with the "all" argument  set to LDAP_MSG_ONE.  
     * This causes ldap_result() to return exactly one  result if at least one 
     * result is available.
     *
     * A timeout value of 10 seconds will cause ldap_result to block for 10 
     * seconds before returning if there are no results.
     */

    while ( !done )
    {
        rc = ldap_result( ld, msgID, LDAP_MSG_ONE, &timeOut, &result);

        switch ( rc )
        {
        case -1:   /* some error occurred */
            ldap_get_option(ld, LDAP_OPT_ERROR_NUMBER, &resultCode);
            printf("Error in ldap_result: %s\n", ldap_err2string( resultCode ));
            break;

        case 0:    /* Timed out, no result yet. */
            break;

        case LDAP_RES_EXTENDED: /* A Monitor Events extension failure */
            done = 1;
            rc = ldap_parse_lburp_operation_response ( ld,
                                                       result,
                                                       &resultCode, 
                                                       &errorMsg, 
                                                       count,
                                                       &lburpResult, 
                                                       0);
            if (rc != LDAP_SUCCESS)
            {
                printf("Error in ldap_parse_lburp_end_response: %d\n", rc);
            }
            else
            {
                switch (resultCode)
                {
                case LDAP_OPERATIONS_ERROR:
                    printf("Server operations error: %s\n", errorMsg);
                    break;

                case LDAP_PROTOCOL_ERROR:
                    printf("Protocol error: %s\n", errorMsg);
                    break;

                case LDAP_UNWILLING_TO_PERFORM:
                    printf("Extension is currently disabled\n");
                    break;

                default:
                    break;
                }
            }
            break;
        default:
            break;
        } /* end switch on rc */
    }

    for (i=0; i<*count; i++) {
       if ( lburpResult[i].resultCode != 0)
       printf("    Record Number:%d dn:%s\n Error Code:%d; Error message:%s\n",
               lburpResult[i].packageID, 
               op[i]->dn, 
               lburpResult[i].resultCode, 
               lburpResult[i].errorMsg);
    }

/* free momory */
exit0:
    if (NULL != errorMsg)
        ldap_memfree(errorMsg);

    if (NULL != result )
        ldap_msgfree(result);

    for (i=0; i<*count; i++)
        free(lburpResult[i].errorMsg);
    free(lburpResult);

    if (resultCode != 0)
        return ( resultCode );
    else 
        return ( rc );
}


static int lburp_end(LDAP *ld, int sequenceNumber)
{
    int msgID, rc, resultCode = 0, done=0;
    LDAPMessage *endResult=NULL;
    char *errorMsg = NULL;
    struct timeval timeOut = {10,0};

    /* Perform the LBURP end extended operation */
    rc = ldap_lburp_end_request(ld, sequenceNumber, &msgID);

    if (rc != LDAP_SUCCESS) {
        printf("ldap_lburp_end_request %s\n", ldap_err2string ( rc ));
        goto exit0;
    }

    /*
     * We call ldap_result with the "all" argument  set to LDAP_MSG_ONE.  
     * This causes ldap_result() to return exactly one  result if at least one 
     * result is available.
     *
     * A timeout value of 10 seconds will cause ldap_result to block for 10 
     * seconds before returning if there are no results.
     */

    while ( !done )
    {
        rc = ldap_result( ld, msgID, LDAP_MSG_ONE, &timeOut, &endResult);

        switch ( rc )
        {
        case -1:   /* some error occurred */
            ldap_get_option(ld, LDAP_OPT_ERROR_NUMBER, &resultCode);
            printf("Error in ldap_result: %s\n", ldap_err2string( resultCode ));
            break;

        case 0:    /* Timed out, no result yet. */
            break;

        case LDAP_RES_EXTENDED: /* A Monitor Events extension failure */
            done = 1;
            rc = ldap_parse_lburp_end_response( ld, 
                                                endResult,
                                                &resultCode, 
                                                &errorMsg, 
                                                0); /* don't free result */

            if (rc != LDAP_SUCCESS)
            {
                printf("Error in ldap_parse_lburp_end_response: %d", rc);
            }
            else
            {
                switch (resultCode)
                {
                case LDAP_OPERATIONS_ERROR:
                    printf("Server operations error: %s\n", errorMsg);
                    break;

                case LDAP_PROTOCOL_ERROR:
                    printf("Protocol error: %s\n", errorMsg);
                    break;

                case LDAP_UNWILLING_TO_PERFORM:
                    printf("Extension is currently disabled\n");
                    break;

                default:
                    break;
                }

            }
            break;

        default:
            break;
      
      } /* end switch on rc */
    }

/* Print the errors in case of errors */
exit0:
    if ( NULL != endResult )
        ldap_msgfree(endResult);

    if (NULL != errorMsg)
        ldap_memfree(errorMsg);

    if ( resultCode != 0 )
        return ( resultCode );
    else 
        return ( rc );
}

