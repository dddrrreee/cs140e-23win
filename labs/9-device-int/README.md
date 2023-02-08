## Lab: device interrupts 

<p align="center">
  <img src="images/glowing-rpi.png" width="450" />
</p>


We're going to setup the pi so you can get GPIO interrupts, speed up
the interrupt handling code, and finish building your sw-uart.

We'll build both today.

Check-off:
  1. `1-vector-base` passes `make checkoff` and you see about a 30% speedup
      from changing to `b`.
  2. `2-gpio-int` passes `make checkoff`.
  3. `3-handlers-gpio-int` passes `make checkoff`.
  4. `4-logic-analyzer`: Your `sw_uart_putc` gives
     reasonable values for its timings.
  5. Ideally: You have the basic `sw_uart_getc` interrupt working and can send
     back and forth.

There are a ton of [EXTENSION](./EXTENSIONS.md).  We actually have a
bunch that are not written, so ask if you want :).

------------------------------------------------------------------------
### Housekeeping:

The uart lab was pretty rough.  We've made a couple changes to hopefully
make things more smooth:
  1. Many people would hit the same hard bugs.  So, in the future:  If 
     you hit anything interesting write it up and post Ed.  This will
     count towards your extension credit especially if it helps a bunch
     of people.  It also gives us a good record of what kind of traps
     nailed people.

  2. To make things easy for switching we will always include a `libpi.a`
     in each lab: this will make it easy to flip back and forth from your
     libpi to a (hopefully) working staff version rather than messing
     around in your `libpi` directory.

     Whenever you hit an "impossible error" I would always recommend
     flipping back to a clean state and seeing if it is still there.  If
     so there's likely some issue with your environment (your pi, your  
     laptop, the USB cable, etc).

   3. If you have any further suggestions, let us know!

------------------------------------------------------------------------
### Part 1: use the vector register: `1-vector-base`

For this you'll do some simple tricks to speed up your interrupt
code and make it more flexble:

You'll write the inline assembly to set the vector base.  See:
  - 3-121 in `../../docs/arm1176.pdf`

What to do:
  - You only have to modify `1-vector-base/vector-base.h` and one line of
    `1-vector-base/interrupt-asm.S`
  - There are two tests: `0-test-checks.c` makes sure you have some of
    the checks and `1-test-run.c` does some timings.  You should see
    over 30% performance improvement.

  - When the tests pass, `mv` the `vector-base.h` file to `libpi/src` and make
    sure they still work.

  - `make checkoff` should pass.

------------------------------------------------------------------------
### Part 2: Implement `2-gpio-int/gpio-int.c`

You should implement the code in `gpio-int.c`: this will mirror 
your `gpio.c` code.

***NOTE: you cannot use raw GPIO addresses: each has to have an enum
and a page number / commment as to why you're doing what you're doing.***

Unlike the last lab, you won't have to debug on the raw hardware first,
there is a (what I hope is finally working) fake pi implementation
in `2-gpio-int` along with all the `.out` files.  You should be able
to compare yours to ours / your partners and see that everyone
has the same thing.

Notes:
  1. I rejected pins that were >= 32.   I did this before issuing a `dev_barrier`.
  2. I enabled the GPIO address before the Broadcom interrupt controller.
  3. The interrupt controller is a different device from GPIO so we need
     dev barriers between them.
  4. We don't know what we were going before the GPIO calls, so always 
     do a dev barrier before and after.
  5. `gpio_has_interrupt` and `gpio_event_detected` call 
     `DEV_VAL32` on their result before returning.

You probably want to work on one routine at a time.
  - the `6-*.c` tests are the easiest.
  - The `7-*.c` tests should be a formality if 6 pass.


You're going to detect when the input transitions from 0 to 1 and 1 to 0.
To reduce the chance we can a spurious spike from noise we are going to
use the GPIO `GPREN` and `GPHEN` registers on page 98 of the broadcom
that will wait for a `011` for low-to-high and a `100` for high-to-low.
(Note that we could do such suppression ourselves if the pi did not
provide it.)

------------------------------------------------------------------------
### Part 3: Implement GPIO interrupt handlers in `3-handlers-gpio`

***NOTE: before you start attach a jumper from pin 20 to pin 21 (i.e.,
have a loopback setup) so that the tests work.***


You'll  now write the handlers that use your `gpio-int.c` implementation.

While the code you write will be pretty small, this directory has a
lot of tests to hopefully isolate any issues.  Don't get worried by
the line-count.

All the code you write will be in:
  - `test-interrupts.c`:
  - you'll write handlers for rising and falling edges, and timer
    intrrupts (just steal the handler code you need from lab 5).
  - just search for the `todo` macros.

Other code:
  - `test-interrupts.h` has calls and wrappers for the tests.
  - `interrupts-asm.S` just has the timer interrupt trampoline.

The tests:
  - `1-*` do a single event.
  - `2-*` do multiple types of events, once each.
  - `3-*` do multiple types of events, many times each.

The Makefile currently uses:
  - the staff libpi `../staff-objs/libpi.a`:
  - uncomment this if you want to use your own.

------------------------------------------------------------------------
### Part 4: a simple digital analyzer : 4-logic-analyzer

Interrupts often make everything worse.  This is a case where you can
use them to find bugs in your code before you use it live.  Note that if
you run your software uart `sw_uart_put8` on the loop-back pin from the
previous part, your interrupt handler will catch all the transitions.
We will use that to make sure your code works.

  1. Look in `2-logic-analyzer.c`
  2. Modify the interrupt handler to catch and record (using cycles) when 
     it was triggered using the circular buffer implementation
     from Part 0.
  3. At the end, print out the times.
  4. They should match what we want!

You can also do `sw_uart_get8`.   Of course, it is waiting for bits
rather than writing them.  As a simple hack, you can just have it write
to the loopback pin as well and then measure when these occur.

------------------------------------------------------------------------
### DONE!

Congratulations.  Interrupts are tricky and you have a lot of working
code.

<p align="center">
  <img src="images/robots-small.png" width="450" />
</p>
