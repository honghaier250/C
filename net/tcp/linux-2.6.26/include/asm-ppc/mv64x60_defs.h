/*
 * include/asm-ppc/mv64x60_defs.h
 *
 * Register definitions for the Marvell/Galileo GT64260, MV64360, etc.
 * host bridges.
 *
 * Author: Mark A. Greer <mgreer@mvista.com>
 *
 * 2001-2002 (c) MontaVista, Software, Inc.  This file is licensed under
 * the terms of the GNU General Public License version 2.  This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#ifndef __ASMPPC_MV64x60_DEFS_H
#define __ASMPPC_MV64x60_DEFS_H

/*
 * Define the Marvell bridges that are supported
 */
#define    MV64x60_TYPE_INVALID            0
#define    MV64x60_TYPE_GT64260A            1
#define    MV64x60_TYPE_GT64260B            2
#define    MV64x60_TYPE_MV64360            3
#define    MV64x60_TYPE_MV64361            4
#define    MV64x60_TYPE_MV64362            5
#define    MV64x60_TYPE_MV64460            6


/* Revisions of each supported chip */
#define    GT64260_REV_A                0x10
#define    GT64260_REV_B                0x20
#define    MV64360                    0x01
#define    MV64460                    0x01

/* Minimum window size supported by 64260 is 1MB */
#define GT64260_WINDOW_SIZE_MIN            0x00100000
#define MV64360_WINDOW_SIZE_MIN            0x00010000

#define    MV64x60_TCLK_FREQ_MAX            133333333U

/* IRQ's for embedded controllers */
#define    MV64x60_IRQ_DEV                1
#define    MV64x60_IRQ_CPU_ERR            3
#define    MV64x60_IRQ_TIMER_0_1            8
#define    MV64x60_IRQ_TIMER_2_3            9
#define    MV64x60_IRQ_TIMER_4_5            10
#define    MV64x60_IRQ_TIMER_6_7            11
#define    MV64x60_IRQ_P1_GPP_0_7            24
#define    MV64x60_IRQ_P1_GPP_8_15            25
#define    MV64x60_IRQ_P1_GPP_16_23        26
#define    MV64x60_IRQ_P1_GPP_24_31        27
#define    MV64x60_IRQ_DOORBELL            28
#define    MV64x60_IRQ_ETH_0            32
#define    MV64x60_IRQ_ETH_1            33
#define    MV64x60_IRQ_ETH_2            34
#define    MV64x60_IRQ_SDMA_0            36
#define    MV64x60_IRQ_I2C                37
#define    MV64x60_IRQ_BRG                39
#define    MV64x60_IRQ_MPSC_0            40
#define    MV64x60_IRQ_MPSC_1            42
#define    MV64x60_IRQ_COMM            43
#define    MV64x60_IRQ_P0_GPP_0_7            56
#define    MV64x60_IRQ_P0_GPP_8_15            57
#define    MV64x60_IRQ_P0_GPP_16_23        58
#define    MV64x60_IRQ_P0_GPP_24_31        59

#define    MV64360_IRQ_PCI0            12
#define    MV64360_IRQ_SRAM_PAR_ERR        13
#define    MV64360_IRQ_PCI1            16
#define    MV64360_IRQ_SDMA_1            38

#define    MV64x60_IRQ_GPP0            64
#define    MV64x60_IRQ_GPP1            65
#define    MV64x60_IRQ_GPP2            66
#define    MV64x60_IRQ_GPP3            67
#define    MV64x60_IRQ_GPP4            68
#define    MV64x60_IRQ_GPP5            69
#define    MV64x60_IRQ_GPP6            70
#define    MV64x60_IRQ_GPP7            71
#define    MV64x60_IRQ_GPP8            72
#define    MV64x60_IRQ_GPP9            73
#define    MV64x60_IRQ_GPP10            74
#define    MV64x60_IRQ_GPP11            75
#define    MV64x60_IRQ_GPP12            76
#define    MV64x60_IRQ_GPP13            77
#define    MV64x60_IRQ_GPP14            78
#define    MV64x60_IRQ_GPP15            79
#define    MV64x60_IRQ_GPP16            80
#define    MV64x60_IRQ_GPP17            81
#define    MV64x60_IRQ_GPP18            82
#define    MV64x60_IRQ_GPP19            83
#define    MV64x60_IRQ_GPP20            84
#define    MV64x60_IRQ_GPP21            85
#define    MV64x60_IRQ_GPP22            86
#define    MV64x60_IRQ_GPP23            87
#define    MV64x60_IRQ_GPP24            88
#define    MV64x60_IRQ_GPP25            89
#define    MV64x60_IRQ_GPP26            90
#define    MV64x60_IRQ_GPP27            91
#define    MV64x60_IRQ_GPP28            92
#define    MV64x60_IRQ_GPP29            93
#define    MV64x60_IRQ_GPP30            94
#define    MV64x60_IRQ_GPP31            95

/* Offsets for register blocks */
#define    GT64260_ENET_PHY_ADDR            0x2000
#define    GT64260_ENET_ESMIR            0x2010
#define GT64260_ENET_0_OFFSET            0x2400
#define GT64260_ENET_1_OFFSET            0x2800
#define GT64260_ENET_2_OFFSET            0x2c00
#define    MV64x60_SDMA_0_OFFSET            0x4000
#define    MV64x60_SDMA_1_OFFSET            0x6000
#define    MV64x60_MPSC_0_OFFSET            0x8000
#define    MV64x60_MPSC_1_OFFSET            0x9000
#define    MV64x60_MPSC_ROUTING_OFFSET        0xb400
#define    MV64x60_SDMA_INTR_OFFSET        0xb800
#define    MV64x60_BRG_0_OFFSET            0xb200
#define    MV64x60_BRG_1_OFFSET            0xb208

/*
 *****************************************************************************
 *
 *    CPU Interface Registers
 *
 *****************************************************************************
 */

/* CPU physical address of bridge's registers */
#define MV64x60_INTERNAL_SPACE_DECODE        0x0068
#define MV64x60_INTERNAL_SPACE_SIZE        0x10000
#define MV64x60_INTERNAL_SPACE_DEFAULT_ADDR    0x14000000

#define    MV64360_CPU_BAR_ENABLE            0x0278

/* CPU Memory Controller Window Registers (4 windows) */
#define    MV64x60_CPU2MEM_WINDOWS            4

#define    MV64x60_CPU2MEM_0_BASE            0x0008
#define    MV64x60_CPU2MEM_0_SIZE            0x0010
#define    MV64x60_CPU2MEM_1_BASE            0x0208
#define    MV64x60_CPU2MEM_1_SIZE            0x0210
#define    MV64x60_CPU2MEM_2_BASE            0x0018
#define    MV64x60_CPU2MEM_2_SIZE            0x0020
#define    MV64x60_CPU2MEM_3_BASE            0x0218
#define    MV64x60_CPU2MEM_3_SIZE            0x0220

/* CPU Device Controller Window Registers (4 windows) */
#define    MV64x60_CPU2DEV_WINDOWS            4

