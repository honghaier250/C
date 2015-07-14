/*
 *    Optimized memory copy routines.
 *
 *    Copyright (C) 2004 Randolph Chung <tausq@debian.org>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *    Portions derived from the GNU C Library
 *    Copyright (C) 1991, 1997, 2003 Free Software Foundation, Inc.
 *
 * Several strategies are tried to try to get the best performance for various
 * conditions. In the optimal case, we copy 64-bytes in an unrolled loop using 
 * fp regs. This is followed by loops that copy 32- or 16-bytes at a time using
 * general registers.  Unaligned copies are handled either by aligning the 
 * destination and then using shift-and-write method, or in a few cases by 
 * falling back to a byte-at-a-time copy.
 *
 * I chose to implement this in C because it is easier to maintain and debug,
 * and in my experiments it appears that the C code generated by gcc (3.3/3.4
 * at the time of writing) is fairly optimal. Unfortunately some of the 
 * semantics of the copy routine (exception handling) is difficult to express
 * in C, so we have to play some tricks to get it to work.
 *
 * All the loads and stores are done via explicit asm() code in order to use
 * the right space registers. 
 * 
 * Testing with various alignments and buffer sizes shows that this code is 
 * often >10x faster than a simple byte-at-a-time copy, even for strangely
 * aligned operands. It is interesting to note that the glibc version
 * of memcpy (written in C) is actually quite fast already. This routine is 
 * able to beat it by 30-40% for aligned copies because of the loop unrolling, 
 * but in some cases the glibc version is still slightly faster. This lends 
 * more credibility that gcc can generate very good code as long as we are 
 * careful.
 *
 * TODO:
 * - cache prefetching needs more experimentation to get optimal settings
 * - try not to use the post-increment address modifiers; they create additional
 *   interlocks
 * - replace byte-copy loops with stybs sequences
 */

#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/compiler.h>
#include <asm/uaccess.h>
#define s_space "%%sr1"
#define d_space "%%sr2"
#else
#include "memcpy.h"
#define s_space "%%sr0"
#define d_space "%%sr0"
#define pa_memcpy new2_copy
#endif

DECLARE_PER_CPU(struct exception_data, exception_data);

#define preserve_branch(label)    do {                    \
    volatile int dummy;                        \
    /* The following branch is never taken, it's just here to  */    \
    /* prevent gcc from optimizing away our exception code. */     \
    if (unlikely(dummy != dummy))                    \
        goto label;                        \
} while (0)

#define get_user_space() (segment_eq(get_fs(), KERNEL_DS) ? 0 : mfsp(3))
#define get_kernel_space() (0)

#define MERGE(w0, sh_1, w1, sh_2)  ({                    \
    unsigned int _r;                        \
    asm volatile (                            \
    "mtsar %3\n"                            \
    "shrpw %1, %2, %%sar, %0\n"                    \
    : "=r"(_r)                            \
    : "r"(w0), "r"(w1), "r"(sh_2)                    \
    );                                \
    _r;                                \
})
#define THRESHOLD    16

#ifdef DEBUG_MEMCPY
#define DPRINTF(fmt, args...) do { printk(KERN_DEBUG "%s:%d:%s ", __FILE__, __LINE__, __func__ ); printk(KERN_DEBUG fmt, ##args ); } while (0)
#else
#define DPRINTF(fmt, args...)
#endif

#define def_load_ai_insn(_insn,_sz,_tt,_s,_a,_t,_e)    \
    __asm__ __volatile__ (                \
    "1:\t" #_insn ",ma " #_sz "(" _s ",%1), %0\n\t"    \
    ASM_EXCEPTIONTABLE_ENTRY(1b,_e)            \
    : _tt(_t), "+r"(_a)                \
    :                         \
    : "r8")

#define def_store_ai_insn(_insn,_sz,_tt,_s,_a,_t,_e)     \
    __asm__ __volatile__ (                \
    "1:\t" #_insn ",ma %1, " #_sz "(" _s ",%0)\n\t"    \
    ASM_EXCEPTIONTABLE_ENTRY(1b,_e)            \
    : "+r"(_a)                     \
    : _tt(_t)                    \
    : "r8")

