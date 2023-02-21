## Hooking translation up to hardware

***NOTE***:
   - I'm rewriting the lab alot over the weekend (we didn't have pinned
     memory last year) but everything in this one should be relevant
     for either tuesday or thursday.

Last lab we did the page table, the main noun of the VM universe.  This
lab we do the main gerunds needed to hook it up to the hardware: 
   - setting up domains.
   - setting up the page table register and ASID.
   - turning on the MMU.
   - making sure the state is coherent.  

You'll write assembly helper routines implement these (put them
in `12-vm/code/your-mmu-asm.s`) and then at the end remove our
`staff-vm-asm.o` from the `Makefile`.  Mechanically, you will go through,
one-at-a-time and replace every function prefixed with `staff_` to be
your own code.  The code is setup so that you can knock these off one
at a time, making sure that things work after each modification.

If you look in `mmu.h` you'll see the five routines you have to 
implement, which will be in `mmu-asm.S`:

    void cp15_domain_ctrl_wr(uint32_t dom_reg);
    void mmu_reset(void);
    void cp15_set_procid_ttbr0(uint32_t proc_and_asid, fld_t *pt);
    void mmu_disable_set_asm(cp15_ctrl_reg1_t c);
    void mmu_enable_set_asm(cp15_ctrl_reg1_t c);

You'll use the B2 chapter to figure these out.  Their callers are in
`mmu.c`.

The different arm-specific data structures have migrated to:
  - `armv6-coprocessor-asm.h` --- many useful assembly instructions and page numbers.
  - `armv6-cp15.h` --- the arm coprocessor 15 definition and related things.
  - `armv6-vm.h` --- the arm vm definitions.

#### Workflow

A key part of this lab will be working closely with your peers and
arguing and double-checking each other's reasoning.

Today's code is short, but  difficult.  Possibly it requires more
careful reasoning than any other OS code you will ever write.  Even if
you are very careful, it is hard to write this code correctly.  And if
you get it wrong, it's very hard to detect the error with a test case.
The fact that it "happened to work" on the tests you tried tells you
very little (only that it didn't break, not that it isn't broken).
But, if you ship such code to thousands of customers they will form a
Normal distribution, and on the extrema, some of them will hit the bug.
And you won't be able to figure out what is going on.

So, the way we handle this is how you generally handle things that are
(1) very difficult to reason about and (2) very difficult to test: careful
discussion with peers and lots of specific comments giving the basis and
logical argument for why you are doing what you are doing.  Without this
informal "proof" someone will have a hard time figuring out if you know
what you are doing and if the code does what it purports to do.

This class generally has no real code style requirements.  However, this
lab has a hard requirement: you must write comments for each important
action stating (1) why you decided to do that (give page numbers) and
(2) what you intend to accomplish (give page numbers).

#### Check-off

You're going to write a tiny amount of code (< 10 lines for each part),
but it has to be the right code.  

You will:

  1. Replace all of our code from last lab and show that the tests
     (not many, sorry) run.

     How: run `make emit` with our code, then do `make check` with yours
     for all tests.

  2. Have detailed comments in your `.S` stating why exactly you did
     what you did with page numbers.     

Extensions:
  - There are *tons* of extensions at the bottom.
  - A favorite: Write some additional test cases!  You will learn
    *tons* from designing tests that can catch errors.  Especially good
    is if you can write tests that find if a given memory synchronization
    is missing.

------------------------------------------------------------------------
#### Flushing stale state.

The trickiest part of this lab is not figuring out the instructions 
to change the state we need, but is making sure you do --- exactly ---
the operations needed to flush all stale state throughout the machine. 
As mentioned in the previous lab, the hardware caches:
  - Page table entries (in the TLB).  If you change the page table,
  you need to flush the entries affected.
  - Memory (in both data and instruction caches).  If you change a virtual
  mapping or change addresses, you need to flush all affected entries.
  - The ARM optionally caches branch targets in a "branch target buffer"
  (`BTB`) so that it can predict execution paths better.  Unlike TLB
  entries, these entries are not tagged with an address space identifier
  (`ASID`).  Thus, you need to flush the BTB on almost all VM changes.
  - Further, ARM prefetches instructions: if you change a translation
  or change the address space you are in, the instructions in the 
  buffer are then almost certainly wrong, and you need to flush the
  prefetch buffer.
  - Finally, when you flush a cache or modify ARM co-processor state,
  there is often no guarantee that the operation has completed when the
  instruction finishes!  So you need to insert barriers.

