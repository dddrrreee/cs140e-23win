## Hooking translation up to hardware

<p align="center">
  <img src="images/class-bug.png" width="450" />
</p>

Last lab we did the page table, the main noun of the VM universe.  This
lab we do the main gerunds needed to hook it up to the hardware: 
   - setting up domains.
   - setting up the page table register and ASID.
   - turning on the MMU.
   - making sure the state is coherent.  

Today we'll write the hardest code of the quarter, but also do so in 
a way where you are surprised if it is broken.

You'll write assembly helper routines implement these (put them in
`13-vm-page-table/code/your-mmu-asm.S`) and then at the end remove our
`staff-mmu-asm.o` from the makefiles in labs 12 and 13.  Mechanically,
you will go through, one-at-a-time and replace every function prefixed
with `staff_` to be your own code.  The code is setup so that you can
knock these off one at a time, making sure that things work after each
modification.

At that point all the code for two different ways to do VM will be yours.
They aren't fancy, but they are complete.  From these working examples
you should be able to make a much fancier system if you are inclined
or (my favorite) a much faster one.  In addition, these are the most
complex features of the ARM.  They are generally the most complex on any
architecture.  Now that you understand them (roughly): there is nothing
harder.  You have the ability to drop in anywhere and figure things out.

Make sure you've read, re-read, re-re-read:

  - B2, especially B2-18 --- B-25.
  - 3-129 has an addition rule for arm1176.
  - 6-9 and related in the arm1176.pdf.
  - [MEMORY-ORDER-cheatsheet.md](MEMORY-ORDER-cheatsheet.md) is a 
    sprawling summary of the above.
  - [DEUX-MEM-ORDER.md](DEUX-MEM-ORDER.md) is a distillation I wrote
    from scratch today to make sure I understood everything.  I think 
    it's a bit simpler.
  - The BTB/BTAC is described in 5-1 --- 5-6 of the arm1176 pdf.


If you look in `mmu.h` you'll see the five routines you have to 
implement, which will be in `mmu-asm.S`:

    void cp15_domain_ctrl_wr(uint32_t dom_reg);
    void mmu_reset(void);
    void mmu_disable_set_asm(cp15_ctrl_reg1_t c);
    void mmu_enable_set_asm(cp15_ctrl_reg1_t c);
    void cp15_set_procid_ttbr0(uint32_t proc_and_asid, fld_t *pt);
    // note: see the errata for this --- we didn't put this in
    // a part below.
    void mmu_sync_pte_mods(void);

  - `armv6-coprocessor-asm.h` --- many useful assembly 
    instructions and page numbers.

#### Workflow

<img src="images/cats-inspecting.png" align="right" width="450px"/>

The lab approach today is different from the others.  Today you must
have a group of 5-8 people that you have discussed and agreed on the
implementation for each of the operations. 

Today's code is short, but  difficult.  Possibly it requires more careful
reasoning than any other OS code (or other code) you have written.
Even if you are very careful, it is hard to write this code correctly.
And if you get it wrong, it's very hard to detect the error with a test
case.  The fact that it "happened to work" on the tests you tried tells
you very little (only that it didn't break, not that it isn't broken).
But, if you ship such code to thousands of sites that will form a Normal
distribution, and on the extrema, some of them will hit the bug.  And you
won't be able to figure out what is going on.

The Stanford algorithm you learn in most classes is: write a bunch of
code quickly, try a bunch of things until it passes a test case (if you
have one) and then move on.  (Yeah, I know: Pot. Kettle. Black.) This
is worse than nothing in our context.  Better to have never written the
VM code since then you know it doesn't work.

<img src="images/bugs-glowing.png" align="right" width="400px"/>

So, the way we handle this is how you generally handle things that are
(1) very difficult to reason about and (2) very difficult to test:
careful discussion with peers and lots of specific comments giving the
basis and logical argument for why you are doing what you are doing.
Without this informal "proof" someone will have a hard time figuring
out if you know what you are doing and if the code does what it purports
to do.  This kind of careful, manual reasoning is the state of the art
--- there is no silver bullet.  You just have to work closely with your
peers, arguing and double-checking each other's reasoning and having a
cite-able sentence for why you did (or did not do) something and in the
order you chose.

This class generally has no real code style requirements.  However,
this lab has a hard requirement: you must write comments for each
important action stating (1) why you decided to do that (give page
numbers or section numbers) and (2) what you intend to accomplish (give
page numbers).

#### Check-off

You're going to write a tiny amount of code (< 20 lines for each part),
but it has to be the right code.  

You will:

  1. Replace all of our code from labs 12 and 13 lab and show that
     the previous tests run.  (Note: our "tests" are incredibly weak so
     this isn't a high bar; apologies.  Next Tuesday will have a more
     ruthless approach)

  2. ***Have detailed comments in your `.S` stating why exactly you did
     what you did with page numbers.***  This is not optional.

  3. Check-off should be 5-8 people at once where anyone can answer
     questions: "why did you do this?"