#define ldbma(_s, _a, _t, _e) def_load_ai_insn(ldbs,1,"=r",_s,_a,_t,_e)
#define stbma(_s, _t, _a, _e) def_store_ai_insn(stbs,1,"r",_s,_a,_t,_e)
#define ldwma(_s, _a, _t, _e) def_load_ai_insn(ldw,4,"=r",_s,_a,_t,_e)
#define stwma(_s, _t, _a, _e) def_store_ai_insn(stw,4,"r",_s,_a,_t,_e)
#define flddma(_s, _a, _t, _e) def_load_ai_insn(fldd,8,"=f",_s,_a,_t,_e)
#define fstdma(_s, _t, _a, _e) def_store_ai_insn(fstd,8,"f",_s,_a,_t,_e)

#define def_load_insn(_insn,_tt,_s,_o,_a,_t,_e)     \
    __asm__ __volatile__ (                \
    "1:\t" #_insn " " #_o "(" _s ",%1), %0\n\t"    \
    ASM_EXCEPTIONTABLE_ENTRY(1b,_e)            \
    : _tt(_t)                     \
    : "r"(_a)                    \
    : "r8")

#define def_store_insn(_insn,_tt,_s,_t,_o,_a,_e)     \
    __asm__ __volatile__ (                \
    "1:\t" #_insn " %0, " #_o "(" _s ",%1)\n\t"     \
    ASM_EXCEPTIONTABLE_ENTRY(1b,_e)            \
    :                         \
    : _tt(_t), "r"(_a)                \
    : "r8")

#define ldw(_s,_o,_a,_t,_e)    def_load_insn(ldw,"=r",_s,_o,_a,_t,_e)
#define stw(_s,_t,_o,_a,_e)     def_store_insn(stw,"r",_s,_t,_o,_a,_e)

#ifdef  CONFIG_PREFETCH
static inline void prefetch_src(const void *addr)
{
    __asm__("ldw 0(" s_space ",%0), %%r0" : : "r" (addr));
}

static inline void prefetch_dst(const void *addr)
{
    __asm__("ldd 0(" d_space ",%0), %%r0" : : "r" (addr));
}
#else
#define prefetch_src(addr) do { } while(0)
#define prefetch_dst(addr) do { } while(0)
#endif

/* Copy from a not-aligned src to an aligned dst, using shifts. Handles 4 words
 * per loop.  This code is derived from glibc. 
 */