Mistakes in the above are incredibly, incredibly nasty.  I believe if
you have one today, you will never track it down before the quarter ends
(I'd be surprised if there were more than 10 people in our building that
could such bugs):

  1. If you get it wrong, your code will likely "work" fine today.  We are
  running with caches disabled, no branch prediction, and strongly-ordered
  memory accesses so many of the gotcha's can't come up.  However, they
  will make a big entrance later when we are more aggressive about speed.
  And since at that point there will be more going on, it will be hard
  to figure out WTH is going wrong.

  2. Because flaws relate to memory --- what values are returned from
  a read, or what values are written --- they give "impossible" bugs
  that you won't even be looking for, so won't see.  (E.g., a write to a
  location disappears despite you staring right at the store that does it,
  a branch is followed the wrong way despite its condition being true).
  They are the ultimate, Godzilla-level memory corruption.

Thus, as in the `uart` lab, you're going to have to rely very strongly
on the documents from ARM and find the exact prose that states the exact
sequence of (oft non-intuitive) actions you have to do.  

These advices are consolidated towards the end of Section B2 of the
ARMv6 manual (`docs/armv6.b2-memory.annot.pdf`).  Useful pages:
  - B2-25: how to change the address space identifier (ASID). 
  - B6-22: all the different ways to flush caches, memory barriers (various
    snapshots below).  As you figured out in the previous lab, the r/pi A+
    we use has split instruction and data caches.
  - B2-23: how to flush after changing a PTE.
  - B2-24: must flush after a CP15.

----------------------------------------------------------------------
## Part 1: setting up domains.

Deliverables:
  1. You should replace `staff_domain_access_ctrl_set` with yours.
     Make sure you obey any requirements for coherence stated in Chapter B2,
     specifically B2-24.  Make sure the code still works!

  2. Copy `2-test-no-access-write.c` to a new test case
     `3-test-no-access-write.c` and
     change it so that the code does not crash when it (1) executes a
     location we do not allow execution of, (2) writes to a location
     that has writes disabled.

Useful pages:
  - B4-10: what the bit values mean for the `domain` field.
  - B4-15: how addresses are translated and checked for faults.
  - B4-27: the location / size of the `domain` field in the segment 
  page table entry.
  - B4-42: setting the domain register.

Useful intuition:
  - When you flush the `BTB`, you need to do a `PrefetchFlush` to wait for
    it to complete (B2.7.5, p B2-24).

#### Some intuition and background on domains.

ARM has an interesting take on protection.  Like most modern architectures
it allows you to mark pages as invalid, read-only, read-write, executable.
However, it gives you a way to quickly disable these restrictions in a
fine-grained way through the use of domains.

Mechanically it works as follows.
  - each page-table entry (PTE) has a 4-bit field stating which single 
  domain (out of 16 possible) the entry belongs to.

  - the system control register (CP15) has a 32-bit domain register (`c3`,
  page B4-42) that contains 2-bits for each of the 16 domains stating
  what mode each the domain is in.  
    - no-access (`0b00`): no load or store can be done to any virtual
    address belonging to the domain;

  - a "client" (`0b01`): all accesses must be consistent with the
    permissions in their associated PTE;

  - a "manager" (`0b11`): no permission checks are done, can read or
    write any virtual address in the PTE region.

  - B4-15: On each memory reference, the hardware looks up the page
    table entry (in reality: the cached TLB entry) for the virtual address,
    gets the domain number, looks up the 2-bit state of the domain in the
    domain register checks if it is allowed.

As a result, you can quickly do a combination of both removing all access
to a set of regions, and granting all access to others by simply writing
a 32-bit value to a single coprocessor register.

