@ Must carefully read B2 of the armv6 manual.  You cannot rely on "well it
@ worked on my test case": mistakes cause stale data, which may or may not
@ show up in your test case (likely not) despite being wildly broken.
@
@ Note: Rd is allegedly a read-only parameter to these instructions, but
@ the manual says SBZ (should be zero).  I think (and the linux source seems
@ to think) that this means we have to explicitly set it to 0.  Not setting
@ Rd=0 is an easy mistake to make.  Perhaps is worth building in (Linux does
@ not, but) --- the cost of an extra mov is negligible given our other 
@ overheads.
@
@ Alot of people put these instructions in inline assembly, wrapped up by 
@ function calls (e.g., cs107e's MMU code).  This practice is dangerous. 
@ For example, if you are flushing caches, you cannot be sure that the 
@ function return, etc does not then reload them, esp w.r.t. the BTB.  I 
@ think the only safe approach is to put directly in assembly so you are 
@ guaranteed no branch, indirect jump, load/store happens in between.
@
@ A lot of MMU code out there appears pretty broken b/c they don't do enough
@ flushing operations and/or rely on gcc code generation to not do the above.

#include "rpi-asm.h"
#include "armv6-coprocessor-asm.h"

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ armv6-coprocessor-asm.h
@   - has many macros for the privileged instructions.
@   - many page numbers 
@
@ make sure you look at B2 as well as 

@ just read the cache type: use C code to figure out if it is unified
@ b6-13, b6-14
MK_FN(get_cache_type)
    asm_todo("implement this")
    bx lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ part 1:  write to the domain register.
@    easy sequencing rule: look at 2.7.6
@
@ depending on your code, you might name this
@ something else (or inline into the c code):
@  void domain_access_ctrl_set(uint32_t dom_reg);
MK_FN(cp15_domain_ctrl_wr)
    asm_todo("implement this thursday")
    bx lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ part 2: 
@ void mmu_reset(void)
@   - called on boot-up.
@   - clear the dcache, icache, tlbs: make sure you obey 
@     any rules from B2
MK_FN(mmu_reset)
    asm_todo("implement this thursday")
    bx lr



@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ part 3
@ void mmu_disable_set_asm(cp15_ctrl_reg1_t c);
@
@ given the control register value <c>, write it 
@ and do any needed sync
@
@ this is a pretty tricky routine.  make sure you handle the 
@ icache and dcache.   when the mmu is off, these mappings will
@ change.  also, dcache does not work with MMU off.
@
@ make sure you look at arm1176, 6-9.
@
MK_FN(mmu_disable_set_asm)

    @ note: this piece of code is mapped into the sample place in both
    @ MMU-enabled and MMU-disabled so BTB and Prefetch entries work identically
    @ to both MMU-enable and disable.   Without this requirement I believe is
    @ very hard to get right (perhaps impossible?) since we cannot flush
    @ the BTB and prefetch and switch MMU on/off atomically.  thus, would
    @ flush, but instructions could get fetched before we could disable,
    @ defeating the purpose.

    asm_todo("implement this thursday")
    bx lr

@ void mmu_enable_set_asm(cp15_ctrl_reg1_t c);
@  called by <mmu.c:mmu_enable()>
@
@ make sure you look at arm1176, 6-9.
@
MK_FN(mmu_enable_set_asm)
    asm_todo("implement this thursday")
    bx lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ part 4
@
@ void cp15_set_procid_ttbr0(uint32_t proc_and_asid, fld_t *pt);
@     sequence from b2-25 + rules from 3-129
@
@ this is the fanciest one.
MK_FN(cp15_set_procid_ttbr0)
    asm_todo("implement this thursday")
    bx lr


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ you need this for pinned-vm.c
@
@ this just turns the MMU on for the first time.
@ r0 = asid
@ r1 = pointer to empty page table
@
@ is a combination/simplification of mmu_init() and 
@ mmu_enable_set_asm().
@
@ also could just call the above from C code! :)
MK_FN(mmu_on_first_time)
    asm_todo("implement this for pinned vm")
    bx lr
