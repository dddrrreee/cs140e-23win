#include "rpi.h"
#include "pinned-vm.h"
#include "mmu.h"
#include "libos-prog.h"

/*
    case   4 * 1024: bits = 0b01; break;
    case  64 * 1024: bits = 0b10; break;
    case      oneMB: bits = 0b11; break;
    case 16 * oneMB: bits = 0b00; break;
*/
enum { 
    PAGE_4K     = 0b01, 
    PAGE_64K    = 0b10, 
    PAGE_1MB    = 0b11, 
    PAGE_16MB   = 0b00
};
enum {  
    _4k     = 4*1024, 
    _64k = 64*1024, 
    _1mb = 1024*1024,
    _16mb = 16*_1mb 
};

static inline unsigned 
pin_nbytes(pin_t attr) {
    switch(attr.pagesize) {
    case 0b01: return _4k;
    case 0b10: return _64k;
    case 0b11: return _1mb;
    case 0b00: return _16mb;
    default: panic("invalid pagesize\n");
    }
}

// 1mb aligned to 1mb, 16mb aligned to 16mb
static inline unsigned 
pin_aligned(uint32_t va, pin_t attr) {
    unsigned n = pin_nbytes(attr);
    switch(n) {
    case _4k:   return va % _4k == 0;
    case _64k:  return va % _64k == 0;
    case _1mb:  return va % _1mb == 0;
    case _16mb: return va % _16mb == 0;
    default: panic("invalid size=%u\n", n);
    }
}

typedef struct {
    uint32_t va, pa;
    pin_t attr;
    unsigned v:1;
} pin_pte_t;

static inline pin_pte_t 
pin_pte_mk(uint32_t va, uint32_t pa, pin_t attr) {
    if(!pin_aligned(va, attr))
        panic("va = %x not aligned to %d\n", va, pin_nbytes(attr));
    return (pin_pte_t) { .va = va, .pa = pa, .attr = attr, .v = 1 };
}

// pinned page table.
enum { MAX_PINS = 8 };

// should we put the asid inside?  for the moment that would makes stuff easy.
typedef struct pin_pt {
    pin_pte_t pins[MAX_PINS];
    unsigned npin;
} pin_pt_t;

static inline unsigned 
pin_pt_add(pin_pt_t *pt, uint32_t va, uint32_t pa, pin_t attr) {
    if(pt->npin >= MAX_PINS)
        panic("too many pins: %d, max=%d\n", pt->npin, MAX_PINS);
    pt->pins[pt->npin++] = pin_pte_mk(va,pa,attr);
    return pt->npin;
}

#include "equiv-os.h"

// this should be part of the pinned page table stuff, but it wasn't 
// fully developed.

enum { NSECTIONS = 60 };
void pin_ident(pin_pt_t *pt, unsigned addr, pin_t attr) {
    uint32_t secn = addr >> 20;

    // don't allocate from section if it's outside of range (wouldn't
    // be used in any case)
    if(secn <= NSECTIONS)
        sec_alloc_exact(secn);
    pin_pt_add(pt, addr, addr, attr);
}

void pin_user(pin_pt_t *pt, uint32_t va_addr, uint32_t pa_addr) {
    pin_t user_attr = pin_mk_user(dom_user, 1, perm_rw_user, MEM_uncached);
    pin_pt_add(pt, va_addr, pa_addr, user_attr);
}

enum {  kern_priv = perm_rw_user };

// pin the global entries: these are true for all processes.
static inline pin_pt_t pin_pt_mk(uint32_t stack_va) {
    pin_pt_t pt = {0};

    pin_t kern_attr = pin_mk_global(dom_kern, kern_priv, MEM_uncached);
    pin_ident(&pt, 0, kern_attr);
    pin_ident(&pt, MB, kern_attr);
    pin_ident(&pt, STACK_ADDR-MB, kern_attr);
    pin_ident(&pt, INT_STACK_ADDR-MB, kern_attr);

    pin_t dev_attr  = pin_mk_global_16mb(dom_kern, kern_priv, MEM_device);
    pin_pt_add(&pt, 0x20000000, 0x20000000, dev_attr);
    pin_pt_add(&pt, 0x20000000, 0x20000000, dev_attr);
    return pt;
}

static int 
pin_is_valid(const pin_pte_t *pte) {
    return pte->v;
}

