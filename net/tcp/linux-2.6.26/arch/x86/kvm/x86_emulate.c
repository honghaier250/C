/******************************************************************************
 * x86_emulate.c
 *
 * Generic x86 (32-bit and 64-bit) instruction decoder and emulator.
 *
 * Copyright (c) 2005 Keir Fraser
 *
 * Linux coding style, mod r/m decoder, segment base fixes, real-mode
 * privileged instructions:
 *
 * Copyright (C) 2006 Qumranet
 *
 *   Avi Kivity <avi@qumranet.com>
 *   Yaniv Kamay <yaniv@qumranet.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 * From: xen-unstable 10676:af9809f51f81a3c43f276f00c81a52ef558afda4
 */

#ifndef __KERNEL__
#include <stdio.h>
#include <stdint.h>
#include <public/xen.h>
#define DPRINTF(_f, _a ...) printf(_f , ## _a)
#else
#include <linux/kvm_host.h>
#define DPRINTF(x...) do {} while (0)
#endif
#include <linux/module.h>
#include <asm/kvm_x86_emulate.h>

/*
 * Opcode effective-address decode tables.
 * Note that we only emulate instructions that have at least one memory
 * operand (excluding implicit stack references). We assume that stack
 * references and instruction fetches will never occur in special memory
 * areas that require emulation. So, for example, 'mov <imm>,<reg>' need
 * not be handled.
 */

/* Operand sizes: 8-bit operands or specified/overridden size. */
#define ByteOp      (1<<0)    /* 8-bit operands. */
/* Destination operand type. */
#define ImplicitOps (1<<1)    /* Implicit in opcode. No generic decode. */
#define DstReg      (2<<1)    /* Register operand. */
#define DstMem      (3<<1)    /* Memory operand. */
#define DstMask     (3<<1)
/* Source operand type. */
#define SrcNone     (0<<3)    /* No source operand. */
#define SrcImplicit (0<<3)    /* Source operand is implicit in the opcode. */
#define SrcReg      (1<<3)    /* Register operand. */
#define SrcMem      (2<<3)    /* Memory operand. */
#define SrcMem16    (3<<3)    /* Memory operand (16-bit). */
#define SrcMem32    (4<<3)    /* Memory operand (32-bit). */
#define SrcImm      (5<<3)    /* Immediate operand. */
#define SrcImmByte  (6<<3)    /* 8-bit sign-extended immediate operand. */
#define SrcMask     (7<<3)
/* Generic ModRM decode. */
#define ModRM       (1<<6)
/* Destination is only written; never read. */
#define Mov         (1<<7)
#define BitOp       (1<<8)
#define MemAbs      (1<<9)      /* Memory operand is absolute displacement */
#define String      (1<<10)     /* String instruction (rep capable) */
#define Stack       (1<<11)     /* Stack instruction (push/pop) */
#define Group       (1<<14)     /* Bits 3:5 of modrm byte extend opcode */
#define GroupDual   (1<<15)     /* Alternate decoding of mod == 3 */
#define GroupMask   0xff        /* Group number stored in bits 0:7 */

enum {
    Group1_80, Group1_81, Group1_82, Group1_83,
    Group1A, Group3_Byte, Group3, Group4, Group5, Group7,
};

static u16 opcode_table[256] = {
    /* 0x00 - 0x07 */
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM,
    ByteOp | DstReg | SrcMem | ModRM, DstReg | SrcMem | ModRM,
    0, 0, 0, 0,
    /* 0x08 - 0x0F */
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM,
    ByteOp | DstReg | SrcMem | ModRM, DstReg | SrcMem | ModRM,
    0, 0, 0, 0,
    /* 0x10 - 0x17 */
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM,
    ByteOp | DstReg | SrcMem | ModRM, DstReg | SrcMem | ModRM,
    0, 0, 0, 0,
    /* 0x18 - 0x1F */
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM,
    ByteOp | DstReg | SrcMem | ModRM, DstReg | SrcMem | ModRM,
    0, 0, 0, 0,
    /* 0x20 - 0x27 */
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM,
    ByteOp | DstReg | SrcMem | ModRM, DstReg | SrcMem | ModRM,
    SrcImmByte, SrcImm, 0, 0,
    /* 0x28 - 0x2F */
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM,
    ByteOp | DstReg | SrcMem | ModRM, DstReg | SrcMem | ModRM,
    0, 0, 0, 0,
    /* 0x30 - 0x37 */
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM,
    ByteOp | DstReg | SrcMem | ModRM, DstReg | SrcMem | ModRM,
    0, 0, 0, 0,
    /* 0x38 - 0x3F */
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM,
    ByteOp | DstReg | SrcMem | ModRM, DstReg | SrcMem | ModRM,
    0, 0, 0, 0,
    /* 0x40 - 0x47 */
    DstReg, DstReg, DstReg, DstReg, DstReg, DstReg, DstReg, DstReg,
    /* 0x48 - 0x4F */
    DstReg, DstReg, DstReg, DstReg,    DstReg, DstReg, DstReg, DstReg,
    /* 0x50 - 0x57 */
    SrcReg | Stack, SrcReg | Stack, SrcReg | Stack, SrcReg | Stack,
    SrcReg | Stack, SrcReg | Stack, SrcReg | Stack, SrcReg | Stack,
    /* 0x58 - 0x5F */
    DstReg | Stack, DstReg | Stack, DstReg | Stack, DstReg | Stack,
    DstReg | Stack, DstReg | Stack, DstReg | Stack, DstReg | Stack,
    /* 0x60 - 0x67 */
    0, 0, 0, DstReg | SrcMem32 | ModRM | Mov /* movsxd (x86/64) */ ,
    0, 0, 0, 0,
    /* 0x68 - 0x6F */
    0, 0, ImplicitOps | Mov | Stack, 0,
    SrcNone  | ByteOp  | ImplicitOps, SrcNone  | ImplicitOps, /* insb, insw/insd */
    SrcNone  | ByteOp  | ImplicitOps, SrcNone  | ImplicitOps, /* outsb, outsw/outsd */
    /* 0x70 - 0x77 */
    ImplicitOps, ImplicitOps, ImplicitOps, ImplicitOps,
    ImplicitOps, ImplicitOps, ImplicitOps, ImplicitOps,
    /* 0x78 - 0x7F */
    ImplicitOps, ImplicitOps, ImplicitOps, ImplicitOps,
    ImplicitOps, ImplicitOps, ImplicitOps, ImplicitOps,
    /* 0x80 - 0x87 */
    Group | Group1_80, Group | Group1_81,
    Group | Group1_82, Group | Group1_83,
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM,
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM,
    /* 0x88 - 0x8F */
    ByteOp | DstMem | SrcReg | ModRM | Mov, DstMem | SrcReg | ModRM | Mov,
    ByteOp | DstReg | SrcMem | ModRM | Mov, DstReg | SrcMem | ModRM | Mov,
    0, ModRM | DstReg, 0, Group | Group1A,
    /* 0x90 - 0x9F */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, ImplicitOps | Stack, ImplicitOps | Stack, 0, 0,
    /* 0xA0 - 0xA7 */
    ByteOp | DstReg | SrcMem | Mov | MemAbs, DstReg | SrcMem | Mov | MemAbs,
    ByteOp | DstMem | SrcReg | Mov | MemAbs, DstMem | SrcReg | Mov | MemAbs,
    ByteOp | ImplicitOps | Mov | String, ImplicitOps | Mov | String,
    ByteOp | ImplicitOps | String, ImplicitOps | String,
    /* 0xA8 - 0xAF */
    0, 0, ByteOp | ImplicitOps | Mov | String, ImplicitOps | Mov | String,
    ByteOp | ImplicitOps | Mov | String, ImplicitOps | Mov | String,
    ByteOp | ImplicitOps | String, ImplicitOps | String,
    /* 0xB0 - 0xBF */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xC0 - 0xC7 */
    ByteOp | DstMem | SrcImm | ModRM, DstMem | SrcImmByte | ModRM,
    0, ImplicitOps | Stack, 0, 0,
    ByteOp | DstMem | SrcImm | ModRM | Mov, DstMem | SrcImm | ModRM | Mov,
    /* 0xC8 - 0xCF */
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xD0 - 0xD7 */
    ByteOp | DstMem | SrcImplicit | ModRM, DstMem | SrcImplicit | ModRM,
    ByteOp | DstMem | SrcImplicit | ModRM, DstMem | SrcImplicit | ModRM,
    0, 0, 0, 0,
    /* 0xD8 - 0xDF */
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xE0 - 0xE7 */
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xE8 - 0xEF */
    ImplicitOps | Stack, SrcImm|ImplicitOps, 0, SrcImmByte|ImplicitOps,
    0, 0, 0, 0,
    /* 0xF0 - 0xF7 */
    0, 0, 0, 0,
    ImplicitOps, ImplicitOps, Group | Group3_Byte, Group | Group3,
    /* 0xF8 - 0xFF */
    ImplicitOps, 0, ImplicitOps, ImplicitOps,
    0, 0, Group | Group4, Group | Group5,
};

