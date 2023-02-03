## Lab: write your own UART implementation.

<p align="center">
  <img src="images/rpi-cables.png" width="450" />
</p>


By the end of this lab, you'll have written your own device driver for
the pi's mini-UART hardware, which is what communicates with the TTY-USB
device you plug into your laptop.  This driver is the last "major" piece
of the pi runtime that you have not built yourself.  At this point, you
can print out the entire system, look at each line of code, and should
hopefully know why it is there and what it is doing.  Importantly you have
reached that magic point of understanding where: there is nothing else.

*NOTE*:
  - Make sure you read through the [mini-UART cheatsheet](../../notes/devices/miniUART.md)
    in the Notes directory.

In addition, you'll implement your own *software* UART implementation
that can entirely bypass the pi hardware and talk directly to the
tty-USB device itself.  I.e., without using the driver above, but instead
building an 8n1 serial protocol with the CP2102 device over two GPIO pins.
Multiple reasons:

   1. The most practical reason: this gives you a "console"
      `printk` you can use when the UART is occupied, which turns out to
      be surprisingly useful.  For example, when you write the UART driver
      above, you can't actually use `printk` to debug it, since doing so
      requires a UART!  As another, when we use the ESP8266 networking
      device in a couple of labs: it needs a UART to communicate with
      the pi.  Without a second UART, we would have no way to print to
      the laptop.  Again: hard to debug if you have no idea what the pi
      is doing.

   2. Gives you an existence proof that you don't need to need to use
      hardware but can roll your own.  This applies to all the different
      hardware protocols  built-in to the pi, such as I2C and SPI.  These
      acronyms may sound ominous, but their implementation is not: just 
      raise or lower pins,  possibly with a time delay.

   3. Writing your own version of a hardware protocol gives you more
      of an intuition for the problems that happen at this level.
      For example, how do you deal with concurrency between two entirely
      separate devices when we don't have the usual fall-back on locks,
      etc?  (Hint: the use of messages and very precise time deadlines
      are fairly common.)

   4. This gives you a small demonstration of how your pi setup,
      which appears trivial, is a hard-real time system, where with
      some care, it possible to hit nanosecond deadlines reliably.
      Difficult to do this on a "real" system such as MacOS or Linux.

### Checkoff