// pin non-global
static inline int 
pin_nonglobal(pin_pt_t *pt, uint32_t asid, int verbose_p) {
    unsigned ng = 0;
    for(unsigned i = 0; i < MAX_PINS; i++) {
        pin_pte_t e = pt->pins[i];

        // we pin invalid entries to make sure we clear out old
        // entries from previous processes
        if(!pin_is_valid(&e)) {
            // staff_pin_mmu_sec(i,e.va, e.pa, e.attr);
            continue;
        }
        if(e.attr.G)
            continue;

        e.attr.asid = asid;
        if(verbose_p)
            output("pinning %d: %x->%x [asid=%d]\n", 
                i, e.va, e.pa, e.attr.asid);
        staff_pin_mmu_sec(i,e.va, e.pa, e.attr);
        ng++;
    }
    return ng;
}

// we could have a global, constant table and a local per process
// table.  the sum of both should be < MAX_ENTRIES.
//
// ignoring all sorts of digressions: this does make it tricky to 
// swap virtual memory with the memory on.  we sovle this by only
// re-inserting non-global entries.   kinda weird hack, but wv.
static inline pin_pt_t *pin_pt_clone(pin_pt_t *pt_old) {
    pin_pt_t *pt_new = kmalloc(sizeof *pt_old);
    *pt_new = *pt_old;

    // atm we assume is entirely global: 
    for(unsigned i = 0; i < pt_new->npin; i++) {
        pin_pte_t *e = &pt_new->pins[i];
        if(!pin_is_valid(e))
            continue;
        if(!e->attr.G)
            panic("only handling global entries.\n");
    }
    return pt_new;
}


// #define NO_VM
void switch_vm(pctx_t *ctx) {
    proc_t *p = ctx->cur_proc;

    if(config.no_vm_p) {
        assert(!p->asid);
        return;
    }

    // interesting: if you reuse the asid, then even though we 
    // overwrite the pin, i get errors.
    int asid = p->asid;

    // aiya: have to pin invalid entries too, otherwise
    // we can have old entries we don't care about.
    unsigned n = pin_nonglobal(p->pt, asid,0);
    if(n>0) {
        const char *name = p->name;
        if(!name)
            name = "unknown";
        // output("pinned %d entries for name=%s\n", n, name);
    }
    staff_cp15_set_procid_ttbr0(p->pid << 8 | asid, ctx->null_pt);
}

#if 0
#include "fixed/byte-array-prog-vec.h"
#include "fixed/byte-array-prog-exit.h"
#include "fixed/byte-array-prog-alloc.h"
#include "fixed/byte-array-prog-fork.h"
#include "fixed/byte-array-prog-fork-delay.h"
#endif

// do a temporary mapping of <sec>
void *pin_tmp_map_set(unsigned idx, uint32_t va_addr, uint32_t pa_sec) {
    // make sure they passed in an actual address.
    assert(addr_to_sec(va_addr));

    pin_t attr = pin_mk_global(dom_kern, kern_priv, MEM_uncached);
    pin_pte_t e = pin_pte_mk(va_addr, pa_sec<<20, attr);

    assert(idx<8);
    staff_pin_mmu_sec(idx, e.va, e.pa, e.attr);
    return (void*)va_addr;
}

// delete temporary mapping of <sec>
void pin_tmp_map_clr(unsigned idx, uint32_t sec) {
    assert(idx<8);

    // you'll need to implement this.
    staff_pin_clear(idx);
}

// issue here is that i think you can't reuse the VA without clean
// and invalidate all the caches, otherwise old mappings could be
// in the cache and mess things up.  if you map it uncached maybe?
void 
pin_vmcopy(pin_pt_t *pt, void *dst_pa, void *src_va, unsigned nbytes) {
    assert(nbytes>0);
    assert(mmu_is_enabled());
    assert(is_aligned_ptr(dst_pa,1<<20));
    uint32_t sec = ptr_to_sec(dst_pa);
    uint32_t off = ptr_to_off(dst_pa);
    if(off)
        panic("off=%d, dst_pa=%x\n", off, dst_pa);

    // this is a free va addr: we do an uncached copy so i think
    // its fine to not worry about.
    static uint32_t va_addr_tmp = 0x50000000;

    // we assume that entry 7 is unused. 
    output("about to map: %x->%x\n", va_addr_tmp, sec<<20);
    char *dst_va = pin_tmp_map_set(7, va_addr_tmp, sec);
    dst_va += off;

    // make sure doesn't overlap
    assert(ptr_to_sec(dst_va + nbytes-1) == ptr_to_sec(dst_va));

    output("about to copy: %x\n", va_addr_tmp);
    memcpy(dst_va, src_va, nbytes);

    output("done!: %x\n", va_addr_tmp);
    pin_tmp_map_clr(7, sec);

    // if we don't do this doesn't work.
    va_addr_tmp += MB;
}

