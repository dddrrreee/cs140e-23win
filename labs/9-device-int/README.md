## Lab 8: device interrupts 

***NOTE: the code is in flux, don't start hacking on the main lab please***
***NOTE: the code is in flux, don't start hacking on the main lab please***
***NOTE: the code is in flux, don't start hacking on the main lab please***
***NOTE: the code is in flux, don't start hacking on the main lab please***

A couple of common things you want in real OS life:
  1. Get interrupts from GPIO pins.
  2. Buffer input and output.

We'll build both today.

Check-off:
  1. Your `gpio` code passes the cksum tests and the loop back tests.
  2. Your `sw_uart_putc` and `sw_uart_get` gives reasonable values for
     its timings.
  3. You have the basic `sw_uart_getc` interrupt working.

------------------------------------------------------------------------
### Part 0: House-keeping (2 minutes)

I moved all the class-provided interrupt support code to
`libpi/staff-src` so that all subsequent labs can use it.
To migrate your code:

  1. Put your assembly code from last lab
     (`7-interrupts/1-syscall/interrupts-asm.S`) in `libpi/src` so all
     your code can use it.  Add this to `Makefile` if needed

  2. We have staff implementations that you can 
     add to `STAFF_OBJS`:

                STAFF_OBJS += staff-objs/sw-uart.o
                STAFF_OBJS += staff-objs/interrupts-asm.o
                STAFF_OBJS += staff-objs/gpio-int.o


  3. Make sure you've looked through and run the two examples in
     `0-circular-example` --- you'll be using the circular FIFO buffer
     implementation today.  It should print `SUCCESS!`.  Look through
     the code to see how to use the circular buffer to share data between
     an interrupt handler and client code.

------------------------------------------------------------------------
### Part 1: Implement raw GPIO interrupt handling.

NOTE: 
    Make sure you put a jumper from pin 20 to pin 21!

You're going to detect when the input transitions from 0 to 1 and 1 to 0.
To reduce the chance we can a spurious spike from noise we are going to
use the GPIO `GPREN` and `GPHEN` registers on page 98 of the broadcom
that will wait for a `011` for low-to-high and a `100` for high-to-low.
(Note that we could do such suppression ourselves if the pi did not
provide it.)

For this test you will just write to the GPIO pins directly and check
that you received the  expected event.  In `1-gpio-int`:

   0. `1-gpio-int-ex.c` has the example driver.
   1. You need to wire your pin 20 to your pin 21 (i.e., have a loopback
      setup) so that the tests work.
   2. There are three test routines in there: work on them in order.
   3. As a useful enhancement, before writing to a pin you should
      record the current cycle count,  set the pin, and in the interrupt
      handler record the time (in cycles) that it received the event.
      Print the time it took both to receive the interrupt (you can
      modify your assmebly) and to completely handle it and return.

#### Testing

If you do a pull, there are more tests in `lipi-fake/tests-gpio-int`.
I put the `.out` files to make checking easier.  I did `dev_barriers`
perhaps more `dev_barriers` than needed in the routines used by the
interrupt handlers.

For what it's worth, here are my checksums and linecounts:

        libpi-fake/tests-gpio-int % cksum *.out
        3619577664 88 6-gpio-event-clear.out
        325921212 90 6-gpio-event-detected.out
        920984819 198 6-gpio-set-falling.out
        869056559 198 6-gpio-set-rising.out
        2919222009 2024 7-gpio-event-clear-n.out
        1966508437 2136 7-gpio-event-detected-n.out
        2310831047 5592 7-gpio-set-falling-n.out
        137501872 5592 7-gpio-set-rising-n.out


        libpi-fake/tests-gpio-int % wc *.out
            4     7    88 6-gpio-event-clear.out
            4     7    90 6-gpio-event-detected.out
            8    11   198 6-gpio-set-falling.out
            8    11   198 6-gpio-set-rising.out
        97   100  2024 7-gpio-event-clear-n.out
        97   100  2136 7-gpio-event-detected-n.out
        225   228  5592 7-gpio-set-falling-n.out
        225   228  5592 7-gpio-set-rising-n.out
        668   692 15918 total
        
