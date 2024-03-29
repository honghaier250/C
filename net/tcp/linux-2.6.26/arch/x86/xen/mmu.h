#ifndef _XEN_MMU_H

#include <linux/linkage.h>
#include <asm/page.h>

enum pt_level {
    PT_PGD,
    PT_PUD,
    PT_PMD,
    PT_PTE
};

/*
 * Page-directory addresses above 4GB do not fit into architectural %cr3.
 * When accessing %cr3, or equivalent field in vcpu_guest_context, guests
 * must use the following accessor macros to pack/unpack valid MFNs.
 *
 * Note that Xen is using the fact that the pagetable base is always
 * page-aligned, and putting the 12 MSB of the address into the 12 LSB
 * of cr3.
 */
#define xen_pfn_to_cr3(pfn) (((unsigned)(pfn) << 12) | ((unsigned)(pfn) >> 20))
#define xen_cr3_to_pfn(cr3) (((unsigned)(cr3) >> 12) | ((unsigned)(cr3) << 20))


void set_pte_mfn(unsigned long vaddr, unsigned long pfn, pgprot_t flags);

void xen_set_pte(pte_t *ptep, pte_t pteval);
void xen_set_pte_at(struct mm_struct *mm, unsigned long addr,
            pte_t *ptep, pte_t pteval);
void xen_set_pmd(pmd_t *pmdp, pmd_t pmdval);

void xen_activate_mm(struct mm_struct *prev, struct mm_struct *next);
void xen_dup_mmap(struct mm_struct *oldmm, struct mm_struct *mm);
void xen_exit_mmap(struct mm_struct *mm);

void xen_pgd_pin(pgd_t *pgd);
//void xen_pgd_unpin(pgd_t *pgd);

pteval_t xen_pte_val(pte_t);
pmdval_t xen_pmd_val(pmd_t);
pgdval_t xen_pgd_val(pgd_t);

pte_t xen_make_pte(pteval_t);
pmd_t xen_make_pmd(pmdval_t);
pgd_t xen_make_pgd(pgdval_t);

void xen_set_pte_at(struct mm_struct *mm, unsigned long addr,
            pte_t *ptep, pte_t pteval);
void xen_set_pte_atomic(pte_t *ptep, pte_t pte);
void xen_set_pud(pud_t *ptr, pud_t val);
void xen_pte_clear(struct mm_struct *mm, unsigned long addr, pte_t *ptep);
void xen_pmd_clear(pmd_t *pmdp);

#endif    /* _XEN_MMU_H */