Extensions:
  - There are *tons* of extensions at the bottom.
  - A favorite: Write some additional test cases!  You will learn
    *tons* from designing tests that can catch errors.  Especially good
    is if you can write tests that find if a given memory synchronization
    is missing.

------------------------------------------------------------------------
#### Flushing stale state.

<img src="images/swirling-code.png" align="right" width="450px"/>

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
## Part 1: `domain_access_ctrl_set()` 

Most of you already have this, but in case not:
  - Implement `domain_access_ctrl_set()` 
  - Make sure you obey any requirements for coherence stated in Chapter B2,
    specifically B2-24 (2.7.6).  Make sure the code still works!
  - Change `staff_domain_access_ctrl_set` to call it.

Compiling:
  - You'll have to modify your `13-vm-page-table/code/Makefile` so that
    it starts compiling your assembly.  (`COMMON_SRC += your-vm-asm.S`).
  - You'll have to modify your `12-pinned-vm/code/Makefile` to
    start using your assembly as well.  There is now a `Makefile.lab14`
    that gives an example.
  - It's possible because of naming you'll get a duplicate warning
    about this one routine.  You can either change the name, or 
    make it into a weak symbol.  For example:

        int __attribute__((weak)) domain_access_ctrl_set(uint32_t d) {
            ...
        }

Useful pages:
  - B4-10: what the bit values mean for the `domain` field.
  - B4-15: how addresses are translated and checked for faults.
  - B4-27: the location / size of the `domain` field in the segment 
    page table entry.
  - B4-42: setting the domain register.

Useful intuition:
  - When you flush the `BTB`, you need to do a `PrefetchFlush` to wait for
    it to complete (B2.7.5, p B2-24).

----------------------------------------------------------------------
##### B4-32: Bits to set in Domain

<p align="right">
<figure>
  <img src="images/part2-domain.png" width="600" />
  <figcaption><strong>B4-32: Bits to set in Domain.</strong></figcaption>
</figure>
</p>

----------------------------------------------------------------------
##### B6-21: Flush prefetch buffer and tricks.

<table><tr><td>
  <img src="images/part3-flushprefetch.png" width="600" />
</td></tr></table>

----------------------------------------------------------------------
##### B6-22: DSB, DMB instruction

<table><tr><td>
  <img src="images/part3-dsb-dmb.png" width=500/>
</td></tr></table>

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 2: Implement `mmu_reset`

This routine gets called after booting up to set all caches to a clean
state by invalidating them.  The MMU is off and should remain off.

You need to:
  - Invalidate all caches (ITLB, DTLB, data cache, instruction
    cache).  Do *not* clean the data cache since it will potentially 
    write garbage back to memory)
  - Make sure you put in any B2 ordering operations.
  - As mentioned above: `armv6-coprocessor-asm.h` has many 
    useful assembly instructions and page numbers.
  - Replace the calls in both labs and make sure your tests still pass.

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 3: implement `mmu_enable_set_asm` and `mmu_disable_set_asm`

Now you can write the code to turn the MMU on/off:
  - `mmu_enable_set_asm`  (called by `mmu_enable` in `mmu.c`).
  - `mmu_disable_set_asm` (called by `mmu_disable` in `mmu.c`).

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
  <img src="images/part2-enable-mmu.png" width=500/>
</td></tr></table>

----------------------------------------------------------------------
##### B4-39 and B4-40: Bits to set to turn on MMU

<table><tr><td>
  <img src="images/part2-control-reg1.png" width=500/>
</td></tr></table>

----------------------------------------------------------------------
##### B6-21: Various invalidation instructions

<table><tr><td>
  <img src="images/part2-inv-tlb.png" width=500/>
</td></tr></table>

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 4: Implement `cp15_set_procid_ttbr0`

<p align="center">
  <img src="images/robots-inspecting.png" width="450" />
</p>

This is the hardest routine.  Make sure each step makes sense to your
partners and there is an explicit reason you're doing it.


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
  <img src="images/part2-control-reg2-ttbr0.png" width=500/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-25: Sync ASID

<table><tr><td>
  <img src="images/part3-sync-asid.png" width=500/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-22: When do you need to flush 

<table><tr><td>
  <img src="images/part3-tlb-maintenance.png" width=500/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-23: How to invalidate after a PTE change

<table><tr><td>
  <img src="images/part3-invalidate-pte.png" width=500/>
</td></tr></table>

----------------------------------------------------------------------
##### B2-24: When to flush BTB

<table><tr><td>
  <img src="images/part3-flush-btb.png" width=500/>
</td></tr></table>

----------------------------------------------------------------------
----------------------------------------------------------------------
## Part 4: Get rid of our code.

You should go through and delete all uses of our code in the lab 12 and
lab 13 makefiles.  At this point, all code is written by you!

<p align="center">
  <img src="images/done-robot.png" width="450" />
</p>

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
