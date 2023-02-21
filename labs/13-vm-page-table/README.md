## Simple virtual memory.

***This is getting rewritten since we already did fixed virtual memory***

Today you'll get a simple "hello world" version of virtual memory
working.  The background reading and intutive chit-chat is in the
[PRELAB.md](PRELAB.md).  Since there's a lot going on today, the lab
`README.md` has been stripped down to mostly mechanical instructions so
you have more time to look at the code.

Since there are a bunch of data structures (in this case for the machine
state) there's a bunch of data structure code.   The rough breakdown:

   - `staff-*.o`: these are the object files we give you to get you
     started. You can view today's and tues's labs as fetchquests for
     how-do-I-do-X where the goal is to implement everything yourself
     and delete our implementations.

  - `mmu.h`: this has the data structures we will use today.   I've tried
    to comment and give some page numbers, but buyer beware.

  - `mmu-helpers.c`: these contain printing and sanity checking routines.

  - `arm-coprocessor-asm.h`: has a fair number of instructions used to
    access the privileged state (typically using "co-processor 15").
    Sometimes the arm docs do not match the syntax expected by the GNU
    assembler.  You can usually figure out how to do the instruction
    by looking in this file for a related one so you can see how the
    operands are ordered.

   - `docs/README.md` gives a rundown of where some key registers /
     machine state is defined.  In general, if the page numbers begin
     with a `b` they are from the armv6 general documents (the pdf's that
     begin with `armv6` such as `armv6.b2-memory.annot.pdf`) Without a
     letter prefix they come from the `arm1176*` pdf's.

What to modify:

  - `mmu.c`: this will hold your MMU code.  Each routine should have
    a corresponding staff implementation.
  - `vm-ident.c` this has simple calls to setup an identity address space.
  - the various tests.

#### Check-off

You need to show that:
  1. You replaced all `staff_mmu_*` routines with yours and everything works.
  2. You can handle protection and unallowed access faults.

------------------------------------------------------------------------------
#### Virtual memory crash course.

You can perhaps skip this, but to repeat the pre-lab:

 - For today's lab, we will just map 1MB regions at a time.  ARM calls
 these "segments".

 - The page table implements a partial function that maps 
   some number of 1MB virtual segment to an identical number of
   1MB physical segments.    

 - Each page table entry will map a single segment or be marked as
   invalid.

 - For speed some number of entries will be cached in the TLB.  Because
   the hardware will look in the page table when a TLB miss occurs, the
   page table format cannot be changed, and is defined by the architecture
   manual (otherwise the hardware will not know what the bits mean).

 - What is the page-table function's domain?  The r/pi has a 32-bit
   address space, which is 4 billion bytes, 4 billion divided by one
   million is 4096.  Thus, the page table needs to map at most 4096
   virtual segments, starting at zero and counting up to 4096.  Thus the
   function's domain are the integers ``[0..4096)`.

 - What is the page-table funtion's range?  Not including GPIO,
   The r/pi has 512MB of memory, so 512 physical segments.  Thus the
   maximum range are the numbers `[0..512)`.

 - While there are many many details in virtual memory, you can
   mitigate any panic by always keeping in mind our extremely simple goal:
   we need to make a trivial integer function that will map `[0...4096)
   ==> [0..512)`.  (GPIO also adds some numbers to the range, but you
   get the idea.)  You built fancier functions in your intro programming
   class.  (In fact, such a function is so simple I'd bet that it wouldn't
   even rise to a programming assignment.)

The only tricky thing here is that we need ours to be very fast.
This mapping (address translation) happens on every instruction,
twice if the instruction is a load or store.  So as you expect we'll
have one or more caches to keep translations (confusingly called
"translation lookaside buffers").  And, as you can figure out on your
own, if we change the function mapping, these caches have to be updated.
Keeping the contents of a table coherent coherent with a translation
cache is alot of work, so machines generally (always?) punt on this,
and it is up to the implementor to flush any needed cache entries when
the mapping changes. (This flush must either only finish when everything
is flushed, or the implementor must insert a barrier to wait).

Finally, as a detail, we have to tell the hardware where to find the
translations for each different address space.  Typically there is a
register you store a pointer to the table (or tables) in.

