#include <stdio.h>
#include <stdlib.h>
#include <openssl/crypto.h>
#include <openssl/engine.h>

int main(void)
{
    ENGINE *e;
    const char *engine_id = "rsax";
    ENGINE_load_builtin_engines();
    e = ENGINE_by_id(engine_id);
    if(!e)
    {
        printf("the engine isn't available\n");
        exit(1);
    }
    if(!ENGINE_init(e)) {
        printf("the engine couldn't initialise, release 'e'\n");
        ENGINE_free(e);
        exit(1);
    }
    if(!ENGINE_set_default_RSA(e))
    {
        printf("This should only happen when 'e' can't initialise, but the previous statement suggests it did.\n");
        abort();
    }

    ENGINE_set_default_DSA(e);
    ENGINE_set_default_ciphers(e);
    /* Release the functional reference from ENGINE_init() */
    ENGINE_finish(e);
    /* Release the structural reference from ENGINE_by_id() */
    ENGINE_free(e);

    printf("test engine ok\n");

    return 1;
}
