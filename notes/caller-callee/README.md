### hack: see which registers `gcc` treats as caller and callee.

If you look at `caller-callee.c` and `caller-callee.list` you can see
which registers `gcc` thinks it must save when clobbered using inline
assembly.

### Background

The ARM, like most modern architectures, splits registers in into two
categories (Q: why?):

  - "Callee-saved": must be saved by a procedure before any use and
    then restored before returning to the calling routine.

  - "Caller-saved": can be used as scratch registers by any procedure
    without saving/restoring them first.  The downside is that if a
    routine needs the contents of a caller-saved register preserved
    across a function call it must save the register before the call
    and restore it afterwards.

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

  - `r4 --- r12, r13, r14`.   You can use `push` and `pop` to do most of
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