#define    MV64x60_CPU2DEV_0_BASE            0x0028
#define    MV64x60_CPU2DEV_0_SIZE            0x0030
#define    MV64x60_CPU2DEV_1_BASE            0x0228
#define    MV64x60_CPU2DEV_1_SIZE            0x0230
#define    MV64x60_CPU2DEV_2_BASE            0x0248
#define    MV64x60_CPU2DEV_2_SIZE            0x0250
#define    MV64x60_CPU2DEV_3_BASE            0x0038
#define    MV64x60_CPU2DEV_3_SIZE            0x0040

#define    MV64x60_CPU2BOOT_0_BASE            0x0238
#define    MV64x60_CPU2BOOT_0_SIZE            0x0240

#define    MV64360_CPU2SRAM_BASE            0x0268

/* CPU Windows to PCI space (2 PCI buses each w/ 1 I/O & 4 MEM windows) */
#define    MV64x60_PCI_BUSES            2
#define    MV64x60_PCI_IO_WINDOWS_PER_BUS        1
#define    MV64x60_PCI_MEM_WINDOWS_PER_BUS        4

#define    MV64x60_CPU2PCI_SWAP_BYTE        0x00000000
#define    MV64x60_CPU2PCI_SWAP_NONE        0x01000000
#define    MV64x60_CPU2PCI_SWAP_BYTE_WORD        0x02000000
#define    MV64x60_CPU2PCI_SWAP_WORD        0x03000000

#define    MV64x60_CPU2PCI_MEM_REQ64        (1<<27)

#define    MV64x60_CPU2PCI0_IO_BASE        0x0048
#define    MV64x60_CPU2PCI0_IO_SIZE        0x0050
#define    MV64x60_CPU2PCI0_MEM_0_BASE        0x0058
#define    MV64x60_CPU2PCI0_MEM_0_SIZE        0x0060
#define    MV64x60_CPU2PCI0_MEM_1_BASE        0x0080
#define    MV64x60_CPU2PCI0_MEM_1_SIZE        0x0088
#define    MV64x60_CPU2PCI0_MEM_2_BASE        0x0258
#define    MV64x60_CPU2PCI0_MEM_2_SIZE        0x0260
#define    MV64x60_CPU2PCI0_MEM_3_BASE        0x0280
#define    MV64x60_CPU2PCI0_MEM_3_SIZE        0x0288

#define    MV64x60_CPU2PCI0_IO_REMAP        0x00f0
#define    MV64x60_CPU2PCI0_MEM_0_REMAP_LO        0x00f8
#define    MV64x60_CPU2PCI0_MEM_0_REMAP_HI        0x0320
#define    MV64x60_CPU2PCI0_MEM_1_REMAP_LO        0x0100
#define    MV64x60_CPU2PCI0_MEM_1_REMAP_HI        0x0328
#define    MV64x60_CPU2PCI0_MEM_2_REMAP_LO        0x02f8
#define    MV64x60_CPU2PCI0_MEM_2_REMAP_HI        0x0330
#define    MV64x60_CPU2PCI0_MEM_3_REMAP_LO        0x0300
#define    MV64x60_CPU2PCI0_MEM_3_REMAP_HI        0x0338

#define    MV64x60_CPU2PCI1_IO_BASE        0x0090
#define    MV64x60_CPU2PCI1_IO_SIZE        0x0098
#define    MV64x60_CPU2PCI1_MEM_0_BASE        0x00a0
#define    MV64x60_CPU2PCI1_MEM_0_SIZE        0x00a8
#define    MV64x60_CPU2PCI1_MEM_1_BASE        0x00b0
#define    MV64x60_CPU2PCI1_MEM_1_SIZE        0x00b8
#define    MV64x60_CPU2PCI1_MEM_2_BASE        0x02a0
#define    MV64x60_CPU2PCI1_MEM_2_SIZE        0x02a8
#define    MV64x60_CPU2PCI1_MEM_3_BASE        0x02b0
#define    MV64x60_CPU2PCI1_MEM_3_SIZE        0x02b8

#define    MV64x60_CPU2PCI1_IO_REMAP        0x0108
#define    MV64x60_CPU2PCI1_MEM_0_REMAP_LO        0x0110
#define    MV64x60_CPU2PCI1_MEM_0_REMAP_HI        0x0340
#define    MV64x60_CPU2PCI1_MEM_1_REMAP_LO        0x0118
#define    MV64x60_CPU2PCI1_MEM_1_REMAP_HI        0x0348
#define    MV64x60_CPU2PCI1_MEM_2_REMAP_LO        0x0310
#define    MV64x60_CPU2PCI1_MEM_2_REMAP_HI        0x0350
#define    MV64x60_CPU2PCI1_MEM_3_REMAP_LO        0x0318
#define    MV64x60_CPU2PCI1_MEM_3_REMAP_HI        0x0358

/* CPU Control Registers */
#define MV64x60_CPU_CONFIG            0x0000
#define MV64x60_CPU_MODE            0x0120
#define MV64x60_CPU_MASTER_CNTL            0x0160
#define MV64x60_CPU_XBAR_CNTL_LO        0x0150
#define MV64x60_CPU_XBAR_CNTL_HI        0x0158
#define MV64x60_CPU_XBAR_TO            0x0168

#define GT64260_CPU_RR_XBAR_CNTL_LO        0x0170
#define GT64260_CPU_RR_XBAR_CNTL_HI        0x0178

#define MV64360_CPU_PADS_CALIBRATION        0x03b4
#define MV64360_CPU_RESET_SAMPLE_LO        0x03c4
#define MV64360_CPU_RESET_SAMPLE_HI        0x03d4

/* SMP Register Map */
#define MV64360_WHO_AM_I            0x0200
#define MV64360_CPU0_DOORBELL            0x0214
#define MV64360_CPU0_DOORBELL_CLR        0x021c
#define MV64360_CPU0_DOORBELL_MASK        0x0234
#define MV64360_CPU1_DOORBELL            0x0224
#define MV64360_CPU1_DOORBELL_CLR        0x022c
#define MV64360_CPU1_DOORBELL_MASK        0x023c
#define MV64360_CPUx_DOORBELL(x)        (0x0214 + ((x)*0x10))
#define MV64360_CPUx_DOORBELL_CLR(x)        (0x021c + ((x)*0x10))
#define MV64360_CPUx_DOORBELL_MASK(x)        (0x0234 + ((x)*0x08))
#define MV64360_SEMAPHORE_0            0x0244
#define MV64360_SEMAPHORE_1            0x024c
#define MV64360_SEMAPHORE_2            0x0254
#define MV64360_SEMAPHORE_3            0x025c
#define MV64360_SEMAPHORE_4            0x0264
#define MV64360_SEMAPHORE_5            0x026c
#define MV64360_SEMAPHORE_6            0x0274
#define MV64360_SEMAPHORE_7            0x027c

/* CPU Sync Barrier Registers */
#define GT64260_CPU_SYNC_BARRIER_PCI0        0x00c0
#define GT64260_CPU_SYNC_BARRIER_PCI1        0x00c8

#define MV64360_CPU0_SYNC_BARRIER_TRIG        0x00c0
#define MV64360_CPU0_SYNC_BARRIER_VIRT        0x00c8
#define MV64360_CPU1_SYNC_BARRIER_TRIG        0x00d0
#define MV64360_CPU1_SYNC_BARRIER_VIRT        0x00d8

