/*
 * Scatterlist Cryptographic API.
 *
 * Copyright (c) 2002 James Morris <jmorris@intercode.com.au>
 * Copyright (c) 2002 David S. Miller (davem@redhat.com)
 * Copyright (c) 2005 Herbert Xu <herbert@gondor.apana.org.au>
 *
 * Portions derived from Cryptoapi, by Alexander Kjeldaas <astor@fast.no>
 * and Nettle, by Niels Möller.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 */

#include <linux/err.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/module.h>
#include <linux/param.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/string.h>
#include "internal.h"

LIST_HEAD(crypto_alg_list);
EXPORT_SYMBOL_GPL(crypto_alg_list);
DECLARE_RWSEM(crypto_alg_sem);
EXPORT_SYMBOL_GPL(crypto_alg_sem);

BLOCKING_NOTIFIER_HEAD(crypto_chain);
EXPORT_SYMBOL_GPL(crypto_chain);

static inline struct crypto_alg *crypto_alg_get(struct crypto_alg *alg)
{
    atomic_inc(&alg->cra_refcnt);
    return alg;
}

struct crypto_alg *crypto_mod_get(struct crypto_alg *alg)
{
    return try_module_get(alg->cra_module) ? crypto_alg_get(alg) : NULL;
}
EXPORT_SYMBOL_GPL(crypto_mod_get);

void crypto_mod_put(struct crypto_alg *alg)
{
    struct module *module = alg->cra_module;

    crypto_alg_put(alg);
    module_put(module);
}
EXPORT_SYMBOL_GPL(crypto_mod_put);

struct crypto_alg *__crypto_alg_lookup(const char *name, u32 type, u32 mask)
{
    struct crypto_alg *q, *alg = NULL;
    int best = -2;

    list_for_each_entry(q, &crypto_alg_list, cra_list) {
        int exact, fuzzy;

        if (crypto_is_moribund(q))
            continue;

        if ((q->cra_flags ^ type) & mask)
            continue;

        if (crypto_is_larval(q) &&
            ((struct crypto_larval *)q)->mask != mask)
            continue;

        exact = !strcmp(q->cra_driver_name, name);
        fuzzy = !strcmp(q->cra_name, name);
        if (!exact && !(fuzzy && q->cra_priority > best))
            continue;

        if (unlikely(!crypto_mod_get(q)))
            continue;

        best = q->cra_priority;
        if (alg)
            crypto_mod_put(alg);
        alg = q;

        if (exact)
            break;
    }

    return alg;
}
EXPORT_SYMBOL_GPL(__crypto_alg_lookup);

static void crypto_larval_destroy(struct crypto_alg *alg)
{
    struct crypto_larval *larval = (void *)alg;

    BUG_ON(!crypto_is_larval(alg));
    if (larval->adult)
        crypto_mod_put(larval->adult);
    kfree(larval);
}

static struct crypto_alg *crypto_larval_alloc(const char *name, u32 type,
                          u32 mask)
{
    struct crypto_alg *alg;
    struct crypto_larval *larval;

    larval = kzalloc(sizeof(*larval), GFP_KERNEL);
    if (!larval)
        return ERR_PTR(-ENOMEM);

    larval->mask = mask;
    larval->alg.cra_flags = CRYPTO_ALG_LARVAL | type;
    larval->alg.cra_priority = -1;
    larval->alg.cra_destroy = crypto_larval_destroy;

    atomic_set(&larval->alg.cra_refcnt, 2);
    strlcpy(larval->alg.cra_name, name, CRYPTO_MAX_ALG_NAME);
    init_completion(&larval->completion);

    down_write(&crypto_alg_sem);
    alg = __crypto_alg_lookup(name, type, mask);
    if (!alg) {
        alg = &larval->alg;
        list_add(&alg->cra_list, &crypto_alg_list);
    }
    up_write(&crypto_alg_sem);

    if (alg != &larval->alg)
        kfree(larval);

    return alg;
}

void crypto_larval_kill(struct crypto_alg *alg)
{
    struct crypto_larval *larval = (void *)alg;

    down_write(&crypto_alg_sem);
    list_del(&alg->cra_list);
    up_write(&crypto_alg_sem);
    complete_all(&larval->completion);
    crypto_alg_put(alg);
}
EXPORT_SYMBOL_GPL(crypto_larval_kill);

static struct crypto_alg *crypto_larval_wait(struct crypto_alg *alg)
{
    struct crypto_larval *larval = (void *)alg;

