#include <stdio.h>
#include <openssl/engine.h>

int load_dynamic_engine(const char *engine_id)
{
    ENGINE *e = NULL;

    ENGINE_load_builtin_engines();

    e = ENGINE_by_id("dynamic");
    ENGINE_ctrl_cmd_string(e, "SO_PATH", "/usr/local/ssl/lib/engines/libgost.so", 0);
    ENGINE_ctrl_cmd_string(e, "ID", "gost", 0);
    ENGINE_ctrl_cmd_string(e, "LOAD", NULL, 0);
    if (NULL == e)
    {
        fprintf(stderr, "ENGINE_ctrl_cmd_string Failed !\n");
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
    const char *engine_id = "ibmca";

    ret = load_dynamic_engine(engine_id);
    if (0 > ret)
    {
        fprintf(stderr, "Something Wrong !\n");
        return -1;
    }

    fprintf(stdout, "Load Dynamic Engine OK !\n");

    return 0;
}
