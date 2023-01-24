## Some different mode bugs.

We'll talk through some different bugs that can show up when switching
modes.  This does two things:
  1. very concrete examples for how the modes and banked registers work.
  2. sharpens intuition / pattern matching for what the bugs look like.

### Mode review

Note:  this discussion overlaps with some other Notes.   Skim if you
already know it.

We have 18 registers:
  - 16 general purpose registers `r0` --- `r15`.  These often have
    "symbolic" names that are intended to make them easier to remember,
    but can also confuse things.   The most common we use are the 
    stack pointer, `sp` (r13), the link register, `lr` (r14) and 
    the program counter, `pc`, (r15).

  - `cpsr` --- the "current program status register", which holdes
    the current mode (its lowest 5 bits), whether interrupts are enabled,
    and other things that we don't care that much about atm.

  - `spsr` --- the saved `cpsr` from the last fault.  If hardware switches
    from one mode to another via a hardware fault (e.g., a system call,
    breakpoint instruction, interrupt, memory faults) the `cpsr` when
    this fault occurs is saved to the `spsr` and the new mode is put
    in the `cpsr`.  Without doing this, we wouldn't know what mode to
    resume to after handling the fault.

As shown on page A2-3 of the ARMv6 document,  there are 7 modes; four
that we use in this lab:

 - User (`0b10000`) : this is user mode, no privileges.  Privileged
   instruction gives a fault.
 - IRQ (`0b10001`) : mode set on interrupt, privileged
 - Supervisor (`0b10011`) : supervisor mode, privileged
 - System (`0b11111`) : system, privileged

Some key things:

 - All modes other than User and System have their own "shadow" or
   "banked" copies of two general purpose registers: the stack pointer
   and the link register as well as their own copy of the `spsr`.

   These banked registers are persistent on mode switches: if you set
   them, switch to another mode, do stuff, and then switch back, they
   retain their values.  For example, if we are in Supervisor mode,
   set the stack pointer to `8`, switch to System mode, do some stuff,
   and then switch back to Supervisor, `sp` will still hold `8`.

   However, this persistence places no obligation on you to use the
   previous value --- you can always set them how you want.

 - System and User share all registers.  However, all others have banked
   copies so do not interfer with them.  A nasty bug is to forget this
   sharing and trash User mode registers when in System mode.  This shouldn't
   show up today.

 - With the exception of User mode, any other mode can manually switch
   to any other by using the privileged `msr` instruction to forcibly
   set the mode.  User mode has no privileges so cannot use these
   instructions.

   In contrast, the method User mode uses to switch to privileged
   execution is to initiate a system call (using the `swi` instruction)
   which is treated just as any other exception in that it jumps to a
   known location, sets the mode (A2-20) to Supervisor and starts running

In any case, the basic idea isn't that complicated:

  - There are different modes.  
  - These modes have their own copies of `sp`, `lr`, and `spsr`.  
  - They generally share the other registers: switching a mode 
    does not change the value of `r0` or `pc`.  
  - We can switch between privileged modes as we want using `msr` 
    instructions (i.e., in all modes other than User).

Banked registers aren't that subtle.  However,  that doesn't prevent
many bugs in practice:

  - Depending on what your code does, it can create bugs by
    forgetting that it shares registers with other modes (e.g.,
    r0-r12, all unbanked) and trashing them.

  - Your code can *also* have bugs by forgetting it has
    private copies of `sp` or `lr` and attempting to use the previous
    values.

We show some examples below.

### Bugs + examples

#### Bug 1: try to switch to Super mode and keep running

We use variations on the following simple program:
  1. `bug1-driver.c`: C code that gets the current mode from `cpsr`
     and prints out some information.
  2. It then calls buggy assembly (bug1-asm.S)
     to change from the current SUPER
     mode to Supervisor.


The C code:
```cpp
// bug1-driver.c
void notmain(void) {
    uint32_t cpsr = cpsr_get();
    printk("cpsr = <%b>\n", cpsr);

    uint32_t mode = cpsr&0b11111;
    printk("     mode = <%b>\n", mode);
    assert(mode == SUPER_MODE);

    printk("    interrupt = <%s>\n",
                ((cpsr >> 7)&1) ? "off" : "on");

    switch_to_system_bug();
    printk("switched to system\n");
}

```

The assembly code:

        #include "rpi-asm.h"
        
        @ use the mrs instruction to get the current program
        @ status register value and return it in r0 
        .global cpsr_get
        cpsr_get:
            mrs  r0, cpsr
            bx lr

        @ switch to SYSTEM mode (0b11111)
        @ and return. 
        @
        @ the way we do it doesn't work of course.
        @ hint: banked registers. 
        .global switch_to_system_bug
        switch_to_system_bug:
            mov r1,  #0b11111
            orr r1,r1,#(1<<7)    @ disable interrupts.
            @ use msr to set the current mode
            msr cpsr, r1
            @ do a prefetch flush to make sure the msr is done
            @ (similar to a memory barrier)
            prefetch_flush(r1)
            @ done: should be in SYSTEM mode, return.
            bx lr


Some quick review:
  - The first four integer / pointer values are passed in `r0`, `r1`,
     `r2`, and `r3` registers.
  - We return an integer / pointer result in `r0`.
  - When a routine is done we return back to the caller using `bx lr`:
    the `lr` register holds the return address.
  - We can trash any caller-saved register we want without saving it.


What happens if we run this code?

Spoiler:
<details>
    <summary>This is a detail </summery>
   This is some tet

</details>

#### Bug 2: try to switch to Super mode and keep running
#### Bug 3: try to switch to Super mode and keep running
#### Bug 4: try to switch to Super mode and keep running





#### Bug 1: ansser
Now, when we run: what happens?