// allocate an asid.
int asid_get(void) {
    static int asid = 1;
    asid++;
    assert(asid < 64);
    return asid;
}

// ok, kind of a pain in the ass: we can't copy if the vm is on.
// this is exec
proc_t *sys_exec(
        pctx_t *ctx, 
        struct prog *prog,
        uint32_t expected_hash) {

    pin_pt_t *pt = ctx->global_pt;

    // assert(!mmu_is_enabled());
    struct bin_header *bin = (void*)prog->code;
    assert(bin->cookie == 0x12345678);
    assert(bin->header_nbytes == sizeof *bin);
    assert(bin->link_addr == 0x300000);
    unsigned tot = bin->code_nbytes + bin->header_nbytes;
    assert(tot == prog->nbytes);
    tot += bin->bss_nbytes;
    tot = pi_roundup(tot,8);

    uint32_t pc = bin->link_addr + bin->header_nbytes;

    proc_t *p = proc_fork_nostack(ctx, (void*)pc, expected_hash);
    p->pt = pin_pt_clone(pt);
    p->name = prog->name;
    p->prog = prog;
    
    p->asid = asid_get();

    // wait: this could potentially be wrong: we need to know how
    // big the BSS is.
    output("total size = %d (bss=%d)\n", tot, bin->bss_nbytes);
    p->brk_addr_init = p->brk_addr = (void*)(bin->link_addr + tot);
    p->regs[13] = bin->link_addr + MB;

#ifdef NO_VM
    uint32_t secn = sec_alloc_exact(addr_to_sec(bin->link_addr));
#else
    uint32_t secn = sec_alloc();
#endif
    // oh: issue: how do we copy?  it's not aliased.
    pin_user(p->pt, bin->link_addr, sec_to_addr(secn));
    // have to do a temp map
    if(!mmu_is_enabled())
        memcpy(sec_to_ptr(secn), prog->code, prog->nbytes);
    else
        pin_vmcopy(p->pt, sec_to_ptr(secn), prog->code, prog->nbytes);

    return p;
}

static pin_pte_t *pin_pt_find_nonglobal(pin_pt_t *pt) {
    pin_pte_t *non_global = 0;

    for(int i = 0; i < pt->npin; i++) {
        pin_pte_t *e = &pt->pins[i];

        if(!pin_is_valid(e))
            continue;
        if(e->attr.G)
            continue;

        assert(!non_global);
        non_global = e;
    }
    return non_global;
}

static inline pin_pte_t pin_user_mk(uint32_t va, uint32_t pa) {
    pin_t user_attr = pin_mk_user(dom_user, 1, perm_rw_user, MEM_uncached);
    return pin_pte_mk(va,pa,user_attr);
}

// actually: do you just copy the entire thing?  i think so.
int sys_fork(pctx_t *ctx, uint32_t regs[17]) {
    proc_t *cur = ctx->cur_proc;
    proc_t *p = kmalloc_aligned(sizeof *p, 16);

    if(cur->respawn)
        panic("can't sys_fork() a process that can respawn <%s>\n", cur->name);

    // ctx->verbose_p = 1;
    *p = *cur;
    memcpy(p->regs, regs, 17*4);
    p->regs[0] = 0;
    p->asid = asid_get();
    p->pid = pid_get();
    p->pt = kmalloc(sizeof *cur->pt);
    *p->pt = *cur->pt;
    p->nkids = 0;
    cur->nkids++;
    p->next = 0;
    p->parent = cur;
    p->expected_hash = 0;
    p->forked_cnt = cur->nkids;
    assert(p->forked_cnt);
    memset(p->kid_list, 0, sizeof p->kid_list);

    // gross. but whatever.
    assert(cur->nkids < MAX_KIDS);
    cur->kid_list[p->forked_cnt] = p;

    pin_pte_t *pte = pin_pt_find_nonglobal(p->pt);
    assert(pte);
    
    uint32_t secn = sec_alloc();

    *pte = pin_user_mk(pte->va, sec_to_addr(secn));
    // pin_user(p->pt, pte->va, sec_to_addr(secn));

    assert(mmu_is_enabled());
    pin_vmcopy(p->pt, sec_to_ptr(secn), (void*)pte->va, MB);

    runq_enq(ctx, p);
    pstats_forked_inc(&ctx->stats,p);
    return cur->nkids;
}

