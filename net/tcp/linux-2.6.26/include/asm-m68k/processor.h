/*
 * include/asm-m68k/processor.h
 *
 * Copyright (C) 1995 Hamish Macdonald
 */

#ifndef __ASM_M68K_PROCESSOR_H
#define __ASM_M68K_PROCESSOR_H

/*
 * Default implementation of macro that returns current
 * instruction pointer ("program counter").
 */
#define current_text_addr() ({ __label__ _l; _l: &&_l;})

#include <linux/thread_info.h>
#include <asm/segment.h>
#include <asm/fpu.h>
#include <asm/ptrace.h>

static inline unsigned long rdusp(void)
{
    unsigned long usp;

    __asm__ __volatile__("move %/usp,%0" : "=a" (usp));
    return usp;
}

static inline void wrusp(unsigned long usp)
{
    __asm__ __volatile__("move %0,%/usp" : : "a" (usp));
}

/*
 * User space process size: 3.75GB. This is hardcoded into a few places,
 * so don't change it unless you know what you are doing.
 */
#ifndef CONFIG_SUN3
#define TASK_SIZE    (0xF0000000UL)
#else
#define TASK_SIZE    (0x0E000000UL)
#endif

#ifdef __KERNEL__
#define STACK_TOP    TASK_SIZE
#define STACK_TOP_MAX    STACK_TOP
#endif

/* This decides where the kernel will search for a free chunk of vm
 * space during mmap's.
 */
#ifndef CONFIG_SUN3
#define TASK_UNMAPPED_BASE    0xC0000000UL
#else
#define TASK_UNMAPPED_BASE    0x0A000000UL
#endif
#define TASK_UNMAPPED_ALIGN(addr, off)    PAGE_ALIGN(addr)

struct thread_struct {
    unsigned long  ksp;        /* kernel stack pointer */
    unsigned long  usp;        /* user stack pointer */
    unsigned short sr;        /* saved status register */
    unsigned short fs;        /* saved fs (sfc, dfc) */
    unsigned long  crp[2];        /* cpu root pointer */
    unsigned long  esp0;        /* points to SR of stack frame */
    unsigned long  faddr;        /* info about last fault */
    int            signo, code;
    unsigned long  fp[8*3];
    unsigned long  fpcntl[3];    /* fp control regs */
    unsigned char  fpstate[FPSTATESIZE];  /* floating point state */
    struct thread_info info;
};

#define INIT_THREAD  {                            \
    .ksp    = sizeof(init_stack) + (unsigned long) init_stack,    \
    .sr    = PS_S,                            \
    .fs    = __KERNEL_DS,                        \
    .info    = INIT_THREAD_INFO(init_task),                \
}

/*
 * Do necessary setup to start up a newly executed thread.
 */
static inline void start_thread(struct pt_regs * regs, unsigned long pc,
                unsigned long usp)
{
    /* reads from user space */
    set_fs(USER_DS);

    regs->pc = pc;
    regs->sr &= ~0x2000;
    wrusp(usp);
}

/* Forward declaration, a strange C thing */
struct task_struct;

/* Free all resources held by a thread. */
static inline void release_thread(struct task_struct *dead_task)
{
}

/* Prepare to copy thread state - unlazy all lazy status */
#define prepare_to_copy(tsk)    do { } while (0)

extern int kernel_thread(int (*fn)(void *), void * arg, unsigned long flags);

/*
 * Free current thread data structures etc..
 */
static inline void exit_thread(void)
{
}

extern unsigned long thread_saved_pc(struct task_struct *tsk);

unsigned long get_wchan(struct task_struct *p);

#define    KSTK_EIP(tsk)    \
    ({            \
    unsigned long eip = 0;     \
    if ((tsk)->thread.esp0 > PAGE_SIZE && \
        (virt_addr_valid((tsk)->thread.esp0))) \
          eip = ((struct pt_regs *) (tsk)->thread.esp0)->pc; \
    eip; })
#define    KSTK_ESP(tsk)    ((tsk) == current ? rdusp() : (tsk)->thread.usp)

#define cpu_relax()    barrier()

#endif
