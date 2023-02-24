### Summarized rules for armv6 / arm1176 

Notes for cs140e.  May not be correct.  If you find a bug please
let us know!

General rules / strategy:
  - everything mapped to same location in global memory
  - no interrupts, no exceptions.
  - no branches (btb flush defeated), no loads (in general).
  - assembly so compiler can't reorder or insert stuff (branches,
    bx lr, ld/st etc)
  - TLB and cache controller is essentially different CPU without any cache
    coherence.
  - change something, must (1) ensure it's done, (2) visible, and then (3) 
    seek and destroy all caches.
  - the modifications *do not effect* the code that does the modifications.

2.7.2 cache maintenance:
  - "dcache; DSB" needed otherwise cache operations not nec complete
      DSB = completed + visible.
  - "icache; DSB; PF" needed for code. DSB = completed, PF=visible.
      (top of B2-22)
  - cache operations do not go backwards: previous instructions safe.
  - cache / branch operations occur in order w.r.t. each other.
  - cache and branch done before PTE mod will complete before it.
  - PTE before cache and branch need B2-22.

tlb maintenance:
  - "TLB maintenance; DSB; PF" needed for both for data/code 
      (bottom B2-22, top of B2-23).  DSB = complete.  PF = visible to 
       the code after.
  - TLB operations occur in order w.r.t. each other.
  - TLB operations do not go backwards: previous instructions safe.
    
pte mods and tlb walk hardware (b2-23):
  - "clean pte from cache; DSB" to make visible to hw.  
  - "clean pte from cache; DSB; PF" to make visible to hw if PT walk caused
      by instruction fetch.  
  - these modifications don't go backwards
  - "PTE; DSB; PF" safest.  always need DSB [good check]

bt (2.7.5): 
  - "flush BTB; PF" always or not guaranteed (PF seems to be DSB for BTB)
  - must do BTB for:
      - enable/disable MMU
      - writing code
      - write new mapping to PTE [modify?]
      - changing TTBR0, TTBR1, TTBCR
      - changing context id (asid+pid)
  - all of these need to end with a PF so the following instructions
    get the update.
  - do not *seem* to need a DSB: (1) 2.7.5 rule doesn't say; (2)
      comments in code don't say; (3) first bullet in 2.7.2 does
      talk about branch operations, but the point about DSB only 
      discusses cache ops, not branc)

cp14/cp15:
  - "cp14/cp14;PF" required so instructions after use any changes. (2.7.6)
  - occur in order if same register
  - "might require other operations" --- have to look at manual.  tricky!
      it's not a huge red flag that "if you don't look can break everything"
      [next]

context id (3.2.47)
  - [procid + asid]
  - 3-129:
    - "must ensure DSB before" [make sure all stores/loads complete before 
          you change context.]
    - "must execute IMB *immediately* after"
    - must run in global b/n changing contextid and IMB.
    - for debugging procid has to be unique.  dunno if matters for us.

 [unrelated: note, can use 3.2.48 registers for whatever.]

  didn't see other stuff.

- read 5-1 through 5-5 for BTAC and prediction.


------------------------------------------------------------------
### Self-modifying code  (B2-22)

Note: if not self-modify (where code already has run and now
we change its bits) can eliminate stuff.

Code: 

        1: STR rx  ; instruction location
        2: clean data cache by MVA ; instruction location
        3: DSB  ; ensures visibility of (2)
        4: invalidate icache by MVA
        5: Invalidate BTB
        6: DSB; ensure (4) completes --- do we need for (5)?
        7: Prefetch flush

notes:
  - 1&2 [easy] ordered by hardware since the MVA overlaps.
  - 2,3,4 [easy]  A=2; DSB; B=4  dsb needed to ensure that cleaning dcache
          (A) happens before invalidating icache (B).  otherwise could refetch
          before clean.  (if it's other location, don't think so?  i.e., is 
          icache allowed to spontaneously refill?)

  - 4,5 occur in order because all cache + BTB ops occur in order w.r.t.
          each other (2.7.2, first bullet)

  - 5: we have to do this b/c the instructions changed (though if this is 
          new code, do we really? or is that redundant? i think for both
          don't need 4 or 5 since will get a compulsory miss)

  - can we flip 4 and 5?   maybe, but clearer to not?
        
  - 6: DSB ensures (4) is done (easy).  does it also need to ensure flush
          BTB visible?
         
  - 7 [easy] have to end w so all previous are visible to the instructions
          after.  

    note sure if can reorder BTB and icache inv.  the current code
    won't apply.  i think can do, but its weird --- don't do b/c you can,
    do what is definitely ok.
------------------------------------------------------------------
#### ASID / PT change (B2-25)

Code:

        1. Change ASID to 0
        2. PrefetchFlush
        3. change TTBR to new page table
        4. PrefetchFlush
        5. Change asid to new value.
        6. anything else ???

Issue:
 - cannot atomically switch atomic and PT.
 - will run with the wrong page table <---> wrong asid.

Strategy:
 - map in the same location.
 - no branches, no loads [other than instruction load]
 - burn asid 0 for the window where things are incoherent.

Notes:
 - 1: before this at step 0 *must have* a DSB. 3-129 
 - 2: changing to asid 0 is a hack so that cache pollution doesn't matter.
 - 3: PF needed b/c 3-129
 - 3: change to new page table [have to]
 - 4: PF b/c 2.7.6: this is a change to cp15 (we aren't doing cache/tlb
         *maintenance) we are just changing some registers.
 - 5: the right context id.
 - 6: need PF.  they say.
 - 7: from 2.7.5: we changed ttbr and context id: we need to do a BTB
         and prefetchflush so the example seems wrong (or at least
         incomplete)
-----------------------------------------------------------------
#### PTE modification (b2-23)

Code:

        1: STR rx ; pte
        2: clean line ; pte
        3: DSB  ; ensure visibility of data cleaned from dcache
        4: invalidate tlb entry by MVA ; page address
        5: invalidate btb
        6: DSB; to ensure (4) completes
        7: prefetchflush

 - 1;2 --- ordered b/c the MVA [cite]  cache ops cannot affect previous
        loads or stores.   so don't need a DSB/DMB.
 - 3: after (2) need a DSB so to ensure completion before invalidating
      the TLB (4) since we don't have guarantees that TLB and cache ops
      are sequential.
 - 4: invalidate tlb [since it might have wrong mapping]
 - 4;5: hmmm.  i don't think 4 and 5 are ordered b/c BTB and TLB 
      not ordered and TLB not completed (bc no DSB)

      works b/c no branches, and code isn't effected by PTE mapping and
      the entry won't be reloaded [no exceptions/interrupts]  and we
      *will* have DSB later.
 - why 5?  (2.7.5) says we need to do this after modifying PTE.
 - 6: we need the DSB to make sure (4) is done.  If not, (4) might
     not complete and the prefetch flush doesn't help.
 - 7: PF needed after BTB flush.
 - i *think* can flip 4 and 5 and also 5 and 6.