    wait_for_completion_interruptible_timeout(&larval->completion, 60 * HZ);
    alg = larval->adult;
    if (alg) {
        if (!crypto_mod_get(alg))
            alg = ERR_PTR(-EAGAIN);
    } else
        alg = ERR_PTR(-ENOENT);
    crypto_mod_put(&larval->alg);

    return alg;
}

static struct crypto_alg *crypto_alg_lookup(const char *name, u32 type,
                        u32 mask)
{
    struct crypto_alg *alg;

    down_read(&crypto_alg_sem);
    alg = __crypto_alg_lookup(name, type, mask);
    up_read(&crypto_alg_sem);

    return alg;
}

struct crypto_alg *crypto_larval_lookup(const char *name, u32 type, u32 mask)
{
    struct crypto_alg *alg;

    if (!name)
        return ERR_PTR(-ENOENT);

    mask &= ~(CRYPTO_ALG_LARVAL | CRYPTO_ALG_DEAD);
    type &= mask;

    alg = try_then_request_module(crypto_alg_lookup(name, type, mask),
                      name);
    if (alg)
        return crypto_is_larval(alg) ? crypto_larval_wait(alg) : alg;

    return crypto_larval_alloc(name, type, mask);
}
EXPORT_SYMBOL_GPL(crypto_larval_lookup);

struct crypto_alg *crypto_alg_mod_lookup(const char *name, u32 type, u32 mask)
{
    struct crypto_alg *alg;
    struct crypto_alg *larval;
    int ok;

    larval = crypto_larval_lookup(name, type, mask);
    if (IS_ERR(larval) || !crypto_is_larval(larval))
        return larval;

    ok = crypto_notify(CRYPTO_MSG_ALG_REQUEST, larval);
    if (ok == NOTIFY_DONE) {
        request_module("cryptomgr");
        ok = crypto_notify(CRYPTO_MSG_ALG_REQUEST, larval);
    }

    if (ok == NOTIFY_STOP)
        alg = crypto_larval_wait(larval);
    else {
        crypto_mod_put(larval);
        alg = ERR_PTR(-ENOENT);
    }
    crypto_larval_kill(larval);
    return alg;
}
EXPORT_SYMBOL_GPL(crypto_alg_mod_lookup);

static int crypto_init_ops(struct crypto_tfm *tfm, u32 type, u32 mask)
{
    const struct crypto_type *type_obj = tfm->__crt_alg->cra_type;

    if (type_obj)
        return type_obj->init(tfm, type, mask);

    switch (crypto_tfm_alg_type(tfm)) {
    case CRYPTO_ALG_TYPE_CIPHER:
        return crypto_init_cipher_ops(tfm);
        
    case CRYPTO_ALG_TYPE_DIGEST:
        return crypto_init_digest_ops(tfm);
        
    case CRYPTO_ALG_TYPE_COMPRESS:
        return crypto_init_compress_ops(tfm);
    
    default:
        break;
    }
    
    BUG();
    return -EINVAL;
}

static void crypto_exit_ops(struct crypto_tfm *tfm)
{
    const struct crypto_type *type = tfm->__crt_alg->cra_type;

    if (type) {
        if (type->exit)
            type->exit(tfm);
        return;
    }

    switch (crypto_tfm_alg_type(tfm)) {
    case CRYPTO_ALG_TYPE_CIPHER:
        crypto_exit_cipher_ops(tfm);
        break;
        
    case CRYPTO_ALG_TYPE_DIGEST:
        crypto_exit_digest_ops(tfm);
        break;
        
    case CRYPTO_ALG_TYPE_COMPRESS:
        crypto_exit_compress_ops(tfm);
        break;
    
    default:
        BUG();
        
    }
}

static unsigned int crypto_ctxsize(struct crypto_alg *alg, u32 type, u32 mask)
{
    const struct crypto_type *type_obj = alg->cra_type;
    unsigned int len;

    len = alg->cra_alignmask & ~(crypto_tfm_ctx_alignment() - 1);
    if (type_obj)
        return len + type_obj->ctxsize(alg, type, mask);

    switch (alg->cra_flags & CRYPTO_ALG_TYPE_MASK) {
    default:
        BUG();

    case CRYPTO_ALG_TYPE_CIPHER:
        len += crypto_cipher_ctxsize(alg);
        break;
        
    case CRYPTO_ALG_TYPE_DIGEST:
        len += crypto_digest_ctxsize(alg);
        break;
        
    case CRYPTO_ALG_TYPE_COMPRESS:
        len += crypto_compress_ctxsize(alg);
        break;
    }

    return len;
}

