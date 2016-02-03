/* $Novell: connpool.c,v 1.3 2003/05/12 12:17:01 $ */
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
   connpool.c 
***************************************************************************
   Description: Sample implementation of a load-balancing LDAP connection pool.
   
                10 functions are defined to allow a connection pool to be
                created and destroyed, and to manage connection requests
                from a caller.
                
                A test program is included to show an example of calling
                connection pool functions.
                
                More detailed documentation on the use of connection pools
                and the function calls is included after the test program.

***************************************************************************/

/* ====================== Start of test program  ==========================*/

#include "connpool.h"
#include <stdio.h>
#include <ldap.h>
#include <stdlib.h>

#if defined(N_PLAT_NLM) && defined(LIBC)
#include <screen.h>
#endif

static char usage[] =
"\n Usage:   connpool <hosts> <port> <login dn> <password>"
"\n                   <min Conns> <max Conns> <test Conns>\n" 
"\n Example: connpool Acme1.com 389 cn=admin,o=Acme secret"
" 2 5 11\n"
"\n     This test will create a pool with 2 hosts, but for convenience"
"\n     the 2nd host will use the same host, port, loginDN, and password."
"\n     The number of min and max conns are PER HOST."
"\n     'testConns' is how many total connections this test will ask for.\n\n";

int main(int argc, char** argv)
{
    char *ldapHost, *loginDN, *password;
    int ldapPort, minConns, maxConns, testConns;
    LDAPConnectionPool *pool;
    int rc, i, version;
    struct timeval tv = {5,0};   /* 5 second timeout on binding */
    LDAP** ldArray;
    LDAPHostPool *badHostPool=NULL;
    char *badHostName=NULL;

    #if defined(N_PLAT_NLM) && defined(LIBC)
    setscreenmode(SCR_NO_MODE);              /* Don't clear screen on exit */
    #endif
    
    if (argc != 8)
    {
        printf("%s", usage);
        return (1);
    }
    ldapHost  = argv[1];
    ldapPort  = atoi(argv[2]);
    loginDN   = argv[3];
    password  = argv[4];
    minConns  = atoi(argv[5]);  /* Create pool with these initial connections */
    maxConns  = atoi(argv[6]);  /* Pool is allowed to grow to this size */
    testConns = atoi(argv[7]);  /* # of test connections to request from pool */ 
    
    /* Set default options for connections in this pool. */
    version=LDAP_VERSION3;
    ldap_set_option(NULL, LDAP_OPT_PROTOCOL_VERSION, &version);
    ldap_set_option(NULL, LDAP_OPT_NETWORK_TIMEOUT, &tv);

    /* Allocate array for test connection handles */
    ldArray = (LDAP**)calloc(testConns, sizeof(LDAP*));
    if (ldArray == NULL)
        return(LDAP_NO_MEMORY);

    
/* Create the pool with one host. */
    printf("\nCreating pool...\n");
    rc = cpoolCreate(
            minConns,           /* Initial # of connections per host */
            maxConns,           /* Maximum # of conns per host */
            ldapHost,           /* Host name */
            ldapPort,           /* Port number */
            loginDN,            /* DN to use for binding */
            password,           /* Password to use for binding */
            NULL,               /* User bind callback function */
            &pool);             /* (out) Returned pool handle */

    printf("  cpoolCreate handle=0%x rc=%d (%s)\n", 
           pool, rc, ldap_err2string(rc));
    if (rc != LDAP_SUCCESS) {
        free(ldArray);
        return(rc);
    }


/* Add an additional host.  Usually this would be a different server, 
 * possibly with a different DN, password, and callback.
 * For testing, we're using the same server but will consider it to
 * be a separate host.
 */
    printf("\nAdding a 2nd host...\n");
    rc = cpoolAddHost(
            pool,               /* Pool handle */
            ldapHost,           /* Host name */
            ldapPort,           /* Port number */
            loginDN,            /* DN to use for binding */
            password,           /* Password to use for binding */
            NULL);              /* User bind callback function */

    printf("  cpoolAddHost rc=%d (%s)\n", rc, ldap_err2string(rc));


/* Ask for some connections. Store the returned ld handles in an array. */    
    printf("\nGet some connections...\n");
    for (i=0; i<testConns; i++) {
        rc = cpoolGetConnection(pool, &ldArray[i]);
        printf("  Connection %d:  0x%x   rc=%d (%s)\n", 
               i, ldArray[i], rc, ldap_err2string(rc));
    }


/* Suppose we want to know which host a particular connection is from. 
 * Ask for host information from our first connection. 
 */
    printf("\nFind host information for connection ld=0x%x...\n", ldArray[0]);
    rc = cpoolGetHost(pool, ldArray[0], &badHostPool, &badHostName);
    if (rc)
        printf("  cpoolGetHost returned %d (%s)\n", rc,ldap_err2string(rc));
    else
        printf("  Connection 0x%x is managed by hostPool 0x%x  (%s)\n",
               ldArray[0], badHostPool, badHostName);


/* Suppose we did a search operation and got a timeout error.
 * We believe the connection has been dropped for some reason.
 * Return this connection to the pool as a 'bad' connection
 * so that it won't subsequently be handed out in this state.
 */       
    printf("\nReturn the first connection as 'bad'...\n");
    rc = cpoolResetBadConnection(pool, ldArray[0]);
    if (rc) {
        printf("  cpoolResetBadConnection returned %d (%s)\n", 
               rc, ldap_err2string(rc));
    }
    else {
        printf("  Connection %d returned to pool in UNBOUND state:"
               "  0x%x   rc=%d (%s)\n", 
               0, ldArray[0], rc, ldap_err2string(rc));
    }

    
/* We suspect one of our hosts has gone down. We may want to temporarily 
 * disable its pool so that no more connections are handed out from this host.
 */
    printf("\nDisabling hostPool 0x%x (%s)...\n", badHostPool, badHostName);
    rc = cpoolDisableHost(pool, badHostPool);
    if (rc) {
        printf("  cpoolDisableHost returned %d (%s)\n", rc,ldap_err2string(rc));
    }
    else {
        printf("  This host's pool is now disabled from handing out "
               "connections.\n");
    }
    

/* Return the connections to the pool for reuse. Since we already returned
   the first connection, the function should return an error when we
   try to return it again. */
    printf("\nReturn the connections...(the first one should be bad)\n");
    for (i=0; i<testConns; i++) {
        rc = cpoolReturnConnection(pool, ldArray[i]);
        printf("  Returned connection %d:  0x%x   rc=%d (%s)\n",
               i, ldArray[i], rc, ldap_err2string(rc));
    }

/* We can also remove a particular host's connections and data
 * structures completely. 
 */
    printf("\nRemoving one hostPool 0x%x (%s)...\n", badHostPool, badHostName);
    rc = cpoolRemoveHost(pool, badHostPool);
    printf("  HostPool removed.  rc=%d (%s)\n", rc, ldap_err2string(rc));


/*  Destroy the entire connection pool.  
 *  Unbinds all connections and frees all data structures.
 *  Applications should make sure no threads are still using connections
 *  before making this call.
 */
    printf("\nDestroying pool...\n");    
    cpoolDestroy(pool);

    printf("  Pool destroyed\n");
    free(ldArray);
    return (0);
}
/* ========================== End of test stub ==============================*/


