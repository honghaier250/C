/*
 * Count register synchronisation.
 *
 * All CPUs will have their count registers synchronised to the CPU0 expirelo
 * value. This can cause a small timewarp for CPU0. All other CPU's should
 * not have done anything significant (but they may have had interrupts
 * enabled briefly - prom_smp_finish() should not be responsible for enabling
 * interrupts...)
 *
 * FIXME: broken for SMTC
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/irqflags.h>
#include <linux/r4k-timer.h>

#include <asm/atomic.h>
#include <asm/barrier.h>
#include <asm/cpumask.h>
#include <asm/mipsregs.h>

static atomic_t __initdata count_start_flag = ATOMIC_INIT(0);
static atomic_t __initdata count_count_start = ATOMIC_INIT(0);
static atomic_t __initdata count_count_stop = ATOMIC_INIT(0);

#define COUNTON    100
#define NR_LOOPS 5

void __init synchronise_count_master(void)
{
    int i;
    unsigned long flags;
    unsigned int initcount;
    int nslaves;

#ifdef CONFIG_MIPS_MT_SMTC
    /*
     * SMTC needs to synchronise per VPE, not per CPU
     * ignore for now
     */
    return;
#endif

    pr_info("Checking COUNT synchronization across %u CPUs: ",
        num_online_cpus());

    local_irq_save(flags);

    /*
     * Notify the slaves that it's time to start
     */
    atomic_set(&count_start_flag, 1);
    smp_wmb();

    /* Count will be initialised to expirelo for all CPU's */
    initcount = expirelo;

    /*
     * We loop a few times to get a primed instruction cache,
     * then the last pass is more or less synchronised and
     * the master and slaves each set their cycle counters to a known
     * value all at once. This reduces the chance of having random offsets
     * between the processors, and guarantees that the maximum
     * delay between the cycle counters is never bigger than
     * the latency of information-passing (cachelines) between
     * two CPUs.
     */

    nslaves = num_online_cpus()-1;
    for (i = 0; i < NR_LOOPS; i++) {
        /* slaves loop on '!= ncpus' */
        while (atomic_read(&count_count_start) != nslaves)
            mb();
        atomic_set(&count_count_stop, 0);
        smp_wmb();

        /* this lets the slaves write their count register */
        atomic_inc(&count_count_start);

        /*
         * Everyone initialises count in the last loop:
         */
        if (i == NR_LOOPS-1)
            write_c0_count(initcount);

        /*
         * Wait for all slaves to leave the synchronization point:
         */
        while (atomic_read(&count_count_stop) != nslaves)
            mb();
        atomic_set(&count_count_start, 0);
        smp_wmb();
        atomic_inc(&count_count_stop);
    }
    /* Arrange for an interrupt in a short while */
    write_c0_compare(read_c0_count() + COUNTON);

    local_irq_restore(flags);

    /*
     * i386 code reported the skew here, but the
     * count registers were almost certainly out of sync
     * so no point in alarming people
     */
    printk("done.\n");
}

void __init synchronise_count_slave(void)
{
    int i;
    unsigned long flags;
    unsigned int initcount;
    int ncpus;

#ifdef CONFIG_MIPS_MT_SMTC
    /*
     * SMTC needs to synchronise per VPE, not per CPU
     * ignore for now
     */
    return;
#endif

    local_irq_save(flags);

    /*
     * Not every cpu is online at the time this gets called,
     * so we first wait for the master to say everyone is ready
     */

    while (!atomic_read(&count_start_flag))
        mb();

    /* Count will be initialised to expirelo for all CPU's */
    initcount = expirelo;

    ncpus = num_online_cpus();
    for (i = 0; i < NR_LOOPS; i++) {
        atomic_inc(&count_count_start);
        while (atomic_read(&count_count_start) != ncpus)
            mb();

        /*
         * Everyone initialises count in the last loop:
         */
        if (i == NR_LOOPS-1)
            write_c0_count(initcount);

        atomic_inc(&count_count_stop);
        while (atomic_read(&count_count_stop) != ncpus)
            mb();
    }
    /* Arrange for an interrupt in a short while */
    write_c0_compare(read_c0_count() + COUNTON);

    local_irq_restore(flags);
}
#undef NR_LOOPS
#endif
