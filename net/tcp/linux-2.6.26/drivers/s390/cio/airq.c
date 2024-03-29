/*
 *  drivers/s390/cio/airq.c
 *    Support for adapter interruptions
 *
 *    Copyright IBM Corp. 1999,2007
 *    Author(s): Ingo Adlung <adlung@de.ibm.com>
 *         Cornelia Huck <cornelia.huck@de.ibm.com>
 *         Arnd Bergmann <arndb@de.ibm.com>
 *         Peter Oberparleiter <peter.oberparleiter@de.ibm.com>
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/rcupdate.h>

#include <asm/airq.h>

#include "cio.h"
#include "cio_debug.h"

#define NR_AIRQS        32
#define NR_AIRQS_PER_WORD    sizeof(unsigned long)
#define NR_AIRQ_WORDS        (NR_AIRQS / NR_AIRQS_PER_WORD)

union indicator_t {
    unsigned long word[NR_AIRQ_WORDS];
    unsigned char byte[NR_AIRQS];
} __attribute__((packed));

struct airq_t {
    adapter_int_handler_t handler;
    void *drv_data;
};

static union indicator_t indicators;
static struct airq_t *airqs[NR_AIRQS];

static int register_airq(struct airq_t *airq)
{
    int i;

    for (i = 0; i < NR_AIRQS; i++)
        if (!cmpxchg(&airqs[i], NULL, airq))
            return i;
    return -ENOMEM;
}

/**
 * s390_register_adapter_interrupt() - register adapter interrupt handler
 * @handler: adapter handler to be registered
 * @drv_data: driver data passed with each call to the handler
 *
 * Returns:
 *  Pointer to the indicator to be used on success
 *  ERR_PTR() if registration failed
 */
void *s390_register_adapter_interrupt(adapter_int_handler_t handler,
                      void *drv_data)
{
    struct airq_t *airq;
    char dbf_txt[16];
    int ret;

    airq = kmalloc(sizeof(struct airq_t), GFP_KERNEL);
    if (!airq) {
        ret = -ENOMEM;
        goto out;
    }
    airq->handler = handler;
    airq->drv_data = drv_data;
    ret = register_airq(airq);
    if (ret < 0)
        kfree(airq);
out:
    snprintf(dbf_txt, sizeof(dbf_txt), "rairq:%d", ret);
    CIO_TRACE_EVENT(4, dbf_txt);
    if (ret < 0)
        return ERR_PTR(ret);
    else
        return &indicators.byte[ret];
}
EXPORT_SYMBOL(s390_register_adapter_interrupt);

/**
 * s390_unregister_adapter_interrupt - unregister adapter interrupt handler
 * @ind: indicator for which the handler is to be unregistered
 */
void s390_unregister_adapter_interrupt(void *ind)
{
    struct airq_t *airq;
    char dbf_txt[16];
    int i;

    i = (int) ((addr_t) ind) - ((addr_t) &indicators.byte[0]);
    snprintf(dbf_txt, sizeof(dbf_txt), "urairq:%d", i);
    CIO_TRACE_EVENT(4, dbf_txt);
    indicators.byte[i] = 0;
    airq = xchg(&airqs[i], NULL);
    /*
     * Allow interrupts to complete. This will ensure that the airq handle
     * is no longer referenced by any interrupt handler.
     */
    synchronize_sched();
    kfree(airq);
}
EXPORT_SYMBOL(s390_unregister_adapter_interrupt);

#define INDICATOR_MASK    (0xffUL << ((NR_AIRQS_PER_WORD - 1) * 8))

void do_adapter_IO(void)
{
    int w;
    int i;
    unsigned long word;
    struct airq_t *airq;

    /*
     * Access indicator array in word-sized chunks to minimize storage
     * fetch operations.
     */
    for (w = 0; w < NR_AIRQ_WORDS; w++) {
        word = indicators.word[w];
        i = w * NR_AIRQS_PER_WORD;
        /*
         * Check bytes within word for active indicators.
         */
        while (word) {
            if (word & INDICATOR_MASK) {
                airq = airqs[i];
                if (likely(airq))
                    airq->handler(&indicators.byte[i],
                              airq->drv_data);
                else
                    /*
                     * Reset ill-behaved indicator.
                     */
                    indicators.byte[i] = 0;
            }
            word <<= 8;
            i++;
        }
    }
}
