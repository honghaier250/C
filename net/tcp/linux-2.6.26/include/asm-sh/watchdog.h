/*
 * include/asm-sh/watchdog.h
 *
 * Copyright (C) 2002, 2003 Paul Mundt
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */
#ifndef __ASM_SH_WATCHDOG_H
#define __ASM_SH_WATCHDOG_H
#ifdef __KERNEL__

#include <linux/types.h>
#include <asm/cpu/watchdog.h>
#include <asm/io.h>

/* 
 * See asm/cpu-sh2/watchdog.h for explanation of this stupidity..
 */
#ifndef WTCNT_R
#  define WTCNT_R    WTCNT
#endif

#ifndef WTCSR_R
#  define WTCSR_R    WTCSR
#endif

#define WTCNT_HIGH    0x5a
#define WTCSR_HIGH    0xa5

#define WTCSR_CKS2    0x04
#define WTCSR_CKS1    0x02
#define WTCSR_CKS0    0x01

/*
 * CKS0-2 supports a number of clock division ratios. At the time the watchdog
 * is enabled, it defaults to a 41 usec overflow period .. we overload this to
 * something a little more reasonable, and really can't deal with anything
 * lower than WTCSR_CKS_1024, else we drop back into the usec range.
 *
 * Clock Division Ratio         Overflow Period
 * --------------------------------------------
 *     1/32 (initial value)       41 usecs
 *     1/64                       82 usecs
 *     1/128                     164 usecs
 *     1/256                     328 usecs
 *     1/512                     656 usecs
 *     1/1024                   1.31 msecs
 *     1/2048                   2.62 msecs
 *     1/4096                   5.25 msecs
 */
#define WTCSR_CKS_32    0x00
#define WTCSR_CKS_64    0x01
#define WTCSR_CKS_128    0x02
#define WTCSR_CKS_256    0x03
#define WTCSR_CKS_512    0x04
#define WTCSR_CKS_1024    0x05
#define WTCSR_CKS_2048    0x06
#define WTCSR_CKS_4096    0x07

/**
 *     sh_wdt_read_cnt - Read from Counter
 *     Reads back the WTCNT value.
 */
static inline __u8 sh_wdt_read_cnt(void)
{
    return ctrl_inb(WTCNT_R);
}

/**
 *    sh_wdt_write_cnt - Write to Counter
 *    @val: Value to write
 *
 *    Writes the given value @val to the lower byte of the timer counter.
 *    The upper byte is set manually on each write.
 */
static inline void sh_wdt_write_cnt(__u8 val)
{
    ctrl_outw((WTCNT_HIGH << 8) | (__u16)val, WTCNT);
}

/**
 *     sh_wdt_read_csr - Read from Control/Status Register
 *
 *    Reads back the WTCSR value.
 */
static inline __u8 sh_wdt_read_csr(void)
{
    return ctrl_inb(WTCSR_R);
}

/**
 *     sh_wdt_write_csr - Write to Control/Status Register
 *     @val: Value to write
 *
 *     Writes the given value @val to the lower byte of the control/status
 *     register. The upper byte is set manually on each write.
 */
static inline void sh_wdt_write_csr(__u8 val)
{
    ctrl_outw((WTCSR_HIGH << 8) | (__u16)val, WTCSR);
}

#endif /* __KERNEL__ */
#endif /* __ASM_SH_WATCHDOG_H */
