#ifndef __PINNED_VM_H__
#define __PINNED_VM_H__
// simple interface for pinned virtual memory: most of it is 
// enum and data structures.  you'll build three routines:
#include "libc/bit-support.h"

// ugh: way too much stuff: we just need the enabled routine.
#include "procmap.h"

// kinda iffy to have this here...
enum { MB = 1024 * 1024 };

// kernel domain = 1.  we leave 0 free so can sanity check.
enum { kern_dom = 1 };



#if 0
    // change <output> to <debug> if you want file/line/func
#   define pin_debug(args...) output("PIN_VM:" args)
#else
#   define pin_debug(args...) do { } while(0)
#endif



// helpers for pinning values into the TLB.
//
// should pull out these enums since generally useful.
// but for a lab we keep things in fewer places to make it 
// easier.

// this enum flag is a three bit value
//      AXP:1 << 2 | AP:2 
// so that you can just bitwise or it into the 
// position for AP (which is adjacent to AXP).
//
// if _priv access = kernel only.
// if _user access, implies kernel access (but
// not sure if this should be default).
//
// see: 3-151 for table or B4-9 for more 
// discussion.
typedef enum {
    perm_rw_user = 0b011, // read-write user 
    perm_ro_user = 0b010, // read-only user
    perm_na_user = 0b001, // no access user

    // kernel only, user no access
    perm_ro_priv = 0b101,
    // perm_rw_priv = perm_na_user,
    perm_rw_priv = perm_na_user,
    perm_na_priv = 0b000,
} mem_perm_t;
static inline int mem_perm_islegal(mem_perm_t p) {
    switch(p) {
    case perm_rw_user:
    case perm_ro_user:
    // case perm_na_user:
    case perm_ro_priv:
    case perm_rw_priv:
    case perm_na_priv:
        return 1;
    default:
        // for today just die.
        panic("illegal permission: %b\n", p);
    }
}


// domain permisson enums: see b4-10
enum {
    DOM_no_access   = 0b00, // any access = fault.
    // client accesses check against permission bits in tlb
    DOM_client      = 0b01,
    // don't use.
    DOM_reserved    = 0b10,
    // TLB access bits are ignored.
    DOM_manager     = 0b11,
};


// caching is controlled by the TEX, C, and B bits.
// these are laid out contiguously:
//      TEX:3 | C:1 << 1 | B:1
// so we construct an enum with that value that maps to
// the description.
// from table 6-2 on 6-15:
#define TEX_C_B(tex,c,b)  ((tex) << 2 | (c) << 1 | (b))
typedef enum { 
//                              TEX   C  B 
    // strongly ordered
    // not shared.
    MEM_device     =  TEX_C_B(    0b000,  0, 0),  
    // normal, non-cached
    MEM_uncached   =  TEX_C_B(    0b001,  0, 0),  

    // write back no alloc
    MEM_wb_noalloc =  TEX_C_B(    0b000,  1, 1),  
    // write through no alloc
    MEM_wt_noalloc =  TEX_C_B(    0b000,  1, 0),  

    // missing a lot.
} mem_attr_t;

// attributes.
typedef struct {
    // for today we only handle 1MB sections.
    uint32_t G,         // is this a global entry?
             asid,
             dom,       // domain id
             pagesize;  // can be 1MB or 16MB

    // permissions for needed to access page.
    //
    // see mem_perm_t above: is the bit merge of 
    // APX and AP so can or into AP position.
    mem_perm_t  AP_perm;

    // caching policy for this memory.
    // 
    // see mem_cache_t enum above.
    // see table on 6-16 b4-8/9
    // for today everything is uncacheable.
    mem_attr_t mem_attr;
} pin_t;

// default: 
//  - 1mb section
//  - not global.
static inline pin_t
pin_mk(uint32_t G,
            uint32_t dom,
            uint32_t asid,
            mem_perm_t perm, 
            mem_attr_t mem_attr) {
    demand(mem_perm_islegal(perm), "invalid permission: %b\n", perm);
    demand(dom <= 16, illegal domain id);
    if(G)
        demand(!asid, "should not have a non-zero asid: %d", asid);
    else {
        demand(asid, should have non-zero asid);
        demand(asid > 0 && asid < 64, invalid asid);
    }

#if 0
    // XXX broken.
    perm = perm_rw_user;
    assert(G);
#endif
    return (pin_t) {
            .dom = dom,
            .asid = asid,
            .G = G,
            // 1MB section.
            .pagesize = 0b11,
            // default: uncacheable
            .mem_attr = MEM_uncached,
            .AP_perm = perm };
}