/****************************************************************************
 *              Connection Pool Sample implementation.
 * 
 * Since creating and tearing down connections is a time-consuming operation,
 * many applications use a connection pool to improve efficiency.
 * A connection pool will initially create and bind a cache of connections.
 * When an application needs a connection, one is obtained from the pool.
 * The application returns it to the pool when done, where it can be reused.
 *
 * This sample implements a connection pool supporting one or multiple hosts
 * operating as peers.  If there are multiple hosts, the connection pool 
 * attempts to balance the load when handing out connections.
 * 
 *   Functions to implement a basic LDAP connection pool.
 *
 *     cpoolCreate              - Creates and binds a pool of LDAP connections.
 *     cpoolGetConnection       - Obtains a bound connection from the pool.
 *     cpoolReturnConnection    - Returns a connection to the pool.
 *     cpoolResetBadConnection  - Returns a bad connection to the pool.  
 *     cpoolDestroy             - Unbinds all the connections and frees 
 *                                memory structures.
 *
 *   The following functions are useful when multiple hosts participate
 *   in the pool.
 *
 *     cpoolAddHost      - Adds an additional load-balancing host to the pool.
 *     cpoolRemoveHost   - Removes a host and its connections from the pool.
 *     cpoolGetHost      - Get host name and hostPool handle for a connection.
 *     cpoolEnableHost   - Enable connections to be handed out from a hostPool.
 *     cpoolDisableHost  - Disable connections from being handed out from 
 *                         a particular hostPool.
 *
 * The main LDAPConnectionPool structure contains one or more LDAPHostPool
 * structures, one for each host.  A 'hostPool' mentioned in this doc
 * refers to the LDAPHostPool structure and its associated connections.
 * 
 * All connections in the pool are in one of three states:
 *   INUSE    - The connection has been given out to an application.
 *   READY    - The connection has been bound and is ready to give out.
 *   UNBOUND  - The handle has not been initialized or bound.  If there are no
 *              READY connections, this handle can be bound and given out.
 *
 * The pool is protected by a mutex, so multiple threads may make simultaneous
 * pool requests.
 * 
 * When building this sample, you should define one of the following compiler
 * symbols to indicate your platform.  (Needed for mutex calls.)
 *    
 *     Windows:            WIN32
 *     NetWare/LIBC:       N_PLAT_NLM and LIBC
 *     NetWare/CLIB:       N_PLAT_NLM
 *     Linux/Solaris/AIX:  UNIX
 *
 *     For single-threaded apps on any platform:   
 *          There is no need for mutex calls.  
 *          Just remove the #error line in connpool.h.
 *
 *
 * WARNING:  Creating connections to a server takes server resources.
 *           An application which blindly fires up many more connections
 *           than it really needs will put an unnecessary burden on the
 *           system and waste resources.
 *
 * Also note that an option exists on eDirectory servers on how long to
 * wait before the server kills an idle connection.  If this setting has
 * been changed from its default 0 (infinite), care must be taken that
 * the server doesn't drop connections that have been established in the pool.
 *  
 * A number of design decisions must be made in implementing a connection pool.
 * Developers may want to modify or refine the choices made in this 
 * implementation. Some of these issues are:
 * 
 * 1.  Binding methods.
 *         We allow a user callback function to bind, or do a simple bind if
 *         one was not specified.
 * 2.  Load balancing algorithm.
 *         We choose the server with the least number of in-use connections.
 *         In case of a tie, we choose the server with the most number of
 *         ready connections.
 * 3.  Handling a server that goes down.
 *         The application must implement a strategy for disabling,
 *         enabling, or removing the host.
 * 4.  Handling idle connection timeout, if applicable.
 *         One could have a thread that periodically performs some activity
 *         on idle connections, or just closes them itself if they've been
 *         idle too long.
 * 5.  Blocking mode for cpoolGetConnection.
 *         This implementation returns an error if no connections are
 *         available.  An implementor may instead wish to block until
 *         one becomes available.
 *
 ***************************************************************************/