The above is pretty much all we will do:
  1. For each virtual address we want to map to a physical address, insert
  the mapping into the table.
  2. Each time we change a mapping, invalidate any cache affected.
  3. Before turning on the MMU, make sure we tell the hardware where to 
     find its translations.

----------------------------------------------------------------------
## Part 0: define the first_level_descriptor structure.

This is a bit basic, but it's good practice.  You'll need to finish the
`struct first_level_descriptor` in file `armv6-vm.h` based on the PTE
layout given on B4-27 (screenshot below):

  - We've defined fields for the section base address, `nG`, `S`,
    `APX`, `TEX`, `AP`, `IMP`, `Domain`, `XN`, `C`, `B`, and the tag.

  - You should look at the structure `struct control_reg1` given in
    `armv6-cp15.h` to see how to use bitfields in C.

Provided helper routines:

  - It is very easy to make mistakes.  If you look in `mmu-helpers.c`
    you can see how the `fld_check_offsets()` routine (modeled on
    `check_control_reg()` that uses the `check_bitfield` macro to verify
    that each field is at its correct bit offset, with its correct
    bit width.

  - There is a routine `fld_print` to print all the fields in your
    structure.

  - HINT: the first field is at offset 0 and the `AssertNow` uses tricks
    to do a compile-time assert.

***When this is done***:
  - the code should compile.
  - `make check` should pass (testing `0-test-structs.c`)

----------------------------------------------------------------------
##### The PTE for 1MB sections document:
<table><tr><td>
  <img src="images/part1-section.png"/>
</td></tr></table>

----------------------------------------------------------------------
## Part 1: make `1-tests*.c` pass

##### Setup the stack mappings in `vm-ident.c`

Add the mapping for the stack and the interrupt stack at the places
indicated. 

   1. Look in `libpi/cs140-start.S` to get where the normal stack is ---
      also recall the stack grows down.
   2. For the interrupt stack use `INT_STACK_ADDR`.

##### Setup your interrupt handlers in `mmu.c:mmu_install_handlers` 

You'll use your `vector_base` code.


***When this is done***:
   - the code should still compile
   - the `1-tests*` should pass `make check`.


----------------------------------------------------------------------
## Part 2: replace the calls to `staff_*`

You'll go through and start implementing your own versions of the 
MMU routines.

  - Implement all routines at the end of `mmu.c` that have an
    `unimplemented()`.

  - If you implement a routine `foo`: you should replace all calls to
    `staff_foo` with calls to `foo` (there are some calls in `1-test*.c`,
    in `mmu.c`, and in `vm-ident.c`.

  - You do not have to implement `domain_access_ctrl_set`: we will do
    this on thursday.  Doing it correctly requires following some rules
    that we don't want to get into here.

You'll write the code to fill in the page table assuming the use of
1MB sections.

The document you'll need for this part is:
  * The annotated B4 of the ARM manual `docs/armv6.b4-mmu.annot.pdf`,
  which describes the page table format(s), and how to setup/manage
  hardware state for page tables and the TLB.


##### implement `mmu_section`

Implement the `mmu_section` routine we used in Part 0.  You'll likely
want to build the `fld_set_base_addr` helper.

The code you wrote then should behave the same.  You'll want to figure
out what all the bits do.  (Hint: most will be set to 0s.)

Useful pages:
  - B4-9: `S`, `R`, `AXP`, `AP` (given below).
  - B4-12: `C`, `B`, `TEX` (given below).
  - B4-25: `nG`, `ASID`, 'XN`.
  - B4-28: bit[18] (the IMP bit) `IMP = 0` for 1MB sections.
  - B4-10: Domain permissions.
  - B4-29: translation of a 1MB section.

The following screenshots are taken from the B4 section, but we inline
them for easy reference:


##### How do I know when I'm done?

***What to do to complete***:
  - remove `staff-mmu.o` from `STAFF_OBJS` in the `Makefile`.
  - to fix a link error, add the following to `mmu.c`:

        void domain_access_ctrl_set(uint32_t r) {
            staff_cp15_domain_ctrl_wr(r);
        }

  - your test should complete as before.

----------------------------------------------------------------------
##### The definitions for `S`, `R`, `AXP`, `AP`:
<table><tr><td>
  <img src="images/part1-s-r-axp-p.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### The definitions for `TEX`, `C`, `B`:
<table><tr><td>
  <img src="images/part1-tex-C-B.png"/>
</td></tr></table>

----------------------------------------------------------------------
##### Description of `XN`, `XP`, etc.

<table><tr><td>
  <img src="images/part1-xp-xn-axp-tex.png"/>
</td></tr></table>

----------------------------------------------------------------------
## Part 2: handle a couple exceptions

*** Fixing the makefile***:
  - add `mmu-except.o` to `SUPPORT_OBJS` in the `Makefile`.

For the next part of the lab, you'll handle two exceptions:
  1. A write close to the end of the stack: you should grow the stack
     and return.
  2. A write to memory that has been marked read-only: you should change
     the permissiona and return.


##### Automatically grow the stack: `2-stack-write.c`

For this test:
  1.  The test is `2-stack-write.c`.
  2. The code is in `mmu-except.c` --- you will need to add this to your
     `Makefile`.  As usual there is a staff version `staff-mmu-except.o`.

What to do:

  - You only have to fill in `data_abort_vector` in `mmu-except.c`.
    You probably want to look at the debug lab to see how to get the fault
    address, etc.

  - The test should run to completion and print `success`.

More detailed, to handle a write to an unmapped section:
  1. Use the "Data Fault Status Register" to get the cause (b4-19,
     b4-20, 
     b4-43,
     b4-44).  And the "Combined Data/FAR" to get the fault address
     (b4-44).  The instruction encodings might be a bit hard to figure
     out, so look in `arm-coprocessor-asm.h`).  You can do these as
     inline assembly (look in
     `cs140e-src/cycle-count.h` for an example); confusingly you'll have
     to use a capitalized opcode `MRC` or `MCR`).

  2. We are doing 1MB segments, so these will be a section violation (encoding
     on b4-20).

  3. Add a mapping for the faulting address, call `mmu_sync_pte_mods()` to
     sync things up, and then return.

  4. As an extension, you can use this method to grow the stack as you access 
     further and further down (heuristic: if the access is within a MB or so 
     of the stack size grow it, otherwise kill it).