// pattern for overriding values.
static inline pin_t
pin_mem_attr_set(pin_t e, mem_attr_t mem) {
    e.mem_attr = mem;
    return e;
}

// if you want to extend sizes.
static inline pin_t
pin_set_size(pin_t e, unsigned size) {
    enum { oneMB = 1024*1024 };
    unsigned bits;
    switch(size) {
    case   4 * 1024: bits = 0b01; break;
    case  64 * 1024: bits = 0b10; break;
    case      oneMB: bits = 0b11; break;
    case 16 * oneMB: bits = 0b00; break;
    default: panic("invalid size=%d!\n", size);
    }
    e.pagesize = bits;
    return e;
}


// make a dev entry: 
//    -global bit set.
//    - non-cacheable
//    - kernel R/W, user no-access
static inline pin_t
pin_mk_device(uint32_t dom) {
    return pin_mk(1, dom, 0, perm_rw_priv, MEM_device);
}
static inline pin_t
pin_mk_device_16mb(uint32_t dom) {
    pin_t p = pin_mk(1, dom, 0, perm_rw_priv, MEM_device);
    p.pagesize = 0;
    return p;
}

// make a global entry: 
//  - global bit set
//  - asid = 0
static inline pin_t
pin_mk_global(uint32_t dom, mem_perm_t perm, mem_attr_t attr) {
    // G=1, asid=0.
    return pin_mk(1, dom, 0, perm, attr);
}
static inline pin_t
pin_mk_global_16mb(uint32_t dom, mem_perm_t perm, mem_attr_t attr) {
    pin_t p = pin_mk_global(dom,perm,attr);
    p.pagesize = 0;
    return p;
}

// global=0
static inline pin_t
pin_mk_user(uint32_t dom, uint32_t asid, mem_perm_t perm, mem_attr_t attr) {
    return pin_mk(0, dom, asid, perm, attr);
}
static inline pin_t
pin_mk_user_16mb(uint32_t dom, uint32_t asid, mem_perm_t perm, mem_attr_t attr) {
    pin_t p = pin_mk_user(dom, asid, perm, attr);
    p.pagesize = 0;
    return p;
}



/******************************************************************
 * routines for pinnin: you'll implement these three.
 */


// our main routine: map <va>-><pa> with 
// attributes <attr> at position <idx> in the
// TLB.
//
// errors:
//  - idx >= 8.
//  - va already mapped.
void pin_mmu_sec(unsigned idx,
                uint32_t va,
                uint32_t pa,
                pin_t attr);

void staff_pin_mmu_sec(unsigned idx,
                uint32_t va,
                uint32_t pa,
                pin_t attr);

// do a manual translation in tlb and see if exists (1)
// returns the result in <result>
int tlb_contains_va(uint32_t *result, uint32_t va);
int staff_tlb_contains_va(uint32_t *result, uint32_t va);


// turn mmu on for the first time: empty_pt is just a 4k vector
// of 0s so we get a fault if it's ever examined.
void staff_mmu_on_first_time(uint32_t asid, void *empty_pt);
void mmu_on_first_time(uint32_t asid, void *empty_pt);


// turn pinned MMU on.
void pin_mmu_on(procmap_t *p);
void staff_pin_mmu_on(procmap_t *p);

#if 0
// turn pinned MMU on with <vecs>
void staff_pin_mmu_on_v(procmap_t *p, uint32_t vecs[]);
// turn pinned MMU system on with default exception
// vectors
//
// hack: relying on gcc removing this routine if its
// not called so that <default_vec_ints> doesn't have
// to be defined.
static inline void 
pin_mmu_on(procmap_t *p) {
    // install the default vectors.
    extern uint32_t default_vec_ints[];
    staff_pin_mmu_on_v(p, default_vec_ints);
}
#endif

// check that <va> is pinned in the tlb.
void pin_check_exists(uint32_t va);
void staff_pin_check_exists(uint32_t va);

// print <msg> then all valid pinned entries.
// note: if you print everything you see a lot of
// garbage in the non-initialized entires --- i'm 
// not sure if we are guaranteed that these have their
// valid bit set to 0?   
void lockdown_print_entries(const char *msg);

#if 0
// identity map (addr->addr) the device memory with 
// right cache and permission:
//    -global bit set.
//    - non-cacheable
//    - kernel R/W, user no-access
static inline void
pin_ident_map_device(uint32_t idx, uint32_t addr, uint32_t dom) {
    // we use ASID = 0 b/c is global.
    staff_mmu_pin_sec(idx, addr, addr, pin_mk_device(dom));
}
#endif

#endif
