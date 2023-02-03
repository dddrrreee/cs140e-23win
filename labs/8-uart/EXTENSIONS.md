-----------------------------------------------------------------------------
#### Make `sw_uart_get8`

You might be able to do this using the microsecond timers.  You might
have to go to cycle counters.

-----------------------------------------------------------------------------
#### Make your bootloader use an adaptive buadrate

Now that you control all the code you can make your bootloader / Unix
side adaptively find the fastest possible baud rate .  You'll have to
be a bit clever to handle the case when you go too-far and there is only
garbage coming out.  Also, when the bootloader jumps to the binary 
code --- this code will have to be smarter about how it re-initializes
the UART (if it does).

-----------------------------------------------------------------------------
#### Go through Thompson's unix implementation and take stuff we can use.

Ken Thompson's Unix implementation is checked into `test-inputs`.
It's succinct and powerful.  Worthwhile code to go through.  So, go
through and try to find stuff we can take!


-----------------------------------------------------------------------------
#### Make a loopback version of your sw-uart that can send and receive to itself

For this: you'd connect a jumper to (say) pins 20 and 21.  Send on
20, receive on 21.  You'll quickly notice that doing so doesn't
work since
the `sw_uart_get8` will spin in a loop waiting for input, which prevents the
`sw_uart_put8` from sending.  The hack:
  1. During the "dead time" in the busy waiting loop in `sw_uart_get8` 
     you can interleave  other tasks as long as control returns soon
     enough.
  2. Thus, you can interleave another task (in this case `sw_uart_put8`) 
     in the delay section of this loop.

     You have to be careful with timing.  Accuracy matters.

This is a very small, easy to understand puzzle, but writing the code
that answers it well is tricky and worthwhile.

-----------------------------------------------------------------------------
#### Extension: speed up your bootloader / my-install

Our current `my-install` and bootloader uses a pretty slow baud rate.  Later
in the quarter this can lead to big lags when we send over larger programs.
Start changing your `my-install` and `bootloader` to use a faster baud.
(There will be a limit on how fast your OS can handle.)   You can do some
simple tests to figure out what seems to work easily, maybe back down a 
step, and then rerun the tracing tests.

----------------------------------------------------------------------------
#### extension: sw-uart speed

Once you have a handle on error, it's a fun hack to see how high of a
baud rate you can squeeze out of this code using the above tricks.
In the end I wound up switching from micro-seconds to using the pi's
cycle counters (`cycle-count.h` has the macros to access) and in-lining
the GPIO routines.  (Note that as you go faster your laptop's OS might
become the problem.)

Note:
  - If you switch baud rates on the pi, you'll need to change
    it in the `pi-cat` code too.***
  - Because the counter will overflow frequently you must be careful
    how you compare values.

----------------------------------------------------------------------------
#### extension: sw-uart use a second tty-usb

We have extras we can give out.

Wiring up the software UART is fairly simple: 

  1. You'll need two GPIO pins, one for transmit, one for receive.
     Configure these as GPIO output and inputs respectively.
     To keep things simple, we just re-use the pins from the hardware uart.

     If you get ambitious we can give you a second tty-usb device and
     you can use that!

  2. Connect these pin's to the CP2102 tty-usb device (as with the 
     hardware remember that `TX` connects to `RX` and vice versa).
  3. Connect the tty-usb to your pi's ground.  ***DO NOT CONNECT TO ITS POWER!!***

  4. When you plug it in, the tty-usb should have a light on it and nothing
     should get hot!

Note, testing is a bit more complicated since you'll have two `UART` devices.

  1. When you connect your pi and figure out its `/dev` name; you will
     give this to the `pi-install` code.

  2. Now connect the auxiliary UART, and figure out its `/dev` name.
     You will give this device name to `pi-cat` which will echo everything
     your code emits.
     
     For example, on Linux, the first device I plug in will be `/dev/ttyUSB0`
     and the second `/dev/ttyUSB1`.  So I would bootload by doing:

            my-install /dev/ttyUSB0 hello.bin

     And running `pi-cat` by:

            pi-cat /dev/ttyUSB1

  3. In general, if your code has called reboot, you do not have to pull
     the usb in/out to reset the pi.  Just re-run the bootloader.  (A simple
     hack is to look at the tty-usb device --- if it is blinking regularly
     you know the pi is sending the first bootloader message :)).
  
Note:  our big issue is with error.  At slow rates, this is probably ok.
However, as the overhead of reading time, writing pins, checking for
deadlines gets larger as compared to `T` you can introduce enough noise
so that you get corrupted data.  Possible options:

  - Compute how long to wait for each bit in a way that does not lead to cumulative
      error (do something smarter than waiting for `T`, `2*T`, etc.)

  - The overhead of time is probably the main issue (measure!), so
      you could inline this.  In general, reading time obviously adds
      unseen overhead, so you have to reason about this.

  - You could also switch to cycles.  (our pi runs at `700MHz` or 7 million
      cycles per second.  You should verify 70 cycles is about 1 micro-second!)

  - Unroll any loop and tune the code.

  - Maybe enable caching.

  - Maybe inlining GPIO operations (probably does not matter as long overhead as `< T`)

  - In general: Measure the overhead of reading time, writing a bit,
      etc.  You want to go after the stuff that happens "later" in the bit
      transmit since it has the most issue and the stuff that has a large
      fixed cost (since that will cause the error to increase the most).

  - Could just hand compute an assembly routine that runs for exactly
      the cycles needed (count the jump and return overhead!).  Or,
      better, write a program to generate this code.

  - Part of "systems" as a discipline is actually measuring what effect
      your changes make.  People are notoriously stupid at actually
      predicting where time goes and what will lead to improvements.

      Of course, measuring the actual error is tricky, since the
      measurement introduces error.  One approach is to write a trivial
      oscilloscope program on the pi that will monitor a GPIO pin on
      another pi for some time window (e.g., a millisecond), record at
      what exact cycle count the pin changed value, and print the results
      after. You'll need a partner (or a 2nd pi) but this is actually a
      close-to trivial program and gives a very good measurement of error
      (it's useful in general, as well).