/* CPU Deadlock and Ordering registers (Rev B part only) */
#define GT64260_CPU_DEADLOCK_ORDERING            0x02d0
#define GT64260_CPU_WB_PRIORITY_BUFFER_DEPTH        0x02d8
#define GT64260_CPU_COUNTERS_SYNC_BARRIER_ATTRIBUTE    0x02e0

/* CPU Access Protection Registers (gt64260 realy has 8 but don't need) */
#define    MV64x260_CPU_PROT_WINDOWS        4

#define    GT64260_CPU_PROT_ACCPROTECT        (1<<16)
#define    GT64260_CPU_PROT_WRPROTECT        (1<<17)
#define    GT64260_CPU_PROT_CACHEPROTECT        (1<<18)

#define    MV64360_CPU_PROT_ACCPROTECT        (1<<20)
#define    MV64360_CPU_PROT_WRPROTECT        (1<<21)
#define    MV64360_CPU_PROT_CACHEPROTECT        (1<<22)
#define    MV64360_CPU_PROT_WIN_ENABLE        (1<<31)

#define MV64x60_CPU_PROT_BASE_0            0x0180
#define MV64x60_CPU_PROT_SIZE_0            0x0188
#define MV64x60_CPU_PROT_BASE_1            0x0190
#define MV64x60_CPU_PROT_SIZE_1            0x0198
#define MV64x60_CPU_PROT_BASE_2            0x01a0
#define MV64x60_CPU_PROT_SIZE_2            0x01a8
#define MV64x60_CPU_PROT_BASE_3            0x01b0
#define MV64x60_CPU_PROT_SIZE_3            0x01b8

#define GT64260_CPU_PROT_BASE_4            0x01c0
#define GT64260_CPU_PROT_SIZE_4            0x01c8
#define GT64260_CPU_PROT_BASE_5            0x01d0
#define GT64260_CPU_PROT_SIZE_5            0x01d8
#define GT64260_CPU_PROT_BASE_6            0x01e0
#define GT64260_CPU_PROT_SIZE_6            0x01e8
#define GT64260_CPU_PROT_BASE_7            0x01f0
#define GT64260_CPU_PROT_SIZE_7            0x01f8

/* CPU Snoop Control Registers (64260 only) */
#define    GT64260_CPU_SNOOP_WINDOWS        4

#define    GT64260_CPU_SNOOP_NONE            0x00000000
#define    GT64260_CPU_SNOOP_WT            0x00010000
#define    GT64260_CPU_SNOOP_WB            0x00020000
#define    GT64260_CPU_SNOOP_MASK            0x00030000
#define    GT64260_CPU_SNOOP_ALL_BITS        GT64260_CPU_SNOOP_MASK

#define GT64260_CPU_SNOOP_BASE_0        0x0380
#define GT64260_CPU_SNOOP_SIZE_0        0x0388
#define GT64260_CPU_SNOOP_BASE_1        0x0390
#define GT64260_CPU_SNOOP_SIZE_1        0x0398
#define GT64260_CPU_SNOOP_BASE_2        0x03a0
#define GT64260_CPU_SNOOP_SIZE_2        0x03a8
#define GT64260_CPU_SNOOP_BASE_3        0x03b0
#define GT64260_CPU_SNOOP_SIZE_3        0x03b8

/* CPU Snoop Control Registers (64360 only) */
#define    MV64360_CPU_SNOOP_WINDOWS        4
#define    MV64360_CPU_SNOOP_NONE            0x00000000
#define    MV64360_CPU_SNOOP_WT            0x00010000
#define    MV64360_CPU_SNOOP_WB            0x00020000
#define    MV64360_CPU_SNOOP_MASK            0x00030000
#define    MV64360_CPU_SNOOP_ALL_BITS        MV64360_CPU_SNOOP_MASK


/* CPU Error Report Registers */
#define MV64x60_CPU_ERR_ADDR_LO            0x0070
#define MV64x60_CPU_ERR_ADDR_HI            0x0078
#define MV64x60_CPU_ERR_DATA_LO            0x0128
#define MV64x60_CPU_ERR_DATA_HI            0x0130
#define MV64x60_CPU_ERR_PARITY            0x0138
#define MV64x60_CPU_ERR_CAUSE            0x0140
#define MV64x60_CPU_ERR_MASK            0x0148

/*
 *****************************************************************************
 *
 *    SRAM Controller Registers
 *
 *****************************************************************************
 */

#define    MV64360_SRAM_CONFIG            0x0380
#define    MV64360_SRAM_TEST_MODE            0x03f4
#define    MV64360_SRAM_ERR_CAUSE            0x0388
#define    MV64360_SRAM_ERR_ADDR_LO        0x0390
#define    MV64360_SRAM_ERR_ADDR_HI        0x03f8
#define    MV64360_SRAM_ERR_DATA_LO        0x0398
#define    MV64360_SRAM_ERR_DATA_HI        0x03a0
#define    MV64360_SRAM_ERR_PARITY            0x03a8

#define    MV64360_SRAM_SIZE            0x00040000 /* 2Mb/256KB SRAM */

/*
 *****************************************************************************
 *
 *    SDRAM/MEM Controller Registers
 *
 *****************************************************************************
 */

/* SDRAM Config Registers (64260) */
#define    GT64260_SDRAM_CONFIG            0x0448

/* SDRAM Error Report Registers (64260) */
#define    GT64260_SDRAM_ERR_DATA_LO        0x0484
#define    GT64260_SDRAM_ERR_DATA_HI        0x0480
#define    GT64260_SDRAM_ERR_ADDR            0x0490
#define    GT64260_SDRAM_ERR_ECC_RCVD        0x0488
#define    GT64260_SDRAM_ERR_ECC_CALC        0x048c
#define    GT64260_SDRAM_ERR_ECC_CNTL        0x0494
#define    GT64260_SDRAM_ERR_ECC_ERR_CNT        0x0498

/* SDRAM Config Registers (64360) */
#define    MV64360_SDRAM_CONFIG            0x1400

/* SDRAM Control Registers */
#define MV64360_D_UNIT_CONTROL_LOW        0x1404
#define MV64360_D_UNIT_CONTROL_HIGH        0x1424
#define MV64460_D_UNIT_MMASK            0x14b0

/* SDRAM Error Report Registers (64360) */
#define    MV64360_SDRAM_ERR_DATA_LO        0x1444
#define    MV64360_SDRAM_ERR_DATA_HI        0x1440
#define    MV64360_SDRAM_ERR_ADDR            0x1450
#define    MV64360_SDRAM_ERR_ECC_RCVD        0x1448
#define    MV64360_SDRAM_ERR_ECC_CALC        0x144c
#define    MV64360_SDRAM_ERR_ECC_CNTL        0x1454
#define    MV64360_SDRAM_ERR_ECC_ERR_CNT        0x1458

/*
 *****************************************************************************
 *
 *    Device/BOOT Controller Registers
 *
 *****************************************************************************
 */

