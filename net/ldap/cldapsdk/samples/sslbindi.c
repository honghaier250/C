/* $Novell: sslbindi.c,v 1.11 2003/05/12 13:09:42 $ */
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
    sslbindi.c
***************************************************************************
   Description: sslbindi.c demonstrates how to use the
                interactive certificate feature in LDAP SSL. The user code
                defines and registers a certificate verification callback
                routine. This callback is called if an unknown certificate
                is received while doing a connection to an LDAP server. The
                callback routine can retrieve information from the
                unknown certificate via helper functions.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ldap_ssl.h>

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   sslbindi <host name> <port number> <login dn> <password> "
"\n" 
"\n Example: sslbindi Acme.com 636 cn=admin,o=Acme secret\n";

int LIBCALL cert_callback
(
   void *pHandle
);


int main(int argc, char *argv[])
{
    int             rc       = 0;
    int             version  = LDAP_VERSION3;
    int             ldapPort = 0;
    char            answer[256];
	char            extraChar = '\n';
    char           *ldapHost = NULL;
    char           *loginDN  = NULL;
    char           *password = NULL;
    LDAP           *ld       = NULL;
    struct timeval timeOut = {10,0};   /* 10 second connection timeout */
    
    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    /*
     * Check for proper perameters
     */
    if (5 != argc)
    {
        printf("%s", usage);
        exit(1);
    }

    ldapHost     = argv[1];
    ldapPort     = atoi(argv[2]);
    loginDN      = argv[3];
    password     = argv[4];
   

    /* Set LDAP version to 3 and set connection timeout. */
    version = LDAP_VERSION3;
    ldap_set_option( NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option( NULL, LDAP_OPT_NETWORK_TIMEOUT, &timeOut);
    
    /* 
     * Initialize the ssl library - No trusted root certificate file is
     * given to force the call of the verification callback routine
     * during the SSL handshake.  At callback time the application can
     * decide to accept or reject the server's certificate.
     */
    rc = ldapssl_client_init( NULL,   /* Certificate File */ 
                              NULL ); /* reserverd, just use NULL */

    if (rc != LDAP_SUCCESS)
    {
        printf("ldapssl_client_init error: %d\n", rc);
        exit(1);
    }

    /*
     * Register a certificate verification callback routine.  This routine
     * will be called when any untrusted server certificate is received
     * when establishing an SSL connection.
     */
    rc = ldapssl_set_verify_callback(cert_callback);
    
    if (rc != LDAP_SUCCESS)
    {
        printf("ldapssl_set_verify_callback error: %d\n", rc);
        ldapssl_client_deinit();
        exit(1);
    }

    do
    {
       /* 
        * Create an SSL enabled LDAP handle
        */
       ld = ldapssl_init(  ldapHost,       /* host name */
                           ldapPort,       /* port number */
                           1 );            /* 0- clear text, 1-enable for ssl */
   
       if (ld == NULL )
       {
           printf("ldapssl_init error\n" );
           ldapssl_client_deinit();
           exit(1);
       }
   
       /*
        * Bind (connect) to the LDAP server.
        */
       rc = ldap_simple_bind_s( ld, loginDN, password);
      
       if (rc != LDAP_SUCCESS )
       {
           printf("ldap_simple_bind_s error: %d, %s\n", rc, ldap_err2string( rc ));
           ldap_unbind_s( ld );
           ldapssl_client_deinit();
           exit(1);
       }
       
       printf("SSL bind successful\n");
         
       /*
        * Unbind
        */
       ldap_unbind_s( ld );
   
       
       printf("\nWould you like bind again? (Y/N): ");
       fgets(answer,255,stdin);
       
    } while (answer[0] == 'Y' || answer[0] == 'y');
    
    /* 
     * Uninitialize the LDAP ssl library
     */
    ldapssl_client_deinit();
    return 0;
}


/*********************************************************************
*
* This is the certificate verification callback routine.  A 
* verification callback routine must return an int and have one 
* void* parameter.  
*
* The routine should return LDAPSSL_CERT_ACCEPT to continue with
* the SSL connection or LDAPSSL_CERT_REJECT to abort the SSL 
* connection.
*
*********************************************************************/
int LIBCALL cert_callback
(
   void *pHandle
)
{
   int                           rc         = 0;   
   int                           callbackRc = LDAPSSL_CERT_REJECT;
   int                           length     = 0;
   int                           certStatus = 0;   
   char                          answer[256];
   char                         *value      = NULL;
   LDAPSSL_Cert                  cert;
   LDAPSSL_Cert_Validity_Period  certPeriod;
   cert.data = NULL;
   cert.length = 0;

   printf("\n");
   printf("Certificate received.\n");
   printf("Certificate Information:\n");
   
   /*
    * The following functions demonstrate retrieving information
    * from the server certificate.  
    */

   /* ldapssl_get_cert_attribute allows applications to query the 
    * size of a particular attribute by passing a NULL in the 
    * value parameter.  If a NULL is passed in, the length parameter
    * is updated with the length of the data but no actual data is 
    * returned.  This allows applications to query an attribute's size
    * and allocate appropriate memory for the value.
    *
    * NOTE: For attributes that are strings the length returned does not 
    * include the NULL termination character.
    */ 

   /* Certificate Status - status is an int
    */
   
   length = sizeof(certStatus);
   rc = ldapssl_get_cert_attribute( pHandle,                 /* cert Handle */
                                    LDAPSSL_CERT_GET_STATUS, /* desired attribute */
                                    &certStatus,             /* attribute value */
                                    &length );               /* length */
   if (LDAPSSL_SUCCESS != rc)
   {
      printf("ldapssl_get_cert_attribute LDAPSSL_CERT_GET_STATUS failed\n");
      goto err;
   }
   printf("    Status:             ");

   switch (certStatus)
   {
      case UNABLE_TO_GET_ISSUER_CERT:
         printf("unable to get issuer certificate\n");
         break;
      case UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
         printf("unable to decode issuer public key\n");
         break;
      case CERT_SIGNATURE_FAILURE:
         printf("certificate signature failure\n");
         break;
      case CERT_NOT_YET_VALID:
         printf("certificate is not yet valid\n");
         break;
      case CERT_HAS_EXPIRED:
         printf("Certificate has expired\n");
         break;
      case ERROR_IN_CERT_NOT_BEFORE_FIELD:
         printf("format error in certificate's notBefore field\n");
         break;
      case ERROR_IN_CERT_NOT_AFTER_FIELD:
         printf("format error in certificate's notAfter field\n");
         break;
      case DEPTH_ZERO_SELF_SIGNED_CERT:
         printf("self signed certificate\n");
         break;
      case SELF_SIGNED_CERT_IN_CHAIN:
         printf("self signed certificate in certificate chain\n");
         break;
      case UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
         printf("unable to get local issuer certificate\n");
         break;
      case UNABLE_TO_VERIFY_LEAF_SIGNATURE:
         printf("unable to verify the first certificate\n");
         break;
      case INVALID_CA:
         printf("invalid CA certificate\n");
         break;
      case PATH_LENGTH_EXCEEDED:
         printf("path length constraint exceeded\n");
         break;
      case INVALID_PURPOSE:
         printf("unsupported certificate purpose\n");
         break;
      case CERT_UNTRUSTED:
         printf("certificate not trusted\n");
         break;
      case CERT_REJECTED:
         printf("certificate rejected\n");
         break;
      default:
         printf(" number %d\n",certStatus);
         break;
   }

   /* Certificate Issuer - attribute is a string
    */

      /* Query Certificate Issuer length
       */
   rc = ldapssl_get_cert_attribute( pHandle,                 /* cert Handle */
                                    LDAPSSL_CERT_ATTR_ISSUER,/* desired attribute */
                                    NULL,                    /* attribute value */
                                    &length );               /* length */
   if (LDAPSSL_SUCCESS != rc)
   {
      printf("ldapssl_get_cert_attribute LDAPSSL_CERT_ATTR_ISSUER length failed\n");
      goto err;
   }
      /* add 1 for NULL termination
       */
   length += 1;
   value = (char *)malloc(length);
   if(NULL == value)
   {
      printf("Could not allocate LDAPSSL_CERT_ATTR_ISSUER buffer!\n");
      goto err;
   }

      /* Retrieve the Issuer  
       */ 
   rc = ldapssl_get_cert_attribute( pHandle,                 /* cert Handle */      
                                    LDAPSSL_CERT_ATTR_ISSUER,/* desired attribute */
                                    value,                  /* attribute value */  
                                    &length );               /* length */           
   if (LDAPSSL_SUCCESS != rc)
   {
      printf("ldapssl_get_cert_attribute LDAPSSL_CERT_ATTR_ISSUER failed\n");
      goto err;
   }
   printf("    Issuer:             %s\n", value);
   free(value);
   value = NULL;


   /* Certificate Subject - attribute is a string
    */

      /* Query Certificate Subject length
       */
   rc = ldapssl_get_cert_attribute( pHandle,                  /* cert Handle */      
                                    LDAPSSL_CERT_ATTR_SUBJECT,/* desired attribute */ 
                                    NULL,                     /* attribute value */  
                                    &length );                /* length */           
   if (LDAPSSL_SUCCESS != rc)
   {
      printf("ldapssl_get_cert_attribute LDAPSSL_CERT_ATTR_SUBJECT length failed\n");
      goto err;
   }

      /* add 1 for NULL termination
       */
   length += 1;
   value = (char *)malloc(length);
   if(NULL == value)
   {
      printf("Could not allocate LDAPSSL_CERT_ATTR_SUBJECT buffer!\n");
      goto err;
   }
   
      /* Retrieve the Subject 
       */ 
   rc = ldapssl_get_cert_attribute( pHandle,                  /* cert Handle */      
                                    LDAPSSL_CERT_ATTR_SUBJECT,/* desired attribute */
                                    value,                    /* attribute value */  
                                    &length );                /* length */           
   if (LDAPSSL_SUCCESS != rc)
   {
      printf("ldapssl_get_cert_attribute LDAPSSL_CERT_ATTR_SUBJECT failed\n");
      goto err;
   }
   printf("    Subject:            %s\n", value);
   free(value);
   value = NULL;

   /* Certificate validity period - attribute is the struct 
    *    LDAPSSL_CERT_VALIDITY_PERIOD - because the value is of fixed 
    *    size there is no need to initially query for size.
    */

      /* Retrieve validity period.
       */
   rc = ldapssl_get_cert_attribute( pHandle,
                                    LDAPSSL_CERT_ATTR_VALIDITY_PERIOD,
                                    &certPeriod,
                                    &length );
   if (LDAPSSL_SUCCESS != rc)
   {
      printf("ldapssl_get_cert_attribute LDAPSSL_CERT_ATTR_VALIDITY_PERIOD failed\n");
      goto err;
   }
      
      /* The validity period consists of a Not Before and Not After time
       */
   printf("    Not Before Time:    %s Type: %s\n", 
                    certPeriod.notBeforeTime,
                    (certPeriod.notBeforeType == LDAPSSL_CERT_UTC_TIME) ?
                    "UTC Time" : "Generalized Time");

   printf("    Not After Time:     %s Type: %s\n", 
                    certPeriod.notAfterTime,
                    (certPeriod.notAfterType == LDAPSSL_CERT_UTC_TIME) ?
                    "UTC Time" : "Generalized Time");


   /* ldapssl_get_cert allows an application to get the certificate in 
    * a buffer encoded in Base64 or DER format -  Applications could
    * then save the certificate buffer to persistant storage (file, 
    * database, etc ).
    */
  
   
         /* Query the length of the buffer needed by first calling 
          * ldapssl_get_cert with the data field set to NULL (ie cert.data=NULL).
          * Upon return the length field is set to the required size.
          */
   cert.data = NULL;
   rc = ldapssl_get_cert( pHandle,                  /* cert handle */
                          LDAPSSL_CERT_BUFFTYPE_DER,/* desired encoding */ 
                          &cert);                   /* certificate structure */
   if (LDAPSSL_SUCCESS != rc)
   {
      printf("ldapssl_get_cert length failed\n");
      goto err;
   }
  
         /* Allocate necessary memory
          */
   cert.data = (void *)malloc(cert.length);
   if(NULL == cert.data)
   {
     printf("Could not allocate buffer!\n");
     goto err;
   }
   
         /* Retrieve the certificate DER Encoded
          */
   rc = ldapssl_get_cert( pHandle,                  /* cert handle */          
                          LDAPSSL_CERT_BUFFTYPE_DER,/* desired encoding */     
                          &cert);                   /* certificate structure */
   if (LDAPSSL_SUCCESS != rc)
   {
      printf("ldapssl_get_cert failed\n");
      goto err;
   }
   

   /* Prompt to continue and accept the certificate or abort - if
    * the user choses to accept the certificate, ldapssl_add_trusted_cert
    * is used to insert the certificate into the list of trusted 
    * certificates.  After inserting the certificate into the list,
    * it will be trusted and this callback will not be called 
    * to verify the certificate again.
    */      
   printf("\nWould you like to accept the certificate? (Y/N): ");
   fgets(answer,255,stdin);

   if (answer[0] == 'Y' || answer[0] == 'y')
   {
      callbackRc = LDAPSSL_CERT_ACCEPT;

      rc = ldapssl_add_trusted_cert( &cert, LDAPSSL_CERT_BUFFTYPE_DER );
      if (LDAPSSL_SUCCESS != rc)
      {
         printf("ldapssl_add_trusted_cert failed\n");
      }
   }
   else
      callbackRc = LDAPSSL_CERT_REJECT;

err:
   
   fflush(stdin);
      
      /* Free the certificate buffer
       */
   if(NULL != cert.data)
      free(cert.data);
      /* Free value buffer
       */
   if(NULL != value)
      free(value);

   return(callbackRc);
}
