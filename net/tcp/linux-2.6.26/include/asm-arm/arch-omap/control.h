#ifndef __ASM_ARCH_CONTROL_H
#define __ASM_ARCH_CONTROL_H

/*
 * include/asm-arm/arch-omap/control.h
 *
 * OMAP2/3 System Control Module definitions
 *
 * Copyright (C) 2007 Texas Instruments, Inc.
 * Copyright (C) 2007 Nokia Corporation
 *
 * Written by Paul Walmsley
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#include <asm/arch/io.h>

#define OMAP242X_CTRL_REGADDR(reg)                    \
    (void __iomem *)IO_ADDRESS(OMAP242X_CTRL_BASE + (reg))
#define OMAP243X_CTRL_REGADDR(reg)                    \
    (void __iomem *)IO_ADDRESS(OMAP243X_CTRL_BASE + (reg))
#define OMAP343X_CTRL_REGADDR(reg)                    \
    (void __iomem *)IO_ADDRESS(OMAP343X_CTRL_BASE + (reg))

/*
 * As elsewhere, the "OMAP2_" prefix indicates that the macro is valid for
 * OMAP24XX and OMAP34XX.
 */

/* Control submodule offsets */

#define OMAP2_CONTROL_INTERFACE        0x000
#define OMAP2_CONTROL_PADCONFS        0x030
#define OMAP2_CONTROL_GENERAL        0x270
#define OMAP343X_CONTROL_MEM_WKUP    0x600
#define OMAP343X_CONTROL_PADCONFS_WKUP    0xa00
#define OMAP343X_CONTROL_GENERAL_WKUP    0xa60

/* Control register offsets - read/write with omap_ctrl_{read,write}{bwl}() */

#define OMAP2_CONTROL_SYSCONFIG        (OMAP2_CONTROL_INTERFACE + 0x10)

/* CONTROL_GENERAL register offsets common to OMAP2 & 3 */
#define OMAP2_CONTROL_DEVCONF0        (OMAP2_CONTROL_GENERAL + 0x0004)
#define OMAP2_CONTROL_MSUSPENDMUX_0    (OMAP2_CONTROL_GENERAL + 0x0020)
#define OMAP2_CONTROL_MSUSPENDMUX_1    (OMAP2_CONTROL_GENERAL + 0x0024)
#define OMAP2_CONTROL_MSUSPENDMUX_2    (OMAP2_CONTROL_GENERAL + 0x0028)
#define OMAP2_CONTROL_MSUSPENDMUX_3    (OMAP2_CONTROL_GENERAL + 0x002c)
#define OMAP2_CONTROL_MSUSPENDMUX_4    (OMAP2_CONTROL_GENERAL + 0x0030)
#define OMAP2_CONTROL_MSUSPENDMUX_5    (OMAP2_CONTROL_GENERAL + 0x0034)
#define OMAP2_CONTROL_SEC_CTRL        (OMAP2_CONTROL_GENERAL + 0x0040)
#define OMAP2_CONTROL_RPUB_KEY_H_0    (OMAP2_CONTROL_GENERAL + 0x0090)
#define OMAP2_CONTROL_RPUB_KEY_H_1    (OMAP2_CONTROL_GENERAL + 0x0094)
#define OMAP2_CONTROL_RPUB_KEY_H_2    (OMAP2_CONTROL_GENERAL + 0x0098)
#define OMAP2_CONTROL_RPUB_KEY_H_3    (OMAP2_CONTROL_GENERAL + 0x009c)

/* 242x-only CONTROL_GENERAL register offsets */
#define OMAP242X_CONTROL_DEVCONF    OMAP2_CONTROL_DEVCONF0 /* match TRM */
#define OMAP242X_CONTROL_OCM_RAM_PERM    (OMAP2_CONTROL_GENERAL + 0x0068)

