/*
 * linux/include/asm-arm/arch-ep93xx/irqs.h
 */

#ifndef __ASM_ARCH_IRQS_H
#define __ASM_ARCH_IRQS_H

#define IRQ_EP93XX_COMMRX        2
#define IRQ_EP93XX_COMMTX        3
#define IRQ_EP93XX_TIMER1        4
#define IRQ_EP93XX_TIMER2        5
#define IRQ_EP93XX_AACINTR        6
#define IRQ_EP93XX_DMAM2P0        7
#define IRQ_EP93XX_DMAM2P1        8
#define IRQ_EP93XX_DMAM2P2        9
#define IRQ_EP93XX_DMAM2P3        10
#define IRQ_EP93XX_DMAM2P4        11
#define IRQ_EP93XX_DMAM2P5        12
#define IRQ_EP93XX_DMAM2P6        13
#define IRQ_EP93XX_DMAM2P7        14
#define IRQ_EP93XX_DMAM2P8        15
#define IRQ_EP93XX_DMAM2P9        16
#define IRQ_EP93XX_DMAM2M0        17
#define IRQ_EP93XX_DMAM2M1        18
#define IRQ_EP93XX_GPIO0MUX        19
#define IRQ_EP93XX_GPIO1MUX        20
#define IRQ_EP93XX_GPIO2MUX        21
#define IRQ_EP93XX_GPIO3MUX        22
#define IRQ_EP93XX_UART1RX        23
#define IRQ_EP93XX_UART1TX        24
#define IRQ_EP93XX_UART2RX        25
#define IRQ_EP93XX_UART2TX        26
#define IRQ_EP93XX_UART3RX        27
#define IRQ_EP93XX_UART3TX        28
#define IRQ_EP93XX_KEY            29
#define IRQ_EP93XX_TOUCH        30
#define EP93XX_VIC1_VALID_IRQ_MASK    0x7ffffffc

#define IRQ_EP93XX_EXT0            32
#define IRQ_EP93XX_EXT1            33
#define IRQ_EP93XX_EXT2            34
#define IRQ_EP93XX_64HZ            35
#define IRQ_EP93XX_WATCHDOG        36
#define IRQ_EP93XX_RTC            37
#define IRQ_EP93XX_IRDA            38
#define IRQ_EP93XX_ETHERNET        39
#define IRQ_EP93XX_EXT3            40
#define IRQ_EP93XX_PROG            41
#define IRQ_EP93XX_1HZ            42
#define IRQ_EP93XX_VSYNC        43
#define IRQ_EP93XX_VIDEO_FIFO        44
#define IRQ_EP93XX_SSP1RX        45
#define IRQ_EP93XX_SSP1TX        46
#define IRQ_EP93XX_GPIO4MUX        47
#define IRQ_EP93XX_GPIO5MUX        48
#define IRQ_EP93XX_GPIO6MUX        49
#define IRQ_EP93XX_GPIO7MUX        50
#define IRQ_EP93XX_TIMER3        51
#define IRQ_EP93XX_UART1        52
#define IRQ_EP93XX_SSP            53
#define IRQ_EP93XX_UART2        54
#define IRQ_EP93XX_UART3        55
#define IRQ_EP93XX_USB            56
#define IRQ_EP93XX_ETHERNET_PME        57
#define IRQ_EP93XX_DSP            58
#define IRQ_EP93XX_GPIO_AB        59
#define IRQ_EP93XX_SAI            60
#define EP93XX_VIC2_VALID_IRQ_MASK    0x1fffffff

#define NR_EP93XX_IRQS            (64 + 24)

#define EP93XX_BOARD_IRQ(x)        (NR_EP93XX_IRQS + (x))
#define EP93XX_BOARD_IRQS        32

#define NR_IRQS                (NR_EP93XX_IRQS + EP93XX_BOARD_IRQS)


#endif
