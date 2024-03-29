/*
 * arch/sh/kernel/cpu/sh2a/clock-sh7203.c
 *
 * SH7203 support for the clock framework
 *
 *  Copyright (C) 2007 Kieran Bingham (MPC-Data Ltd)
 *
 * Based on clock-sh7263.c
 *  Copyright (C) 2006  Yoshinori Sato
 *
 * Based on clock-sh4.c
 *  Copyright (C) 2005  Paul Mundt
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/clock.h>
#include <asm/freq.h>
#include <asm/io.h>

static const int pll1rate[]={8,12,16,0};
static const int pfc_divisors[]={1,2,3,4,6,8,12};
#define ifc_divisors pfc_divisors

#if (CONFIG_SH_CLK_MD == 0)
#define PLL2 (1)
#elif (CONFIG_SH_CLK_MD == 1)
#define PLL2 (2)
#elif (CONFIG_SH_CLK_MD == 2)
#define PLL2 (4)
#elif (CONFIG_SH_CLK_MD == 3)
#define PLL2 (4)
#else
#error "Illegal Clock Mode!"
#endif

static void master_clk_init(struct clk *clk)
{
    clk->rate *= pll1rate[(ctrl_inw(FREQCR) >> 8) & 0x0003] * PLL2 ;
}

static struct clk_ops sh7203_master_clk_ops = {
    .init        = master_clk_init,
};

static void module_clk_recalc(struct clk *clk)
{
    int idx = (ctrl_inw(FREQCR) & 0x0007);
    clk->rate = clk->parent->rate / pfc_divisors[idx];
}

static struct clk_ops sh7203_module_clk_ops = {
    .recalc        = module_clk_recalc,
};

static void bus_clk_recalc(struct clk *clk)
{
    int idx = (ctrl_inw(FREQCR) & 0x0007);
    clk->rate = clk->parent->rate / pfc_divisors[idx-2];
}

static struct clk_ops sh7203_bus_clk_ops = {
    .recalc        = bus_clk_recalc,
};

static void cpu_clk_recalc(struct clk *clk)
{
    clk->rate = clk->parent->rate;
}

static struct clk_ops sh7203_cpu_clk_ops = {
    .recalc        = cpu_clk_recalc,
};

static struct clk_ops *sh7203_clk_ops[] = {
    &sh7203_master_clk_ops,
    &sh7203_module_clk_ops,
    &sh7203_bus_clk_ops,
    &sh7203_cpu_clk_ops,
};

void __init arch_init_clk_ops(struct clk_ops **ops, int idx)
{
    if (idx < ARRAY_SIZE(sh7203_clk_ops))
        *ops = sh7203_clk_ops[idx];
}