static inline unsigned long copy_dstaligned(unsigned long dst, unsigned long src, unsigned long len, unsigned long o_dst, unsigned long o_src, unsigned long o_len)
{
    /* gcc complains that a2 and a3 may be uninitialized, but actually
     * they cannot be.  Initialize a2/a3 to shut gcc up.
     */
    register unsigned int a0, a1, a2 = 0, a3 = 0;
    int sh_1, sh_2;
    struct exception_data *d;

    /* prefetch_src((const void *)src); */

    /* Calculate how to shift a word read at the memory operation
       aligned srcp to make it aligned for copy.  */
    sh_1 = 8 * (src % sizeof(unsigned int));
    sh_2 = 8 * sizeof(unsigned int) - sh_1;

    /* Make src aligned by rounding it down.  */
    src &= -sizeof(unsigned int);

    switch (len % 4)
    {
        case 2:
            /* a1 = ((unsigned int *) src)[0];
               a2 = ((unsigned int *) src)[1]; */
            ldw(s_space, 0, src, a1, cda_ldw_exc);
            ldw(s_space, 4, src, a2, cda_ldw_exc);
            src -= 1 * sizeof(unsigned int);
            dst -= 3 * sizeof(unsigned int);
            len += 2;
            goto do1;
        case 3:
            /* a0 = ((unsigned int *) src)[0];
               a1 = ((unsigned int *) src)[1]; */
            ldw(s_space, 0, src, a0, cda_ldw_exc);
            ldw(s_space, 4, src, a1, cda_ldw_exc);
            src -= 0 * sizeof(unsigned int);
            dst -= 2 * sizeof(unsigned int);
            len += 1;
            goto do2;
        case 0:
            if (len == 0)
                return 0;
            /* a3 = ((unsigned int *) src)[0];
               a0 = ((unsigned int *) src)[1]; */
            ldw(s_space, 0, src, a3, cda_ldw_exc);
            ldw(s_space, 4, src, a0, cda_ldw_exc);
            src -=-1 * sizeof(unsigned int);
            dst -= 1 * sizeof(unsigned int);
            len += 0;
            goto do3;
        case 1:
            /* a2 = ((unsigned int *) src)[0];
               a3 = ((unsigned int *) src)[1]; */
            ldw(s_space, 0, src, a2, cda_ldw_exc);
            ldw(s_space, 4, src, a3, cda_ldw_exc);
            src -=-2 * sizeof(unsigned int);
            dst -= 0 * sizeof(unsigned int);
            len -= 1;
            if (len == 0)
                goto do0;
            goto do4;            /* No-op.  */
    }

    do
    {
        /* prefetch_src((const void *)(src + 4 * sizeof(unsigned int))); */
do4:
        /* a0 = ((unsigned int *) src)[0]; */
        ldw(s_space, 0, src, a0, cda_ldw_exc);
        /* ((unsigned int *) dst)[0] = MERGE (a2, sh_1, a3, sh_2); */
        stw(d_space, MERGE (a2, sh_1, a3, sh_2), 0, dst, cda_stw_exc);
do3:
        /* a1 = ((unsigned int *) src)[1]; */
        ldw(s_space, 4, src, a1, cda_ldw_exc);
        /* ((unsigned int *) dst)[1] = MERGE (a3, sh_1, a0, sh_2); */
        stw(d_space, MERGE (a3, sh_1, a0, sh_2), 4, dst, cda_stw_exc);
do2:
        /* a2 = ((unsigned int *) src)[2]; */
        ldw(s_space, 8, src, a2, cda_ldw_exc);
        /* ((unsigned int *) dst)[2] = MERGE (a0, sh_1, a1, sh_2); */
        stw(d_space, MERGE (a0, sh_1, a1, sh_2), 8, dst, cda_stw_exc);
do1:
        /* a3 = ((unsigned int *) src)[3]; */
        ldw(s_space, 12, src, a3, cda_ldw_exc);
        /* ((unsigned int *) dst)[3] = MERGE (a1, sh_1, a2, sh_2); */
        stw(d_space, MERGE (a1, sh_1, a2, sh_2), 12, dst, cda_stw_exc);

        src += 4 * sizeof(unsigned int);
        dst += 4 * sizeof(unsigned int);
        len -= 4;
    }
    while (len != 0);

do0:
    /* ((unsigned int *) dst)[0] = MERGE (a2, sh_1, a3, sh_2); */
    stw(d_space, MERGE (a2, sh_1, a3, sh_2), 0, dst, cda_stw_exc);

    preserve_branch(handle_load_error);
    preserve_branch(handle_store_error);

    return 0;

handle_load_error:
    __asm__ __volatile__ ("cda_ldw_exc:\n");
    d = &__get_cpu_var(exception_data);
    DPRINTF("cda_ldw_exc: o_len=%lu fault_addr=%lu o_src=%lu ret=%lu\n",
        o_len, d->fault_addr, o_src, o_len - d->fault_addr + o_src);
    return o_len * 4 - d->fault_addr + o_src;

handle_store_error:
    __asm__ __volatile__ ("cda_stw_exc:\n");
    d = &__get_cpu_var(exception_data);
    DPRINTF("cda_stw_exc: o_len=%lu fault_addr=%lu o_dst=%lu ret=%lu\n",
        o_len, d->fault_addr, o_dst, o_len - d->fault_addr + o_dst);
    return o_len * 4 - d->fault_addr + o_dst;
}


