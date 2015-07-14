#ifndef _ASM_S390_SPARSEMEM_H
#define _ASM_S390_SPARSEMEM_H

#define SECTION_SIZE_BITS    25

#ifdef CONFIG_64BIT

#define MAX_PHYSADDR_BITS    42
#define MAX_PHYSMEM_BITS    42

#else

#define MAX_PHYSADDR_BITS    31
#define MAX_PHYSMEM_BITS    31

#endif /* CONFIG_64BIT */

#endif /* _ASM_S390_SPARSEMEM_H */
