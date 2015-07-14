#ifndef ASM_HYPERTRANSPORT_H
#define ASM_HYPERTRANSPORT_H

/*
 * Constants for x86 Hypertransport Interrupts.
 */

#define HT_IRQ_LOW_BASE            0xf8000000

#define HT_IRQ_LOW_VECTOR_SHIFT        16
#define HT_IRQ_LOW_VECTOR_MASK        0x00ff0000
#define HT_IRQ_LOW_VECTOR(v)                        \
    (((v) << HT_IRQ_LOW_VECTOR_SHIFT) & HT_IRQ_LOW_VECTOR_MASK)

#define HT_IRQ_LOW_DEST_ID_SHIFT    8
#define HT_IRQ_LOW_DEST_ID_MASK        0x0000ff00
#define HT_IRQ_LOW_DEST_ID(v)                        \
    (((v) << HT_IRQ_LOW_DEST_ID_SHIFT) & HT_IRQ_LOW_DEST_ID_MASK)

#define HT_IRQ_LOW_DM_PHYSICAL        0x0000000
#define HT_IRQ_LOW_DM_LOGICAL        0x0000040

#define HT_IRQ_LOW_RQEOI_EDGE        0x0000000
#define HT_IRQ_LOW_RQEOI_LEVEL        0x0000020


#define HT_IRQ_LOW_MT_FIXED        0x0000000
#define HT_IRQ_LOW_MT_ARBITRATED    0x0000004
#define HT_IRQ_LOW_MT_SMI        0x0000008
#define HT_IRQ_LOW_MT_NMI        0x000000c
#define HT_IRQ_LOW_MT_INIT        0x0000010
#define HT_IRQ_LOW_MT_STARTUP        0x0000014
#define HT_IRQ_LOW_MT_EXTINT        0x0000018
#define HT_IRQ_LOW_MT_LINT1        0x000008c
#define HT_IRQ_LOW_MT_LINT0        0x0000098

#define HT_IRQ_LOW_IRQ_MASKED        0x0000001


#define HT_IRQ_HIGH_DEST_ID_SHIFT    0
#define HT_IRQ_HIGH_DEST_ID_MASK    0x00ffffff
#define HT_IRQ_HIGH_DEST_ID(v)                        \
    ((((v) >> 8) << HT_IRQ_HIGH_DEST_ID_SHIFT) & HT_IRQ_HIGH_DEST_ID_MASK)

#endif /* ASM_HYPERTRANSPORT_H */
