/*
 * Common hardware definitions
 *
 * Author: Kevin Hilman, MontaVista Software, Inc. <source@mvista.com>
 *
 * 2007 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

/*
 * Base register addresses
 */
#define DAVINCI_DMA_3PCC_BASE            (0x01C00000)
#define DAVINCI_DMA_3PTC0_BASE            (0x01C10000)
#define DAVINCI_DMA_3PTC1_BASE            (0x01C10400)
#define DAVINCI_I2C_BASE            (0x01C21000)
#define DAVINCI_PWM0_BASE            (0x01C22000)
#define DAVINCI_PWM1_BASE            (0x01C22400)
#define DAVINCI_PWM2_BASE            (0x01C22800)
#define DAVINCI_SYSTEM_MODULE_BASE        (0x01C40000)
#define DAVINCI_PLL_CNTRL0_BASE            (0x01C40800)
#define DAVINCI_PLL_CNTRL1_BASE            (0x01C40C00)
#define DAVINCI_PWR_SLEEP_CNTRL_BASE        (0x01C41000)
#define DAVINCI_SYSTEM_DFT_BASE            (0x01C42000)
#define DAVINCI_IEEE1394_BASE            (0x01C60000)
#define DAVINCI_USB_OTG_BASE            (0x01C64000)
#define DAVINCI_CFC_ATA_BASE            (0x01C66000)
#define DAVINCI_SPI_BASE            (0x01C66800)
#define DAVINCI_GPIO_BASE            (0x01C67000)
#define DAVINCI_UHPI_BASE            (0x01C67800)
#define DAVINCI_VPSS_REGS_BASE            (0x01C70000)
#define DAVINCI_EMAC_CNTRL_REGS_BASE        (0x01C80000)
#define DAVINCI_EMAC_WRAPPER_CNTRL_REGS_BASE    (0x01C81000)
#define DAVINCI_EMAC_WRAPPER_RAM_BASE        (0x01C82000)
#define DAVINCI_MDIO_CNTRL_REGS_BASE        (0x01C84000)
#define DAVINCI_IMCOP_BASE            (0x01CC0000)
#define DAVINCI_ASYNC_EMIF_CNTRL_BASE        (0x01E00000)
#define DAVINCI_VLYNQ_BASE            (0x01E01000)
#define DAVINCI_MCBSP_BASE            (0x01E02000)
#define DAVINCI_MMC_SD_BASE            (0x01E10000)
#define DAVINCI_MS_BASE                (0x01E20000)
#define DAVINCI_ASYNC_EMIF_DATA_CE0_BASE    (0x02000000)
#define DAVINCI_ASYNC_EMIF_DATA_CE1_BASE    (0x04000000)
#define DAVINCI_ASYNC_EMIF_DATA_CE2_BASE    (0x06000000)
#define DAVINCI_ASYNC_EMIF_DATA_CE3_BASE    (0x08000000)
#define DAVINCI_VLYNQ_REMOTE_BASE        (0x0C000000)

#endif /* __ASM_ARCH_HARDWARE_H */