To see how these pieces play together, consider an example where code
with more privileges (e.g., the OS) wants to run code that has less
privileges using the same address translations (e.g., a device driver
it doesn't trust).
   - The OS assigns the device driver a unique domain id (e.g., `2`).
   - The OS tags all PTE entries the driver is allowed to touch with `2`
   in the `domain` field.
   - When the OS is running it sets all domains to manager (`0b11`) mode
   so that it can read and write all memory.
   - When the OS wants to call the device driver, it switches the state of
   domain `2` to be a client (`0b01`) and all other domains as no-access
   (`0b00`).

Result:
  1. When the driver code runs, it cannot corrupt any other kernel memory.
  2. Switching domains is fast compared to switching page tables (the
  typical approach).  
  3. As a nice bonus: All the addresses are the same in both pieces of
  code, which makes many things easier.

In terms of our data structures: 
  - Assume we gave the driver access to the single virtual segment range
  `[0x100000, 0x20000)`, which is segment 1 (`0x100000 >> 20 = 1`).
  
  - We would set the `domain` field for the associated page table entry 
  containing the first level descriptor to `2`: i.e., `pt[1].domain = 2`.

  - Before starting the device driver we would write `0b01 << 2` into
  register 3 of CP15.   I.e., domain 2 is in client mode, all other
  domains (`0, 1, 3..15`) are in no-access mode.

----------------------------------------------------------------------
##### B4-32: Bits to set in Domain
<table><tr><td>
  <img src="images/part2-domain.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B6-21: Flush prefetch buffer and tricks.

<table><tr><td>
  <img src="images/part3-flushprefetch.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B6-22: DSB, DMB instruction

<table><tr><td>
  <img src="images/part3-dsb-dmb.png"/>
</td></tr></table>

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 2: Implement `cp15_set_procid_ttbr0`

Deliverable:
   - Set the page table pointer and address space identifier by replacing
    `staff_set_procid_ttbr0` with yours.  Make sure you can switch between
    multiple address spaces.

Where and what:

  1. B4-41: The hardware has to be able to find the page table when
  there is a TLB miss.  You will write the address of the page table to
  the page table register `ttbr0`, set both `TTBR1` and `TTBRD` to `0`.
  Note the alignment restriction!

  2.  B4-52: The ARM allows each TLB entry to be tagged with an address
  space identifier so you don't have to flush when you switch address
  spaces.  Set the current address space identifier (pick a number between
  `1..63`).

  3. Coherence requirements: B2-21, B2-22, B2-23, B2-24 rules for changing
  page table register. And B2-25 the cookbook for changing an `ASID`.

----------------------------------------------------------------------
##### B4-41: Setting page table pointer.

<table><tr><td>
  <img src="images/part2-control-reg2-ttbr0.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-25: Sync ASID

<table><tr><td>
  <img src="images/part3-sync-asid.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-22: When do you need to flush 

<table><tr><td>
  <img src="images/part3-tlb-maintenance.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-23: How to invalidate after a PTE change

<table><tr><td>
  <img src="images/part3-invalidate-pte.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-24: When to flush BTB

<table><tr><td>
  <img src="images/part3-flush-btb.png"/>
</td></tr></table>

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 3: implement `mmu_enable_set_asm` and `mmu_disable_set_asm`

Now you can write the code to turn the MMU on/off:
  - `mmu_enable_set_asm`
  - `mmu_disable_set_asm`

The high-level sequence is given on page 6-9 of the `arm1176.pdf` document
(screen shot below).  You will also have to flush:
   - all caches (D/I cache, the I/D TLBs)
   - PrefetchBuffer.
   - BTB
   - and wait for everything correctly.

We provided macros for most of these; but you should check that they
are correct.

   * Note that the flush instruction cache operation has bugs in 
     some ARM v6 chips, so we provided the recommended sequences (taken
     from Linux).

----------------------------------------------------------------------
##### 6-9: Protocol for turning on MMU.

<table><tr><td>
  <img src="images/part2-enable-mmu.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B4-39 and B4-40: Bits to set to turn on MMU

<table><tr><td>
  <img src="images/part2-control-reg1.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### B6-21: Various invalidation instructions

<table><tr><td>
  <img src="images/part2-inv-tlb.png"/>
</td></tr></table>

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 4: Get rid of our code.

You should go through and delete all of our files, changing the `Makefile`
to remove references to them.  At this point, all code is written by you!

----------------------------------------------------------------------
----------------------------------------------------------------------
## Extensions
 
We did the bare minimum; lots of useful things to add.

The main one:

  - Write a bunch of tests that can detect problems!  You will get
    serious extra credit and our thanks.  Doing this both gives a good set
    of tests, and is a great way to empirically build your understanding
    of how the machine works.

Some additional ones:

  - Write faster flushing operations:  What we have is very slow in
    that it flushes everything rather than flushing just the needed
    virtual address.  Change the PTE modification code to be more precise.

  - You can make a virtual memory system that does not use page tables
    by carefully adding entries to the "locked" region in the TLB on
    miss (or before).  It's an interesting exercise to redo the VM in
    this way.

  - Use memory protection to improve your digital analyzer error.
    You can use memory protection to eliminate the if-statement
    checks in the logic analyzer and replace it with a loop that
    simply (1) reads the cycle counter and the pin and (2) writes
    them out to a log.  If you do not map the page after the log,
    it will get a fault when it runs past the end --- at this point
    you can reconstruct when the pin changes occured and print them
    out.

    Because you no longer have to check for timeout or for end of log,
    the loop should just be a few instructions that fit within a single
    prefetch.

    This is a very old trick that not a lot of people know about anymore.
    Garbage collectors use it, along with other things.

  - Make a flush routine that only flushes the specific VA information.
    Measure the cost difference (huge).

  - Set up code so that it cleans the cache rather than just invalidates.
  - Write code to make it easy to look up a PTE (`mmu_lookup_pte(void *addr)`)
    and change permissions, write-buffer, etc.
  - Set-up two-level paging.
  - Set-up 16MB paging.