void vm_respawn(proc_t *p) {
    if(!p->prog)
        return;

    // lockdown_print_entries("respawn");
    assert(p->pt);
    unsigned respawn_p = 0;

    // gross hack: look through for the non-global.
    for(int i = 0; i < p->pt->npin; i++) {
        pin_pte_t *e = &p->pt->pins[i];

        if(!pin_is_valid(e))
            continue;
        if(e->attr.G)
            continue;

        // still in this address space: should be valid.
        struct prog *prog = p->prog;
        struct bin_header *h = (void*)prog->code;

        p->brk_addr = p->brk_addr_init;

        if(e->va != h->link_addr)
            panic("idx=%d: have a link address =%x, but non-global va=%x->pa=%x\n",
                i, h->link_addr, e->va, e->pa);

        // if you have the cache enabled, will have to flush it.
        if(caches_is_enabled())
            todo("need to flush the data out of the cache\n");

        memset((void*)e->va, 0, MB);
        memcpy((void*)e->va, prog->code, prog->nbytes);
        // reset the stack pointer.
        p->regs[13] = h->link_addr + MB;
        respawn_p = 1;
    }
    assert(respawn_p);
    output("respawned: %s!\n", p->prog->name);
}

void equiv_driver(pctx_t *ctx) {
    kmalloc_init_set_start((void*)MB, MB);
    assert(is_aligned_ptr(kmalloc_heap_end(), MB));

    // pinned page table.
    ctx->null_pt = kmalloc_aligned(4096 * 4, 1 << 14);
    demand((unsigned)ctx->null_pt % (1<<14) == 0, must be 14 bit aligned);

    sec_alloc_init(NSECTIONS);

    pin_pt_t pt = pin_pt_mk(0);

    // pin all.
    for(unsigned i = 0; i < MAX_PINS; i++) {
        pin_pte_t *e = &pt.pins[i];
        if(!pin_is_valid(e))
            continue;
        assert(e->attr.G);
        staff_pin_mmu_sec(i,e->va, e->pa, e->attr);
    }

    // should check output since it encodes the scheduling decisions,
    // which should be the same.
    // enum { R = 100 };
    
    config.random_switch = 0;
    config.do_vm_off_on = 0;

    staff_domain_access_ctrl_set(dom_mask);
    assert(!mmu_is_enabled());

    staff_mmu_on_first_time(1, ctx->null_pt);
    assert(mmu_is_enabled());
    for(unsigned i = 0; i < MAX_PINS; i++) { 
        pin_pte_t *e = &pt.pins[i];
        if(!pin_is_valid(e))
            staff_pin_check_exists(e->va);
    }
    assert(mmu_is_enabled());

    ctx->global_pt = kmalloc(sizeof pt);
    *ctx->global_pt = pt;

    // this is supposed to fork anything.
    init(ctx);

#if 0

    if(0) { 
        sys_exec(ctx, &prog_fork_delay, 0x77cc8e24); //  R*2;
        sys_exec(ctx, &prog_fork_delay, 0x77cc8e24); //  R*2;
        sys_exec(ctx, &prog_fork_delay, 0x77cc8e24); //  R*2;
    }

    sys_exec(ctx, &prog_fork_delay, 0x77cc8e24);
    sys_exec(ctx, &prog_fork, 0x48eb8af8);
    for(int i = 0; i < 3; i++) {
        sys_exec(ctx, &prog_vec, 0x2e18f06c)->respawn = R; 
        sys_exec(ctx, &prog_exit, 0xcb436bdd)->respawn = 2*R;
        sys_exec(ctx, &prog_alloc, 0xf8b44b9b)->respawn = 2*R;
    }
#endif

    pstats_start(&ctx->stats);
    pstats_print("start", &ctx->stats, 0);
    assert(mmu_is_enabled());

    brkpt_mismatch_start();
    proc_run_one(ctx);
}
