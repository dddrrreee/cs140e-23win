## A very-incomplete threads writeup

Recall we split threads into two types:
  - Non-preemptive threads run until they explicitly yield the processor.

  - Pre-emptive threads can be interrupted at any time (e.g., if their
    time slice expires, or a higher-priority thread becomes
    runnable, etc).

The trade-offs:
  - Cooperative threads make preserving large invariants easy: by
    default, all code is a critical section, only broken up when you
    yield [Atul et al].  Their downside is that if they don't yield
    "often enough" they add large, difficult-to-debug latencies.

  - Pre-emptive threads let you to eliminate the need to trust
    that the threading code yields control in a well-behaved way, which
    is why multi-user OSes (Linux and MacOS) preempt user processes.
    However, they typically make it much more difficult to write correct
    code.  (E.g., would your gpio implementation work if it was called
    by multiple pre-emptive threads?  UART?   Why not?)


Generally, at a minimum, each thread has its own stack --- which has
to be "large enough" so that it doesn't get overrun --- and the thread
package has to provide an assembly routine to "context switch" from one
thread to another.  Context switching works by:

   1. Saving all registers from the first thread onto its stack and storing
      this new stack pointer in the thread control block.
   2. Loading all registers for the second from where they were saved.
   3. Changing the program counter value to resume the new thread.


----------------------------------------------------------------------
### Review: A crash course in ARM registers

You should have read the ARM assembly chapter in the PRELAB
(`docs/subroutines.hohl-arm-asm.pdf`); this is just a cliff-notes version.

Context-switching may sound mysterious, but mechanically it is simple: we
just need to need to save all the registers in use.  The ARM has sixteen
general-purpose registers `r0`---`r15` which can be saved and restored
using standard load and store instructions.  If you look through the
`.list` files in any of the pi program directories, you'll see lots of
examples of saving and loading registers.

Even easier, because we are doing non-preemptive threads we don't have to
save all the registers, just the "callee saved" ones.  The ARM, like most
modern architectures, splits registers in into two categories (Q: why?):

  - "Callee-saved": must be saved by a procedure before any use and
    then restored before returning to the calling routine.

  - "Caller-saved": can be used as scratch registers by any procedure
    without saving/restoring them first.  The downside is that if a
    routine needs the contents of a caller-saved register preserved
    across a function call it must save the register before the call
    and restore it afterwards.

As we mentioned, a non-preemptive thread only context switches if it
explicitly calls a thread yield procedure of some kind (in our case,
`rpi_yield()`).  As a result we only need to save the "callee-saved"
registers, since any live caller-saved registers must be already saved
by the compiler.

Recall our hack from `../../notes/caller-callee/README.md` You can always
modify/use that kind of code to double-check your understanding.

Recall:
   - `r4 --- r11`: callee-saved.

   - `r13` : stack pointer (`sp`).  Today it doesn't matter, but in the
      general case where you save registers onto the sack, you will
      be using the stack pointer `sp`.  Be careful when you save this
      register.  (How to tell if the stack grows up or down?)

   - `r14` : link register (`lr`), holds the address of the instruction
     after the call instruction that called the current routine (in our
     case `rpi_yield`).  Since any subsequent call will overwrite this
     register we need to save it during context switching.

   - `r15`: program counter (`pc`).  Writing a value `val` to `pc` causes
     in control to immediately jump to `val`.  (Irrespective of whether
     `addr` points to valid code or not.)  From above: moving `lr` to `pc`
     will return from a call.  Oddly, we do not have to save this value!

So, to summarize, context-switching must save:

  - `r4 --- r11, r13, r14`.   You can use `push` and `pop` to do most of
    this.

For reference, the [ARM procedure call ABI](http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042f/IHI0042F_aapcs.pdf) document:
<table><tr><td>
  <img src="images/arm-registers2.png"/>
</td></tr></table>


The link
[here](https://azeria-labs.com/arm-data-types-and-registers-part-2/)
has a more informal rundown of the above.

You can find a bunch of of information in the ARM Manual:
<table><tr><td>
  <img src="images/arm-registers.png"/>
</td></tr></table>
