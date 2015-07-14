/*
 * include/asm-arm/arch-pnx4008/platform.h
 *
 * PNX4008 Base addresses - header file
 *
 * Author: Dmitry Chigirev <source@mvista.com>
 *
 * Based on reference code received from Philips:
 * Copyright (C) 2003 Philips Semiconductors
 *
 * 2005 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */


#ifndef __ASM_ARCH_PLATFORM_H__
#define __ASM_ARCH_PLATFORM_H__

#define PNX4008_IRAM_BASE        0x08000000
#define PNX4008_IRAM_SIZE        0x00010000
#define PNX4008_YUV_SLAVE_BASE        0x10000000
#define PNX4008_DUM_SLAVE_BASE        0x18000000
#define PNX4008_NDF_FLASH_BASE        0x20020000
#define PNX4008_SPI1_BASE        0x20088000
#define PNX4008_SPI2_BASE        0x20090000
#define PNX4008_SD_CONFIG_BASE        0x20098000
#define PNX4008_FLASH_DATA        0x200B0000
#define PNX4008_MLC_FLASH_BASE        0x200B8000
#define PNX4008_JPEG_CONFIG_BASE    0x300A0000
#define PNX4008_DMA_CONFIG_BASE        0x31000000
#define PNX4008_USB_CONFIG_BASE        0x31020000
#define PNX4008_SDRAM_CFG_BASE        0x31080000
#define PNX4008_AHB2FAB_BASE        0x40000000
#define PNX4008_PWRMAN_BASE        0x40004000
#define PNX4008_INTCTRLMIC_BASE        0x40008000
#define PNX4008_INTCTRLSIC1_BASE    0x4000C000
#define PNX4008_INTCTRLSIC2_BASE    0x40010000
#define PNX4008_HSUART1_BASE        0x40014000
#define PNX4008_HSUART2_BASE        0x40018000
#define PNX4008_HSUART7_BASE        0x4001C000
#define PNX4008_RTC_BASE        0x40024000
#define PNX4008_PIO_BASE        0x40028000
#define PNX4008_MSTIMER_BASE        0x40034000
#define PNX4008_HSTIMER_BASE        0x40038000
#define PNX4008_WDOG_BASE        0x4003C000
#define PNX4008_DEBUG_BASE        0x40040000
#define PNX4008_TOUCH1_BASE        0x40048000
#define PNX4008_KEYSCAN_BASE        0x40050000
#define PNX4008_UARTCTRL_BASE        0x40054000
#define PNX4008_PWM_BASE        0x4005C000
#define PNX4008_UART3_BASE        0x40080000
#define PNX4008_UART4_BASE        0x40088000
#define PNX4008_UART5_BASE        0x40090000
#define PNX4008_UART6_BASE        0x40098000
#define PNX4008_I2C1_BASE        0x400A0000
#define PNX4008_I2C2_BASE        0x400A8000
#define PNX4008_MAGICGATE_BASE        0x400B0000
#define PNX4008_DUMCONF_BASE        0x400B8000
#define PNX4008_DUM_MAINCFG_BASE           0x400BC000
#define PNX4008_DSP_BASE        0x400C0000
#define PNX4008_PROFCOUNTER_BASE    0x400C8000
#define PNX4008_CRYPTO_BASE        0x400D0000
#define PNX4008_CAMIFCONF_BASE        0x400D8000
#define PNX4008_YUV2RGB_BASE        0x400E0000
#define PNX4008_AUDIOCONFIG_BASE    0x400E8000

#endif