/* Returns 0 for success, otherwise, returns number of bytes not transferred. */
unsigned long pa_memcpy(void *dstp, const void *srcp, unsigned long len)
{
    register unsigned long src, dst, t1, t2, t3;
    register unsigned char *pcs, *pcd;
    register unsigned int *pws, *pwd;
    register double *pds, *pdd;
    unsigned long ret = 0;
    unsigned long o_dst, o_src, o_len;
    struct exception_data *d;

    src = (unsigned long)srcp;
    dst = (unsigned long)dstp;
    pcs = (unsigned char *)srcp;
    pcd = (unsigned char *)dstp;

    o_dst = dst; o_src = src; o_len = len;

    /* prefetch_src((const void *)srcp); */

    if (len < THRESHOLD)
        goto byte_copy;

    /* Check alignment */
    t1 = (src ^ dst);
    if (unlikely(t1 & (sizeof(double)-1)))
        goto unaligned_copy;

    /* src and dst have same alignment. */

    /* Copy bytes till we are double-aligned. */
    t2 = src & (sizeof(double) - 1);
    if (unlikely(t2 != 0)) {
        t2 = sizeof(double) - t2;
        while (t2 && len) {
            /* *pcd++ = *pcs++; */
            ldbma(s_space, pcs, t3, pmc_load_exc);
            len--;
            stbma(d_space, t3, pcd, pmc_store_exc);
            t2--;
        }
    }

    pds = (double *)pcs;
    pdd = (double *)pcd;

#if 0
    /* Copy 8 doubles at a time */
    while (len >= 8*sizeof(double)) {
        register double r1, r2, r3, r4, r5, r6, r7, r8;
        /* prefetch_src((char *)pds + L1_CACHE_BYTES); */
        flddma(s_space, pds, r1, pmc_load_exc);
        flddma(s_space, pds, r2, pmc_load_exc);
        flddma(s_space, pds, r3, pmc_load_exc);
        flddma(s_space, pds, r4, pmc_load_exc);
        fstdma(d_space, r1, pdd, pmc_store_exc);
        fstdma(d_space, r2, pdd, pmc_store_exc);
        fstdma(d_space, r3, pdd, pmc_store_exc);
        fstdma(d_space, r4, pdd, pmc_store_exc);

#if 0
        if (L1_CACHE_BYTES <= 32)
            prefetch_src((char *)pds + L1_CACHE_BYTES);
#endif
        flddma(s_space, pds, r5, pmc_load_exc);
        flddma(s_space, pds, r6, pmc_load_exc);
        flddma(s_space, pds, r7, pmc_load_exc);
        flddma(s_space, pds, r8, pmc_load_exc);
        fstdma(d_space, r5, pdd, pmc_store_exc);
        fstdma(d_space, r6, pdd, pmc_store_exc);
        fstdma(d_space, r7, pdd, pmc_store_exc);
        fstdma(d_space, r8, pdd, pmc_store_exc);
        len -= 8*sizeof(double);
    }
#endif

    pws = (unsigned int *)pds;
    pwd = (unsigned int *)pdd;

word_copy:
    while (len >= 8*sizeof(unsigned int)) {
        register unsigned int r1,r2,r3,r4,r5,r6,r7,r8;
        /* prefetch_src((char *)pws + L1_CACHE_BYTES); */
        ldwma(s_space, pws, r1, pmc_load_exc);
        ldwma(s_space, pws, r2, pmc_load_exc);
        ldwma(s_space, pws, r3, pmc_load_exc);
        ldwma(s_space, pws, r4, pmc_load_exc);
        stwma(d_space, r1, pwd, pmc_store_exc);
        stwma(d_space, r2, pwd, pmc_store_exc);
        stwma(d_space, r3, pwd, pmc_store_exc);
        stwma(d_space, r4, pwd, pmc_store_exc);

        ldwma(s_space, pws, r5, pmc_load_exc);
        ldwma(s_space, pws, r6, pmc_load_exc);
        ldwma(s_space, pws, r7, pmc_load_exc);
        ldwma(s_space, pws, r8, pmc_load_exc);
        stwma(d_space, r5, pwd, pmc_store_exc);
        stwma(d_space, r6, pwd, pmc_store_exc);
        stwma(d_space, r7, pwd, pmc_store_exc);
        stwma(d_space, r8, pwd, pmc_store_exc);
        len -= 8*sizeof(unsigned int);
    }

    while (len >= 4*sizeof(unsigned int)) {
        register unsigned int r1,r2,r3,r4;
        ldwma(s_space, pws, r1, pmc_load_exc);
        ldwma(s_space, pws, r2, pmc_load_exc);
        ldwma(s_space, pws, r3, pmc_load_exc);
        ldwma(s_space, pws, r4, pmc_load_exc);
        stwma(d_space, r1, pwd, pmc_store_exc);
        stwma(d_space, r2, pwd, pmc_store_exc);
        stwma(d_space, r3, pwd, pmc_store_exc);
        stwma(d_space, r4, pwd, pmc_store_exc);
        len -= 4*sizeof(unsigned int);
    }

    pcs = (unsigned char *)pws;
    pcd = (unsigned char *)pwd;

byte_copy:
    while (len) {
        /* *pcd++ = *pcs++; */
        ldbma(s_space, pcs, t3, pmc_load_exc);
        stbma(d_space, t3, pcd, pmc_store_exc);
        len--;
    }

    return 0;

unaligned_copy:
    /* possibly we are aligned on a word, but not on a double... */
    if (likely(t1 & (sizeof(unsigned int)-1)) == 0) {
        t2 = src & (sizeof(unsigned int) - 1);

        if (unlikely(t2 != 0)) {
            t2 = sizeof(unsigned int) - t2;
            while (t2) {
                /* *pcd++ = *pcs++; */
                ldbma(s_space, pcs, t3, pmc_load_exc);
                stbma(d_space, t3, pcd, pmc_store_exc);
                len--;
                t2--;
            }
        }

        pws = (unsigned int *)pcs;
        pwd = (unsigned int *)pcd;
        goto word_copy;
    }

    /* Align the destination.  */
    if (unlikely((dst & (sizeof(unsigned int) - 1)) != 0)) {
        t2 = sizeof(unsigned int) - (dst & (sizeof(unsigned int) - 1));
        while (t2) {
            /* *pcd++ = *pcs++; */
            ldbma(s_space, pcs, t3, pmc_load_exc);
            stbma(d_space, t3, pcd, pmc_store_exc);
            len--;
            t2--;
        }
        dst = (unsigned long)pcd;
        src = (unsigned long)pcs;
    }

    ret = copy_dstaligned(dst, src, len / sizeof(unsigned int), 
        o_dst, o_src, o_len);
    if (ret)
        return ret;

    pcs += (len & -sizeof(unsigned int));
    pcd += (len & -sizeof(unsigned int));
    len %= sizeof(unsigned int);

    preserve_branch(handle_load_error);
    preserve_branch(handle_store_error);

    goto byte_copy;

handle_load_error:
    __asm__ __volatile__ ("pmc_load_exc:\n");
    d = &__get_cpu_var(exception_data);
    DPRINTF("pmc_load_exc: o_len=%lu fault_addr=%lu o_src=%lu ret=%lu\n",
        o_len, d->fault_addr, o_src, o_len - d->fault_addr + o_src);
    return o_len - d->fault_addr + o_src;

handle_store_error:
    __asm__ __volatile__ ("pmc_store_exc:\n");
    d = &__get_cpu_var(exception_data);
    DPRINTF("pmc_store_exc: o_len=%lu fault_addr=%lu o_dst=%lu ret=%lu\n",
        o_len, d->fault_addr, o_dst, o_len - d->fault_addr + o_dst);
    return o_len - d->fault_addr + o_dst;
}

#ifdef __KERNEL__
unsigned long copy_to_user(void __user *dst, const void *src, unsigned long len)
{
    mtsp(get_kernel_space(), 1);
    mtsp(get_user_space(), 2);
    return pa_memcpy((void __force *)dst, src, len);
}

unsigned long copy_from_user(void *dst, const void __user *src, unsigned long len)
{
    mtsp(get_user_space(), 1);
    mtsp(get_kernel_space(), 2);
    return pa_memcpy(dst, (void __force *)src, len);
}

unsigned long copy_in_user(void __user *dst, const void __user *src, unsigned long len)
{
    mtsp(get_user_space(), 1);
    mtsp(get_user_space(), 2);
    return pa_memcpy((void __force *)dst, (void __force *)src, len);
}


void * memcpy(void * dst,const void *src, size_t count)
{
    mtsp(get_kernel_space(), 1);
    mtsp(get_kernel_space(), 2);
    pa_memcpy(dst, src, count);
    return dst;
}

EXPORT_SYMBOL(copy_to_user);
EXPORT_SYMBOL(copy_from_user);
EXPORT_SYMBOL(copy_in_user);
EXPORT_SYMBOL(memcpy);
#endif