/* Device Control Registers */
#define    MV64x60_DEV_BANK_PARAMS_0        0x045c
#define    MV64x60_DEV_BANK_PARAMS_1        0x0460
#define    MV64x60_DEV_BANK_PARAMS_2        0x0464
#define    MV64x60_DEV_BANK_PARAMS_3        0x0468
#define    MV64x60_DEV_BOOT_PARAMS            0x046c
#define    MV64x60_DEV_IF_CNTL            0x04c0
#define    MV64x60_DEV_IF_XBAR_CNTL_LO        0x04c8
#define    MV64x60_DEV_IF_XBAR_CNTL_HI        0x04cc
#define    MV64x60_DEV_IF_XBAR_CNTL_TO        0x04c4

/* Device Interrupt Registers */
#define    MV64x60_DEV_INTR_CAUSE            0x04d0
#define    MV64x60_DEV_INTR_MASK            0x04d4
#define    MV64x60_DEV_INTR_ERR_ADDR        0x04d8

#define    MV64360_DEV_INTR_ERR_DATA        0x04dc
#define    MV64360_DEV_INTR_ERR_PAR        0x04e0

/*
 *****************************************************************************
 *
 *    PCI Bridge Interface Registers
 *
 *****************************************************************************
 */

/* PCI Configuration Access Registers */
#define    MV64x60_PCI0_CONFIG_ADDR        0x0cf8
#define    MV64x60_PCI0_CONFIG_DATA        0x0cfc
#define    MV64x60_PCI0_IACK            0x0c34

#define    MV64x60_PCI1_CONFIG_ADDR        0x0c78
#define    MV64x60_PCI1_CONFIG_DATA        0x0c7c
#define    MV64x60_PCI1_IACK            0x0cb4

/* PCI Control Registers */
#define    MV64x60_PCI0_CMD            0x0c00
#define    MV64x60_PCI0_MODE            0x0d00
#define    MV64x60_PCI0_TO_RETRY            0x0c04
#define    MV64x60_PCI0_RD_BUF_DISCARD_TIMER    0x0d04
#define    MV64x60_PCI0_MSI_TRIGGER_TIMER        0x0c38
#define    MV64x60_PCI0_ARBITER_CNTL        0x1d00
#define    MV64x60_PCI0_XBAR_CNTL_LO        0x1d08
#define    MV64x60_PCI0_XBAR_CNTL_HI        0x1d0c
#define    MV64x60_PCI0_XBAR_CNTL_TO        0x1d04
#define    MV64x60_PCI0_RD_RESP_XBAR_CNTL_LO    0x1d18
#define    MV64x60_PCI0_RD_RESP_XBAR_CNTL_HI    0x1d1c
#define    MV64x60_PCI0_SYNC_BARRIER        0x1d10
#define    MV64x60_PCI0_P2P_CONFIG            0x1d14
#define    MV64x60_PCI0_INTR_MASK

#define    GT64260_PCI0_P2P_SWAP_CNTL        0x1d54

#define    MV64x60_PCI1_CMD            0x0c80
#define    MV64x60_PCI1_MODE            0x0d80
#define    MV64x60_PCI1_TO_RETRY            0x0c84
#define    MV64x60_PCI1_RD_BUF_DISCARD_TIMER    0x0d84
#define    MV64x60_PCI1_MSI_TRIGGER_TIMER        0x0cb8
#define    MV64x60_PCI1_ARBITER_CNTL        0x1d80
#define    MV64x60_PCI1_XBAR_CNTL_LO        0x1d88
#define    MV64x60_PCI1_XBAR_CNTL_HI        0x1d8c
#define    MV64x60_PCI1_XBAR_CNTL_TO        0x1d84
#define    MV64x60_PCI1_RD_RESP_XBAR_CNTL_LO    0x1d98
#define    MV64x60_PCI1_RD_RESP_XBAR_CNTL_HI    0x1d9c
#define    MV64x60_PCI1_SYNC_BARRIER        0x1d90
#define    MV64x60_PCI1_P2P_CONFIG            0x1d94

#define    GT64260_PCI1_P2P_SWAP_CNTL        0x1dd4

/* Different modes that the pci hoses can be in (bits 5:4 in PCI Mode reg) */
#define    MV64x60_PCIMODE_CONVENTIONAL        0
#define    MV64x60_PCIMODE_PCIX_66            (1 << 4)
#define    MV64x60_PCIMODE_PCIX_100        (2 << 4)
#define    MV64x60_PCIMODE_PCIX_133        (3 << 4)
#define    MV64x60_PCIMODE_MASK            (0x3 << 4)

/* PCI Access Control Regions Registers */
#define    GT64260_PCI_ACC_CNTL_PREFETCHEN        (1<<12)
#define    GT64260_PCI_ACC_CNTL_DREADEN        (1<<13)
#define    GT64260_PCI_ACC_CNTL_RDPREFETCH        (1<<16)
#define    GT64260_PCI_ACC_CNTL_RDLINEPREFETCH    (1<<17)
#define    GT64260_PCI_ACC_CNTL_RDMULPREFETCH    (1<<18)
#define    GT64260_PCI_ACC_CNTL_MBURST_32_BTYES    0x00000000
#define    GT64260_PCI_ACC_CNTL_MBURST_64_BYTES    0x00100000
#define    GT64260_PCI_ACC_CNTL_MBURST_128_BYTES    0x00200000
#define    GT64260_PCI_ACC_CNTL_MBURST_MASK    0x00300000
#define    GT64260_PCI_ACC_CNTL_SWAP_BYTE        0x00000000
#define    GT64260_PCI_ACC_CNTL_SWAP_NONE        0x01000000
#define    GT64260_PCI_ACC_CNTL_SWAP_BYTE_WORD    0x02000000
#define    GT64260_PCI_ACC_CNTL_SWAP_WORD        0x03000000
#define    GT64260_PCI_ACC_CNTL_SWAP_MASK        0x03000000
#define    GT64260_PCI_ACC_CNTL_ACCPROT        (1<<28)
#define    GT64260_PCI_ACC_CNTL_WRPROT        (1<<29)

#define    GT64260_PCI_ACC_CNTL_ALL_BITS    (GT64260_PCI_ACC_CNTL_PREFETCHEN |    \
                     GT64260_PCI_ACC_CNTL_DREADEN |       \
                     GT64260_PCI_ACC_CNTL_RDPREFETCH |    \
                     GT64260_PCI_ACC_CNTL_RDLINEPREFETCH |\
                     GT64260_PCI_ACC_CNTL_RDMULPREFETCH | \
                     GT64260_PCI_ACC_CNTL_MBURST_MASK |   \
                     GT64260_PCI_ACC_CNTL_SWAP_MASK |     \
                     GT64260_PCI_ACC_CNTL_ACCPROT|        \
                     GT64260_PCI_ACC_CNTL_WRPROT)

