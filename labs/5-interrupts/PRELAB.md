## Prelab for interrupts

For this lab, we're going to do interrupts and using them to build system
calls and device interrupts.

---------------------------------------------------------------------------
### Readings

Non-optional primary sources in `6-interruptsd/docs`:

  - `BCM2835-ARM-timer-int.annot.pdf` --- excerpt from the Broadcom document,
     discusses how to enable both general and timer interrupts.

   - `armv6-interrupts.annot.pdf` ---  excerpt from the ARMv6 document in 
     our main `doc` directory.  Discusses where and how interrupts are delivered.
     You should figure out where the handler lives, and registers are written
     what values, and how to restore them.

Non-optional reading on the arm in general:

  - [INTERRUPTS](../../notes/interrupts/INTERRUPT-CHEAT-SHEET.md): this is a cheat sheet of useful page
    numbers and some notes on how the ARMv6 does exceptions.

  - [caller-callee registers](../../notes/caller-callee/README.md):
    this shows a cute trick on how to derive which registers `gcc` treats
    as caller (must be saved by the caller if it wants to use them after
    a procedure call) and callee (must be saved by a procedure before
    it can use them and restored when it returns).

  - [mode bugs](../../notes/mode-bugs/README.md): these are examples
    of different mistakes to make with modes and banked registers.
    Should make these concepts much clearer since you can just run
    the code.

Supplemental readings in the class `docs` diretory:

  1. `hohl-book-interrupts.annot.pdf`: if you were confused
     about the interrupts, this is a good book chapter to read.
  2. `subroutines.hohl-arm-asm.pdf`: this is a good review
     of ARM assembly as used by procedure calls: stack allocation,
     caller and callee saved registers, parameter passing, etc.
  3. `IHI0042F_aapcs.pdf`: this gives you a detailed view
     of the procedure call standard for the ARM.

Background information on ARM inline assembly (we will use this throughout
the quarter):

  - [arm assembly quick ref](../../docs/arm-asm-quick-ref.pdf)
  - [gcc inline assembly introduction](http://199.104.150.52/computers/gcc_inline.html)
  - [gcc arm inline assembly cookbook](../../docs/ARM-GCC-Inline-Assembler-Cookbook.pdf)
