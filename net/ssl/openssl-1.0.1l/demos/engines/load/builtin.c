#include <stdio.h>
#include <openssl/engine.h>

int load_engine(const char *engine_id)
{
    ENGINE *e = NULL;

    ENGINE_load_builtin_engines();

    e = ENGINE_by_id(engine_id);
    if (NULL == e)
    {
        fprintf(stderr, "ENGINE_by_id Failed !\n");
        goto err;
    }

    if (!ENGINE_init(e))
    {
        fprintf(stderr, "ENGINE_init Failed !\n");
        goto err;
    }

    if (!ENGINE_set_default_RSA(e))
    {
        fprintf(stderr, "ENGINE_set_default_RSA Failed !\n");
        goto err;
    }

    ENGINE_set_default_DSA(e);
    ENGINE_set_default_ciphers(e);

    ENGINE_finish(e);
    ENGINE_free(e);
    return 0;

err:
    ENGINE_free(e);
    return -1;
}

int main()
{
    int ret = 0;
    const char *engine_id = "rsax";

    ret = load_engine(engine_id);
    if (0 > ret)
    {
        fprintf(stderr, "Something Wrong !\n");
        return -1;
    }

    fprintf(stdout, "Load Engine OK !\n");
    return 0;
}