/* 243x-only CONTROL_GENERAL register offsets */
/* CONTROL_IVA2_BOOT{ADDR,MOD} are at the same place on 343x - noted below */
#define OMAP243X_CONTROL_DEVCONF1    (OMAP2_CONTROL_GENERAL + 0x0078)
#define OMAP243X_CONTROL_CSIRXFE    (OMAP2_CONTROL_GENERAL + 0x007c)
#define OMAP243X_CONTROL_IVA2_BOOTADDR    (OMAP2_CONTROL_GENERAL + 0x0190)
#define OMAP243X_CONTROL_IVA2_BOOTMOD    (OMAP2_CONTROL_GENERAL + 0x0194)
#define OMAP243X_CONTROL_IVA2_GEMCFG    (OMAP2_CONTROL_GENERAL + 0x0198)

/* 24xx-only CONTROL_GENERAL register offsets */
#define OMAP24XX_CONTROL_DEBOBS        (OMAP2_CONTROL_GENERAL + 0x0000)
#define OMAP24XX_CONTROL_EMU_SUPPORT    (OMAP2_CONTROL_GENERAL + 0x0008)
#define OMAP24XX_CONTROL_SEC_TEST    (OMAP2_CONTROL_GENERAL + 0x0044)
#define OMAP24XX_CONTROL_PSA_CTRL    (OMAP2_CONTROL_GENERAL + 0x0048)
#define OMAP24XX_CONTROL_PSA_CMD    (OMAP2_CONTROL_GENERAL + 0x004c)
#define OMAP24XX_CONTROL_PSA_VALUE    (OMAP2_CONTROL_GENERAL + 0x0050)
#define OMAP24XX_CONTROL_SEC_EMU    (OMAP2_CONTROL_GENERAL + 0x0060)
#define OMAP24XX_CONTROL_SEC_TAP    (OMAP2_CONTROL_GENERAL + 0x0064)
#define OMAP24XX_CONTROL_OCM_PUB_RAM_ADD    (OMAP2_CONTROL_GENERAL + 0x006c)
#define OMAP24XX_CONTROL_EXT_SEC_RAM_START_ADD    (OMAP2_CONTROL_GENERAL + 0x0070)
#define OMAP24XX_CONTROL_EXT_SEC_RAM_STOP_ADD    (OMAP2_CONTROL_GENERAL + 0x0074)
#define OMAP24XX_CONTROL_SEC_STATUS        (OMAP2_CONTROL_GENERAL + 0x0080)
#define OMAP24XX_CONTROL_SEC_ERR_STATUS        (OMAP2_CONTROL_GENERAL + 0x0084)
#define OMAP24XX_CONTROL_STATUS            (OMAP2_CONTROL_GENERAL + 0x0088)
#define OMAP24XX_CONTROL_GENERAL_PURPOSE_STATUS    (OMAP2_CONTROL_GENERAL + 0x008c)
#define OMAP24XX_CONTROL_RAND_KEY_0    (OMAP2_CONTROL_GENERAL + 0x00a0)
#define OMAP24XX_CONTROL_RAND_KEY_1    (OMAP2_CONTROL_GENERAL + 0x00a4)
#define OMAP24XX_CONTROL_RAND_KEY_2    (OMAP2_CONTROL_GENERAL + 0x00a8)
#define OMAP24XX_CONTROL_RAND_KEY_3    (OMAP2_CONTROL_GENERAL + 0x00ac)
#define OMAP24XX_CONTROL_CUST_KEY_0    (OMAP2_CONTROL_GENERAL + 0x00b0)
#define OMAP24XX_CONTROL_CUST_KEY_1    (OMAP2_CONTROL_GENERAL + 0x00b4)
#define OMAP24XX_CONTROL_TEST_KEY_0    (OMAP2_CONTROL_GENERAL + 0x00c0)
#define OMAP24XX_CONTROL_TEST_KEY_1    (OMAP2_CONTROL_GENERAL + 0x00c4)
#define OMAP24XX_CONTROL_TEST_KEY_2    (OMAP2_CONTROL_GENERAL + 0x00c8)
#define OMAP24XX_CONTROL_TEST_KEY_3    (OMAP2_CONTROL_GENERAL + 0x00cc)
#define OMAP24XX_CONTROL_TEST_KEY_4    (OMAP2_CONTROL_GENERAL + 0x00d0)
#define OMAP24XX_CONTROL_TEST_KEY_5    (OMAP2_CONTROL_GENERAL + 0x00d4)
#define OMAP24XX_CONTROL_TEST_KEY_6    (OMAP2_CONTROL_GENERAL + 0x00d8)
#define OMAP24XX_CONTROL_TEST_KEY_7    (OMAP2_CONTROL_GENERAL + 0x00dc)
#define OMAP24XX_CONTROL_TEST_KEY_8    (OMAP2_CONTROL_GENERAL + 0x00e0)
#define OMAP24XX_CONTROL_TEST_KEY_9    (OMAP2_CONTROL_GENERAL + 0x00e4)