#include "connpool.h"
#include <ldap.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
static int lockAndValidatePool (LDAPConnectionPool *pool);

/*************************************************************************** 
createConnectionPool
  Initializes structures and creates the pool of bound connections.
     
    minConns -       Starting # of connections (per host)
    maxConns -       Max # of connections (per host)
    host     -       Host name.  May be followed by colon and port number.
    port     -       Port number to use for connections (if not specified with
                     host name)
    loginDN  -       Login DN to use
    password -       Password to use
    bindCallback -   If present, this user function is called to perform the 
                     LDAP init and bind.  
                   
                     If NULL, a simple bind will be performed using the given
                     loginDN and password.
                     
                     The callback function allows the application to create SSL
                     connections, or to bind in any other way it chooses.
                     
                     The bindCallback function passes these parameters:
                        int bindCallback ( host, port, loginDN, password )
                     
    pool       (out) If successful, returns a handle to the connection pool
                     which will be used in subsequent calls.  Otherwise, the 
                     pool handle is set to NULL.                     

  Return code:  0 for success.  LDAP error code for failure.
                (Server down, timeout, memory allocation error, etc.)
                -1 for mutex error.
                
An array of 'maxConns' session handles is allocated.  'minConns' of them 
are initialized and bound, ready to hand out.  If no ready handles are 
available when requested, the number of bound handles is allowed to grow 
up to 'maxConns'.
    
 ***************************************************************************/
int cpoolCreate (
    int  minConns,     
    int  maxConns,     
    char *host,        
    int  port,         
    char *loginDN,      
    char *password,     
    BIND_CALLBACK *bindCallback,
    LDAPConnectionPool **poolHandle
)
{
    int rc;
    LDAPConnectionPool *pool;

    /* Check parameters. */
    *poolHandle = NULL;
    if (minConns<0 || maxConns<1 || maxConns<minConns)  
        return LDAP_PARAM_ERROR;


    /* Allocate and initialize the main LDAPConnectionPool structure. */
    pool = (LDAPConnectionPool *)calloc(1, sizeof(LDAPConnectionPool));
    if (pool == NULL)
        return LDAP_NO_MEMORY;

    pool->maxConns = maxConns;
    pool->minConns = minConns;
    pool->valid = CPOOL_VALID;
    
    /* Allocate the mutex which will control this pool */
    rc = CP_mutex_init(&pool->mutex);   /* Error code -1 for mutex failure */
    if (rc != 0) {
        free(pool);
        return rc;
    }

    /* Add the initial host. */
    rc = cpoolAddHost(pool, host, port, loginDN, password, bindCallback);
    if (rc == 0) {
        
        /* Host has been successfully added and all initial connections bound.  
         * Return the pool handle. 
         */
        *poolHandle = pool;
        return LDAP_SUCCESS;

    }
    
    
    /* Error condition occurred.  Clean up partially created pool. */
    cpoolDestroy(pool);
    return rc;
}


/*************************************************************************** 
  cpoolAddHost - Add a new host as a load-sharing peer.
     
    pool     -       Handle to the connection pool created by cpoolCreate().
    host     -       Host name.  May be followed by colon and port number.
    port     -       Port number to use for connections (if not specified with
                     host name)
    loginDN  -       Login DN to use
    password -       Password to use
    bindCallback -   If present, this user function is called to perform the 
                     LDAP init and bind.  
                   
                     If NULL, a simple bind will be performed using the given
                     loginDN and password.
                     
                     The callback function allows the application to create SSL
                     connections, or to bind in any other way it chooses.
                     
                     The bindCallback function passes these parameters:
                        int bindCallback ( host, port, loginDN, password )
                     
    Return code:  0 for success.  LDAP error code for failure.
                  (Server down, timeout, memory allocation error, etc.)
                  
Adds an additional set of connections for a different host.  
Creates and initializes a new LDAPHostPool structure, with up to 'maxConns'
connections.  'minConns' of them are initially bound.
     
 ***************************************************************************/
