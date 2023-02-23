## B-2 armv6.pdf: Memory Order cheatsheet.

Very subtle chapter.   Sentences can carry the weight of weeks (more) if 
you don't act correctly based on their implications.  Nasty:
  - Code will just work if you make mistakes.
  - Only in full system will you hit (occassionally) the bug.  Which will
    lead to memory corruption.  Which won't show up perhaps for thousands 
    (millions?) of cycles.  The corruption will be "impossible" based on the 
    program addresses.
  - Very hard to test even if do on purpose.  And passing test does not
    mean you did correctly --- just that your test was insufficiently 
    powerful.  (Our code today!)

The following is a raw list of rules from the text and then an attempted
summary.  There might be mistakes.  Let me know if so!

----------------------------------------------------------------------
#### Operations that force order (A before B)

Life is causal.  You (1) open a door and (2) walk through it.  However,
hardware runs many things simultaneously and the implementation is allowed
to do things in different order.  Even with ordering the first operation
A might complete after a second B --- if they are not purely independent
this overlap can cause interference.  An example is clearing the TLB or
cache: this will likely take significant time [measure!]

Operations to add ordering:
  - DMB (2.6.1): all explicit [load/store, but not instruction fetch]
    before DMB are globally *observed* before any explicit [load/store]
    after is observered.

    *HOWEVER*: does not guarantee *completion* of load/store.
    Especially does not guarantee completion of non-load/store operations.
    [page table, tlb flush, etc]  So, in general today, doesn't seem to
    help us much.

  - DSB (2.6.2):  includes all cache, tlb and branch prediction
    maintanance ops as well as load/store.

    DSB completes only after the completion of all preceeding 
      + explicit memory operations
      + cache, branch, tlb maintance

      - NOTE: this only helps order operations w.r.t. loads/stores: necessary
        but not sufficient for instruction fetches.  [see next]

  - PrefetchFlush (2.6.3): flushes pipeline: everything after is
    re-fetched.  do after changes to ASID, tlb ops, branch predictor
    ops, cp15 regs so instructions get refetched w/ right context.
    [XXX: What exactly gets cached or checked when?]

    Also: if you change instruction memory.

----------------------------------------------------------------------
#### 2.7.1: coherence

Tension:
   - correctness: we want memory to act as if there is one copy of X.  -
   speed: X and its meta-data (e.g., VA to PA translations) gets cached in
     different ways.

     when you write X or change its mapping, these different caches may
     not be automatically sync'd: have to seek and destroy all copies
     to don't access stale data.