/* 34xx-only CONTROL_GENERAL register offsets */
#define OMAP343X_CONTROL_PADCONF_OFF    (OMAP2_CONTROL_GENERAL + 0x0000)
#define OMAP343X_CONTROL_MEM_DFTRW0    (OMAP2_CONTROL_GENERAL + 0x0008)
#define OMAP343X_CONTROL_MEM_DFTRW1    (OMAP2_CONTROL_GENERAL + 0x000c)
#define OMAP343X_CONTROL_DEVCONF1    (OMAP2_CONTROL_GENERAL + 0x0068)
#define OMAP343X_CONTROL_CSIRXFE        (OMAP2_CONTROL_GENERAL + 0x006c)
#define OMAP343X_CONTROL_SEC_STATUS        (OMAP2_CONTROL_GENERAL + 0x0070)
#define OMAP343X_CONTROL_SEC_ERR_STATUS        (OMAP2_CONTROL_GENERAL + 0x0074)
#define OMAP343X_CONTROL_SEC_ERR_STATUS_DEBUG    (OMAP2_CONTROL_GENERAL + 0x0078)
#define OMAP343X_CONTROL_STATUS            (OMAP2_CONTROL_GENERAL + 0x0080)
#define OMAP343X_CONTROL_GENERAL_PURPOSE_STATUS    (OMAP2_CONTROL_GENERAL + 0x0084)
#define OMAP343X_CONTROL_RPUB_KEY_H_4    (OMAP2_CONTROL_GENERAL + 0x00a0)
#define OMAP343X_CONTROL_RAND_KEY_0    (OMAP2_CONTROL_GENERAL + 0x00a8)
#define OMAP343X_CONTROL_RAND_KEY_1    (OMAP2_CONTROL_GENERAL + 0x00ac)
#define OMAP343X_CONTROL_RAND_KEY_2    (OMAP2_CONTROL_GENERAL + 0x00b0)
#define OMAP343X_CONTROL_RAND_KEY_3    (OMAP2_CONTROL_GENERAL + 0x00b4)
#define OMAP343X_CONTROL_TEST_KEY_0    (OMAP2_CONTROL_GENERAL + 0x00c8)
#define OMAP343X_CONTROL_TEST_KEY_1    (OMAP2_CONTROL_GENERAL + 0x00cc)
#define OMAP343X_CONTROL_TEST_KEY_2    (OMAP2_CONTROL_GENERAL + 0x00d0)
#define OMAP343X_CONTROL_TEST_KEY_3    (OMAP2_CONTROL_GENERAL + 0x00d4)
#define OMAP343X_CONTROL_TEST_KEY_4    (OMAP2_CONTROL_GENERAL + 0x00d8)
#define OMAP343X_CONTROL_TEST_KEY_5    (OMAP2_CONTROL_GENERAL + 0x00dc)
#define OMAP343X_CONTROL_TEST_KEY_6    (OMAP2_CONTROL_GENERAL + 0x00e0)
#define OMAP343X_CONTROL_TEST_KEY_7    (OMAP2_CONTROL_GENERAL + 0x00e4)
#define OMAP343X_CONTROL_TEST_KEY_8    (OMAP2_CONTROL_GENERAL + 0x00e8)
#define OMAP343X_CONTROL_TEST_KEY_9    (OMAP2_CONTROL_GENERAL + 0x00ec)
#define OMAP343X_CONTROL_TEST_KEY_10    (OMAP2_CONTROL_GENERAL + 0x00f0)
#define OMAP343X_CONTROL_TEST_KEY_11    (OMAP2_CONTROL_GENERAL + 0x00f4)
#define OMAP343X_CONTROL_TEST_KEY_12    (OMAP2_CONTROL_GENERAL + 0x00f8)
#define OMAP343X_CONTROL_TEST_KEY_13    (OMAP2_CONTROL_GENERAL + 0x00fc)
#define OMAP343X_CONTROL_IVA2_BOOTADDR    (OMAP2_CONTROL_GENERAL + 0x0190)
#define OMAP343X_CONTROL_IVA2_BOOTMOD    (OMAP2_CONTROL_GENERAL + 0x0194)