int cpoolAddHost (
    LDAPConnectionPool *pool,
    char *host,        
    int  port,         
    char *loginDN,      
    char *password,     
    BIND_CALLBACK *bindCallback
)
{
    int n, i, rc = LDAP_NO_MEMORY;
    void *p;
    LDAPHostPool* hp = NULL;

    /* Lock mutex and check pool validity. */    
    if (lockAndValidatePool(pool) != LDAP_SUCCESS)
        return LDAP_PARAM_ERROR;
    
    /* Create or enlarge the null-terminated array of pointers to 
     * LDAPHostPool structs, one pointer for each host. 
     * Realloc size is the # of existing hosts + 1 new + 1 Null ptr.
     */
    p =  realloc(pool->hp, (pool->numhosts+2) * sizeof(LDAPHostPool**));
    if (p == NULL)
        goto error;
    pool->hp = (LDAPHostPool**)p;
    pool->hp[ pool->numhosts+1 ] = NULL;      /* Null terminator */

    n = pool->numhosts;     /* n = Index of the host we're adding */
    pool->numhosts++;

    /* Allocate and init the LDAPHostPool structure. */
    pool->hp[n] = (LDAPHostPool *)calloc(1, sizeof(LDAPHostPool));
    hp = pool->hp[n];       /* Temp ptr to the new LDAPHostPool struct */
    if (hp == NULL)
        goto error;
    
    if (host == NULL || *host == 0)  
        host = "localhost";    
    hp->host = strdup(host);
    if (hp->host == NULL)
        goto error;
    
    if (port == 0)  
        port = LDAP_PORT;
    hp->port = port;

    /* Store loginDN and password for subsequent binds.
     * If storing the password this way is a security risk for 
     * your application, you may want to handle this differently.
     * i.e. Remove the code to store the password, and don't send
     * the password as a parameter in the callback function.
     */
    if (loginDN) {
        hp->loginDN = strdup(loginDN);
        if (hp->loginDN == NULL)
            goto error;
    }
    if (password) {
        hp->password = strdup(password);
        if (hp->password == NULL)
            goto error;
    }
    hp->bindCallback = bindCallback;

    /* Array of ld handles */
    hp->ld = (LDAP **)calloc(pool->maxConns, sizeof(LDAP *));
    if (hp->ld == NULL )
        goto error;

    /* Array of status values */
    hp->status = (int *)calloc(pool->maxConns, sizeof(int));
    if (hp->status == NULL )
        goto error;

    hp->unbound = pool->maxConns;

    
    /* Now init and bind 'minConns' connections for this host. */
    for (i=0; i<pool->minConns; i++) {

        LDAP *ld;
        
        if (bindCallback) {     /* Callback function supplied */

            rc = (*bindCallback)(host, port, loginDN, password, &ld);
            if (rc != LDAP_SUCCESS)
                goto error;
        
        } else {                /* No callback function, do simple bind. */

            ld = ldap_init(host, port );
            if (ld == NULL) {
                rc = LDAP_NO_MEMORY;
                goto error;
            }
            rc = ldap_simple_bind_s(ld, loginDN, password);
            if (rc != LDAP_SUCCESS) {
                ldap_unbind(ld);
                goto error;
            }
        }

        hp->ld[i] = ld;
        hp->status[i] = CPOOL_READY;
        hp->ready++;
        hp->unbound--;

    }  /* end for i<minConns */
    
    CP_mutex_unlock(&pool->mutex);
    return LDAP_SUCCESS;

    /* Error condition occurred.  Clean up partially created host pool. */
error:
    CP_mutex_unlock(&pool->mutex);
    cpoolRemoveHost(pool, hp);
    return rc;
}


/*************************************************************************** 
  cpoolRemoveHost() - Remove a host and all its connections from the pool.          
 
    pool     -       Handle to the connection pool created by cpoolCreate().
    hostPool -       A pointer to the specific host's LDAPHostPool structure.
                     This pointer is obtained with cpoolGetHost().
                         
  Return code:  0 for success.  LDAP error code for failure.
                  
Remove an entire host from the pool.   All the hosts connections are unbound,
and the data structures freed.  This call might be used if it is determined
that a host server has gone down, and won't be coming back.
Note that cpoolDestroy() removes all hosts; it is not necessary for the
application to remove them itself when shutting down.
 ***************************************************************************/