###### Catch insufficient privileges: `2-test-no-access-write.c`

A big part of VM is what to do when a translation does not exist,
or the operation on it has insufficient privilege (e.g., a write to a
read-only segment).

Fortunately, handling these operations isn't much different from how we did 
interrupts and system calls:
  1. You define an exception handler (in our case `data_abort_vector`).
  2. When you get a fault, you read the fault status register 
     to get the reason (b4-20) and also the fault address (b4-44).
  3. If the fault is recoverable, you can take an action, otherwise just 
     kill the process (for us: reboot).

To handle a read or write to a section that has insufficient permission:

  1. Get the cause and fault, check that it is a section permission error.
  2. Change the permissions to what the access needs.
  3. Call `mmu_sync_pte_mods()` to sync things up.
  4. Return.


This test:
  - sets the faulting address in `proc.fault_addr`.
  - in `data_abort_vector`:  see if the fault adddress matches this,
    and if so, add the mapping and return.
  - the test should complete.

----------------------------------------------------------------------
## Part 3: die with informative error messages.

Look at the `3-test\*c` tests: these read, write and execute unmapped memory.
Each should die and print an informative error message.

  1. The tests set the address they are reading, writing or jumping to 
     as `proc.die_addr` --- you should check that the fault address you 
     get is the same as this expected fault address.
  
  2. I print something along the lines of:


        3-test-die-unmapped-read.out:ERROR: attempting to load unmapped addr 0x8400000: dying [reason=101]
        3-test-die-unmapped-run.out:ERROR: attempting to run unmapped addr 0x8400000 [reason=101]
        3-test-die-unmapped-write.out:ERROR: attempting to store unmapped addr 0x8400000: dying [reason=101]

        3-test-die-permission-error.out:ERROR: attempting to run addr 0x8400000 with no permissions [reason=1101]
        3-test-die-permission-error-write.out:ERROR: attempting to store addr 0x8400000 permission error: dying [reason=1101]


-----------------------------------------------------------------------
### Extensions

There's a ton of extensions to do:
  1. Do real permissions.
  2. Do domains.
  3. Do smaller pages.
