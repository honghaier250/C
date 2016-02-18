#include <stdio.h>
#include <apr_pools.h>
#include <apr_shm.h>
#include <apr.h>

int main()
{
    apr_pool_initialize();

    apr_pool_t *pool;
    const char *file = "mmap_file";

    apr_status_t rc;
    rc = apr_pool_create(&pool, NULL);
    if (rc != APR_SUCCESS) {
        fprintf(stderr, "apr_create_pool error\n");
        return 1;
    }

    apr_shm_t *shm;
    rc = apr_shm_create(&shm, 20008, file, pool);
    if (rc != APR_SUCCESS) {
        fprintf(stderr, "apr_create_shm error\n");
        return 1;
    }
    apr_pool_destroy(pool);

    printf("create shm ok\n");
    return 0;
}