static u16 twobyte_table[256] = {
    /* 0x00 - 0x0F */
    0, Group | GroupDual | Group7, 0, 0, 0, 0, ImplicitOps, 0,
    ImplicitOps, ImplicitOps, 0, 0, 0, ImplicitOps | ModRM, 0, 0,
    /* 0x10 - 0x1F */
    0, 0, 0, 0, 0, 0, 0, 0, ImplicitOps | ModRM, 0, 0, 0, 0, 0, 0, 0,
    /* 0x20 - 0x2F */
    ModRM | ImplicitOps, ModRM, ModRM | ImplicitOps, ModRM, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x30 - 0x3F */
    ImplicitOps, 0, ImplicitOps, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x40 - 0x47 */
    DstReg | SrcMem | ModRM | Mov, DstReg | SrcMem | ModRM | Mov,
    DstReg | SrcMem | ModRM | Mov, DstReg | SrcMem | ModRM | Mov,
    DstReg | SrcMem | ModRM | Mov, DstReg | SrcMem | ModRM | Mov,
    DstReg | SrcMem | ModRM | Mov, DstReg | SrcMem | ModRM | Mov,
    /* 0x48 - 0x4F */
    DstReg | SrcMem | ModRM | Mov, DstReg | SrcMem | ModRM | Mov,
    DstReg | SrcMem | ModRM | Mov, DstReg | SrcMem | ModRM | Mov,
    DstReg | SrcMem | ModRM | Mov, DstReg | SrcMem | ModRM | Mov,
    DstReg | SrcMem | ModRM | Mov, DstReg | SrcMem | ModRM | Mov,
    /* 0x50 - 0x5F */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x60 - 0x6F */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x70 - 0x7F */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0x80 - 0x8F */
    ImplicitOps, ImplicitOps, ImplicitOps, ImplicitOps,
    ImplicitOps, ImplicitOps, ImplicitOps, ImplicitOps,
    ImplicitOps, ImplicitOps, ImplicitOps, ImplicitOps,
    ImplicitOps, ImplicitOps, ImplicitOps, ImplicitOps,
    /* 0x90 - 0x9F */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xA0 - 0xA7 */
    0, 0, 0, DstMem | SrcReg | ModRM | BitOp, 0, 0, 0, 0,
    /* 0xA8 - 0xAF */
    0, 0, 0, DstMem | SrcReg | ModRM | BitOp, 0, 0, 0, 0,
    /* 0xB0 - 0xB7 */
    ByteOp | DstMem | SrcReg | ModRM, DstMem | SrcReg | ModRM, 0,
        DstMem | SrcReg | ModRM | BitOp,
    0, 0, ByteOp | DstReg | SrcMem | ModRM | Mov,
        DstReg | SrcMem16 | ModRM | Mov,
    /* 0xB8 - 0xBF */
    0, 0, DstMem | SrcImmByte | ModRM, DstMem | SrcReg | ModRM | BitOp,
    0, 0, ByteOp | DstReg | SrcMem | ModRM | Mov,
        DstReg | SrcMem16 | ModRM | Mov,
    /* 0xC0 - 0xCF */
    0, 0, 0, DstMem | SrcReg | ModRM | Mov, 0, 0, 0, ImplicitOps | ModRM,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xD0 - 0xDF */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xE0 - 0xEF */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* 0xF0 - 0xFF */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static u16 group_table[] = {
    [Group1_80*8] =
    ByteOp | DstMem | SrcImm | ModRM, ByteOp | DstMem | SrcImm | ModRM,
    ByteOp | DstMem | SrcImm | ModRM, ByteOp | DstMem | SrcImm | ModRM,
    ByteOp | DstMem | SrcImm | ModRM, ByteOp | DstMem | SrcImm | ModRM,
    ByteOp | DstMem | SrcImm | ModRM, ByteOp | DstMem | SrcImm | ModRM,
    [Group1_81*8] =
    DstMem | SrcImm | ModRM, DstMem | SrcImm | ModRM,
    DstMem | SrcImm | ModRM, DstMem | SrcImm | ModRM,
    DstMem | SrcImm | ModRM, DstMem | SrcImm | ModRM,
    DstMem | SrcImm | ModRM, DstMem | SrcImm | ModRM,
    [Group1_82*8] =
    ByteOp | DstMem | SrcImm | ModRM, ByteOp | DstMem | SrcImm | ModRM,
    ByteOp | DstMem | SrcImm | ModRM, ByteOp | DstMem | SrcImm | ModRM,
    ByteOp | DstMem | SrcImm | ModRM, ByteOp | DstMem | SrcImm | ModRM,
    ByteOp | DstMem | SrcImm | ModRM, ByteOp | DstMem | SrcImm | ModRM,
    [Group1_83*8] =
    DstMem | SrcImmByte | ModRM, DstMem | SrcImmByte | ModRM,
    DstMem | SrcImmByte | ModRM, DstMem | SrcImmByte | ModRM,
    DstMem | SrcImmByte | ModRM, DstMem | SrcImmByte | ModRM,
    DstMem | SrcImmByte | ModRM, DstMem | SrcImmByte | ModRM,
    [Group1A*8] =
    DstMem | SrcNone | ModRM | Mov | Stack, 0, 0, 0, 0, 0, 0, 0,
    [Group3_Byte*8] =
    ByteOp | SrcImm | DstMem | ModRM, 0,
    ByteOp | DstMem | SrcNone | ModRM, ByteOp | DstMem | SrcNone | ModRM,
    0, 0, 0, 0,
    [Group3*8] =
    DstMem | SrcImm | ModRM | SrcImm, 0,
    DstMem | SrcNone | ModRM, ByteOp | DstMem | SrcNone | ModRM,
    0, 0, 0, 0,
    [Group4*8] =
    ByteOp | DstMem | SrcNone | ModRM, ByteOp | DstMem | SrcNone | ModRM,
    0, 0, 0, 0, 0, 0,
    [Group5*8] =
    DstMem | SrcNone | ModRM, DstMem | SrcNone | ModRM, 0, 0,
    SrcMem | ModRM, 0, SrcMem | ModRM | Stack, 0,
    [Group7*8] =
    0, 0, ModRM | SrcMem, ModRM | SrcMem,
    SrcNone | ModRM | DstMem | Mov, 0,
    SrcMem16 | ModRM | Mov, SrcMem | ModRM | ByteOp,
};

static u16 group2_table[] = {
    [Group7*8] =
    SrcNone | ModRM, 0, 0, 0,
    SrcNone | ModRM | DstMem | Mov, 0,
    SrcMem16 | ModRM | Mov, 0,
};

/* EFLAGS bit definitions. */
#define EFLG_OF (1<<11)
#define EFLG_DF (1<<10)
#define EFLG_SF (1<<7)
#define EFLG_ZF (1<<6)
#define EFLG_AF (1<<4)
#define EFLG_PF (1<<2)
#define EFLG_CF (1<<0)

/*
 * Instruction emulation:
 * Most instructions are emulated directly via a fragment of inline assembly
 * code. This allows us to save/restore EFLAGS and thus very easily pick up
 * any modified flags.
 */

#if defined(CONFIG_X86_64)
#define _LO32 "k"        /* force 32-bit operand */
#define _STK  "%%rsp"        /* stack pointer */
#elif defined(__i386__)
#define _LO32 ""        /* force 32-bit operand */
#define _STK  "%%esp"        /* stack pointer */
#endif

/*
 * These EFLAGS bits are restored from saved value during emulation, and
 * any changes are written back to the saved value after emulation.
 */
#define EFLAGS_MASK (EFLG_OF|EFLG_SF|EFLG_ZF|EFLG_AF|EFLG_PF|EFLG_CF)

/* Before executing instruction: restore necessary bits in EFLAGS. */
#define _PRE_EFLAGS(_sav, _msk, _tmp)                    \
    /* EFLAGS = (_sav & _msk) | (EFLAGS & ~_msk); _sav &= ~_msk; */ \
    "movl %"_sav",%"_LO32 _tmp"; "                                  \
    "push %"_tmp"; "                                                \
    "push %"_tmp"; "                                                \
    "movl %"_msk",%"_LO32 _tmp"; "                                  \
    "andl %"_LO32 _tmp",("_STK"); "                                 \
    "pushf; "                                                       \
    "notl %"_LO32 _tmp"; "                                          \
    "andl %"_LO32 _tmp",("_STK"); "                                 \
    "andl %"_LO32 _tmp","__stringify(BITS_PER_LONG/4)"("_STK"); "    \
    "pop  %"_tmp"; "                                                \
    "orl  %"_LO32 _tmp",("_STK"); "                                 \
    "popf; "                                                        \
    "pop  %"_sav"; "

/* After executing instruction: write-back necessary bits in EFLAGS. */
#define _POST_EFLAGS(_sav, _msk, _tmp) \
    /* _sav |= EFLAGS & _msk; */        \
    "pushf; "                \
    "pop  %"_tmp"; "            \
    "andl %"_msk",%"_LO32 _tmp"; "        \
    "orl  %"_LO32 _tmp",%"_sav"; "

/* Raw emulation: instruction has two explicit operands. */
#define __emulate_2op_nobyte(_op,_src,_dst,_eflags,_wx,_wy,_lx,_ly,_qx,_qy) \
    do {                                     \
        unsigned long _tmp;                        \
                                        \
        switch ((_dst).bytes) {                        \
        case 2:                                \
            __asm__ __volatile__ (                    \
                _PRE_EFLAGS("0", "4", "2")            \
                _op"w %"_wx"3,%1; "                \
                _POST_EFLAGS("0", "4", "2")            \
                : "=m" (_eflags), "=m" ((_dst).val),        \
                  "=&r" (_tmp)                    \
                : _wy ((_src).val), "i" (EFLAGS_MASK));     \
            break;                            \
        case 4:                                \
            __asm__ __volatile__ (                    \
                _PRE_EFLAGS("0", "4", "2")            \
                _op"l %"_lx"3,%1; "                \
                _POST_EFLAGS("0", "4", "2")            \
                : "=m" (_eflags), "=m" ((_dst).val),        \
                  "=&r" (_tmp)                    \
                : _ly ((_src).val), "i" (EFLAGS_MASK));     \
            break;                            \
        case 8:                                \
            __emulate_2op_8byte(_op, _src, _dst,            \
                        _eflags, _qx, _qy);            \
            break;                            \
        }                                \
    } while (0)

#define __emulate_2op(_op,_src,_dst,_eflags,_bx,_by,_wx,_wy,_lx,_ly,_qx,_qy) \
    do {                                     \
        unsigned long __tmp;                         \
        switch ((_dst).bytes) {                             \
        case 1:                                 \
            __asm__ __volatile__ (                     \
                _PRE_EFLAGS("0", "4", "2")             \
                _op"b %"_bx"3,%1; "                 \
                _POST_EFLAGS("0", "4", "2")             \
                : "=m" (_eflags), "=m" ((_dst).val),         \
                  "=&r" (__tmp)                     \
                : _by ((_src).val), "i" (EFLAGS_MASK));      \
            break;                             \
        default:                             \
            __emulate_2op_nobyte(_op, _src, _dst, _eflags,         \
                         _wx, _wy, _lx, _ly, _qx, _qy);  \
            break;                             \
        }                                 \
    } while (0)

/* Source operand is byte-sized and may be restricted to just %cl. */
#define emulate_2op_SrcB(_op, _src, _dst, _eflags)                      \
    __emulate_2op(_op, _src, _dst, _eflags,                \
              "b", "c", "b", "c", "b", "c", "b", "c")

/* Source operand is byte, word, long or quad sized. */
#define emulate_2op_SrcV(_op, _src, _dst, _eflags)                      \
    __emulate_2op(_op, _src, _dst, _eflags,                \
              "b", "q", "w", "r", _LO32, "r", "", "r")

/* Source operand is word, long or quad sized. */
#define emulate_2op_SrcV_nobyte(_op, _src, _dst, _eflags)               \
    __emulate_2op_nobyte(_op, _src, _dst, _eflags,            \
                 "w", "r", _LO32, "r", "", "r")

/* Instruction has only one explicit operand (no source operand). */
#define emulate_1op(_op, _dst, _eflags)                                    \
    do {                                \
        unsigned long _tmp;                    \
                                    \
        switch ((_dst).bytes) {                        \
        case 1:                            \
            __asm__ __volatile__ (                \
                _PRE_EFLAGS("0", "3", "2")        \
                _op"b %1; "                \
                _POST_EFLAGS("0", "3", "2")        \
                : "=m" (_eflags), "=m" ((_dst).val),    \
                  "=&r" (_tmp)                \
                : "i" (EFLAGS_MASK));            \
            break;                        \
        case 2:                            \
            __asm__ __volatile__ (                \
                _PRE_EFLAGS("0", "3", "2")        \
                _op"w %1; "                \
                _POST_EFLAGS("0", "3", "2")        \
                : "=m" (_eflags), "=m" ((_dst).val),    \
                  "=&r" (_tmp)                \
                : "i" (EFLAGS_MASK));            \
            break;                        \
        case 4:                            \
            __asm__ __volatile__ (                \
                _PRE_EFLAGS("0", "3", "2")        \
                _op"l %1; "                \
                _POST_EFLAGS("0", "3", "2")        \
                : "=m" (_eflags), "=m" ((_dst).val),    \
                  "=&r" (_tmp)                \
                : "i" (EFLAGS_MASK));            \
            break;                        \
        case 8:                            \
            __emulate_1op_8byte(_op, _dst, _eflags);    \
            break;                        \
        }                            \
    } while (0)

/* Emulate an instruction with quadword operands (x86/64 only). */
#if defined(CONFIG_X86_64)
#define __emulate_2op_8byte(_op, _src, _dst, _eflags, _qx, _qy)           \
    do {                                  \
        __asm__ __volatile__ (                      \
            _PRE_EFLAGS("0", "4", "2")              \
            _op"q %"_qx"3,%1; "                  \
            _POST_EFLAGS("0", "4", "2")              \
            : "=m" (_eflags), "=m" ((_dst).val), "=&r" (_tmp) \
            : _qy ((_src).val), "i" (EFLAGS_MASK));        \
    } while (0)

#define __emulate_1op_8byte(_op, _dst, _eflags)                           \
    do {                                  \
        __asm__ __volatile__ (                      \
            _PRE_EFLAGS("0", "3", "2")              \
            _op"q %1; "                      \
            _POST_EFLAGS("0", "3", "2")              \
            : "=m" (_eflags), "=m" ((_dst).val), "=&r" (_tmp) \
            : "i" (EFLAGS_MASK));                  \
    } while (0)

#elif defined(__i386__)
#define __emulate_2op_8byte(_op, _src, _dst, _eflags, _qx, _qy)
#define __emulate_1op_8byte(_op, _dst, _eflags)
#endif                /* __i386__ */

/* Fetch next part of the instruction being emulated. */
#define insn_fetch(_type, _size, _eip)                                  \
({    unsigned long _x;                        \
    rc = do_insn_fetch(ctxt, ops, (_eip), &_x, (_size));        \
    if (rc != 0)                            \
        goto done;                        \
    (_eip) += (_size);                        \
    (_type)_x;                            \
})

static inline unsigned long ad_mask(struct decode_cache *c)
{
    return (1UL << (c->ad_bytes << 3)) - 1;
}

/* Access/update address held in a register, based on addressing mode. */
static inline unsigned long
address_mask(struct decode_cache *c, unsigned long reg)
{
    if (c->ad_bytes == sizeof(unsigned long))
        return reg;
    else
        return reg & ad_mask(c);
}

static inline unsigned long
register_address(struct decode_cache *c, unsigned long base, unsigned long reg)
{
    return base + address_mask(c, reg);
}

static inline void
register_address_increment(struct decode_cache *c, unsigned long *reg, int inc)
{
    if (c->ad_bytes == sizeof(unsigned long))
        *reg += inc;
    else
        *reg = (*reg & ~ad_mask(c)) | ((*reg + inc) & ad_mask(c));
}

static inline void jmp_rel(struct decode_cache *c, int rel)
{
    register_address_increment(c, &c->eip, rel);
}

static int do_fetch_insn_byte(struct x86_emulate_ctxt *ctxt,
                  struct x86_emulate_ops *ops,
                  unsigned long linear, u8 *dest)
{
    struct fetch_cache *fc = &ctxt->decode.fetch;
    int rc;
    int size;

    if (linear < fc->start || linear >= fc->end) {
        size = min(15UL, PAGE_SIZE - offset_in_page(linear));
        rc = ops->read_std(linear, fc->data, size, ctxt->vcpu);
        if (rc)
            return rc;
        fc->start = linear;
        fc->end = linear + size;
    }
    *dest = fc->data[linear - fc->start];
    return 0;
}

static int do_insn_fetch(struct x86_emulate_ctxt *ctxt,
             struct x86_emulate_ops *ops,
             unsigned long eip, void *dest, unsigned size)
{
    int rc = 0;

    eip += ctxt->cs_base;
    while (size--) {
        rc = do_fetch_insn_byte(ctxt, ops, eip++, dest++);
        if (rc)
            return rc;
    }
    return 0;
}

/*
 * Given the 'reg' portion of a ModRM byte, and a register block, return a
 * pointer into the block that addresses the relevant register.
 * @highbyte_regs specifies whether to decode AH,CH,DH,BH.
 */
static void *decode_register(u8 modrm_reg, unsigned long *regs,
                 int highbyte_regs)
{
    void *p;

    p = &regs[modrm_reg];
    if (highbyte_regs && modrm_reg >= 4 && modrm_reg < 8)
        p = (unsigned char *)&regs[modrm_reg & 3] + 1;
    return p;
}

static int read_descriptor(struct x86_emulate_ctxt *ctxt,
               struct x86_emulate_ops *ops,
               void *ptr,
               u16 *size, unsigned long *address, int op_bytes)
{
    int rc;

    if (op_bytes == 2)
        op_bytes = 3;
    *address = 0;
    rc = ops->read_std((unsigned long)ptr, (unsigned long *)size, 2,
               ctxt->vcpu);
    if (rc)
        return rc;
    rc = ops->read_std((unsigned long)ptr + 2, address, op_bytes,
               ctxt->vcpu);
    return rc;
}

static int test_cc(unsigned int condition, unsigned int flags)
{
    int rc = 0;

    switch ((condition & 15) >> 1) {
    case 0: /* o */
        rc |= (flags & EFLG_OF);
        break;
    case 1: /* b/c/nae */
        rc |= (flags & EFLG_CF);
        break;
    case 2: /* z/e */
        rc |= (flags & EFLG_ZF);
        break;
    case 3: /* be/na */
        rc |= (flags & (EFLG_CF|EFLG_ZF));
        break;
    case 4: /* s */
        rc |= (flags & EFLG_SF);
        break;
    case 5: /* p/pe */
        rc |= (flags & EFLG_PF);
        break;
    case 7: /* le/ng */
        rc |= (flags & EFLG_ZF);
        /* fall through */
    case 6: /* l/nge */
        rc |= (!(flags & EFLG_SF) != !(flags & EFLG_OF));
        break;
    }

    /* Odd condition identifiers (lsb == 1) have inverted sense. */
    return (!!rc ^ (condition & 1));
}

static void decode_register_operand(struct operand *op,
                    struct decode_cache *c,
                    int inhibit_bytereg)
{
    unsigned reg = c->modrm_reg;
    int highbyte_regs = c->rex_prefix == 0;

    if (!(c->d & ModRM))
        reg = (c->b & 7) | ((c->rex_prefix & 1) << 3);
    op->type = OP_REG;
    if ((c->d & ByteOp) && !inhibit_bytereg) {
        op->ptr = decode_register(reg, c->regs, highbyte_regs);
        op->val = *(u8 *)op->ptr;
        op->bytes = 1;
    } else {
        op->ptr = decode_register(reg, c->regs, 0);
        op->bytes = c->op_bytes;
        switch (op->bytes) {
        case 2:
            op->val = *(u16 *)op->ptr;
            break;
        case 4:
            op->val = *(u32 *)op->ptr;
            break;
        case 8:
            op->val = *(u64 *) op->ptr;
            break;
        }
    }
    op->orig_val = op->val;
}

static int decode_modrm(struct x86_emulate_ctxt *ctxt,
            struct x86_emulate_ops *ops)
{
    struct decode_cache *c = &ctxt->decode;
    u8 sib;
    int index_reg = 0, base_reg = 0, scale, rip_relative = 0;
    int rc = 0;

    if (c->rex_prefix) {
        c->modrm_reg = (c->rex_prefix & 4) << 1;    /* REX.R */
        index_reg = (c->rex_prefix & 2) << 2; /* REX.X */
        c->modrm_rm = base_reg = (c->rex_prefix & 1) << 3; /* REG.B */
    }

    c->modrm = insn_fetch(u8, 1, c->eip);
    c->modrm_mod |= (c->modrm & 0xc0) >> 6;
    c->modrm_reg |= (c->modrm & 0x38) >> 3;
    c->modrm_rm |= (c->modrm & 0x07);
    c->modrm_ea = 0;
    c->use_modrm_ea = 1;

    if (c->modrm_mod == 3) {
        c->modrm_ptr = decode_register(c->modrm_rm,
                           c->regs, c->d & ByteOp);
        c->modrm_val = *(unsigned long *)c->modrm_ptr;
        return rc;
    }

    if (c->ad_bytes == 2) {
        unsigned bx = c->regs[VCPU_REGS_RBX];
        unsigned bp = c->regs[VCPU_REGS_RBP];
        unsigned si = c->regs[VCPU_REGS_RSI];
        unsigned di = c->regs[VCPU_REGS_RDI];

        /* 16-bit ModR/M decode. */
        switch (c->modrm_mod) {
        case 0:
            if (c->modrm_rm == 6)
                c->modrm_ea += insn_fetch(u16, 2, c->eip);
            break;
        case 1:
            c->modrm_ea += insn_fetch(s8, 1, c->eip);
            break;
        case 2:
            c->modrm_ea += insn_fetch(u16, 2, c->eip);
            break;
        }
        switch (c->modrm_rm) {
        case 0:
            c->modrm_ea += bx + si;
            break;
        case 1:
            c->modrm_ea += bx + di;
            break;
        case 2:
            c->modrm_ea += bp + si;
            break;
        case 3:
            c->modrm_ea += bp + di;
            break;
        case 4:
            c->modrm_ea += si;
            break;
        case 5:
            c->modrm_ea += di;
            break;
        case 6:
            if (c->modrm_mod != 0)
                c->modrm_ea += bp;
            break;
        case 7:
            c->modrm_ea += bx;
            break;
        }
        if (c->modrm_rm == 2 || c->modrm_rm == 3 ||
            (c->modrm_rm == 6 && c->modrm_mod != 0))
            if (!c->override_base)
                c->override_base = &ctxt->ss_base;
        c->modrm_ea = (u16)c->modrm_ea;
    } else {
        /* 32/64-bit ModR/M decode. */
        switch (c->modrm_rm) {
        case 4:
        case 12:
            sib = insn_fetch(u8, 1, c->eip);
            index_reg |= (sib >> 3) & 7;
            base_reg |= sib & 7;
            scale = sib >> 6;

            switch (base_reg) {
            case 5:
                if (c->modrm_mod != 0)
                    c->modrm_ea += c->regs[base_reg];
                else
                    c->modrm_ea +=
                        insn_fetch(s32, 4, c->eip);
                break;
            default:
                c->modrm_ea += c->regs[base_reg];
            }
            switch (index_reg) {
            case 4:
                break;
            default:
                c->modrm_ea += c->regs[index_reg] << scale;
            }
            break;
        case 5:
            if (c->modrm_mod != 0)
                c->modrm_ea += c->regs[c->modrm_rm];
            else if (ctxt->mode == X86EMUL_MODE_PROT64)
                rip_relative = 1;
            break;
        default:
            c->modrm_ea += c->regs[c->modrm_rm];
            break;
        }
        switch (c->modrm_mod) {
        case 0:
            if (c->modrm_rm == 5)
                c->modrm_ea += insn_fetch(s32, 4, c->eip);
            break;
        case 1:
            c->modrm_ea += insn_fetch(s8, 1, c->eip);
            break;
        case 2:
            c->modrm_ea += insn_fetch(s32, 4, c->eip);
            break;
        }
    }
    if (rip_relative) {
        c->modrm_ea += c->eip;
        switch (c->d & SrcMask) {
        case SrcImmByte:
            c->modrm_ea += 1;
            break;
        case SrcImm:
            if (c->d & ByteOp)
                c->modrm_ea += 1;
            else
                if (c->op_bytes == 8)
                    c->modrm_ea += 4;
                else
                    c->modrm_ea += c->op_bytes;
        }
    }
done:
    return rc;
}

static int decode_abs(struct x86_emulate_ctxt *ctxt,
              struct x86_emulate_ops *ops)
{
    struct decode_cache *c = &ctxt->decode;
    int rc = 0;

    switch (c->ad_bytes) {
    case 2:
        c->modrm_ea = insn_fetch(u16, 2, c->eip);
        break;
    case 4:
        c->modrm_ea = insn_fetch(u32, 4, c->eip);
        break;
    case 8:
        c->modrm_ea = insn_fetch(u64, 8, c->eip);
        break;
    }
done:
    return rc;
}

int
x86_decode_insn(struct x86_emulate_ctxt *ctxt, struct x86_emulate_ops *ops)
{
    struct decode_cache *c = &ctxt->decode;
    int rc = 0;
    int mode = ctxt->mode;
    int def_op_bytes, def_ad_bytes, group;

    /* Shadow copy of register state. Committed on successful emulation. */

    memset(c, 0, sizeof(struct decode_cache));
    c->eip = ctxt->vcpu->arch.rip;
    memcpy(c->regs, ctxt->vcpu->arch.regs, sizeof c->regs);

    switch (mode) {
    case X86EMUL_MODE_REAL:
    case X86EMUL_MODE_PROT16:
        def_op_bytes = def_ad_bytes = 2;
        break;
    case X86EMUL_MODE_PROT32:
        def_op_bytes = def_ad_bytes = 4;
        break;
#ifdef CONFIG_X86_64
    case X86EMUL_MODE_PROT64:
        def_op_bytes = 4;
        def_ad_bytes = 8;
        break;
#endif
    default:
        return -1;
    }

    c->op_bytes = def_op_bytes;
    c->ad_bytes = def_ad_bytes;

    /* Legacy prefixes. */
    for (;;) {
        switch (c->b = insn_fetch(u8, 1, c->eip)) {
        case 0x66:    /* operand-size override */
            /* switch between 2/4 bytes */
            c->op_bytes = def_op_bytes ^ 6;
            break;
        case 0x67:    /* address-size override */
            if (mode == X86EMUL_MODE_PROT64)
                /* switch between 4/8 bytes */
                c->ad_bytes = def_ad_bytes ^ 12;
            else
                /* switch between 2/4 bytes */
                c->ad_bytes = def_ad_bytes ^ 6;
            break;
        case 0x2e:    /* CS override */
            c->override_base = &ctxt->cs_base;
            break;
        case 0x3e:    /* DS override */
            c->override_base = &ctxt->ds_base;
            break;
        case 0x26:    /* ES override */
            c->override_base = &ctxt->es_base;
            break;
        case 0x64:    /* FS override */
            c->override_base = &ctxt->fs_base;
            break;
        case 0x65:    /* GS override */
            c->override_base = &ctxt->gs_base;
            break;
        case 0x36:    /* SS override */
            c->override_base = &ctxt->ss_base;
            break;
        case 0x40 ... 0x4f: /* REX */
            if (mode != X86EMUL_MODE_PROT64)
                goto done_prefixes;
            c->rex_prefix = c->b;
            continue;
        case 0xf0:    /* LOCK */
            c->lock_prefix = 1;
            break;
        case 0xf2:    /* REPNE/REPNZ */
            c->rep_prefix = REPNE_PREFIX;
            break;
        case 0xf3:    /* REP/REPE/REPZ */
            c->rep_prefix = REPE_PREFIX;
            break;
        default:
            goto done_prefixes;
        }

        /* Any legacy prefix after a REX prefix nullifies its effect. */

        c->rex_prefix = 0;
    }

done_prefixes:

    /* REX prefix. */
    if (c->rex_prefix)
        if (c->rex_prefix & 8)
            c->op_bytes = 8;    /* REX.W */

    /* Opcode byte(s). */
    c->d = opcode_table[c->b];
    if (c->d == 0) {
        /* Two-byte opcode? */
        if (c->b == 0x0f) {
            c->twobyte = 1;
            c->b = insn_fetch(u8, 1, c->eip);
            c->d = twobyte_table[c->b];
        }
    }

    if (c->d & Group) {
        group = c->d & GroupMask;
        c->modrm = insn_fetch(u8, 1, c->eip);
        --c->eip;

        group = (group << 3) + ((c->modrm >> 3) & 7);
        if ((c->d & GroupDual) && (c->modrm >> 6) == 3)
            c->d = group2_table[group];
        else
            c->d = group_table[group];
    }

    /* Unrecognised? */
    if (c->d == 0) {
        DPRINTF("Cannot emulate %02x\n", c->b);
        return -1;
    }

    if (mode == X86EMUL_MODE_PROT64 && (c->d & Stack))
        c->op_bytes = 8;

    /* ModRM and SIB bytes. */
    if (c->d & ModRM)
        rc = decode_modrm(ctxt, ops);
    else if (c->d & MemAbs)
        rc = decode_abs(ctxt, ops);
    if (rc)
        goto done;

    if (!c->override_base)
        c->override_base = &ctxt->ds_base;
    if (mode == X86EMUL_MODE_PROT64 &&
        c->override_base != &ctxt->fs_base &&
        c->override_base != &ctxt->gs_base)
        c->override_base = NULL;

    if (c->override_base)
        c->modrm_ea += *c->override_base;

    if (c->ad_bytes != 8)
        c->modrm_ea = (u32)c->modrm_ea;
    /*
     * Decode and fetch the source operand: register, memory
     * or immediate.
     */
    switch (c->d & SrcMask) {
    case SrcNone:
        break;
    case SrcReg:
        decode_register_operand(&c->src, c, 0);
        break;
    case SrcMem16:
        c->src.bytes = 2;
        goto srcmem_common;
    case SrcMem32:
        c->src.bytes = 4;
        goto srcmem_common;
    case SrcMem:
        c->src.bytes = (c->d & ByteOp) ? 1 :
                               c->op_bytes;
        /* Don't fetch the address for invlpg: it could be unmapped. */
        if (c->twobyte && c->b == 0x01 && c->modrm_reg == 7)
            break;
    srcmem_common:
        /*
         * For instructions with a ModR/M byte, switch to register
         * access if Mod = 3.
         */
        if ((c->d & ModRM) && c->modrm_mod == 3) {
            c->src.type = OP_REG;
            c->src.val = c->modrm_val;
            c->src.ptr = c->modrm_ptr;
            break;
        }
        c->src.type = OP_MEM;
        break;
    case SrcImm:
        c->src.type = OP_IMM;
        c->src.ptr = (unsigned long *)c->eip;
        c->src.bytes = (c->d & ByteOp) ? 1 : c->op_bytes;
        if (c->src.bytes == 8)
            c->src.bytes = 4;
        /* NB. Immediates are sign-extended as necessary. */
        switch (c->src.bytes) {
        case 1:
            c->src.val = insn_fetch(s8, 1, c->eip);
            break;
        case 2:
            c->src.val = insn_fetch(s16, 2, c->eip);
            break;
        case 4:
            c->src.val = insn_fetch(s32, 4, c->eip);
            break;
        }
        break;
    case SrcImmByte:
        c->src.type = OP_IMM;
        c->src.ptr = (unsigned long *)c->eip;
        c->src.bytes = 1;
        c->src.val = insn_fetch(s8, 1, c->eip);
        break;
    }

    /* Decode and fetch the destination operand: register or memory. */
    switch (c->d & DstMask) {
    case ImplicitOps:
        /* Special instructions do their own operand decoding. */
        return 0;
    case DstReg:
        decode_register_operand(&c->dst, c,
             c->twobyte && (c->b == 0xb6 || c->b == 0xb7));
        break;
    case DstMem:
        if ((c->d & ModRM) && c->modrm_mod == 3) {
            c->dst.type = OP_REG;
            c->dst.val = c->dst.orig_val = c->modrm_val;
            c->dst.ptr = c->modrm_ptr;
            break;
        }
        c->dst.type = OP_MEM;
        break;
    }

done:
    return (rc == X86EMUL_UNHANDLEABLE) ? -1 : 0;
}

static inline void emulate_push(struct x86_emulate_ctxt *ctxt)
{
    struct decode_cache *c = &ctxt->decode;

    c->dst.type  = OP_MEM;
    c->dst.bytes = c->op_bytes;
    c->dst.val = c->src.val;
    register_address_increment(c, &c->regs[VCPU_REGS_RSP], -c->op_bytes);
    c->dst.ptr = (void *) register_address(c, ctxt->ss_base,
                           c->regs[VCPU_REGS_RSP]);
}

static inline int emulate_grp1a(struct x86_emulate_ctxt *ctxt,
                struct x86_emulate_ops *ops)
{
    struct decode_cache *c = &ctxt->decode;
    int rc;

    rc = ops->read_std(register_address(c, ctxt->ss_base,
                        c->regs[VCPU_REGS_RSP]),
               &c->dst.val, c->dst.bytes, ctxt->vcpu);
    if (rc != 0)
        return rc;

    register_address_increment(c, &c->regs[VCPU_REGS_RSP], c->dst.bytes);

    return 0;
}

static inline void emulate_grp2(struct x86_emulate_ctxt *ctxt)
{
    struct decode_cache *c = &ctxt->decode;
    switch (c->modrm_reg) {
    case 0:    /* rol */
        emulate_2op_SrcB("rol", c->src, c->dst, ctxt->eflags);
        break;
    case 1:    /* ror */
        emulate_2op_SrcB("ror", c->src, c->dst, ctxt->eflags);
        break;
    case 2:    /* rcl */
        emulate_2op_SrcB("rcl", c->src, c->dst, ctxt->eflags);
        break;
    case 3:    /* rcr */
        emulate_2op_SrcB("rcr", c->src, c->dst, ctxt->eflags);
        break;
    case 4:    /* sal/shl */
    case 6:    /* sal/shl */
        emulate_2op_SrcB("sal", c->src, c->dst, ctxt->eflags);
        break;
    case 5:    /* shr */
        emulate_2op_SrcB("shr", c->src, c->dst, ctxt->eflags);
        break;
    case 7:    /* sar */
        emulate_2op_SrcB("sar", c->src, c->dst, ctxt->eflags);
        break;
    }
}

static inline int emulate_grp3(struct x86_emulate_ctxt *ctxt,
                   struct x86_emulate_ops *ops)
{
    struct decode_cache *c = &ctxt->decode;
    int rc = 0;

    switch (c->modrm_reg) {
    case 0 ... 1:    /* test */
        emulate_2op_SrcV("test", c->src, c->dst, ctxt->eflags);
        break;
    case 2:    /* not */
        c->dst.val = ~c->dst.val;
        break;
    case 3:    /* neg */
        emulate_1op("neg", c->dst, ctxt->eflags);
        break;
    default:
        DPRINTF("Cannot emulate %02x\n", c->b);
        rc = X86EMUL_UNHANDLEABLE;
        break;
    }
    return rc;
}

static inline int emulate_grp45(struct x86_emulate_ctxt *ctxt,
                   struct x86_emulate_ops *ops)
{
    struct decode_cache *c = &ctxt->decode;

    switch (c->modrm_reg) {
    case 0:    /* inc */
        emulate_1op("inc", c->dst, ctxt->eflags);
        break;
    case 1:    /* dec */
        emulate_1op("dec", c->dst, ctxt->eflags);
        break;
    case 4: /* jmp abs */
        c->eip = c->src.val;
        break;
    case 6:    /* push */
        emulate_push(ctxt);
        break;
    }
    return 0;
}

static inline int emulate_grp9(struct x86_emulate_ctxt *ctxt,
                   struct x86_emulate_ops *ops,
                   unsigned long memop)
{
    struct decode_cache *c = &ctxt->decode;
    u64 old, new;
    int rc;

    rc = ops->read_emulated(memop, &old, 8, ctxt->vcpu);
    if (rc != 0)
        return rc;

    if (((u32) (old >> 0) != (u32) c->regs[VCPU_REGS_RAX]) ||
        ((u32) (old >> 32) != (u32) c->regs[VCPU_REGS_RDX])) {

        c->regs[VCPU_REGS_RAX] = (u32) (old >> 0);
        c->regs[VCPU_REGS_RDX] = (u32) (old >> 32);
        ctxt->eflags &= ~EFLG_ZF;

    } else {
        new = ((u64)c->regs[VCPU_REGS_RCX] << 32) |
               (u32) c->regs[VCPU_REGS_RBX];

        rc = ops->cmpxchg_emulated(memop, &old, &new, 8, ctxt->vcpu);
        if (rc != 0)
            return rc;
        ctxt->eflags |= EFLG_ZF;
    }
    return 0;
}

static inline int writeback(struct x86_emulate_ctxt *ctxt,
                struct x86_emulate_ops *ops)
{
    int rc;
    struct decode_cache *c = &ctxt->decode;

    switch (c->dst.type) {
    case OP_REG:
        /* The 4-byte case *is* correct:
         * in 64-bit mode we zero-extend.
         */
        switch (c->dst.bytes) {
        case 1:
            *(u8 *)c->dst.ptr = (u8)c->dst.val;
            break;
        case 2:
            *(u16 *)c->dst.ptr = (u16)c->dst.val;
            break;
        case 4:
            *c->dst.ptr = (u32)c->dst.val;
            break;    /* 64b: zero-ext */
        case 8:
            *c->dst.ptr = c->dst.val;
            break;
        }
        break;
    case OP_MEM:
        if (c->lock_prefix)
            rc = ops->cmpxchg_emulated(
                    (unsigned long)c->dst.ptr,
                    &c->dst.orig_val,
                    &c->dst.val,
                    c->dst.bytes,
                    ctxt->vcpu);
        else
            rc = ops->write_emulated(
                    (unsigned long)c->dst.ptr,
                    &c->dst.val,
                    c->dst.bytes,
                    ctxt->vcpu);
        if (rc != 0)
            return rc;
        break;
    case OP_NONE:
        /* no writeback */
        break;
    default:
        break;
    }
    return 0;
}

int
x86_emulate_insn(struct x86_emulate_ctxt *ctxt, struct x86_emulate_ops *ops)
{
    unsigned long memop = 0;
    u64 msr_data;
    unsigned long saved_eip = 0;
    struct decode_cache *c = &ctxt->decode;
    int rc = 0;

    /* Shadow copy of register state. Committed on successful emulation.
     * NOTE: we can copy them from vcpu as x86_decode_insn() doesn't
     * modify them.
     */

    memcpy(c->regs, ctxt->vcpu->arch.regs, sizeof c->regs);
    saved_eip = c->eip;

    if (((c->d & ModRM) && (c->modrm_mod != 3)) || (c->d & MemAbs))
        memop = c->modrm_ea;

    if (c->rep_prefix && (c->d & String)) {
        /* All REP prefixes have the same first termination condition */
        if (c->regs[VCPU_REGS_RCX] == 0) {
            ctxt->vcpu->arch.rip = c->eip;
            goto done;
        }
        /* The second termination condition only applies for REPE
         * and REPNE. Test if the repeat string operation prefix is
         * REPE/REPZ or REPNE/REPNZ and if it's the case it tests the
         * corresponding termination condition according to:
         *     - if REPE/REPZ and ZF = 0 then done
         *     - if REPNE/REPNZ and ZF = 1 then done
         */
        if ((c->b == 0xa6) || (c->b == 0xa7) ||
                (c->b == 0xae) || (c->b == 0xaf)) {
            if ((c->rep_prefix == REPE_PREFIX) &&
                ((ctxt->eflags & EFLG_ZF) == 0)) {
                    ctxt->vcpu->arch.rip = c->eip;
                    goto done;
            }
            if ((c->rep_prefix == REPNE_PREFIX) &&
                ((ctxt->eflags & EFLG_ZF) == EFLG_ZF)) {
                ctxt->vcpu->arch.rip = c->eip;
                goto done;
            }
        }
        c->regs[VCPU_REGS_RCX]--;
        c->eip = ctxt->vcpu->arch.rip;
    }

    if (c->src.type == OP_MEM) {
        c->src.ptr = (unsigned long *)memop;
        c->src.val = 0;
        rc = ops->read_emulated((unsigned long)c->src.ptr,
                    &c->src.val,
                    c->src.bytes,
                    ctxt->vcpu);
        if (rc != 0)
            goto done;
        c->src.orig_val = c->src.val;
    }

    if ((c->d & DstMask) == ImplicitOps)
        goto special_insn;


    if (c->dst.type == OP_MEM) {
        c->dst.ptr = (unsigned long *)memop;
        c->dst.bytes = (c->d & ByteOp) ? 1 : c->op_bytes;
        c->dst.val = 0;
        if (c->d & BitOp) {
            unsigned long mask = ~(c->dst.bytes * 8 - 1);

            c->dst.ptr = (void *)c->dst.ptr +
                           (c->src.val & mask) / 8;
        }
        if (!(c->d & Mov) &&
                   /* optimisation - avoid slow emulated read */
            ((rc = ops->read_emulated((unsigned long)c->dst.ptr,
                       &c->dst.val,
                      c->dst.bytes, ctxt->vcpu)) != 0))
            goto done;
    }
    c->dst.orig_val = c->dst.val;

special_insn:

    if (c->twobyte)
        goto twobyte_insn;

    switch (c->b) {
    case 0x00 ... 0x05:
          add:        /* add */
        emulate_2op_SrcV("add", c->src, c->dst, ctxt->eflags);
        break;
    case 0x08 ... 0x0d:
          or:        /* or */
        emulate_2op_SrcV("or", c->src, c->dst, ctxt->eflags);
        break;
    case 0x10 ... 0x15:
          adc:        /* adc */
        emulate_2op_SrcV("adc", c->src, c->dst, ctxt->eflags);
        break;
    case 0x18 ... 0x1d:
          sbb:        /* sbb */
        emulate_2op_SrcV("sbb", c->src, c->dst, ctxt->eflags);
        break;
    case 0x20 ... 0x23:
          and:        /* and */
        emulate_2op_SrcV("and", c->src, c->dst, ctxt->eflags);
        break;
    case 0x24:              /* and al imm8 */
        c->dst.type = OP_REG;
        c->dst.ptr = &c->regs[VCPU_REGS_RAX];
        c->dst.val = *(u8 *)c->dst.ptr;
        c->dst.bytes = 1;
        c->dst.orig_val = c->dst.val;
        goto and;
    case 0x25:              /* and ax imm16, or eax imm32 */
        c->dst.type = OP_REG;
        c->dst.bytes = c->op_bytes;
        c->dst.ptr = &c->regs[VCPU_REGS_RAX];
        if (c->op_bytes == 2)
            c->dst.val = *(u16 *)c->dst.ptr;
        else
            c->dst.val = *(u32 *)c->dst.ptr;
        c->dst.orig_val = c->dst.val;
        goto and;
    case 0x28 ... 0x2d:
          sub:        /* sub */
        emulate_2op_SrcV("sub", c->src, c->dst, ctxt->eflags);
        break;
    case 0x30 ... 0x35:
          xor:        /* xor */
        emulate_2op_SrcV("xor", c->src, c->dst, ctxt->eflags);
        break;
    case 0x38 ... 0x3d:
          cmp:        /* cmp */
        emulate_2op_SrcV("cmp", c->src, c->dst, ctxt->eflags);
        break;
    case 0x40 ... 0x47: /* inc r16/r32 */
        emulate_1op("inc", c->dst, ctxt->eflags);
        break;
    case 0x48 ... 0x4f: /* dec r16/r32 */
        emulate_1op("dec", c->dst, ctxt->eflags);
        break;
    case 0x50 ... 0x57:  /* push reg */
        c->dst.type  = OP_MEM;
        c->dst.bytes = c->op_bytes;
        c->dst.val = c->src.val;
        register_address_increment(c, &c->regs[VCPU_REGS_RSP],
                       -c->op_bytes);
        c->dst.ptr = (void *) register_address(
            c, ctxt->ss_base, c->regs[VCPU_REGS_RSP]);
        break;
    case 0x58 ... 0x5f: /* pop reg */
    pop_instruction:
        if ((rc = ops->read_std(register_address(c, ctxt->ss_base,
            c->regs[VCPU_REGS_RSP]), c->dst.ptr,
            c->op_bytes, ctxt->vcpu)) != 0)
            goto done;

        register_address_increment(c, &c->regs[VCPU_REGS_RSP],
                       c->op_bytes);
        c->dst.type = OP_NONE;    /* Disable writeback. */
        break;
    case 0x63:        /* movsxd */
        if (ctxt->mode != X86EMUL_MODE_PROT64)
            goto cannot_emulate;
        c->dst.val = (s32) c->src.val;
        break;
    case 0x6a: /* push imm8 */
        c->src.val = 0L;
        c->src.val = insn_fetch(s8, 1, c->eip);
        emulate_push(ctxt);
        break;
    case 0x6c:        /* insb */
    case 0x6d:        /* insw/insd */
         if (kvm_emulate_pio_string(ctxt->vcpu, NULL,
                1,
                (c->d & ByteOp) ? 1 : c->op_bytes,
                c->rep_prefix ?
                address_mask(c, c->regs[VCPU_REGS_RCX]) : 1,
                (ctxt->eflags & EFLG_DF),
                register_address(c, ctxt->es_base,
                         c->regs[VCPU_REGS_RDI]),
                c->rep_prefix,
                c->regs[VCPU_REGS_RDX]) == 0) {
            c->eip = saved_eip;
            return -1;
        }
        return 0;
    case 0x6e:        /* outsb */
    case 0x6f:        /* outsw/outsd */
        if (kvm_emulate_pio_string(ctxt->vcpu, NULL,
                0,
                (c->d & ByteOp) ? 1 : c->op_bytes,
                c->rep_prefix ?
                address_mask(c, c->regs[VCPU_REGS_RCX]) : 1,
                (ctxt->eflags & EFLG_DF),
                register_address(c, c->override_base ?
                            *c->override_base :
                            ctxt->ds_base,
                         c->regs[VCPU_REGS_RSI]),
                c->rep_prefix,
                c->regs[VCPU_REGS_RDX]) == 0) {
            c->eip = saved_eip;
            return -1;
        }
        return 0;
    case 0x70 ... 0x7f: /* jcc (short) */ {
        int rel = insn_fetch(s8, 1, c->eip);

        if (test_cc(c->b, ctxt->eflags))
            jmp_rel(c, rel);
        break;
    }
    case 0x80 ... 0x83:    /* Grp1 */
        switch (c->modrm_reg) {
        case 0:
            goto add;
        case 1:
            goto or;
        case 2:
            goto adc;
        case 3:
            goto sbb;
        case 4:
            goto and;
        case 5:
            goto sub;
        case 6:
            goto xor;
        case 7:
            goto cmp;
        }
        break;
    case 0x84 ... 0x85:
        emulate_2op_SrcV("test", c->src, c->dst, ctxt->eflags);
        break;
    case 0x86 ... 0x87:    /* xchg */
        /* Write back the register source. */
        switch (c->dst.bytes) {
        case 1:
            *(u8 *) c->src.ptr = (u8) c->dst.val;
            break;
        case 2:
            *(u16 *) c->src.ptr = (u16) c->dst.val;
            break;
        case 4:
            *c->src.ptr = (u32) c->dst.val;
            break;    /* 64b reg: zero-extend */
        case 8:
            *c->src.ptr = c->dst.val;
            break;
        }
        /*
         * Write back the memory destination with implicit LOCK
         * prefix.
         */
        c->dst.val = c->src.val;
        c->lock_prefix = 1;
        break;
    case 0x88 ... 0x8b:    /* mov */
        goto mov;
    case 0x8d: /* lea r16/r32, m */
        c->dst.val = c->modrm_ea;
        break;
    case 0x8f:        /* pop (sole member of Grp1a) */
        rc = emulate_grp1a(ctxt, ops);
        if (rc != 0)
            goto done;
        break;
    case 0x9c: /* pushf */
        c->src.val =  (unsigned long) ctxt->eflags;
        emulate_push(ctxt);
        break;
    case 0x9d: /* popf */
        c->dst.ptr = (unsigned long *) &ctxt->eflags;
        goto pop_instruction;
    case 0xa0 ... 0xa1:    /* mov */
        c->dst.ptr = (unsigned long *)&c->regs[VCPU_REGS_RAX];
        c->dst.val = c->src.val;
        break;
    case 0xa2 ... 0xa3:    /* mov */
        c->dst.val = (unsigned long)c->regs[VCPU_REGS_RAX];
        break;
    case 0xa4 ... 0xa5:    /* movs */
        c->dst.type = OP_MEM;
        c->dst.bytes = (c->d & ByteOp) ? 1 : c->op_bytes;
        c->dst.ptr = (unsigned long *)register_address(c,
                           ctxt->es_base,
                           c->regs[VCPU_REGS_RDI]);
        if ((rc = ops->read_emulated(register_address(c,
              c->override_base ? *c->override_base :
                    ctxt->ds_base,
                    c->regs[VCPU_REGS_RSI]),
                    &c->dst.val,
                    c->dst.bytes, ctxt->vcpu)) != 0)
            goto done;
        register_address_increment(c, &c->regs[VCPU_REGS_RSI],
                       (ctxt->eflags & EFLG_DF) ? -c->dst.bytes
                               : c->dst.bytes);
        register_address_increment(c, &c->regs[VCPU_REGS_RDI],
                       (ctxt->eflags & EFLG_DF) ? -c->dst.bytes
                               : c->dst.bytes);
        break;
    case 0xa6 ... 0xa7:    /* cmps */
        c->src.type = OP_NONE; /* Disable writeback. */
        c->src.bytes = (c->d & ByteOp) ? 1 : c->op_bytes;
        c->src.ptr = (unsigned long *)register_address(c,
                c->override_base ? *c->override_base :
                           ctxt->ds_base,
                           c->regs[VCPU_REGS_RSI]);
        if ((rc = ops->read_emulated((unsigned long)c->src.ptr,
                        &c->src.val,
                        c->src.bytes,
                        ctxt->vcpu)) != 0)
            goto done;

        c->dst.type = OP_NONE; /* Disable writeback. */
        c->dst.bytes = (c->d & ByteOp) ? 1 : c->op_bytes;
        c->dst.ptr = (unsigned long *)register_address(c,
                           ctxt->es_base,
                           c->regs[VCPU_REGS_RDI]);
        if ((rc = ops->read_emulated((unsigned long)c->dst.ptr,
                        &c->dst.val,
                        c->dst.bytes,
                        ctxt->vcpu)) != 0)
            goto done;

        DPRINTF("cmps: mem1=0x%p mem2=0x%p\n", c->src.ptr, c->dst.ptr);

        emulate_2op_SrcV("cmp", c->src, c->dst, ctxt->eflags);

        register_address_increment(c, &c->regs[VCPU_REGS_RSI],
                       (ctxt->eflags & EFLG_DF) ? -c->src.bytes
                                  : c->src.bytes);
        register_address_increment(c, &c->regs[VCPU_REGS_RDI],
                       (ctxt->eflags & EFLG_DF) ? -c->dst.bytes
                                  : c->dst.bytes);

        break;
    case 0xaa ... 0xab:    /* stos */
        c->dst.type = OP_MEM;
        c->dst.bytes = (c->d & ByteOp) ? 1 : c->op_bytes;
        c->dst.ptr = (unsigned long *)register_address(c,
                           ctxt->es_base,
                           c->regs[VCPU_REGS_RDI]);
        c->dst.val = c->regs[VCPU_REGS_RAX];
        register_address_increment(c, &c->regs[VCPU_REGS_RDI],
                       (ctxt->eflags & EFLG_DF) ? -c->dst.bytes
                               : c->dst.bytes);
        break;
    case 0xac ... 0xad:    /* lods */
        c->dst.type = OP_REG;
        c->dst.bytes = (c->d & ByteOp) ? 1 : c->op_bytes;
        c->dst.ptr = (unsigned long *)&c->regs[VCPU_REGS_RAX];
        if ((rc = ops->read_emulated(register_address(c,
                c->override_base ? *c->override_base :
                           ctxt->ds_base,
                         c->regs[VCPU_REGS_RSI]),
                         &c->dst.val,
                         c->dst.bytes,
                         ctxt->vcpu)) != 0)
            goto done;
        register_address_increment(c, &c->regs[VCPU_REGS_RSI],
                       (ctxt->eflags & EFLG_DF) ? -c->dst.bytes
                               : c->dst.bytes);
        break;
    case 0xae ... 0xaf:    /* scas */
        DPRINTF("Urk! I don't handle SCAS.\n");
        goto cannot_emulate;
    case 0xc0 ... 0xc1:
        emulate_grp2(ctxt);
        break;
    case 0xc3: /* ret */
        c->dst.ptr = &c->eip;
        goto pop_instruction;
    case 0xc6 ... 0xc7:    /* mov (sole member of Grp11) */
    mov:
        c->dst.val = c->src.val;
        break;
    case 0xd0 ... 0xd1:    /* Grp2 */
        c->src.val = 1;
        emulate_grp2(ctxt);
        break;
    case 0xd2 ... 0xd3:    /* Grp2 */
        c->src.val = c->regs[VCPU_REGS_RCX];
        emulate_grp2(ctxt);
        break;
    case 0xe8: /* call (near) */ {
        long int rel;
        switch (c->op_bytes) {
        case 2:
            rel = insn_fetch(s16, 2, c->eip);
            break;
        case 4:
            rel = insn_fetch(s32, 4, c->eip);
            break;
        default:
            DPRINTF("Call: Invalid op_bytes\n");
            goto cannot_emulate;
        }
        c->src.val = (unsigned long) c->eip;
        jmp_rel(c, rel);
        c->op_bytes = c->ad_bytes;
        emulate_push(ctxt);
        break;
    }
    case 0xe9: /* jmp rel */
    case 0xeb: /* jmp rel short */
        jmp_rel(c, c->src.val);
        c->dst.type = OP_NONE; /* Disable writeback. */
        break;
    case 0xf4:              /* hlt */
        ctxt->vcpu->arch.halt_request = 1;
        goto done;
    case 0xf5:    /* cmc */
        /* complement carry flag from eflags reg */
        ctxt->eflags ^= EFLG_CF;
        c->dst.type = OP_NONE;    /* Disable writeback. */
        break;
    case 0xf6 ... 0xf7:    /* Grp3 */
        rc = emulate_grp3(ctxt, ops);
        if (rc != 0)
            goto done;
        break;
    case 0xf8: /* clc */
        ctxt->eflags &= ~EFLG_CF;
        c->dst.type = OP_NONE;    /* Disable writeback. */
        break;
    case 0xfa: /* cli */
        ctxt->eflags &= ~X86_EFLAGS_IF;
        c->dst.type = OP_NONE;    /* Disable writeback. */
        break;
    case 0xfb: /* sti */
        ctxt->eflags |= X86_EFLAGS_IF;
        c->dst.type = OP_NONE;    /* Disable writeback. */
        break;
    case 0xfe ... 0xff:    /* Grp4/Grp5 */
        rc = emulate_grp45(ctxt, ops);
        if (rc != 0)
            goto done;
        break;
    }

writeback:
    rc = writeback(ctxt, ops);
    if (rc != 0)
        goto done;

    /* Commit shadow register state. */
    memcpy(ctxt->vcpu->arch.regs, c->regs, sizeof c->regs);
    ctxt->vcpu->arch.rip = c->eip;

done:
    if (rc == X86EMUL_UNHANDLEABLE) {
        c->eip = saved_eip;
        return -1;
    }
    return 0;

twobyte_insn:
    switch (c->b) {
    case 0x01: /* lgdt, lidt, lmsw */
        switch (c->modrm_reg) {
            u16 size;
            unsigned long address;

        case 0: /* vmcall */
            if (c->modrm_mod != 3 || c->modrm_rm != 1)
                goto cannot_emulate;

            rc = kvm_fix_hypercall(ctxt->vcpu);
            if (rc)
                goto done;

            /* Let the processor re-execute the fixed hypercall */
            c->eip = ctxt->vcpu->arch.rip;
            /* Disable writeback. */
            c->dst.type = OP_NONE;
            break;
        case 2: /* lgdt */
            rc = read_descriptor(ctxt, ops, c->src.ptr,
                         &size, &address, c->op_bytes);
            if (rc)
                goto done;
            realmode_lgdt(ctxt->vcpu, size, address);
            /* Disable writeback. */
            c->dst.type = OP_NONE;
            break;
        case 3: /* lidt/vmmcall */
            if (c->modrm_mod == 3 && c->modrm_rm == 1) {
                rc = kvm_fix_hypercall(ctxt->vcpu);
                if (rc)
                    goto done;
                kvm_emulate_hypercall(ctxt->vcpu);
            } else {
                rc = read_descriptor(ctxt, ops, c->src.ptr,
                             &size, &address,
                             c->op_bytes);
                if (rc)
                    goto done;
                realmode_lidt(ctxt->vcpu, size, address);
            }
            /* Disable writeback. */
            c->dst.type = OP_NONE;
            break;
        case 4: /* smsw */
            c->dst.bytes = 2;
            c->dst.val = realmode_get_cr(ctxt->vcpu, 0);
            break;
        case 6: /* lmsw */
            realmode_lmsw(ctxt->vcpu, (u16)c->src.val,
                      &ctxt->eflags);
            c->dst.type = OP_NONE;
            break;
        case 7: /* invlpg*/
            emulate_invlpg(ctxt->vcpu, memop);
            /* Disable writeback. */
            c->dst.type = OP_NONE;
            break;
        default:
            goto cannot_emulate;
        }
        break;
    case 0x06:
        emulate_clts(ctxt->vcpu);
        c->dst.type = OP_NONE;
        break;
    case 0x08:        /* invd */
    case 0x09:        /* wbinvd */
    case 0x0d:        /* GrpP (prefetch) */
    case 0x18:        /* Grp16 (prefetch/nop) */
        c->dst.type = OP_NONE;
        break;
    case 0x20: /* mov cr, reg */
        if (c->modrm_mod != 3)
            goto cannot_emulate;
        c->regs[c->modrm_rm] =
                realmode_get_cr(ctxt->vcpu, c->modrm_reg);
        c->dst.type = OP_NONE;    /* no writeback */
        break;
    case 0x21: /* mov from dr to reg */
        if (c->modrm_mod != 3)
            goto cannot_emulate;
        rc = emulator_get_dr(ctxt, c->modrm_reg, &c->regs[c->modrm_rm]);
        if (rc)
            goto cannot_emulate;
        c->dst.type = OP_NONE;    /* no writeback */
        break;
    case 0x22: /* mov reg, cr */
        if (c->modrm_mod != 3)
            goto cannot_emulate;
        realmode_set_cr(ctxt->vcpu,
                c->modrm_reg, c->modrm_val, &ctxt->eflags);
        c->dst.type = OP_NONE;
        break;
    case 0x23: /* mov from reg to dr */
        if (c->modrm_mod != 3)
            goto cannot_emulate;
        rc = emulator_set_dr(ctxt, c->modrm_reg,
                     c->regs[c->modrm_rm]);
        if (rc)
            goto cannot_emulate;
        c->dst.type = OP_NONE;    /* no writeback */
        break;
    case 0x30:
        /* wrmsr */
        msr_data = (u32)c->regs[VCPU_REGS_RAX]
            | ((u64)c->regs[VCPU_REGS_RDX] << 32);
        rc = kvm_set_msr(ctxt->vcpu, c->regs[VCPU_REGS_RCX], msr_data);
        if (rc) {
            kvm_inject_gp(ctxt->vcpu, 0);
            c->eip = ctxt->vcpu->arch.rip;
        }
        rc = X86EMUL_CONTINUE;
        c->dst.type = OP_NONE;
        break;
    case 0x32:
        /* rdmsr */
        rc = kvm_get_msr(ctxt->vcpu, c->regs[VCPU_REGS_RCX], &msr_data);
        if (rc) {
            kvm_inject_gp(ctxt->vcpu, 0);
            c->eip = ctxt->vcpu->arch.rip;
        } else {
            c->regs[VCPU_REGS_RAX] = (u32)msr_data;
            c->regs[VCPU_REGS_RDX] = msr_data >> 32;
        }
        rc = X86EMUL_CONTINUE;
        c->dst.type = OP_NONE;
        break;
    case 0x40 ... 0x4f:    /* cmov */
        c->dst.val = c->dst.orig_val = c->src.val;
        if (!test_cc(c->b, ctxt->eflags))
            c->dst.type = OP_NONE; /* no writeback */
        break;
    case 0x80 ... 0x8f: /* jnz rel, etc*/ {
        long int rel;

        switch (c->op_bytes) {
        case 2:
            rel = insn_fetch(s16, 2, c->eip);
            break;
        case 4:
            rel = insn_fetch(s32, 4, c->eip);
            break;
        case 8:
            rel = insn_fetch(s64, 8, c->eip);
            break;
        default:
            DPRINTF("jnz: Invalid op_bytes\n");
            goto cannot_emulate;
        }
        if (test_cc(c->b, ctxt->eflags))
            jmp_rel(c, rel);
        c->dst.type = OP_NONE;
        break;
    }
    case 0xa3:
          bt:        /* bt */
        c->dst.type = OP_NONE;
        /* only subword offset */
        c->src.val &= (c->dst.bytes << 3) - 1;
        emulate_2op_SrcV_nobyte("bt", c->src, c->dst, ctxt->eflags);
        break;
    case 0xab:
          bts:        /* bts */
        /* only subword offset */
        c->src.val &= (c->dst.bytes << 3) - 1;
        emulate_2op_SrcV_nobyte("bts", c->src, c->dst, ctxt->eflags);
        break;
    case 0xb0 ... 0xb1:    /* cmpxchg */
        /*
         * Save real source value, then compare EAX against
         * destination.
         */
        c->src.orig_val = c->src.val;
        c->src.val = c->regs[VCPU_REGS_RAX];
        emulate_2op_SrcV("cmp", c->src, c->dst, ctxt->eflags);
        if (ctxt->eflags & EFLG_ZF) {
            /* Success: write back to memory. */
            c->dst.val = c->src.orig_val;
        } else {
            /* Failure: write the value we saw to EAX. */
            c->dst.type = OP_REG;
            c->dst.ptr = (unsigned long *)&c->regs[VCPU_REGS_RAX];
        }
        break;
    case 0xb3:
          btr:        /* btr */
        /* only subword offset */
        c->src.val &= (c->dst.bytes << 3) - 1;
        emulate_2op_SrcV_nobyte("btr", c->src, c->dst, ctxt->eflags);
        break;
    case 0xb6 ... 0xb7:    /* movzx */
        c->dst.bytes = c->op_bytes;
        c->dst.val = (c->d & ByteOp) ? (u8) c->src.val
                               : (u16) c->src.val;
        break;
    case 0xba:        /* Grp8 */
        switch (c->modrm_reg & 3) {
        case 0:
            goto bt;
        case 1:
            goto bts;
        case 2:
            goto btr;
        case 3:
            goto btc;
        }
        break;
    case 0xbb:
          btc:        /* btc */
        /* only subword offset */
        c->src.val &= (c->dst.bytes << 3) - 1;
        emulate_2op_SrcV_nobyte("btc", c->src, c->dst, ctxt->eflags);
        break;
    case 0xbe ... 0xbf:    /* movsx */
        c->dst.bytes = c->op_bytes;
        c->dst.val = (c->d & ByteOp) ? (s8) c->src.val :
                            (s16) c->src.val;
        break;
    case 0xc3:        /* movnti */
        c->dst.bytes = c->op_bytes;
        c->dst.val = (c->op_bytes == 4) ? (u32) c->src.val :
                            (u64) c->src.val;
        break;
    case 0xc7:        /* Grp9 (cmpxchg8b) */
        rc = emulate_grp9(ctxt, ops, memop);
        if (rc != 0)
            goto done;
        c->dst.type = OP_NONE;
        break;
    }
    goto writeback;

cannot_emulate:
    DPRINTF("Cannot emulate %02x\n", c->b);
    c->eip = saved_eip;
    return -1;
}
