#include <linux/types.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

#include "soft-fp.h"
#include "double.h"
#include "single.h"

int
fadds(void *frD, void *frA, void *frB)
{
    FP_DECL_D(A);
    FP_DECL_D(B);
    FP_DECL_D(R);
    int ret = 0;

#ifdef DEBUG
    printk("%s: %p %p %p\n", __func__, frD, frA, frB);
#endif

    __FP_UNPACK_D(A, frA);
    __FP_UNPACK_D(B, frB);

#ifdef DEBUG
    printk("A: %ld %lu %lu %ld (%ld)\n", A_s, A_f1, A_f0, A_e, A_c);
    printk("B: %ld %lu %lu %ld (%ld)\n", B_s, B_f1, B_f0, B_e, B_c);
#endif

    if (A_s != B_s && A_c == FP_CLS_INF && B_c == FP_CLS_INF)
        ret |= EFLAG_VXISI;

    FP_ADD_D(R, A, B);

#ifdef DEBUG
    printk("D: %ld %lu %lu %ld (%ld)\n", R_s, R_f1, R_f0, R_e, R_c);
#endif

    return (ret | __FP_PACK_DS(frD, R));
}