/*
 * REVISIT: This list of registers is not comprehensive - there are more
 * that should be added.
 */

/*
 * Control module register bit defines - these should eventually go into
 * their own regbits file.  Some of these will be complicated, depending
 * on the device type (general-purpose, emulator, test, secure, bad, other)
 * and the security mode (secure, non-secure, don't care)
 */
/* CONTROL_DEVCONF0 bits */
#define OMAP24XX_USBSTANDBYCTRL        (1 << 15)
#define OMAP2_MCBSP2_CLKS_MASK        (1 << 6)
#define OMAP2_MCBSP1_CLKS_MASK        (1 << 2)

/* CONTROL_DEVCONF1 bits */
#define OMAP2_MCBSP5_CLKS_MASK        (1 << 4) /* > 242x */
#define OMAP2_MCBSP4_CLKS_MASK        (1 << 2) /* > 242x */
#define OMAP2_MCBSP3_CLKS_MASK        (1 << 0) /* > 242x */

/* CONTROL_STATUS bits */
#define OMAP2_DEVICETYPE_MASK        (0x7 << 8)
#define OMAP2_SYSBOOT_5_MASK        (1 << 5)
#define OMAP2_SYSBOOT_4_MASK        (1 << 4)
#define OMAP2_SYSBOOT_3_MASK        (1 << 3)
#define OMAP2_SYSBOOT_2_MASK        (1 << 2)
#define OMAP2_SYSBOOT_1_MASK        (1 << 1)
#define OMAP2_SYSBOOT_0_MASK        (1 << 0)

#ifndef __ASSEMBLY__
#if defined(CONFIG_ARCH_OMAP2) || defined(CONFIG_ARCH_OMAP3)
extern void omap_ctrl_base_set(u32 base);
extern u32 omap_ctrl_base_get(void);
extern u8 omap_ctrl_readb(u16 offset);
extern u16 omap_ctrl_readw(u16 offset);
extern u32 omap_ctrl_readl(u16 offset);
extern void omap_ctrl_writeb(u8 val, u16 offset);
extern void omap_ctrl_writew(u16 val, u16 offset);
extern void omap_ctrl_writel(u32 val, u16 offset);
#else
#define omap_ctrl_base_set(x)        WARN_ON(1)
#define omap_ctrl_base_get()        0
#define omap_ctrl_readb(x)        0
#define omap_ctrl_readw(x)        0
#define omap_ctrl_readl(x)        0
#define omap_ctrl_writeb(x, y)        WARN_ON(1)
#define omap_ctrl_writew(x, y)        WARN_ON(1)
#define omap_ctrl_writel(x, y)        WARN_ON(1)
#endif
#endif    /* __ASSEMBLY__ */

#endif /* __ASM_ARCH_CONTROL_H */

