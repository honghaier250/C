/*
 * File:         include/asm-blackfin/bfin-global.h
 * Based on:
 * Author: *
 * Created:
 * Description:  Global extern defines for blackfin
 *
 * Modified:
 *               Copyright 2004-2006 Analog Devices Inc.
 *
 * Bugs:         Enter bugs at http://blackfin.uclinux.org/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _BFIN_GLOBAL_H_
#define _BFIN_GLOBAL_H_

#ifndef __ASSEMBLY__

#include <asm-generic/sections.h>
#include <asm/ptrace.h>
#include <asm/user.h>
#include <linux/linkage.h>
#include <linux/types.h>

#if defined(CONFIG_DMA_UNCACHED_4M)
# define DMA_UNCACHED_REGION (4 * 1024 * 1024)
#elif defined(CONFIG_DMA_UNCACHED_2M)
# define DMA_UNCACHED_REGION (2 * 1024 * 1024)
#elif defined(CONFIG_DMA_UNCACHED_1M)
# define DMA_UNCACHED_REGION (1024 * 1024)
#else
# define DMA_UNCACHED_REGION (0)
#endif

extern unsigned long get_cclk(void);
extern unsigned long get_sclk(void);
extern unsigned long sclk_to_usecs(unsigned long sclk);
extern unsigned long usecs_to_sclk(unsigned long usecs);

extern void dump_bfin_process(struct pt_regs *regs);
extern void dump_bfin_mem(struct pt_regs *regs);
extern void dump_bfin_trace_buffer(void);

extern int init_arch_irq(void);
extern void bfin_reset(void);
extern void _cplb_hdr(void);
/* Blackfin cache functions */
extern void bfin_icache_init(void);
extern void bfin_dcache_init(void);
extern int read_iloc(void);
extern int bfin_console_init(void);
extern asmlinkage void lower_to_irq14(void);
extern asmlinkage void bfin_return_from_exception(void);
extern void init_exception_vectors(void);
extern void init_dma(void);
extern void program_IAR(void);
extern void evt14_softirq(void);
extern asmlinkage void asm_do_IRQ(unsigned int irq, struct pt_regs *regs);
extern void bfin_gpio_interrupt_setup(int irq, int irq_pfx, int type);
extern int bfin_internal_set_wake(unsigned int irq, unsigned int state);

extern asmlinkage void finish_atomic_sections (struct pt_regs *regs);
extern char fixed_code_start;
extern char fixed_code_end;
extern int atomic_xchg32(void);
extern int atomic_cas32(void);
extern int atomic_add32(void);
extern int atomic_sub32(void);
extern int atomic_ior32(void);
extern int atomic_and32(void);
extern int atomic_xor32(void);
extern void safe_user_instruction(void);
extern void sigreturn_stub(void);

extern void *l1_data_A_sram_alloc(size_t);
extern void *l1_data_B_sram_alloc(size_t);
extern void *l1_inst_sram_alloc(size_t);
extern void *l1_data_sram_alloc(size_t);
extern void *l1_data_sram_zalloc(size_t);
extern int l1_data_A_sram_free(const void*);
extern int l1_data_B_sram_free(const void*);
extern int l1_inst_sram_free(const void*);
extern int l1_data_sram_free(const void*);
extern int sram_free(const void*);

#define L1_INST_SRAM        0x00000001
#define L1_DATA_A_SRAM        0x00000002
#define L1_DATA_B_SRAM        0x00000004
#define L1_DATA_SRAM        0x00000006
extern void *sram_alloc_with_lsl(size_t, unsigned long);
extern int sram_free_with_lsl(const void*);

extern const char bfin_board_name[];
extern unsigned long wall_jiffies;

extern unsigned long bfin_sic_iwr[];
extern u16 _bfin_swrst; /* shadow for Software Reset Register (SWRST) */
extern struct file_operations dpmc_fops;
extern unsigned long _ramstart, _ramend, _rambase;
extern unsigned long memory_start, memory_end, physical_mem_end;
extern char _stext_l1[], _etext_l1[], _sdata_l1[], _edata_l1[], _sbss_l1[],
    _ebss_l1[], _l1_lma_start[], _sdata_b_l1[], _ebss_b_l1[];

#ifdef CONFIG_MTD_UCLINUX
extern unsigned long memory_mtd_start, memory_mtd_end, mtd_size;
#endif

#endif

#endif                /* _BLACKFIN_H_ */