int cpoolRemoveHost (
    LDAPConnectionPool *pool,
    LDAPHostPool *hp
)
{
    int i,n;

    if (hp == NULL)  
        return LDAP_PARAM_ERROR;
    
    /* Lock mutex and check pool validity. */    
    if (lockAndValidatePool(pool) != LDAP_SUCCESS)
        return LDAP_PARAM_ERROR;
    
    /* Find this entry in the array of pointers to LDAPHostPool structs. */
    for (n=0; pool->hp[n]; n++) {
        if (pool->hp[n] == hp)      /* Is this the one we're looking for? */
            break;
    }

    /* If the pool does not contain this host in its list, 
     * don't try to remove it.  Another thread may have already done so.
     */
    if (pool->hp[n] == NULL) {
        CP_mutex_unlock(&pool->mutex);
        return LDAP_NO_SUCH_OBJECT;
    }


    /* Unbind all bound connections. */
    for (i=0; i<pool->maxConns; i++) {

        if (hp->status[i] != CPOOL_UNBOUND)
            ldap_unbind(hp->ld[i]);
    }

    /* Free the LDAPHostPool structure and the memory it holds. */
    free(hp->status);      /* Free the status array */
    free(hp->ld);          /* Free the ld handle array */
    free(hp->host);        /* Free the host string */
    free(hp->loginDN);     /* Free the login DN string */
    free(hp->password);    /* Free the password string */
    free(hp);              /* Free the LDAPHostPool structure itself */


    /* Remove this pointer from the list by shifting the rest down. */
    for (i=n; pool->hp[i]; i++)
        pool->hp[i] = pool->hp[i+1];

    pool->numhosts--;       /* If we removed it from list, decrement. */

    CP_mutex_unlock(&pool->mutex);
    return LDAP_SUCCESS;
}


/*************************************************************************** 
  cpoolGetConnection()  -  Obtains a bound connection from the pool.
     
    pool  -  Handle to the connection pool created by cpoolCreate().
    pld   -  (out) The returned connection.  (LDAP session handle) 
     
  Returns 0 for success, LDAP error code if failure.
    
  If a connection is ready it is returned immediately.  
  If there are unbound connections, a connection is bound and returned.  
  If a bind failure occurs, the LDAP error code is returned.
  If all connections are in use, LDAP_CONSTRAINT_VIOLATION is returned.
     
     
  This function performs some simple load balancing by choosing a 
  connection from the host with the least number of in-use pool connections.
  The load balancing code may be customized by the implementor.
  
  The mutex is released while the actual bind takes place, allowing other
  threads to access the pool during this time.
  
 ***************************************************************************/

int cpoolGetConnection (
    LDAPConnectionPool *pool,     /* Pool handle */
    LDAP **pld                    /* (out)  LDAP session handle */
)
{
    int n;              /* Best host to use */
    int lowest_inuse;   /* Best host's in-use count */
    int highest_ready;  /* Best host's ready count */
    int i = 0, rc = 0;
    int bind_in_progress = 0;
    LDAPHostPool *hp;

    *pld = NULL;        /* Make sure we return NULL session handle if error */
    /* Lock mutex and check pool validity. */    
    if (lockAndValidatePool(pool) != LDAP_SUCCESS)
        return LDAP_PARAM_ERROR;
    
    /*---------------- Start Load Balancing Algorithm -----------------------
     * 
     * Many different strategies are possible for load balancing.
     * In this implementation, we choose the enabled host with the smallest
     * number of in-use connections.  In case of a tie, we choose the
     * host with the highest number of ready connections.
     * 
     * The result of this algorithm is 'n', the index of the host to use.
     * Range 0 to (numhosts-1).
     * If there are no viable hosts, n is set to numhosts.
     */

    /* Find the first enabled host */
    for (n=0; pool->hp[n]; n++) {
        if (pool->hp[n]->disabled == 0)     /* Is host enabled? */
            break;
    }
    
    if (n < pool->numhosts) {   /* Were there any viable hosts? */

        /* Consider this host the best one so far */
        lowest_inuse = pool->hp[n]->inuse;
        highest_ready = pool->hp[n]->ready;
    
        /* Loop through the remaining enabled hosts looking for a better one */
        for (i=n+1; pool->hp[i]; i++) {

            if (pool->hp[i]->disabled)      /* If host is disabled, skip it. */
                break;
    
            /* Is the in-use count the same or better? */
            if (pool->hp[i]->inuse <= lowest_inuse) {
    
                if( pool->hp[i]->inuse == lowest_inuse) {  /* In case of tie */
                    
                    /* If ready count is lower, skip it. */
                    if (pool->hp[i]->ready <= highest_ready)
                        continue;
                }
        
                /* Host i is better.  Switch to it. */
                n=i;
                lowest_inuse = pool->hp[i]->inuse;
                highest_ready = pool->hp[i]->ready;
            }
        }
    }
    /*----------------- End Load Balancing Algorithm ----------------------*/

    
    if (n >= pool->numhosts)        /* No viable hosts?  Jump out. */
        goto error;

    /* We have chosen host 'n' as the one from which to obtain a connection.
     * If this host's pool has any ready connections, find the first one. 
     */
    hp = pool->hp[n];               /* Save ptr to this LDAPHostPool */
    if (hp->ready > 0) {
    
        for (i=0; i<pool->maxConns; i++) {
    
            if (hp->status[i] == CPOOL_READY) {
                
                /* We have a connection ready to hand out.  Mark it inuse. */
                hp->status[i] = CPOOL_INUSE;
                hp->ready--;
                hp->inuse++;
    
                *pld = hp->ld[i];
                
                CP_mutex_unlock(&pool->mutex);
                return LDAP_SUCCESS;
            }
        }
    }

    /* If no ready connections, find the first unbound connection. */
    rc = LDAP_CONSTRAINT_VIOLATION;     /* If all slots in use */
    if (hp->unbound > 0) {

        for (i=0; i<pool->maxConns; i++) {

            if (hp->status[i] == CPOOL_UNBOUND) {

                /* We have a slot in which to create a connection. */
                LDAP *ld;

                /* Mark this connection as INUSE now, so we can 
                   release the mutex while binding. */
                bind_in_progress = 1;
                hp->status[i] = CPOOL_INUSE;
                hp->unbound--;
                hp->inuse++;
                CP_mutex_unlock(&pool->mutex);

                if (hp->bindCallback) {     /* Callback function supplied */
    
                    rc = (*(hp->bindCallback))(hp->host, 
                                               hp->port, 
                                               hp->loginDN, 
                                               hp->password, 
                                               &ld);
                    if (rc != LDAP_SUCCESS)
                        break;
                
                } else {          /* No callback function, do simple bind. */
    
                    ld = ldap_init(hp->host, hp->port );
                    if (ld == NULL) {
                        rc = LDAP_NO_MEMORY;
                        break;
                    }
                    rc = ldap_simple_bind_s(ld, hp->loginDN, hp->password);
                    if (rc != LDAP_SUCCESS) {
                        ldap_unbind(ld);
                        break;
                    }
                }

                /* An unbound connection has been successfully bound. */
                *pld = hp->ld[i] = ld;
                
                return LDAP_SUCCESS;
            }
        }  /* end for */
    }   /* end if unbound > 0 */

    
    /* Error condition.  No slots available, or bind error occurred.
     * If an error occurred during bind, we must mark the connection unbound
     * and adjust the totals. 
     */
error:    
    if (bind_in_progress) {  /* If bind error, pool is unlocked. */
        CP_mutex_lock(&pool->mutex);   /* Must lock pool to adjust totals */
        hp->status[i] = CPOOL_UNBOUND;
        hp->unbound++;
        hp->inuse--;
    }
    
    CP_mutex_unlock(&pool->mutex);
    return rc;
}