These have a bunch of `dev_barriers` so probably checking without them
first.  For ordering, I enabled the GPIO address first and then the IRQ
address (both are read-modify-write).  The `.out` files are checked in
so you can see what the actual differences are.

Extension:  
  1. If you have a touch touch sensor use it to trigger interrupts and
     use this to trigger an LED.
  2. To make it fanciery, use two sensors, one to make the LED brighter,
     one to make it darker.

------------------------------------------------------------------------
### Part 2: a simple digital analyzer : 2-logic-analyzer

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
### Part 3: use the vector register: 3-vector-base

For this you'll do some simple tricks to speed up your interrupt
code and make it more flexble:

You'll write the inline assembly to set the vector base.  See:
  - 3-121 in `../../docs/arm1176.pdf`

What to do:
  - You only have to modify `1-vector-base/vector-base.h`
  - There are two tests (you have to modify the makefile to run each).
  - When the tests pass, move the `vector-base.h` file to `libpi/src` and make
    sure they still work.

  - The test `0-test-vector-base.c` should should show a speedup and
    complete correctly.

  2. Make your own: `libpi/src/int-init-reg.c` that implements the 
     routine:

        void int_init_reg(void *int_vector_addr)

     Which should be a re-implementation of `int_init` using the vector
     base register file (which you will have to include) instead of
     copying the table itself.

     As usual: Make sure to add that file to `put-your-src-here.mk`.

  3. Make a copy of your `1-gpio-int` directory and convert it over to
     use the vector base method.  

  4. Use the prelab code to check which registers you can remove
     from your save-restore in the interrupt handler.   If should be 
     the case that if you "clobber" a callee-saved register you do
     not want to skip, `gcc` will not save it.

------------------------------------------------------------------------
------------------------------------------------------------------------
------------------------------------------------------------------------
### Homework 1: change the hardware UART to use interrupts.

***THIS is getting written***

Modify augment your UART driver so that clients can set up
interrupts by adding a routine:
   - `uart_init_int()`: enable the hardware UART's interrupts for
    both for transmission and receive.  

You will have to provide different versions:
  - `uart_putc_int`: puts a character on a circular buffer for the interrupt
    handler to transmit.

  - `uart_getc_int`: pulls a character from a circular buffer (if any)
    that was placed there by the UART interrupt handler.

  - `uart_has_data_int`: checks if there is data on the circular 
    buffer or queue.

  - Interrupt handler: should try to push any characters it can from 
    its transmit queue to the hardware FIFO (until it is full).  It 
    should pull as many characters as the hardware FIFO has.

You need to think carefully so that you don't have characters sitting idle
on the circular buffer.  Also, handle the case where there is no room!



------------------------------------------------------------------------
### Extension: a interrupt-based software uart

As with lab 6, the code in the digital analyzer shares a lot of 
similarity with the software UART code.  So we'll build on part
2 to make your software uart much more robust by having it 
record the transitions in the interrupt handler (using a circular 
buffer) that later code pulls out.


Our key problem with `sw_uart_get` is that if the signal comes in
and we are not waiting for it, we're going to lose it.  So our 
first step is to just fix this problem by:
  1. Triggering an interrupt when you receive a start bit (i.e., the
     GPIO pin goes from its initial `1` reading to `0`).
  2. Push the value and time in the circular buffer (as in part 2).
  3. Have `sw_uart_get8_int` reconstruct the character using the
     timing data.

#### Discussion: Handling concurrency

I claimed, somewhat duplicitously, that we were going to do pre-emptive
threads "soon".  But, of course, we are already doing them --- we have
just been calling them interrupt handlers.  These handlers can interrupt
you at any time and start messing with state.  If the interrupted
code was reading/writing the same locations we could have a corruption
("race conditions").

