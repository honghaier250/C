/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2000 Harald Koerfgen
 */

#ifndef __ASM_IP32_INTS_H
#define __ASM_IP32_INTS_H

#include <asm/irq.h>

/*
 * This list reflects the assignment of interrupt numbers to
 * interrupting events.  Order is fairly irrelevant to handling
 * priority.  This differs from irix.
 */

enum ip32_irq_no {
    /*
     * CPU interrupts are 0 ... 7
     */

    CRIME_IRQ_BASE            = MIPS_CPU_IRQ_BASE + 8,

    /*
     * MACE
     */
    MACE_VID_IN1_IRQ        = CRIME_IRQ_BASE,
    MACE_VID_IN2_IRQ,
    MACE_VID_OUT_IRQ,
    MACE_ETHERNET_IRQ,
    /* SUPERIO, MISC, and AUDIO are MACEISA */
    __MACE_SUPERIO,
    __MACE_MISC,
    __MACE_AUDIO,
    MACE_PCI_BRIDGE_IRQ,

    /*
     * MACEPCI
     */
    MACEPCI_SCSI0_IRQ,
    MACEPCI_SCSI1_IRQ,
    MACEPCI_SLOT0_IRQ,
    MACEPCI_SLOT1_IRQ,
    MACEPCI_SLOT2_IRQ,
    MACEPCI_SHARED0_IRQ,
    MACEPCI_SHARED1_IRQ,
    MACEPCI_SHARED2_IRQ,

    /*
     * CRIME
     */
    CRIME_GBE0_IRQ,
    CRIME_GBE1_IRQ,
    CRIME_GBE2_IRQ,
    CRIME_GBE3_IRQ,
    CRIME_CPUERR_IRQ,
    CRIME_MEMERR_IRQ,
    CRIME_RE_EMPTY_E_IRQ,
    CRIME_RE_FULL_E_IRQ,
    CRIME_RE_IDLE_E_IRQ,
    CRIME_RE_EMPTY_L_IRQ,
    CRIME_RE_FULL_L_IRQ,
    CRIME_RE_IDLE_L_IRQ,
    CRIME_SOFT0_IRQ,
    CRIME_SOFT1_IRQ,
    CRIME_SOFT2_IRQ,
    CRIME_SYSCORERR_IRQ        = CRIME_SOFT2_IRQ,
    CRIME_VICE_IRQ,

    /*
     * MACEISA
     */
    MACEISA_AUDIO_SW_IRQ,
    MACEISA_AUDIO_SC_IRQ,
    MACEISA_AUDIO1_DMAT_IRQ,
    MACEISA_AUDIO1_OF_IRQ,
    MACEISA_AUDIO2_DMAT_IRQ,
    MACEISA_AUDIO2_MERR_IRQ,
    MACEISA_AUDIO3_DMAT_IRQ,
    MACEISA_AUDIO3_MERR_IRQ,
    MACEISA_RTC_IRQ,
    MACEISA_KEYB_IRQ,
    /* MACEISA_KEYB_POLL is not an IRQ */
    __MACEISA_KEYB_POLL,
    MACEISA_MOUSE_IRQ,
    /* MACEISA_MOUSE_POLL is not an IRQ */
    __MACEISA_MOUSE_POLL,
    MACEISA_TIMER0_IRQ,
    MACEISA_TIMER1_IRQ,
    MACEISA_TIMER2_IRQ,
    MACEISA_PARALLEL_IRQ,
    MACEISA_PAR_CTXA_IRQ,
    MACEISA_PAR_CTXB_IRQ,
    MACEISA_PAR_MERR_IRQ,
    MACEISA_SERIAL1_IRQ,
    MACEISA_SERIAL1_TDMAT_IRQ,
    MACEISA_SERIAL1_TDMAPR_IRQ,
    MACEISA_SERIAL1_TDMAME_IRQ,
    MACEISA_SERIAL1_RDMAT_IRQ,
    MACEISA_SERIAL1_RDMAOR_IRQ,
    MACEISA_SERIAL2_IRQ,
    MACEISA_SERIAL2_TDMAT_IRQ,
    MACEISA_SERIAL2_TDMAPR_IRQ,
    MACEISA_SERIAL2_TDMAME_IRQ,
    MACEISA_SERIAL2_RDMAT_IRQ,
    MACEISA_SERIAL2_RDMAOR_IRQ,

    IP32_IRQ_MAX            = MACEISA_SERIAL2_RDMAOR_IRQ
};

#endif /* __ASM_IP32_INTS_H */
