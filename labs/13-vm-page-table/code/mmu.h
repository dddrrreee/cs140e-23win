#ifndef __MMU_H__
#define __MMU_H__
/******************************************************************************
 * engler, cs140e: simple mmu system interface.
 */

// get the different cp15 data structures.
#include "armv6-cp15.h"
// get the different cp15 data structures.
#include "armv6-vm.h"

/* one time initialation of caches, tlb, etc */
void mmu_init(void);
void staff_mmu_init(void);
// install trap handlers: should make more fine-grained.
void mmu_install_handlers(void);


// allocate page table and initialize.  handles alignment.
//
// Naive: we assume a single page table, which forces it to be 4096 entries.
// Why 4096:
//      - ARM = 32-bit addresses.
//      - 2^32 = 4GB
//      - we use 1MB sections (i.e., "page size" = 1MB).
//      - there are 4096 1MB sections in 4GB (4GB = 1MB * 4096).
//      - therefore page table must have 4096 entries.
//
// Note: If you want 4k pages, need to use the ARM 2-level page table format.
// These also map 1MB (otherwise hard to mix 1MB sections and 4k pages).
fld_t *mmu_pt_alloc(unsigned n_entries);

// initialize a preallocated chunk of memory
fld_t *mmu_pt_init(void *addr, unsigned nbytes);


// map a 1mb section starting at <va> to <pa>
fld_t *mmu_map_section(fld_t *pt, uint32_t va, uint32_t pa, uint32_t dom);
fld_t *staff_mmu_map_section(fld_t *pt, uint32_t va, uint32_t pa, uint32_t dom);
// map <nsec> 1mb sections starting at <va> to <pa>
void mmu_map_sections(fld_t *pt, unsigned va, unsigned pa, unsigned nsec, uint32_t dom);
void staff_mmu_map_sections(fld_t *pt, unsigned va, unsigned pa, unsigned nsec, uint32_t dom);


// lookup section <va> in page table <pt>
fld_t *mmu_lookup(fld_t *pt, uint32_t va);

// called to sync after a set of pte modifications: flushes everything.
void mmu_sync_pte_mods(void);
void staff_mmu_sync_pte_mods(void);

// *<pte> = e.   more precisely flushes state.
void mmu_sync_pte_mod(fld_t *pte, fld_t e);




/*
 * enable, disable init.
 */

// turn on/off mmu: handles all the cache flushing, barriers, etc.
void mmu_enable(void);
void mmu_disable(void);

void staff_mmu_enable(void);
void staff_mmu_disable(void);

// same as disable/enable except client gives the control reg to use --- 
// this allows messing with cache state, etc.
void mmu_disable_set(cp15_ctrl_reg1_t c);
void mmu_enable_set(cp15_ctrl_reg1_t c);

// <pid>= process id.
// <asid>= address space identifier, can't be 0, must be < 64
// pt is the page table address
void set_procid_ttbr0(unsigned pid, unsigned asid, fld_t *pt);
void staff_set_procid_ttbr0(unsigned pid, unsigned asid, fld_t *pt);

// set the 16 2-bit access control fields and do any needed coherence.
void domain_access_ctrl_set(uint32_t d);
void staff_domain_access_ctrl_set(uint32_t d);

// return the domain access control register.
uint32_t domain_access_ctrl_get(void);
uint32_t staff_domain_access_ctrl_get(void);

// set protection for [va,va+nsection) to <perm>
void mmu_mprotect(fld_t *pt, unsigned va, unsigned nsec, unsigned perm);
void staff_mmu_mprotect(fld_t *pt, unsigned va, unsigned nsec, unsigned perm);

// helpers
// mark [va, va+ nsec * 1MB) with the indicated permissions.
static inline void 
mmu_mark_sec_no_access(fld_t *pt, unsigned va, unsigned nsec) 
    { mmu_mprotect(pt,va,nsec, AP_no_access); }
static inline void 
mmu_mark_sec_readonly(fld_t *pt, unsigned va, unsigned nsec) 
    { mmu_mprotect(pt,va,nsec, AP_rd_only); }
static inline void 
mmu_mark_sec_rw(fld_t *pt, unsigned va, unsigned nsec) 
    { mmu_mprotect(pt,va,nsec, AP_rw); }

/*****************************************************************
 * mmu assembly routines: these are mainly what you will implement in
 * lab 15 (today).
 *
 *
 * sort-of organized from easiest to hardest.
 */

// write the values for the domain control reg and do any needed coherence.
//void cp15_domain_ctrl_wr(uint32_t dom_reg);
void staff_cp15_domain_ctrl_wr(uint32_t dom_reg);

// one time initialization of the machine state.  cache/tlb should not be active yet
// so just invalidate.  
//void mmu_reset(void);
void staff_mmu_reset(void);

// sets both the procid and the ttbr0 register: does any needed coherence.
// i *think* we absolutely must do both of these at once.
//void cp15_set_procid_ttbr0(uint32_t proc_and_asid, fld_t *pt);
void staff_cp15_set_procid_ttbr0(uint32_t proc_and_asid, fld_t *pt);

// sets the control register to <c> and does any needed coherence operations.
//void mmu_disable_set_asm(cp15_ctrl_reg1_t c);
//void mmu_enable_set_asm(cp15_ctrl_reg1_t c);

void staff_mmu_disable_set_asm(cp15_ctrl_reg1_t c);
void staff_mmu_enable_set_asm(cp15_ctrl_reg1_t c);



/******************************************************************
 * simple helpers.
 */
int mmu_is_enabled(void);


// b4-20
enum {
    SECTION_XLATE_FAULT = 0b00101,
    SECTION_PERM_FAULT = 0b1101,
};

// mask off the sec bits
static inline unsigned mmu_sec_bits_only(unsigned u) { return u & ~((1<<21)-1); }
// extrac the (virt/phys) section number.
static inline unsigned mmu_sec_num(unsigned u)       { return u >> 20; }



static inline uint32_t mod_pow2(uint32_t x, uint32_t n) {
    assert(n<32);
    uint32_t rem = x % (1<<n);

    if(rem)
        panic("cannot divide %x by 2^%d : remainder=%x\n", x, n, rem);
    return 1;
}
static inline uint32_t mod_pow2_ptr(void *x, uint32_t n) {
    return mod_pow2((uint32_t)x,n);
}

// turn off all the output from mmu helpers
void mmu_be_quiet(void);


#endif