#define    MV64360_PCI_ACC_CNTL_ENABLE        (1<<0)
#define    MV64360_PCI_ACC_CNTL_REQ64        (1<<1)
#define    MV64360_PCI_ACC_CNTL_SNOOP_NONE        0x00000000
#define    MV64360_PCI_ACC_CNTL_SNOOP_WT        0x00000004
#define    MV64360_PCI_ACC_CNTL_SNOOP_WB        0x00000008
#define    MV64360_PCI_ACC_CNTL_SNOOP_MASK        0x0000000c
#define    MV64360_PCI_ACC_CNTL_ACCPROT        (1<<4)
#define    MV64360_PCI_ACC_CNTL_WRPROT        (1<<5)
#define    MV64360_PCI_ACC_CNTL_SWAP_BYTE        0x00000000
#define    MV64360_PCI_ACC_CNTL_SWAP_NONE        0x00000040
#define    MV64360_PCI_ACC_CNTL_SWAP_BYTE_WORD    0x00000080
#define    MV64360_PCI_ACC_CNTL_SWAP_WORD        0x000000c0
#define    MV64360_PCI_ACC_CNTL_SWAP_MASK        0x000000c0
#define    MV64360_PCI_ACC_CNTL_MBURST_32_BYTES    0x00000000
#define    MV64360_PCI_ACC_CNTL_MBURST_64_BYTES    0x00000100
#define    MV64360_PCI_ACC_CNTL_MBURST_128_BYTES    0x00000200
#define    MV64360_PCI_ACC_CNTL_MBURST_MASK    0x00000300
#define    MV64360_PCI_ACC_CNTL_RDSIZE_32_BYTES    0x00000000
#define    MV64360_PCI_ACC_CNTL_RDSIZE_64_BYTES    0x00000400
#define    MV64360_PCI_ACC_CNTL_RDSIZE_128_BYTES    0x00000800
#define    MV64360_PCI_ACC_CNTL_RDSIZE_256_BYTES    0x00000c00
#define    MV64360_PCI_ACC_CNTL_RDSIZE_MASK    0x00000c00

#define    MV64360_PCI_ACC_CNTL_ALL_BITS    (MV64360_PCI_ACC_CNTL_ENABLE |        \
                     MV64360_PCI_ACC_CNTL_REQ64 |        \
                     MV64360_PCI_ACC_CNTL_SNOOP_MASK |  \
                     MV64360_PCI_ACC_CNTL_ACCPROT |        \
                     MV64360_PCI_ACC_CNTL_WRPROT |        \
                     MV64360_PCI_ACC_CNTL_SWAP_MASK |   \
                     MV64360_PCI_ACC_CNTL_MBURST_MASK | \
                     MV64360_PCI_ACC_CNTL_RDSIZE_MASK)

#define    MV64x60_PCI0_ACC_CNTL_0_BASE_LO        0x1e00
#define    MV64x60_PCI0_ACC_CNTL_0_BASE_HI        0x1e04
#define    MV64x60_PCI0_ACC_CNTL_0_SIZE        0x1e08
#define    MV64x60_PCI0_ACC_CNTL_1_BASE_LO        0x1e10
#define    MV64x60_PCI0_ACC_CNTL_1_BASE_HI        0x1e14
#define    MV64x60_PCI0_ACC_CNTL_1_SIZE        0x1e18
#define    MV64x60_PCI0_ACC_CNTL_2_BASE_LO        0x1e20
#define    MV64x60_PCI0_ACC_CNTL_2_BASE_HI        0x1e24
#define    MV64x60_PCI0_ACC_CNTL_2_SIZE        0x1e28
#define    MV64x60_PCI0_ACC_CNTL_3_BASE_LO        0x1e30
#define    MV64x60_PCI0_ACC_CNTL_3_BASE_HI        0x1e34
#define    MV64x60_PCI0_ACC_CNTL_3_SIZE        0x1e38
#define    MV64x60_PCI0_ACC_CNTL_4_BASE_LO        0x1e40
#define    MV64x60_PCI0_ACC_CNTL_4_BASE_HI        0x1e44
#define    MV64x60_PCI0_ACC_CNTL_4_SIZE        0x1e48
#define    MV64x60_PCI0_ACC_CNTL_5_BASE_LO        0x1e50
#define    MV64x60_PCI0_ACC_CNTL_5_BASE_HI        0x1e54
#define    MV64x60_PCI0_ACC_CNTL_5_SIZE        0x1e58

#define    GT64260_PCI0_ACC_CNTL_6_BASE_LO        0x1e60
#define    GT64260_PCI0_ACC_CNTL_6_BASE_HI        0x1e64
#define    GT64260_PCI0_ACC_CNTL_6_SIZE        0x1e68
#define    GT64260_PCI0_ACC_CNTL_7_BASE_LO        0x1e70
#define    GT64260_PCI0_ACC_CNTL_7_BASE_HI        0x1e74
#define    GT64260_PCI0_ACC_CNTL_7_SIZE        0x1e78

#define    MV64x60_PCI1_ACC_CNTL_0_BASE_LO        0x1e80
#define    MV64x60_PCI1_ACC_CNTL_0_BASE_HI        0x1e84
#define    MV64x60_PCI1_ACC_CNTL_0_SIZE        0x1e88
#define    MV64x60_PCI1_ACC_CNTL_1_BASE_LO        0x1e90
#define    MV64x60_PCI1_ACC_CNTL_1_BASE_HI        0x1e94
#define    MV64x60_PCI1_ACC_CNTL_1_SIZE        0x1e98
#define    MV64x60_PCI1_ACC_CNTL_2_BASE_LO        0x1ea0
#define    MV64x60_PCI1_ACC_CNTL_2_BASE_HI        0x1ea4
#define    MV64x60_PCI1_ACC_CNTL_2_SIZE        0x1ea8
#define    MV64x60_PCI1_ACC_CNTL_3_BASE_LO        0x1eb0
#define    MV64x60_PCI1_ACC_CNTL_3_BASE_HI        0x1eb4
#define    MV64x60_PCI1_ACC_CNTL_3_SIZE        0x1eb8
#define    MV64x60_PCI1_ACC_CNTL_4_BASE_LO        0x1ec0
#define    MV64x60_PCI1_ACC_CNTL_4_BASE_HI        0x1ec4
#define    MV64x60_PCI1_ACC_CNTL_4_SIZE        0x1ec8
#define    MV64x60_PCI1_ACC_CNTL_5_BASE_LO        0x1ed0
#define    MV64x60_PCI1_ACC_CNTL_5_BASE_HI        0x1ed4
#define    MV64x60_PCI1_ACC_CNTL_5_SIZE        0x1ed8

#define    GT64260_PCI1_ACC_CNTL_6_BASE_LO        0x1ee0
#define    GT64260_PCI1_ACC_CNTL_6_BASE_HI        0x1ee4
#define    GT64260_PCI1_ACC_CNTL_6_SIZE        0x1ee8
#define    GT64260_PCI1_ACC_CNTL_7_BASE_LO        0x1ef0
#define    GT64260_PCI1_ACC_CNTL_7_BASE_HI        0x1ef4
#define    GT64260_PCI1_ACC_CNTL_7_SIZE        0x1ef8

/* PCI Snoop Control Registers (64260 only) */
#define    GT64260_PCI_SNOOP_NONE            0x00000000
#define    GT64260_PCI_SNOOP_WT            0x00001000
#define    GT64260_PCI_SNOOP_WB            0x00002000