Example state we have to keep coherent:
  - if data read/written: data cache (several levels) and write buffer
    (if it's modified in cache)
  - if executed: instruction cache , prefetch buffer, branch prediction
    [btac/btb]
  - meta data: tlb entries, asid

  - device memory not synced with cache memory
  - instruction / data not synced.

----------------------------------------------------------------------
#### 2.7.2 Ordering

  + B2.7.2 (b2-21): all cache and branch predictor maintenance operations
    are executed in program order w.r.t. each other.  (Otherwise we'd
    have to stick ordering operations before/after each).

      - A; B  - if A,B are branch predict/cache op, A always has effect
        before B.  A<B.

    [NOTE: are we guaranteed that they *complete*?]
  
  + ... if these appear before an instruction that modifies a page table
    they are guaranteed to occur before page table mod is visible.  (XXX:
    I'm not sure if this applies to modifications in exception handlers?)

  - B2.7.2 (b2-21): page table modifications that occur before management
    operations require the sequence on page B2-22 before the modification
    is guaranteed to be visible.

  + DMB: causes all *cache* (not branch prediction) maintance ops to be
    visible to all explicit loads and stores appearing after the DMB.
    (XXX: instruction prefetch explicit?  I don't think so?)

  - DMB: *does not* ensure visible to all other observers (e.g., page
    table walk done by hardware)

      - `A; DSB; B` --- A will be visible to B if B is an explicit load or
        store.

    Because it doesn't guarantee completion we do not use DMB today :)

  + DSB: *does* ensure completion of all maintance ops / modifications
    so they will be done and visible after the instruction to all
    observers.

      - `A; DSB; B` --- A completed before B for all values of A and B.

  + Prefetch flush or return from exception: causes all branch predict
    operations before to be visible to instructions after. (note how 
    the wording differs from DMB above, which just says "cache maintance 
    operations")

      - `A; PrefetchFlush; B`: the effects of any branch predictor maintance
        op in A will be visible to B.

  - exception causes all BP ops prior to the instruction that caused
    exception to be visible to all instructions after exception entry.
    [including instruction fetch of those inst]

  - Data cache maintance by set/way only guaranteed complete after DSB

  - Instruction cache maintance only guaranteed to be completed after
    DSB barrier.

  - ... visible to instruction fetch after PrefetchFlush or RFE.

  - instruction cache maintance only guaranteed *complete* after DSB
  - instruction cache maintance only guaranteed *visible* after preftech flush

***Summary***:
  - no maintanance operation can influence previous loads or stores.
  - maintanance operation ordered sequentially w.r.t. each other.  thus, 
    an operation cannot "reach back" and affect previous maintanance 
    operations.
  - [only] DSB guarantees completion.
  - Even with DSB, instruction memory requires a PrefetchFlush [data does not]
  - It appears that taking an exception / returning from an exception is
    equivalant to a prefetch flush

------------------------------------------------------------------
#### Close reading of code to modify instruction memory (B2-22)

Code:

        1: STR rx  ; instruction location
        2: clean data cache by MVA ; instruction location
        3: DSB  ; ensures visibility of (2)
        4: invalidate icache by MVA
        5: Invalidate BTB
        6: DSB; ensure (4) completes
        7: Prefetch flush

(A not very satisfying) Discussion:
   - 1,2 don't need ordering b/c maintenance can't effect previous load
     store.
   - (3) ensures (2) completes before the icache invalidation. 

     AFAIK: we need (3) this b/c (2) and (4) do set/way modifications,
     which requires a DSB before visible.  Q: if we nuked the entire
     cache: do we need this?

     Ensuring data cache entry is flushed to memory before invalidating
     the corresponding icache entry makes sense.  However: do we
     absolutely need this if this code does not use the modified code?
     I *think* not.

   - 4,5: these are ordered by the rules of 2.7.2 so we don't need a
     DSB between them.  Also: it appears not to matter b/c no branches.

   - 5: as stated in 2.7.5:  need this b/c the code changed and so the
     branch predictor contents could be stale.

     [NOTE: i'm unclear on how *exactly* the staleness could cause issues.
     is the BTB allowed to have partially decoded?]

   - 5, 7: this sequence required whenever we invalidate BTB (see 2.7.5).

   - 5, 6: I think clearer to reverse (5) and (6) : but all examples
     show in this order. Why?  Just b/c both are long operations and
     there are no branches?  Are they relying on (4) and (5) occuring
     in order?   

----------------------------------------------------------------------
#### B2.7.3 TLB maintance ops

Rules:

   + DSB: just as with cache ops, only thing that guarantees TLB ops complete.
   + PrefetchFlush: causes all previous *completed* to be visible to subsequent
     (including fetch)
   + Exception: causes all TLB maintance ops to be completed.  (same
     as previous)
   + tlb maintance executed in program order w.r.t. each other [but i think
     can be reordered w.r.t. cache / branch ops]
   + tlb ops cannot affect previous loads stores [just was with cache /branch]

   - instruction/unified TLB op only guaranteed to be visible to IF after
    DSB and PrefetchFLush

Summary:
   - same as cache/branch: cannot affect previous loads/stores
   - same as cache/branch: operations occur sequentially w.r.t. each other.
   - same as cache/branch: only DSB guarantees completion.
   - same as cache/branch: instruction mem requires DSB; PrefetchFlush.
 
----------------------------------------------------------------------
#### Rules for writing PTE entry (B2-23)

Main problem:
   - read write to PT memory: need to make sure PT walk (observer) sees.
   - read write PT memory: when does hit TLB?

Rules:
  - write to PT only guarenteed to be seen by PT walk from load/store
    after DSB.
  - write to PT cannot affect previous loads or stores.
  - if PTE cached: must clean cache and force to memory [ hardware
    page table logic allowed to work with with uncached memory]
  - if PTE covers instruction memory: even after clean, and DSB:
    still have to do a prefetch flush.
  - ... from ifetch after DSB;PrefetchFlush


----------------------------------------------------------------------
#### Close reading of code to change PTE (B2-23):

Code (b2-23):

        1: STR rx ; pte
        2: clean line ; pte
        3: DSB  ; ensure visibility of data cleaned from dcache
        4: invalidate tlb entry by MVA ; page address
        5: invalidate btb
        6: DSB; to ensure (4) completes
        7: prefetchflush

Discussion:
   - 1,2: we know 1 and 2 can't be reordered b/c the rules say cache
     ops (2) cannot affect previous loads or stores (1) so do not 
     need a DMB or DSB.
   - 3: after we clean the line (2) we need the DSB (3) to ensure
     completion before invalidating the TLB (4) since we don't have
     guarantees that TLB and cache ops are sequential.  

     [BUT: if we don't do this what really is the issue here?  i think
     the only thing is that the TLB entry could be reloaded before the
     PTE is synced to memory.  if this code does not use the PTE mapping,
     the entry can't be reloaded and I don't think it matters.  not sure.]

   - 4: we need to invalidate the TLB entry b/c we changed the PTE.
   - 4,5: why no DSB?   no branches and code mapped in the same place.
     note: I *do not* think these are guaranteed in order since (4)
     is a TLB entry.

   - why 5?  (2.7.5) says we need to do this after modifying PTE.

     [note: do we need this if it's guaranteed to *not* be instruction
     memory?  maybe no?]

     however: (4) is not necessarily complete even though could effect
     branches.  why ok?  i think b/c we don't take a branch.  if we did?

   - 6: we need the DSB to make sure (4) is done.  If not, (4) might
     not complete and the prefetch flush doesn't help.

     [NOTE: (6) not needed for (5): (7) prefetchflush is enough.]

   - 7: force (5) to be complete.   

     i think it might also be needed if the PTE could affect subsequent
     instructions and thus could have been fetched with old mapping.
     [if *not* instruction memory: don't think is true?]

   - pretty sure interrupts must be off!

   - i think we can flip 5 and 6. 

----------------------------------------------------------------------
##### B2.7.5: Branch predictor maintenance

Note: 2.7.2 also cover BP maintance rules w.r.t. cache maintance
operations.

Rules:

  - invalidation of BTAC (BTB) only guaranteed complete after
    PrefetchFlush or excption/rfe.

    so: whenever see invalidate BTB must have prefetchflush after.

    Q: does it ever make sense to have these without a DSB?  [i don't
    think so since maintance won't be guaranteed, but perhaps with
    PTE mod?]

  - must invalidate BTB after:
     - enable/disable MMU
     - writing new data to instruction memory
     - write new mapping to PTE [but not general mod?]
     - changing TTBR0, TTBR1, TTBCR
     - changing context id

----------------------------------------------------------------------
##### B2.7.6   cp15 regs

Rules:

    - no cp14/cp15 modification can affect previous explicit mem ops.
    - ... only visible to subsequent instructions after a PrefetchFlush
      or excetion/rfe.

Sequentially consistent when:

    - using same register number to read (MRC) after a write (MCR).
    - using same regiser number for two MCR; MCR  the second will
      be the winner.

----------------------------------------------------------------------
#### Close reading of code to change TTBR (B2-25)

Atomicity issue:
  - can't change ASID and TTBR atomically.
  - also: can't flush btb and prefetch buffer at same time as you 
    ASID and TTBR

  - so: old asid can tag TLB entries for new page table
  - so: new asid can tag TLB entries for old page table

Their recommended hack: 

        1. Change ASID to 0
        2. PrefetchFlush
        3. change TTBR to new page table
        4. PrefetchFlush
        5. Change asid to new value.
        6. anything else ???

Discussion:
   - we need (2) b/c instructions could have been fetched/decoded with
      old ASID (not 0).   or is this b/c (1) is not guaranteed to be
      complete before (3)?
   - we need (4) same.
   - 1.3,5 --- from 2.7.6: prefetchflush ensures these cp15 operations
     are visible to instructions after.   these do not seem to count as
     TLB maintance.

   - 6: from 2.7.5 we changed ttbr and context id: we need to do a BTB
     and prefetchflush so the example seems wrong (or at least incomplete)

----------------------------------------------------------------------
#### B2.7.8 `cpsr`

Using cpsr to change permission: need to prefetchflush

----------------------------------------------------------------------
#### Full summary of rules

AFAIK these are the distilled rules:

Ordering:
  - no operation (cp15, branch, tlb, etc) can affect previous loads
     or stores.
  - any change to cp14 and cp15 guaranteed visible to all instructions
    after if you do a prefetchflush (2.7.6) --- do not need a DSB.
  - cache/branch sequentialy consistent w.r.t. each other
  - tlb sequentialy consistent w.r.t. each other
  - cp15 sequentialy consistent w.r.t. each other
  - ops that effect data memory: need DSB to guarantee completion.
  - ops that affect instruction memory: need DSB and then PrefetchFlush.
     in addition may need to flush BTB (see: below)

Invalidate BTB after:
  - enable/disable MMU
  - writing new data to instruction memory
  - write new mapping to PTE [but not general mod?]
  - changing TTBR0, TTBR1, TTBCR
  - changing context id
  - When you flush BTB: need prefetchflush
    after to ensure completes (2.7.5):

            Invalidate BTB
            PrefetchFlush

    NOTE: you *do not* need a DSB.  The examples have this, but for
    other reasons.

Atomicity problems:
  - Cannot change state and flush BTB atomically (i.e.,
    there is a race between changing and flushing the BTB).
  - Cannot change page table pointer register and asid at the same time.
    so can pick up bad mappings.
  - Hack solution:
     1. Mapping this code in same location 
     2. Don't have branches/jumps to avoid BTB issue (why we use assembly)
     3. Use ASID 0 hack.

Misc:
  - AFAIK: must run with interrupts disabled.
  - Page table walks allowed to (must?) work with physical memory
    so any modification requires flushing back to memory.
  - It appears that taking an exception / returning from an exception is
    equivalant to a prefetch flush