Show that:
   1. You have a quick way of switching directories, especially switching to
      libpi and back.  Try setting up aliases based on the `pushd` and `popd`
      commands.  Don't keep re-typing-out `cd ../../../libpi` and `cd
      ../labs/8-uart/whatever`; that's a waste of your lab time.
  
   2. `1-uart`: `make checkoff` passes.  You should first ensure you
       can pass the tests one by one to make debugging easier.
     
      Your `uart.c` code should make it clear why you did what you did,
      and supporting reasons --- i.e., have page numbers and partial
      quotes for each thing you did.

   3. You should put your `uart.c` in `libpi/src` and
      update the `libpi/makefile`.   Remake your bootloader and put it
      on your sd card.

      You should then make sure that `make check` and `make checkoff`
      in lab 7 still works.

   4. Your fake pi implementation for `uart.c` gives the
      same hash as everyone else.  Note, there are multiple ways to do
      same thing, so maybe do the first one as a way to resolve ambiguity.

   5. Your software UART can reliably print and echo text between the pi
      and your laptop.

There are tons and tons of [EXTENSIONS](./EXTENSIONS.md)

-------------------------------------------------------------------------
#### The general goals

The main goal of this lab is to try to take the confusing prose and
extract the flow chart for how to enable the miniUART with the baud rate
set to 115200.  You're looking for sentences/rules:

  * What values you write to disable something you don't need
	(e.g., interrupts).

  * What values you have to write to enable things you need (miniUART).

  * How to get the gpio TX, RX pins to do what you need.

  * How to receive/send data.  The mini-UART has a fixed sized transmit
	buffer, so you'll need to figure out if there is room to transmit.
	Also, you'll have to detect when data is not there (for `uart_get8`).

  * In general, anything stating you have to do X before Y.

You don't have enough information to understand all the document.  This is
the normal state of affairs.  So you're going to start practicing how
to find what you need and interpolate the rest.

The main thing is to not get too worked up by not understanding something,
and slide forward to what you do get, and cut down the residue until
you have what you need.

-----------------------------------------------------------------------
### Part 1. implement a UART device deriver:

Our general development style will be to write a new piece of
functionality in a private lab directory where it won't mess with anything
else, test or (better) equivalence check it, and then, migrate it into
your main `libpi` library so it can be used by subsequent programs.

Concretely, you will implement the routines in `8-lab/1-uart/uart.c`.
The main tricky ones:

  1. `void uart_init(void)`: called to setup the miniUART.  It should
     set the baud rate to `115,200` and leave the miniUART in its default
     `8n1` configuration.  Before starting, it should explicitly disable
     the UART in case it was already running (since we bootloader,
     it will be).

  2. `int uart_get8(void)`: blocks until it can read a byte from
     the mini-UART, and returns the byte as a signed `int` (for sort-of
     consistency with `getc`).

  3. `void uart_put8(unsigned c)`: puts the byte `c` onto the UART transmit
     queue.  If necessary, it blocks until there is space.

General approach for `uart_init`:
  1. You need to turn on the UART in AUX.  Make sure you
     read-modify-write --- don't kill the SPIm enables.
  2. Immediately disable tx/rx (you don't want to send garbage).
  3. Figure out which registers you can ignore (e.g., IO, p 11).
     Many devices have many registers you can skip.
  4. Find and clear all parts of its state (e.g., FIFO queues) since we
     are not absolutely positive they do not hold garbage.  Disable
     interrupts.
  5. Configure: 115200 Baud, 8 bits, 1 start bit, 1 stop bit.  No flow
     control.
  6. Enable tx/rx.  It should be working!

If you run `make` in `4-uart/1-uart` it will build:
  - A simple `hello` you can use to test. I'd suggest shipping it over with your
    bootloader.  

A possibly-nasty issue: 

  1. If you test using the bootloader (recommended!), that code obviously
    already initializes the UART.  As a result, your code might appear
    to work when it does not.

  2. To get around this issue, when everything seems to work, change
     the `Makefile` to use `hello-disable.c` instead of `hello.c`
     This will repeatedly disable and enable the uart --- not a perfect
     test, but a bit more rigorous.
 
  3. Once your code works, make swap out the `uart.c` in the `libpi/Makefile`
     to use yours:  copy `uart.c` to `src/uart.c`, remove it from the current
     `1-uart/Makefile`, recompile.

  4. Make sure `make check` for the tests in lab 3 still work.
     and see that it boots the `hello` you used above.

-----------------------------------------------------------------------
### Part 2. re-install your bootloader with the new uart code.

Now change your bootloader to use the new `uart.c`:

  1. Copy `uart.c` into `libpi/src` and update your libpi `Makefile`.
  2. Add `UART` to to your bootloader where it prints your name and recompile.
  3. Copy it as `kernel.img` to your SD card.
  4. Nothing you changed should have made any difference in behavior.
     So: Make sure that `1-uart` tests still pass after replacing the
     bootloader.
     Then do `make checkoff` in the previous lab `7-bootloader/checkoff`
     to make sure they still work, too.
  
-----------------------------------------------------------------------
##### Part 3. `2-fake-pi`

The fake pi is in `2-fake-pi`.

  1. You'll have to modify the `Makefile` so that it knows where to find your
     `uart.c`.  If you give it the path, it should work.  If not let me know.

     `cd` into `tests-uart` and run the test for `0-uart-init.c`.  You
     may have addresses written in order (there are multiple legal 
     orders). If you don't match ours,  check with someone else and
     have some reason that the reorder you do is ok.

     You'll have to compare your `.out` files with other people.  You
     probably want to run these by hand one at a time.

     NOTE: A big disadvantage of these checks is that they require
     the same reads and writes be done in the same order.  This is
     unrealistic and makes checking kinda of a pain.   We will fix this
     in the homework.

-----------------------------------------------------------------------
### Part 4. implement `sw_put8` for a software UART.

Note:
   - To use our `sw-uart.o`:
     you can uncomment the line in `3-sw-uart:Makefile` and comment
     out yours.

This part of the lab is from a cute hack Jonathan Kula did in 2021's
class as part of his final project.

While the hardware folks in the class likely won't make this
assumption-mistake it's easy as software people (e.g., me) to assume
things such as: "well, the tty-usb device wants to talk to a 8n1-UART
so I need to configure the pi hardware UART to do so."  This belief,
of course, is completely false --- the 8n1 "protocol" is just a fancy
word for setting pins high or low for some amount of time for output,
and reading them similarly for input.  So, of course, we can do this
just using GPIO.  (You can do the same for other protocols as well,
such as I2C, that are built-in to the pi's hardware.)

As described in 
[UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)
the protocol to transmit a byte `B` at a baud rate B is pretty simple.

  1. For a given baud rate, compute how many micro-seconds
     `T` you write each bit.  For example, for 115,200, this is:
     `(1000*1000)/115200 = 8.68`.  (NOTE: we will use cycles rather
     than micro-seconds since that is much easier to make accurate.
     The A+ runs at `700MHz` so that is 700 * 1000 * 1000 cycles per
     second or about `6076` cycles per bit.)

To transmit:
  1. write a 0 (start) for T.
  2. write each bit value in the given byte for T (starting at bit 0, bit 1, ...).
  3. write a 1 (stop) for at-least T.

Adding input is good.  Two issues:
  1. The GPIO pins (obvious) have no buffering, so if you are reading
     from the RX pin when the input arrives it will disappear.

  2. To minimize problems with the edges of the transitions being off
     I'd have your code read until you see a start bit, delay `T/2` and then
     start sampling the data bits so that you are right in the center of 
     the bit transmission.

The code is in `3-uart:

  1. Your software UART code goes in `sw-uart.c`.
  2. You should start testing it by doing a `make run` or 
     `make check` of `hello.c`.
  3. Then `make checkoff` should pass.  It does a linker trick to replace
     the libpi.a UART implementation with your software uart and 
     re-runs the tests from `1-uart` to ensure they behave the same.
     The comments in `sw-uart-veneer.c` describe what is going on.