#define    GT64260_PCI0_SNOOP_0_BASE_LO        0x1f00
#define    GT64260_PCI0_SNOOP_0_BASE_HI        0x1f04
#define    GT64260_PCI0_SNOOP_0_SIZE        0x1f08
#define    GT64260_PCI0_SNOOP_1_BASE_LO        0x1f10
#define    GT64260_PCI0_SNOOP_1_BASE_HI        0x1f14
#define    GT64260_PCI0_SNOOP_1_SIZE        0x1f18
#define    GT64260_PCI0_SNOOP_2_BASE_LO        0x1f20
#define    GT64260_PCI0_SNOOP_2_BASE_HI        0x1f24
#define    GT64260_PCI0_SNOOP_2_SIZE        0x1f28
#define    GT64260_PCI0_SNOOP_3_BASE_LO        0x1f30
#define    GT64260_PCI0_SNOOP_3_BASE_HI        0x1f34
#define    GT64260_PCI0_SNOOP_3_SIZE        0x1f38

#define    GT64260_PCI1_SNOOP_0_BASE_LO        0x1f80
#define    GT64260_PCI1_SNOOP_0_BASE_HI        0x1f84
#define    GT64260_PCI1_SNOOP_0_SIZE        0x1f88
#define    GT64260_PCI1_SNOOP_1_BASE_LO        0x1f90
#define    GT64260_PCI1_SNOOP_1_BASE_HI        0x1f94
#define    GT64260_PCI1_SNOOP_1_SIZE        0x1f98
#define    GT64260_PCI1_SNOOP_2_BASE_LO        0x1fa0
#define    GT64260_PCI1_SNOOP_2_BASE_HI        0x1fa4
#define    GT64260_PCI1_SNOOP_2_SIZE        0x1fa8
#define    GT64260_PCI1_SNOOP_3_BASE_LO        0x1fb0
#define    GT64260_PCI1_SNOOP_3_BASE_HI        0x1fb4
#define    GT64260_PCI1_SNOOP_3_SIZE        0x1fb8

/* PCI Error Report Registers */
#define MV64x60_PCI0_ERR_SERR_MASK        0x0c28
#define MV64x60_PCI0_ERR_ADDR_LO        0x1d40
#define MV64x60_PCI0_ERR_ADDR_HI        0x1d44
#define MV64x60_PCI0_ERR_DATA_LO        0x1d48
#define MV64x60_PCI0_ERR_DATA_HI        0x1d4c
#define MV64x60_PCI0_ERR_CMD            0x1d50
#define MV64x60_PCI0_ERR_CAUSE            0x1d58
#define MV64x60_PCI0_ERR_MASK            0x1d5c

#define MV64x60_PCI1_ERR_SERR_MASK        0x0ca8
#define MV64x60_PCI1_ERR_ADDR_LO        0x1dc0
#define MV64x60_PCI1_ERR_ADDR_HI        0x1dc4
#define MV64x60_PCI1_ERR_DATA_LO        0x1dc8
#define MV64x60_PCI1_ERR_DATA_HI        0x1dcc
#define MV64x60_PCI1_ERR_CMD            0x1dd0
#define MV64x60_PCI1_ERR_CAUSE            0x1dd8
#define MV64x60_PCI1_ERR_MASK            0x1ddc

/* PCI Slave Address Decoding Registers */
#define    MV64x60_PCI0_MEM_0_SIZE            0x0c08
#define    MV64x60_PCI0_MEM_1_SIZE            0x0d08
#define    MV64x60_PCI0_MEM_2_SIZE            0x0c0c
#define    MV64x60_PCI0_MEM_3_SIZE            0x0d0c
#define    MV64x60_PCI1_MEM_0_SIZE            0x0c88
#define    MV64x60_PCI1_MEM_1_SIZE            0x0d88
#define    MV64x60_PCI1_MEM_2_SIZE            0x0c8c
#define    MV64x60_PCI1_MEM_3_SIZE            0x0d8c

#define    MV64x60_PCI0_BAR_ENABLE            0x0c3c
#define    MV64x60_PCI1_BAR_ENABLE            0x0cbc

#define    MV64x60_PCI0_PCI_DECODE_CNTL        0x0d3c
#define    MV64x60_PCI1_PCI_DECODE_CNTL        0x0dbc

#define    MV64x60_PCI0_SLAVE_MEM_0_REMAP        0x0c48
#define    MV64x60_PCI0_SLAVE_MEM_1_REMAP        0x0d48
#define    MV64x60_PCI0_SLAVE_MEM_2_REMAP        0x0c4c
#define    MV64x60_PCI0_SLAVE_MEM_3_REMAP        0x0d4c
#define    MV64x60_PCI0_SLAVE_DEV_0_REMAP        0x0c50
#define    MV64x60_PCI0_SLAVE_DEV_1_REMAP        0x0d50
#define    MV64x60_PCI0_SLAVE_DEV_2_REMAP        0x0d58
#define    MV64x60_PCI0_SLAVE_DEV_3_REMAP        0x0c54
#define    MV64x60_PCI0_SLAVE_BOOT_REMAP        0x0d54
#define    MV64x60_PCI0_SLAVE_P2P_MEM_0_REMAP_LO    0x0d5c
#define    MV64x60_PCI0_SLAVE_P2P_MEM_0_REMAP_HI    0x0d60
#define    MV64x60_PCI0_SLAVE_P2P_MEM_1_REMAP_LO    0x0d64
#define    MV64x60_PCI0_SLAVE_P2P_MEM_1_REMAP_HI    0x0d68
#define    MV64x60_PCI0_SLAVE_P2P_IO_REMAP        0x0d6c
#define    MV64x60_PCI0_SLAVE_CPU_REMAP        0x0d70

#define    MV64x60_PCI1_SLAVE_MEM_0_REMAP        0x0cc8
#define    MV64x60_PCI1_SLAVE_MEM_1_REMAP        0x0dc8
#define    MV64x60_PCI1_SLAVE_MEM_2_REMAP        0x0ccc
#define    MV64x60_PCI1_SLAVE_MEM_3_REMAP        0x0dcc
#define    MV64x60_PCI1_SLAVE_DEV_0_REMAP        0x0cd0
#define    MV64x60_PCI1_SLAVE_DEV_1_REMAP        0x0dd0
#define    MV64x60_PCI1_SLAVE_DEV_2_REMAP        0x0dd8
#define    MV64x60_PCI1_SLAVE_DEV_3_REMAP        0x0cd4
#define    MV64x60_PCI1_SLAVE_BOOT_REMAP        0x0dd4
#define    MV64x60_PCI1_SLAVE_P2P_MEM_0_REMAP_LO    0x0ddc
#define    MV64x60_PCI1_SLAVE_P2P_MEM_0_REMAP_HI    0x0de0
#define    MV64x60_PCI1_SLAVE_P2P_MEM_1_REMAP_LO    0x0de4
#define    MV64x60_PCI1_SLAVE_P2P_MEM_1_REMAP_HI    0x0de8
#define    MV64x60_PCI1_SLAVE_P2P_IO_REMAP        0x0dec
#define    MV64x60_PCI1_SLAVE_CPU_REMAP        0x0df0

#define    MV64360_PCICFG_CPCI_HOTSWAP        0x68

/*
 *****************************************************************************
 *
 *    ENET Controller Interface Registers
 *
 *****************************************************************************
 */

/* ENET Controller Window Registers (6 windows) */
#define    MV64360_ENET2MEM_WINDOWS        6

