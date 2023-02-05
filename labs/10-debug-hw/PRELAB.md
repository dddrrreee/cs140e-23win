### Prelab: Using debug hardware

We're going to do tricks using debugging hardware to catch errors in
our code.  As part of this you'll need to be able to set code and data
*watchpoints* and enable *single-stepping* execution.

We are getting into the more advanced readings, so you'll likely have
to read each part of the arm manual several times.  I've annotated some
of the key bits to hopefully help.


Low-level ARM readings for the lab:

  - Re-read the armv6 chapter from the interrupts lab
    (`6-interrupts/docs/armv6-interrupts.annot.pdf`).  You're going to
    want to have a good grasp.

    Pay close attention to: the execution modes, which registers are
    shadowed, what's in the `cpsr` register, how to change modes using
    the `cps` instruction since it is faster than our current use of
    read-modify-write with `mrs`and `msr`.  Also, pay attention to
    discussion about how to access the user-level shadow registers from
    priviledged code.  (Load and store instructions using the `^` symbol,
    also how to change the `spsr`.)

  - [debug chapter](./docs/arm1176-ch13-debug.pdf): figure out how to
    set up single-stepping execution.  This is a long chapter.  It's going
    to need several passes.  We're almost certainly going to give you the
    `.o` files for this code for this particular lab, but then have you
    build your own version soon.  You should try to get a reasonable
    feel for what is going on, so the lab is not entirely a black box.

  - [fault registers](./docs/arm1176-fault-regs.pdf): These are useful
    for figuring out which fault happened.


Since the documents can be a bit much, we also give a 
[cheat sheet for chapter 13](./DEBUG-cheat-sheet.md).
