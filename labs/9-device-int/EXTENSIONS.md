## Extensions

#### Simple extension

Extension:  
  1. If you have a touch touch sensor use it to trigger interrupts and
     use this to trigger an LED.
  2. To make it fanciery, use two sensors, one to make the LED brighter,
     one to make it darker.

------------------------------------------------------------------------
### Interesting: make your sw_uart_get8/sw_uart_put8 as fast as possible.


Simple puzzle: If you have a loopback, sw-uart interrupt based approach:
how fast can you make it?

There's a ton of overhead in the current implementation:
  0. If you're energetic you can re-purpose your `gprof` implementation
     from lab 5 to see where the time is spent.  This is an interesting
     fun excercise on its own (especially if you do backtraces).
  1. The GPIO and PUT/GET procedure calls are a major source of overhead.
     Make clean inlined versions will speed stuff up.  
  2. You can also switch to "Fast Interrupts" and do a custom handler.
  3. To deal with large constants: there are three co-processor registers
     you can use to hold them.
  4. Lots of tricks, including getting rid of dev barriers :)

------------------------------------------------------------------------
### Interesting: a software UART network

Given a working software UART:
  1. Hook up two pi's using two jumper wires to form a network.
  2. You can then send and receive bytes using your software UART 
     implementation.  (Similar to how your laptop and pi do bootloading).
  3. Challenge: how high of a bandwidth can you get?  

This is related to the previous, but in a bit more of an interesting
(but also hard to debug) form factor.


------------------------------------------------------------------------
### Hard extension: change the hardware UART to use interrupts.

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