void crypto_shoot_alg(struct crypto_alg *alg)
{
    down_write(&crypto_alg_sem);
    alg->cra_flags |= CRYPTO_ALG_DYING;
    up_write(&crypto_alg_sem);
}
EXPORT_SYMBOL_GPL(crypto_shoot_alg);

struct crypto_tfm *__crypto_alloc_tfm(struct crypto_alg *alg, u32 type,
                      u32 mask)
{
    struct crypto_tfm *tfm = NULL;
    unsigned int tfm_size;
    int err = -ENOMEM;

    tfm_size = sizeof(*tfm) + crypto_ctxsize(alg, type, mask);
    tfm = kzalloc(tfm_size, GFP_KERNEL);
    if (tfm == NULL)
        goto out_err;

    tfm->__crt_alg = alg;

    err = crypto_init_ops(tfm, type, mask);
    if (err)
        goto out_free_tfm;

    if (alg->cra_init && (err = alg->cra_init(tfm))) {
        if (err == -EAGAIN)
            crypto_shoot_alg(alg);
        goto cra_init_failed;
    }

    goto out;

cra_init_failed:
    crypto_exit_ops(tfm);
out_free_tfm:
    kfree(tfm);
out_err:
    tfm = ERR_PTR(err);
out:
    return tfm;
}
EXPORT_SYMBOL_GPL(__crypto_alloc_tfm);

/*
 *    crypto_alloc_base - Locate algorithm and allocate transform
 *    @alg_name: Name of algorithm
 *    @type: Type of algorithm
 *    @mask: Mask for type comparison
 *
 *    crypto_alloc_base() will first attempt to locate an already loaded
 *    algorithm.  If that fails and the kernel supports dynamically loadable
 *    modules, it will then attempt to load a module of the same name or
 *    alias.  If that fails it will send a query to any loaded crypto manager
 *    to construct an algorithm on the fly.  A refcount is grabbed on the
 *    algorithm which is then associated with the new transform.
 *
 *    The returned transform is of a non-determinate type.  Most people
 *    should use one of the more specific allocation functions such as
 *    crypto_alloc_blkcipher.
 *
 *    In case of error the return value is an error pointer.
 */
struct crypto_tfm *crypto_alloc_base(const char *alg_name, u32 type, u32 mask)
{
    struct crypto_tfm *tfm;
    int err;

    for (;;) {
        struct crypto_alg *alg;

        alg = crypto_alg_mod_lookup(alg_name, type, mask);
        if (IS_ERR(alg)) {
            err = PTR_ERR(alg);
            goto err;
        }

        tfm = __crypto_alloc_tfm(alg, type, mask);
        if (!IS_ERR(tfm))
            return tfm;

        crypto_mod_put(alg);
        err = PTR_ERR(tfm);

err:
        if (err != -EAGAIN)
            break;
        if (signal_pending(current)) {
            err = -EINTR;
            break;
        }
    }

    return ERR_PTR(err);
}
EXPORT_SYMBOL_GPL(crypto_alloc_base);
 
/*
 *    crypto_free_tfm - Free crypto transform
 *    @tfm: Transform to free
 *
 *    crypto_free_tfm() frees up the transform and any associated resources,
 *    then drops the refcount on the associated algorithm.
 */
void crypto_free_tfm(struct crypto_tfm *tfm)
{
    struct crypto_alg *alg;
    int size;

    if (unlikely(!tfm))
        return;

    alg = tfm->__crt_alg;
    size = sizeof(*tfm) + alg->cra_ctxsize;

    if (alg->cra_exit)
        alg->cra_exit(tfm);
    crypto_exit_ops(tfm);
    crypto_mod_put(alg);
    memset(tfm, 0, size);
    kfree(tfm);
}

EXPORT_SYMBOL_GPL(crypto_free_tfm);

int crypto_has_alg(const char *name, u32 type, u32 mask)
{
    int ret = 0;
    struct crypto_alg *alg = crypto_alg_mod_lookup(name, type, mask);
    
    if (!IS_ERR(alg)) {
        crypto_mod_put(alg);
        ret = 1;
    }
    
    return ret;
}
EXPORT_SYMBOL_GPL(crypto_has_alg);

MODULE_DESCRIPTION("Cryptographic core API");
MODULE_LICENSE("GPL");
