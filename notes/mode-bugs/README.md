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

```cpp
bug1-driver.c
```


#### Bug 2: try to switch to Super mode and keep running
#### Bug 3: try to switch to Super mode and keep running
#### Bug 4: try to switch to Super mode and keep running
