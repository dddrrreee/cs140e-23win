#ifndef __SIMPLE_MMU_H__
#define __SIMPLE_MMU_H__
#include "armv6-cp15.h"

void mmu_init(void);
void staff_mmu_init(void);

void staff_mmu_reset(void);
void mmu_reset(void);

void mmu_disable(void);
void staff_mmu_disable(void);

void mmu_enable(void);
void staff_mmu_enable(void);

void set_procid_ttbr0(unsigned pid, unsigned asid, void *pt);

// set the 16 2-bit access control fields and do any needed coherence.
void domain_access_ctrl_set(uint32_t d);
void staff_domain_access_ctrl_set(uint32_t d);

static inline int mmu_is_enabled(void) {
    return cp15_ctrl_reg1_rd().MMU_enabled != 0;
}

void staff_mmu_disable_set_asm(cp15_ctrl_reg1_t c);
void staff_mmu_enable_set_asm(cp15_ctrl_reg1_t c);

//void cp15_domain_ctrl_wr(uint32_t dom_reg);
void staff_cp15_domain_ctrl_wr(uint32_t dom_reg);

void cp15_set_procid_ttbr0(uint32_t proc_and_asid, void *pt);
void staff_cp15_set_procid_ttbr0(uint32_t proc_and_asid, void *pt);

void set_procid_ttbr0(unsigned pid, unsigned asid, void *pt);
void staff_set_procid_ttbr0(unsigned pid, unsigned asid, void *pt);

void cp15_itlb_inv(void);
void cp15_dtlb_inv(void);
void cp15_tlbs_inv(void);


#endif