Thus, we need to start (and should have already started) thinking
about how to avoid these cases.  You might immediately start thinking
"well use a lock".  But, of course, locks won't work here --- if the
lock is held, what can the interrupt handler do?  You could also think,
"well disable interrupts" but:
  1. Disabling/enabling is that is costly (measure it!).

  2. Doing so leads to all sorts of nasty errors: we've found thousands of 
     bugs in linux where they block with interrupts disabled, neglect
     to re-enable on interrupt paths, etc.

  3. Related, it makes it very difficult to reliably hit hard real-time
     deadlines (as we need above) since you have more and more chunks
     of code that invisibly delay interrupts.

The easiest way is to avoid errors is to simply not share any state.
(This is why your laptop is currently able to run 10s-100s of processes
and they don't need to use locks or anything else to prevent problems.)

We do a variation of this in our circular queue implementation to
share data between the interrupt and non-interrupt code.  The `head`
is only modified by the interrupt handler pushing bytes into the buffer.
The `tail` only modified by the non-interrupt code consuming it.  Thus,
as long as there are only these two threads, we do not need locks.
If we want more threads to remove entries, we could lock just the `tail`.

#### How to build it.

You should copy whatever code you want and modify the makefil to make it.
The structure should should be pretty simple:
  1. Setup so you receive falling edges (the start bit).
  2. Run the `sw_uart_getc` in the interrupt handler and push the received
     character into the buffer.   
  3. Before returning and re-enabling interupts, make sure you have seen
     the start bit. 

  4. Write a `sw_uart_getc_int` that pulls characters from the circular
     queue.  Test that this works!

  5. Note that we run the interrupt handler while you were doing a
     `sw_uart_putc` we will mess up the timings.  For our current code,
     use a variable similar to your `gprof` code to detect if this
     happens and just panic.  (We will do something better, soon.)

------------------------------------------------------------------------
### Extension do a smarter sw-uart interrupt handler.

Obviously, sitting and polling in the interrupt handler to get a
character is a bit outrageous.  For homework, you should reduce the
work in the handler to almost nothing.  There's various approaches;
below I'll discuss two that are perhaps closest to things we've covered.

Approach 1: enable the clock to sample half-way between the readings
(as your software version does).  This is pretty easy, and is low-ish
overhead.  The drawback is that that if we are using the clock for other
things, (e.g., preemptive threading scheduling) it can require some care
to meld them.  A quick sketch:
  1. Compute the load register / prescalar values you need to hit
     the correct sampling.

  2. Enable a high-to-low edge interrupt (so you detect the 
     UART `start` bit).

  3. When you get the interrupt, as a first hack: spin for `n/2` cycles
     and then enable
     the timer and return.
  4. Sample 9 times.
  5. Disable the timer, output the character to the circular buffer.

After this works, make step 3 more precise so you don't have to spin
(perhaps by messing with the load register or by over-sampling).
You may also have to measure the overhead of getting into / out of the
interrupt handler.

Approach 2: get interrupts at each rising and falling edge, and
measure the time between them, and use this time to figure out the
bits.  The advantage is that we do not have to mess with the clock.
The disadavantage is that the logic can be either more complicated or more
fragile (or both).  For example, a noise spike can lead to a short pulse
that means nothing but we would pick it up.   We rely on the pi filtering
these out (read the edge detection description).  A quick sketch:

  1. Setup both high-to-low and low-to-high interrupts.
  2. When you get one, record the difference from the last interrupt.
  3. You could either put this number directly into a circular buffer,
     or you could compute the number of clocks (approximately) since
     the last and put this number of 1s or 0s into the buffer or you
     could try to compute the entire character and only put that in at
     the end.  (This is called "timing recovery.")
  4. The client pulls these values out and computes the character.

To reduce complexity and make debugging easier, you often want the
monitoring code to just log what it sees and then have non-interrupt code
do most of the processing later.  In our case, it may be better to not
compute the character in the interrupt handler: if you got it wrong,
it's going to be trickier to debug (e.g., you typically cannot even
safely print a debug message other than outright `panic`'ing).  However,
you *could* try doing it that way and see if it is more intuitive.
That way at least you are always just putting a character into the buffer.
We'll give full credit either way.  You should record errors, however,
ideally in an error log, but at least as a counter.