#define    MV64360_ENET2MEM_0_BASE            0x2200
#define    MV64360_ENET2MEM_0_SIZE            0x2204
#define    MV64360_ENET2MEM_1_BASE            0x2208
#define    MV64360_ENET2MEM_1_SIZE            0x220c
#define    MV64360_ENET2MEM_2_BASE            0x2210
#define    MV64360_ENET2MEM_2_SIZE            0x2214
#define    MV64360_ENET2MEM_3_BASE            0x2218
#define    MV64360_ENET2MEM_3_SIZE            0x221c
#define    MV64360_ENET2MEM_4_BASE            0x2220
#define    MV64360_ENET2MEM_4_SIZE            0x2224
#define    MV64360_ENET2MEM_5_BASE            0x2228
#define    MV64360_ENET2MEM_5_SIZE            0x222c

#define    MV64360_ENET2MEM_SNOOP_NONE        0x00000000
#define    MV64360_ENET2MEM_SNOOP_WT        0x00001000
#define    MV64360_ENET2MEM_SNOOP_WB        0x00002000

#define    MV64360_ENET2MEM_BAR_ENABLE        0x2290

#define    MV64360_ENET2MEM_ACC_PROT_0        0x2294
#define    MV64360_ENET2MEM_ACC_PROT_1        0x2298
#define    MV64360_ENET2MEM_ACC_PROT_2        0x229c

/*
 *****************************************************************************
 *
 *    MPSC Controller Interface Registers
 *
 *****************************************************************************
 */

/* MPSC Controller Window Registers (4 windows) */
#define    MV64360_MPSC2MEM_WINDOWS        4

#define    MV64360_MPSC2MEM_0_BASE            0xf200
#define    MV64360_MPSC2MEM_0_SIZE            0xf204
#define    MV64360_MPSC2MEM_1_BASE            0xf208
#define    MV64360_MPSC2MEM_1_SIZE            0xf20c
#define    MV64360_MPSC2MEM_2_BASE            0xf210
#define    MV64360_MPSC2MEM_2_SIZE            0xf214
#define    MV64360_MPSC2MEM_3_BASE            0xf218
#define    MV64360_MPSC2MEM_3_SIZE            0xf21c

#define MV64360_MPSC_0_REMAP            0xf240
#define MV64360_MPSC_1_REMAP            0xf244

#define    MV64360_MPSC2MEM_SNOOP_NONE        0x00000000
#define    MV64360_MPSC2MEM_SNOOP_WT        0x00001000
#define    MV64360_MPSC2MEM_SNOOP_WB        0x00002000

#define    MV64360_MPSC2MEM_BAR_ENABLE        0xf250

#define    MV64360_MPSC2MEM_ACC_PROT_0        0xf254
#define    MV64360_MPSC2MEM_ACC_PROT_1        0xf258

#define    MV64360_MPSC2REGS_BASE            0xf25c

/*
 *****************************************************************************
 *
 *    Timer/Counter Interface Registers
 *
 *****************************************************************************
 */

#define    MV64x60_TIMR_CNTR_0            0x0850
#define    MV64x60_TIMR_CNTR_1            0x0854
#define    MV64x60_TIMR_CNTR_2            0x0858
#define    MV64x60_TIMR_CNTR_3            0x085c
#define    MV64x60_TIMR_CNTR_0_3_CNTL        0x0864
#define    MV64x60_TIMR_CNTR_0_3_INTR_CAUSE    0x0868
#define    MV64x60_TIMR_CNTR_0_3_INTR_MASK        0x086c

#define    GT64260_TIMR_CNTR_4            0x0950
#define    GT64260_TIMR_CNTR_5            0x0954
#define    GT64260_TIMR_CNTR_6            0x0958
#define    GT64260_TIMR_CNTR_7            0x095c
#define    GT64260_TIMR_CNTR_4_7_CNTL        0x0964
#define    GT64260_TIMR_CNTR_4_7_INTR_CAUSE    0x0968
#define    GT64260_TIMR_CNTR_4_7_INTR_MASK        0x096c

/*
 *****************************************************************************
 *
 *    Communications Controller
 *
 *****************************************************************************
 */

#define    GT64260_SER_INIT_PCI_ADDR_HI        0xf320
#define    GT64260_SER_INIT_LAST_DATA        0xf324
#define    GT64260_SER_INIT_CONTROL        0xf328
#define    GT64260_SER_INIT_STATUS            0xf32c

#define    MV64x60_COMM_ARBITER_CNTL        0xf300
#define    MV64x60_COMM_CONFIG            0xb40c
#define    MV64x60_COMM_XBAR_TO            0xf304
#define    MV64x60_COMM_INTR_CAUSE            0xf310
#define    MV64x60_COMM_INTR_MASK            0xf314
#define    MV64x60_COMM_ERR_ADDR            0xf318

#define MV64360_COMM_ARBITER_CNTL        0xf300

/*
 *****************************************************************************
 *
 *    IDMA Controller Interface Registers
 *
 *****************************************************************************
 */

/* IDMA Controller Window Registers (8 windows) */
#define    MV64360_IDMA2MEM_WINDOWS        8

#define    MV64360_IDMA2MEM_0_BASE            0x0a00
#define    MV64360_IDMA2MEM_0_SIZE            0x0a04
#define    MV64360_IDMA2MEM_1_BASE            0x0a08
#define    MV64360_IDMA2MEM_1_SIZE            0x0a0c
#define    MV64360_IDMA2MEM_2_BASE            0x0a10
#define    MV64360_IDMA2MEM_2_SIZE            0x0a14
#define    MV64360_IDMA2MEM_3_BASE            0x0a18
#define    MV64360_IDMA2MEM_3_SIZE            0x0a1c
#define    MV64360_IDMA2MEM_4_BASE            0x0a20
#define    MV64360_IDMA2MEM_4_SIZE            0x0a24
#define    MV64360_IDMA2MEM_5_BASE            0x0a28
#define    MV64360_IDMA2MEM_5_SIZE            0x0a2c
#define    MV64360_IDMA2MEM_6_BASE            0x0a30
#define    MV64360_IDMA2MEM_6_SIZE            0x0a34
#define    MV64360_IDMA2MEM_7_BASE            0x0a38
#define    MV64360_IDMA2MEM_7_SIZE            0x0a3c

#define    MV64360_IDMA2MEM_SNOOP_NONE        0x00000000
#define    MV64360_IDMA2MEM_SNOOP_WT        0x00001000
#define    MV64360_IDMA2MEM_SNOOP_WB        0x00002000

#define    MV64360_IDMA2MEM_BAR_ENABLE        0x0a80

#define    MV64360_IDMA2MEM_ACC_PROT_0        0x0a70
#define    MV64360_IDMA2MEM_ACC_PROT_1        0x0a74
#define    MV64360_IDMA2MEM_ACC_PROT_2        0x0a78
#define    MV64360_IDMA2MEM_ACC_PROT_3        0x0a7c

#define    MV64x60_IDMA_0_OFFSET            0x0800
#define    MV64x60_IDMA_1_OFFSET            0x0804
#define    MV64x60_IDMA_2_OFFSET            0x0808
#define    MV64x60_IDMA_3_OFFSET            0x080c
#define    MV64x60_IDMA_4_OFFSET            0x0900
#define    MV64x60_IDMA_5_OFFSET            0x0904
#define    MV64x60_IDMA_6_OFFSET            0x0908
#define    MV64x60_IDMA_7_OFFSET            0x090c