/*************************************************************************** 
  cpoolReturnConnection()  -  Returns a connection to the pool.  
 
    pool  -  Handle to the connection pool created by cpoolCreate().
    ld   -   The connection being returned.
     
  Returns 0 for success, LDAP error code if failure.
    
  The connection is returned to the particular host's pool from
  which it came, and is set to the READY state.
  
  If the connection is not in the pool, LDAP_NO_SUCH_OBJECT is returned.
  If the connection has already been returned or is for some reason not in
  the INUSE state, LDAP_PARAM_ERROR is returned.
  
  If the application modifies options in the session handle via 
  ldap_set_option, it should set them back to the default state before 
  returning the handle.  Alternatively, this function could could be 
  changed to reset all options when the handle is returned.
 ***************************************************************************/

int cpoolReturnConnection (
    LDAPConnectionPool *pool,     /* Pool handle */
    LDAP *ld                      /* LDAP session handle being returned */
)
{
    int n, i;
    LDAPHostPool *hp;
    int rc = LDAP_PARAM_ERROR;

    /* Lock mutex and check pool validity. */    
    if (lockAndValidatePool(pool) != LDAP_SUCCESS)
        return LDAP_PARAM_ERROR;

    /* Note:  An application may have modified some of the default 
     *        options on this session handle, such as search time
     *        limit or referral handling.  An implementer may choose
     *        to reset some or all options back to default values here.
     *        This implementation does not attempt to reset options.
     */       

    /* Find the ld session handle in the pool */
    for (n=0; pool->hp[n]; n++) {          /* Loop through each host */        
        hp = pool->hp[n];        
        for (i=0; i<pool->maxConns; i++) {      /* Loop through each conn */
            if (hp->ld[i] == ld) {  /* Is this the one we're looking for? */

                /* Found it.  We expect it to be in the INUSE state.
                 * Change it to the READY state. 
                 */
                if (hp->status[i] == CPOOL_INUSE) {
                    hp->status[i] = CPOOL_READY;
                    hp->ready++;
                    hp->inuse--;
                    rc = LDAP_SUCCESS;
                }
                
                CP_mutex_unlock(&pool->mutex);
                return rc;
            }
        }  /* end for each connection */
    }  /* end for each host */

    /* They've handed us a session handle that's not in the pool. */
    CP_mutex_unlock(&pool->mutex);
    return LDAP_NO_SUCH_OBJECT;
}


/*************************************************************************** 
  cpoolResetBadConnection()  -  Unbinds and returns a connection to the pool.  
    
    pool  -  Handle to the connection pool created by cpoolCreate().
    ld   -   The connection being returned.
    
    Returns 0 for success, LDAP error code if failure.
    
    If a connection has been dropped or is otherwise bad, we don't want it to 
    go back into the pool and be handed out again in this state.  This 
    function calls unbind on the session handle and marks it as UNBOUND.  
    If needed later, the connection will be rebound before being handed out.  
    
    If the connection is not in the pool, LDAP_NO_SUCH_OBJECT is returned.
    If the connection has already been returned or is for some reason not in
    the INUSE state, LDAP_PARAM_ERROR is returned.
  
 ***************************************************************************/

