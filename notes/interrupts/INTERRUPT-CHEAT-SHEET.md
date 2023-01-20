## ARM interrupts: Incomplete cheat sheet notes on the readings.

These are not complete.  I'm typing them out before lab, so they hopefully
are not incorrect!


---------------------------------------------------------------------
### ARM Chapter A2:   Programmer's model

Taken from `7-interrupts/docs/armv6-interrupts.annot.pdf`, which is just
a chapter from `armv6.annot.pdf` in the main `docs` directory.

Useful pages in A2:
  - modes: A2-3.
  - what registers are banked per mode: A2-5.
  - `cpsr`: A2-11.
  - address of exception vectors: A2-16.

PC after each exception (everything is "pc+4" except for data abort):
  - `reset` (A2-18): not recoverable.
  - `data abort` (A2-21): 8 bytes past.  The sole +8.
  - `undefined` (A2-19): 4 bytes past the undefined instruction.
  - `swi` (system call,, A2-20): 4 bytes past the `swi` instruction.
  - `prefetch` (A2-20): 4 bytes past.
  - `IRQ` (A2-24) : 4 bytes past.

Registers and instructions:
  - Instructions are 4 bytes and also aligned to 4 bytes.

  - For the most part: you have to load values into one of the fifteen
    general purpose registers (`r0` ... `r14`) before you operate on them.
    I.e., ARM is a "load-store" architecture.

  - Most registers are general-purpose registers in that they can be
    used for most things.  The two exceptions: (1) the program counter
    (pc) register `r15` cannot be used for other purposes and (2),
    the branch and link instructions (`bl` and `blx`) are hard-coded to
    write the return address to `r14`. (See: A2-9.)  By convention the
    stack pointer is in `r13`, but you could write your own compiler to
    use a different one.

  - Like many other architectures, ARM general-purpose registers are
    divided into two categories: "caller-saved" or "callee-saved"
    indicating who has to save them.

    Caller-saved are not preserved across procedure calls; you must
    explicitly save (store to the stack) or restore (load from the stack)
    if you want to use their values across a call.

    Callee-saved are preserved across procedure calls.  Thus, you must
    save them save before you use them).

Like other architectures, ARM has multiple execution modes, though it
has more than many:

  - The seven modes are listed on A2-3.

  - Like everyone else, it has an unprivileged used mode
    (which cannot issue privileged instructions) and a variety of
    privileged modes.
  - You can see the current mode in the `cpsr` registers.
    (See: A2-11.) For example, User mode (`usr`) will have mode bits
    `0b10000` in bits 0-4 in `cpsr`,

  - You can turn-off/turn-on (disable/enable) interrupts by writing bit
    `I` (7) in the `cpsr`.

  - Note: the condition code flags (bits 28-31) in the `cpsr` are
    caller-saved, in that the upper-bits can (almost certainly: will)
    be used by any routine you call.  This matters for threading: when
    we build a non-pre-emptive thread package, you won't have to save the
    `cspr`; you will for pre-emptive.

  - Note: the rest of the bits in the `cpsr` are callee-saved in that
    you need to preserve them by doing read-modify-write operations on the
    `cpsr`.   For example: you wouldn't want to overwrite the interrupt
    enabled value (bit 7) when you were modifying something else.  Easy,
    bad mistake: will work on small test code, then blow up when you 
    plug into a big system.

  - For the most part the privileged modes are entered from an exceptions:
    such as memory faults, data aborts, interrupt (IRQ)  or system calls.

  - A2-5: Each exception level has its own private copy of at least:
    (1) the `spsr` (which holds the original `cpsr`), (2) the stack
    pointer register, `r13`, (3) the `lr` (link regiser, `r14).  These are
    referred to as "banked" registers, though often people call them
    "shadow" copies.  The "fast interrupt" also has shadow copies of
    `r8-r14`.

  - These shadow registers trip people up initially, but the intuition is
    pretty simple: When you take an exception, you need several pieces
    of state.

    First, you need to know where to resume execution (otherwise you
    couldn't ever handle it besides, say, rebooting).  ARMv6 handles
    this by storing the resumption addrss in the exception's `lr`.
    (Confusingly, with either `4` or `8` added to it.)  You resume by
    jumping back to that location.  An alternative equivalant view:
    you need to set the program counter to a piece of exception code,
    but the pc is already in use by the original program, so somehow
    you need to save it before overwriting it.  (See: page A2-8.)

    Second, you need some way to use registers (since load-store
    architecture), but all of them are live.  We could push them onto the
    original stack, but that requires a valid, trusted stack pointer.
    In general you don't want to trust the stack pointer of code that
    just took an exception and/or is unpriviledged.  ARMv6 thus provides
    a private `sp` (r13) register for each mode (again, Figure A2-1 on
    page A2-5).

    Third, you don't want to overwrite the `cpsr` of the original code,
    so you save it into a shadow copy, the `spsr`.

    Note: there are many other ways to solve these problems, and at least
    one machine has likely tried anything you can think of.   One way to
    provide registers but side-step mode-specific ones is to use scratch
    registers, as was done on MIPS (`k0` and `k1`).

  - The general model on the arm: when you get an exception, the CPU
    will save the current `pc` into the exception's shadow `lr` register,
    the current `cpsr` into the exception's `spsr`, and, based on the
    exception type switch modes and jump to a fixed program counter
    address.  At this point, the exception handler will often save
    the caller-saved registers and call C code.

  - Note: the shadow register values are preserved when you exit the
    exception and thus will be there when you return.  You can use this
    to do a one-time setup of the stack pointer if you want.  We do not
    do this to keep the code simple.

  - You need a way to resume from an exception: this involves jumping to
    the address in the `lr` (typically, subtracting a constant) and
    simultaenously restoring the `spsr` to the `cspr`.  There are a
    variety of instructions that do this.  They typically have a caret
    `^` in them.  (See: A2-17.)

Exception handlers:

  - When an exception occurs, where does the machine jump to?   The
    default method on ARMv6 (like many other architectures) is to jump
    to a fixed address.   You will need to copy your handler code to
    this address.

  - A2-16: gives the addresses. If you notice, the reset exception is
    at address `0x0`: i.e., `NULL`!   Very odd :)

  - If you look at the other addresses you notide that for the most
    part they are 4 bytes from each other.  Since ARM instructions are
    4 bytes, that means we can run a single instruction before falling
    through to the next address.  The typical method is to have a single
    word "trampoline" that simply jumps to the location of the real 
    exception vector.  (Which is usually a trampoline that saves some
    registers in assembly, jumps to C code, and then jumps back to 
    the original location).

  - A2-19: 

---------------------------------------------------------------------
### Broadcom ch 7

Interrupts are similar to a device in that have to figure out how to:
  0. Disable all during setup.
  1. Set all interrupt state to known values.
  2. Configure interrupt sources (can have multiple).
  3. Enable.  
  4. When get one: figure out which it was and then clear it.

Interrupts:
   - Must clear them, or when you jump back, will get it immediately again.
   - Interrupts are disabled in the interrupt handler.
   - You can get synchronous exceptions in the exception handler (e.g., 
     division by zero, page fault) so make sure you don't!   It will
     destroy your state unless you took steps to allow nested interrupts.


Broadcom:
  - Check "basic bending register" to see source.
  - Basic interrupt enalbe (p 117): not read modify write!  Just write it,
    similar to GPIO set.
  - Basic interrupt disable (p 118): write a 1 to disable source.  Just
    like GPIO clear.