#define    MV64x60_IDMA_BYTE_COUNT            (0x0800 - MV64x60_IDMA_0_OFFSET)
#define    MV64x60_IDMA_SRC_ADDR            (0x0810 - MV64x60_IDMA_0_OFFSET)
#define    MV64x60_IDMA_DST_ADDR            (0x0820 - MV64x60_IDMA_0_OFFSET)
#define    MV64x60_IDMA_NEXT_DESC            (0x0830 - MV64x60_IDMA_0_OFFSET)
#define    MV64x60_IDMA_CUR_DESC            (0x0870 - MV64x60_IDMA_0_OFFSET)
#define    MV64x60_IDMA_SRC_PCI_ADDR_HI        (0x0890 - MV64x60_IDMA_0_OFFSET)
#define    MV64x60_IDMA_DST_PCI_ADDR_HI        (0x08a0 - MV64x60_IDMA_0_OFFSET)
#define    MV64x60_IDMA_NEXT_DESC_PCI_ADDR_HI    (0x08b0 - MV64x60_IDMA_0_OFFSET)
#define    MV64x60_IDMA_CONTROL_LO            (0x0840 - MV64x60_IDMA_0_OFFSET)
#define    MV64x60_IDMA_CONTROL_HI            (0x0880 - MV64x60_IDMA_0_OFFSET)

#define    MV64x60_IDMA_0_3_ARBITER_CNTL        0x0860
#define    MV64x60_IDMA_4_7_ARBITER_CNTL        0x0960

#define    MV64x60_IDMA_0_3_XBAR_TO        0x08d0
#define    MV64x60_IDMA_4_7_XBAR_TO        0x09d0

#define    MV64x60_IDMA_0_3_INTR_CAUSE        0x08c0
#define    MV64x60_IDMA_0_3_INTR_MASK        0x08c4
#define    MV64x60_IDMA_0_3_ERROR_ADDR        0x08c8
#define    MV64x60_IDMA_0_3_ERROR_SELECT        0x08cc
#define    MV64x60_IDMA_4_7_INTR_CAUSE        0x09c0
#define    MV64x60_IDMA_4_7_INTR_MASK        0x09c4
#define    MV64x60_IDMA_4_7_ERROR_ADDR        0x09c8
#define    MV64x60_IDMA_4_7_ERROR_SELECT        0x09cc

/*
 *****************************************************************************
 *
 *    Watchdog Timer Interface Registers
 *
 *****************************************************************************
 */

#define    MV64x60_WDT_WDC                0xb410
#define    MV64x60_WDT_WDV                0xb414


/*
 *****************************************************************************
 *
 *     General Purpose Pins Controller Interface Registers
 *
 *****************************************************************************
 */

#define    MV64x60_GPP_IO_CNTL            0xf100
#define    MV64x60_GPP_LEVEL_CNTL            0xf110
#define    MV64x60_GPP_VALUE            0xf104
#define    MV64x60_GPP_INTR_CAUSE            0xf108
#define    MV64x60_GPP_INTR_MASK            0xf10c
#define    MV64x60_GPP_VALUE_SET            0xf118
#define    MV64x60_GPP_VALUE_CLR            0xf11c


/*
 *****************************************************************************
 *
 *    Multi-Purpose Pins Controller Interface Registers
 *
 *****************************************************************************
 */

#define    MV64x60_MPP_CNTL_0            0xf000
#define    MV64x60_MPP_CNTL_1            0xf004
#define    MV64x60_MPP_CNTL_2            0xf008
#define    MV64x60_MPP_CNTL_3            0xf00c
#define    GT64260_MPP_SERIAL_PORTS_MULTIPLEX    0xf010

#define MV64x60_ETH_BAR_GAP            0x8
#define MV64x60_ETH_SIZE_REG_GAP        0x8
#define MV64x60_ETH_HIGH_ADDR_REMAP_REG_GAP    0x4
#define MV64x60_ETH_PORT_ACCESS_CTRL_GAP    0x4

#define MV64x60_EBAR_ATTR_DRAM_CS0        0x00000E00
#define MV64x60_EBAR_ATTR_DRAM_CS1        0x00000D00
#define MV64x60_EBAR_ATTR_DRAM_CS2        0x00000B00
#define MV64x60_EBAR_ATTR_DRAM_CS3        0x00000700

#define MV64x60_EBAR_ATTR_CBS_SRAM_BLOCK0    0x00000000
#define MV64x60_EBAR_ATTR_CBS_SRAM_BLOCK1    0x00000100
#define MV64x60_EBAR_ATTR_CBS_SRAM        0x00000000
#define MV64x60_EBAR_ATTR_CBS_CPU_BUS        0x00000800


/*
 *****************************************************************************
 *
 *    Interrupt Controller Interface Registers
 *
 *****************************************************************************
 */

#define    GT64260_IC_OFFSET            0x0c18

#define    GT64260_IC_MAIN_CAUSE_LO        0x0c18
#define    GT64260_IC_MAIN_CAUSE_HI        0x0c68
#define    GT64260_IC_CPU_INTR_MASK_LO        0x0c1c
#define    GT64260_IC_CPU_INTR_MASK_HI        0x0c6c
#define    GT64260_IC_CPU_SELECT_CAUSE        0x0c70
#define    GT64260_IC_PCI0_INTR_MASK_LO        0x0c24
#define    GT64260_IC_PCI0_INTR_MASK_HI        0x0c64
#define    GT64260_IC_PCI0_SELECT_CAUSE        0x0c74
#define    GT64260_IC_PCI1_INTR_MASK_LO        0x0ca4
#define    GT64260_IC_PCI1_INTR_MASK_HI        0x0ce4
#define    GT64260_IC_PCI1_SELECT_CAUSE        0x0cf4
#define    GT64260_IC_CPU_INT_0_MASK        0x0e60
#define    GT64260_IC_CPU_INT_1_MASK        0x0e64
#define    GT64260_IC_CPU_INT_2_MASK        0x0e68
#define    GT64260_IC_CPU_INT_3_MASK        0x0e6c

#define    MV64360_IC_OFFSET            0x0000

#define    MV64360_IC_MAIN_CAUSE_LO        0x0004
#define    MV64360_IC_MAIN_CAUSE_HI        0x000c
#define    MV64360_IC_CPU0_INTR_MASK_LO        0x0014
#define    MV64360_IC_CPU0_INTR_MASK_HI        0x001c
#define    MV64360_IC_CPU0_SELECT_CAUSE        0x0024
#define    MV64360_IC_CPU1_INTR_MASK_LO        0x0034
#define    MV64360_IC_CPU1_INTR_MASK_HI        0x003c
#define    MV64360_IC_CPU1_SELECT_CAUSE        0x0044
#define    MV64360_IC_INT0_MASK_LO            0x0054
#define    MV64360_IC_INT0_MASK_HI            0x005c
#define    MV64360_IC_INT0_SELECT_CAUSE        0x0064
#define    MV64360_IC_INT1_MASK_LO            0x0074
#define    MV64360_IC_INT1_MASK_HI            0x007c
#define    MV64360_IC_INT1_SELECT_CAUSE        0x0084

#endif /* __ASMPPC_MV64x60_DEFS_H */