int cpoolResetBadConnection (
    LDAPConnectionPool *pool,     /* Pool handle */
    LDAP *ld                      /* Bad LDAP session handle being returned */
)
{
    int n, i;
    LDAPHostPool *hp;
    int rc = LDAP_PARAM_ERROR;

    /* Lock mutex and check pool validity. */    
    if (lockAndValidatePool(pool) != LDAP_SUCCESS)
        return LDAP_PARAM_ERROR;

    /* Find the ld session handle in the pool */
    for (n=0; pool->hp[n]; n++) {          /* Loop through each host */        
        hp = pool->hp[n];        
        for (i=0; i<pool->maxConns; i++) {      /* Loop through each conn */
            if (hp->ld[i] == ld) {  /* Is this the one we're looking for? */

                /* Found it.  We expect it to be in the INUSE state.
                 * Unbind and change it to the UNBOUND state. 
                 */
                if (hp->status[i] == CPOOL_INUSE) {
                    ldap_unbind(hp->ld[i]);
                    hp->status[i] = CPOOL_UNBOUND;
                    hp->ld[i] = NULL;
                    hp->unbound++;
                    hp->inuse--;
                    rc = LDAP_SUCCESS;
                }

                CP_mutex_unlock(&pool->mutex);
                return rc;
            }
        }
    }

    /* The handle was not in the pool. */
    CP_mutex_unlock(&pool->mutex);
    return LDAP_NO_SUCH_OBJECT;
}


/*************************************************************************** 
  cpoolGetHost()  -  Get the hostPool pointer and host name 
                     corresponding to a particular connection.
    
    pool     -  Handle to the connection pool created by cpoolCreate().
    ld       -  The connection for which host information is desired.
    hostPool -  (out) Returns a pointer to the LDAPHostPool which manages this
                      connection.
    hostname -  (out) Returns a pointer to the host name.
    
  Returns 0 for success, LDAP error code if failure.
    
  The hostPool pointer may be subsequently used to enable, disable, or 
  remove a particular hosts pool of connections.
  
  Do not modify or free the contents of the returned pointers.  
 
 ***************************************************************************/
int cpoolGetHost (
    LDAPConnectionPool *pool,     /* Pool handle */
    LDAP *ld,                     /* LD handle to look for */
    LDAPHostPool **hostPool,      /* (out) Returned handle to the hostPool */
    char **hostname               /* (out) Returned pointer to host name. */
)
{
    int n, i;
    LDAPHostPool *hp;

    /* Lock mutex and check pool validity. */    
    if (lockAndValidatePool(pool) != LDAP_SUCCESS)
        return LDAP_PARAM_ERROR;

    /* Find the ld session handle in the pool */
    for (n=0; pool->hp[n]; n++) {          /* Loop through each host */        
        hp = pool->hp[n];        
        for (i=0; i<pool->maxConns; i++) {      /* Loop through each conn */
            if (hp->ld[i] == ld) {  /* Is this the one we're looking for? */

                /* Found it. Return pointers. */
                if (hostPool)
                    *hostPool = hp;
                if (hostname)
                    *hostname = hp->host;
                CP_mutex_unlock(&pool->mutex);
                return LDAP_SUCCESS;
            }
        }
    }

    /* The handle was not in the pool. */
    CP_mutex_unlock(&pool->mutex);
    return LDAP_NO_SUCH_OBJECT;
}

/*************************************************************************** 
  cpoolEnableHost()  -  Enables a host to give out connections.
 
    pool     -  Handle to the connection pool created by cpoolCreate().
    hostPool -  Pointer to the LDAPHostPool structure of the host pool
                to enable. 
 
  Returns 0 for success, LDAP error code if failure.
  
  Note that if a host has gone down, all connections will be lost.
  Enabling and disabling the host won't restore the connections.
  The host must be removed and then added, establishing a different
  set of connections.
 
 ***************************************************************************/

int cpoolEnableHost (
    LDAPConnectionPool *pool,      /* Pool handle */
    LDAPHostPool *hp               /* HostPool handle */
)
{
    /* Lock mutex and check pool validity. */    
    if (lockAndValidatePool(pool) != LDAP_SUCCESS)
        return LDAP_PARAM_ERROR;

    hp->disabled = 0;             /* Set this host to the enabled state */
    
    CP_mutex_unlock(&pool->mutex);
    return LDAP_SUCCESS;
}


/*************************************************************************** 
  cpoolDisableHost()  -  Disables a host from handing out connections.
    
    pool     -  Handle to the connection pool created by cpoolCreate().
    hostPool -  Pointer to the LDAPHostPool structure of the host pool
                to disable. 
    
  Returns 0 for success, LDAP error code if failure.
    
  Connections can still be returned to the host, 
  and existing connections are not disturbed.
  
  An application that determines a host has gone down could disable the host
  while signalling threads and waiting for them to return their connections
  to the bad server.
  
 ***************************************************************************/

int cpoolDisableHost (
    LDAPConnectionPool *pool,      /* Pool handle */
    LDAPHostPool *hp               /* HostPool handle */
)
{
    /* Lock mutex and check pool validity. */    
    if (lockAndValidatePool(pool) != LDAP_SUCCESS)
        return LDAP_PARAM_ERROR;

    hp->disabled = 1;              /* Set this host to the disabled state */
    
    CP_mutex_unlock(&pool->mutex);
    return LDAP_SUCCESS;
}


/*************************************************************************** 
  cpoolDestroy - Unbinds all the connections and frees the memory structures.  
 
    pool     -  Handle to the connection pool created by cpoolCreate().

    Returns 0 for success, LDAP error code if failure.

  Destroys the connection pool by unbinding all session handles and freeing
  all memory structures.  Applications should not call this function while
  other threads are using connections from the pool.
 
 ***************************************************************************/

void cpoolDestroy (
    LDAPConnectionPool *pool      /* Pool handle */
)
{
    if (pool == NULL)  
        return;
    /* Don't set the mutex here.  It is set inside cpoolRemoveHost(). */

    /* Remove the first host in the list until there are no more hosts. */
    while (pool->hp[0])
        cpoolRemoveHost(pool, pool->hp[0]);
    
    free(pool->hp);                 /* Free the array of pointers */

    pool->valid = 0;                /* Mark this pool as invalid */
    CP_mutex_destroy(&pool->mutex);

    free(pool);               /* Free the main LDAPConnectionPool structure */

}

/*************************************************************************** 
 lockAndValidatePool - Internal function to lock the pool mutex
                       ensure the pool is valid.  
 ***************************************************************************/

static int lockAndValidatePool (
    LDAPConnectionPool *pool      /* Pool handle */
)
{
    if (pool == NULL)  
        return LDAP_PARAM_ERROR;
    CP_mutex_lock(&pool->mutex);

    if (pool->valid != CPOOL_VALID) {
        CP_mutex_unlock(&pool->mutex);
        return LDAP_PARAM_ERROR;
    }

    return LDAP_SUCCESS;
}


/*************************************************************************** 
    Mutex functions for various platforms.
 ***************************************************************************/

/* Mutex functions for Win32. */
#if defined WIN32

int CP_mutex_init( CP_Mutex *mutex) 
{
    *mutex = CreateMutex(NULL,0,NULL);
    return (*mutex ? 0 : -1);
}
int CP_mutex_lock( CP_Mutex *mutex)
{
    DWORD status = WaitForSingleObject(*mutex, INFINITE);
    return (status==-1 ? -1 : 0); 
}
int CP_mutex_unlock( CP_Mutex *mutex) 
{ 
    ReleaseMutex(*mutex);
    return 0; 
}
int CP_mutex_destroy( CP_Mutex *mutex)
{
    CloseHandle(*mutex);
    return 0;
}


/* Mutex functions for NetWare/LIBC */
#elif (defined (N_PLAT_NLM)  && defined (LIBC)) || defined (MODESTO)

int CP_mutex_init( CP_Mutex *mutex) 
{
    *mutex = NXMutexAlloc(0, 0, NULL);
    return (*mutex ? 0 : -1);
}
int CP_mutex_lock( CP_Mutex *mutex)
{
    return NXLock(*mutex);
}
int CP_mutex_unlock( CP_Mutex *mutex) 
{ 
    return NXUnlock(*mutex);
}
int CP_mutex_destroy( CP_Mutex *mutex)
{
    NXMutexFree(*mutex);
    return 0;
}


/* Mutex functions for NetWare/CLIB */
#elif defined (N_PLAT_NLM)

int CP_mutex_init( CP_Mutex *mutex) 
{
    *mutex = OpenLocalSemaphore(1);   /* Binary semaphore */
    return (*mutex ? 0 : -1);
}
int CP_mutex_lock( CP_Mutex *mutex)
{
    return WaitOnLocalSemaphore(*mutex);
}
int CP_mutex_unlock( CP_Mutex *mutex) 
{ 
    return SignalLocalSemaphore(*mutex);
}
int CP_mutex_destroy( CP_Mutex *mutex)
{
    return CloseLocalSemaphore(*mutex);
}


/* Mutex functions for Posix systems (LINUX/SOLARIS/AIX) */
#elif defined (UNIX)

int CP_mutex_init( CP_Mutex *mutex) 
{
    *mutex = malloc(sizeof(pthread_mutex_t));
    if (*mutex == NULL)
        return -1;
    pthread_mutex_init(*mutex, NULL);
    return (mutex ? 0 : -1);
}
int CP_mutex_lock( CP_Mutex *mutex)
{
    return pthread_mutex_lock(*mutex);
}
int CP_mutex_unlock( CP_Mutex *mutex) 
{ 
    return pthread_mutex_unlock(*mutex);
}
int CP_mutex_destroy( CP_Mutex *mutex)
{
    int rc = pthread_mutex_destroy(*mutex);
    free(*mutex);
    return rc;
}


/* Dummy mutex functions */
#else

int CP_mutex_init( CP_Mutex *mutex) { return 0; }
int CP_mutex_lock( CP_Mutex *mutex) { return 0; }
int CP_mutex_unlock( CP_Mutex *mutex)  { return 0; }
int CP_mutex_destroy( CP_Mutex *mutex) { return 0; }

#endif





