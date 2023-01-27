## Lab 2: build a bootloader.

***NOTE: Make sure you start with the [PRELAB](PRELAB.md)!***

The next few labs will remove to remove all magic from your pi setup
by going downward and having you write and replace all of our code.
The only code running when you boot (and the only code you run to boot)
will either be stuff you wrote, or simple library routines we give you
(which you can also replace!).

Today's lab is a big step in that direction.  You will:

  - Write your own bootloader (both the Unix and pi side) to replace
     what we used last lab (i.e., `pi-install` and `bootloader.bin`).

In the next lab you will write your own versions of the low-level device
code your bootloader needs to control the UART hardware (what talks to
the TTY-USB device connecting your pi and laptop).  You will then be
able to drop in your `gpio.o` and your `uart.o` and replace almost all
of our hardware-level code.


#### A strong coding hint (PLEASE DO THIS!)

For this lab you're writing two pieces of code that talk to each other
through messages.  If at all possible I would strongly suggest you figure
out how to have two code windows side-by-side, one holding the pi code,
the other holding the unix code.  This allows you to easily check that
the bytes you send in one have a matching set of receives on the other.

With two coding windows, if you have a good grasp of the protocol below
and of the helper functions, you may be able to type all this out in
about 10-20 minutes.  If not, it could take hours.

#### Sign off.

The checkoff for this lab is pretty simple: bootload some programs
and check they work the same.  More specifically:

  - `make check` in `tests/output-boot-trace` should pass.
  - [checkoff-tests/README](checkoff-tests/README.md) gives more decription.

More detailed:

  0. When using your `my-install` and `bootloader` make sure the
     `TRACE` calls printed from the unix side remain the same when
     using both our pi bootloader and yours.  They should also match
     other people.

     These include the `PUT` and `GET` calls the unix side does to send
     to the pi and the `HASH` output it computes (which prints a checksum
     of the sent code).

  1. Also, the output of the program should match, irrespective of the
     bootloader.  So, for example, you should be able to run all the
     `2-trace` tests with our bootloader and `my-install` and the old
     bootloader and `pi-install`.

  2. Start by running `./my-install hello.bin` in the `unix-side` directory.
     We will add additional tests.

  3. Show us your code so we can check for some common mistakes.

----------------------------------------------------------------------
#### The bootloader protocol.

At a high level, the protocol works as follows: Here you'll write
implement the code to:
  1. pi: spin in a loop, periodically asking for the program to run;
  2. unix: send the program code;
  3. pi: receive the program code, copy it to where it should go, and
     jump to it.
  4. To detect corruption, we use a checksum (a hash) that we can
     compare the received data too.

This is a stripped down version (explained more below):

     =======================================================
             pi side             |               unix side
     -------------------------------------------------------
      put_uint(GET_PROG_INFO)+ ----->

                                      put_uint(PUT_PROG_INFO);
                                      put_uint(ARMBASE);
                                      put_uint(nbytes);
                             <------- put_uint(crc32(code));

      put_uint32(GET_CODE)
      put_uint32(crc32)      ------->
                                      <check crc = the crc value sent>
                                      put_uint(PUT_CODE);
                                      foreach b in code
                                           put_byte(b);
                              <-------
     <copy code to addr>
     <check code crc32>
     put_uint(BOOT_SUCCESS)
                              ------->
                                       <done!>
                                       start echoing any pi output to 
                                       the terminal.
     =======================================================

More descriptively:

  1. The pi will repeatedly signal it is ready to receive the program by
     sending `GET_PROG_INFO` requests.  (This is given in the starter
     code.)

     (Q: Why can't the pi simply send a single `GET_PROG_INFO` request?)

  2. When the unix side receives a `GET_PROG_INFO`, it sends back
     `PUT_PROG_INFO` along with: the constant to load the code at (for
     the moment, `ARMBASE` which is `0x8000`), the size of the code in
     bytes,  and a CRC (a collision resistant checksum) of the code.

     (Q: Why not skip step 1 and simply have the unix side start first
     with this step?)

     Since the pi could have sent many `GET_PROG_INFO` requests before
     we serviced it, the Unix code will attempt to drain these out.
     Also, when the Unix code starts, it discards any garbage left in
     the tty until it hits a `GET_PROG_INFO`.

  3. The pi-side checks the code address and the size, and if OK (i.e.,
     it does not collide with its own currently running code) it sends a
     `GET_CODE` request along with the CRC value it received in step
     2 (so the server can check it).  Otherwise is sends an error
     `BAD_CODE_ADDR` and reboots.

  4. The unix side sends `PUT_CODE` and the code.

  5. The pi side copies the received code into memory starting at the
     indicated code address (from step 2).  It then computes
     `crc32(addr,nbytes)` over this range and compares it to the
     expected value received in step 2.  If they do not match, it sends
     `BAD_CODE_CKSUM`.  If so, it sends back `BOOT_SUCCESS`.

  6. Once the Unix side receives `BOOT_SUCCESS` it simply echoes all
     subsequent received bytes to the terminal.

  7. If at any point the pi side receives an unexpected message, it
     sends a `BOOT_ERROR` message to the unix side and reboots.
     If at any point the Unix side receives an unexpected or error
     message it simply exits with an error.

The use of send-response in the protocol is intended to prevent the
Unix-side from overrunning the pi sides finite-sized UART queue.  The CRC
and other checks guard against corruption.

I would start small: do each message, check that it works, then do
the next.  If you go in small, verifiable steps and check at each point,
this can go fairly smoothly.  If you do everything all-at-once and then
have to play wack-a-mole with various bugs that arise from combinations
of mistakes, this will take awhile.

And don't forget: there are a bunch of useful error checking macros in
`libunix/demand.h` and uses in `libunix/*.c`.



--------------------------------------------------------------------
### Step 0: make sure your pi works before starting!

Do these steps first:

  1. Before you make any changes, make sure your pi is working: 

        `pi-install unix-side/hello.bin`

  2. Set aside the working microSD card from step 1 and do not modify it
     during this lab.

  3. If you have a second micrsd: Make a second working micro-SD card
     by copying the firmware over it.  Run `hello.bin` to make sure
     it works.

     You will only ever modify this second microSD.  Leave the first one
     alone.  

     If your pi stops working during the lab, this gives you an easy,
     quick way to check during the lab if a hardware problem has come up
     (e.g., a loose wire) or if it's just a bug in your bootloader code: just
     swap them and run `hello.bin`.

     Several people last year wasted hours trying to fix their software
     when in fact it was a hardware problem.  You always want a quick
     way to delta debug, especially since we are all remote.

     Ideally: you have a second pi.

--------------------------------------------------------------------
### Step 1: write the unix side `my-install` first

Debugging the pi code will be painful, especially since it requires
copying files to the microSD etc.  So we first start with the much nicer
task of replacing the Unix side bootloader.  If you write it correctly,
it should work seamlessly with the original pi-side.


Where is the code:

  1. The sub-directory `unix-side` has the code that will run on
     your Unix laptop. You'll implement the code missing in
     `simple-boot.c:simple_boot`. 
 
  2. The header file `unix-side/simple-boot.h` has a bunch of useful
     utility routines you should use.     In particular, it has tracing
     versions of `PUT` and `GET` that you can use to print the bytes
     going back and forth.

  3. The header file `pi-side/simple-boot-defs.h` has the enums
     you should use when sending protocol messages. 

  4. The directory `cs140e-21spr/libunix` has a set of Unix
     utility routines.  By now you've implemented two of these:
     `find_ttyusb` and `read_file`.

What to do:

  0. Start working on getting `my-install unix-side/hello.bin` to 
     boot correctly.
  1. You should enable tracing for everything besides sending the
     raw code over so you can compare what you do to other people.
  2. Make sure you check that the `TRACE:HASH` output matches
     other people: if not, there is a bug in your `read_file`.

  3. Once this works, run the other tests we provide (we will add
     these during the lab).

  4. Copy `my-install` to your `~/bin` and resource your shell. 
     Now you can just use yours! 


##### A common unix-side mistake.

A key feature you have that earlier student's did not is the ability
to use `putk` from your bootloader code.  (Described more in part 2.)
This makes debugging wildly easier.  (Embarrassingly, I only realized
the trick to allow easy printing last year.)  Without output, all bugs
lead to: "my pi isn't responding," which is difficult function to invert
back to root cause.

However, this does lead to a common mistake on the unix side:

Common mistaks:
  1. Make sure you use the `get_op` routine for any word that could
     be a protocol opcode.  Otherwise you won't correctly handle when
     the pi-side sends a `putk` message (see below).

--------------------------------------------------------------------
### Step 2: write the pi side bootloader

Now you'll write the pi-side.  It should mirror the unix side code and
`PUT` values the unix side has done a `GET` on, and vice-versa.

Where is the code:

  1. `pi-side/get-code.h`: all the code you write will go in here.
  2. `pi-side/bootloader.c`: this has the `notmain` code that calls
     your `get_code` implementation and then jumps and executes it
     (using `BRANCHTO`).

     It also defines the input and output routines and then includes
     your code so it can use them.  This odd structure makes it easier
     to repurpose `get_code` for other devices, which in turn makes it
     easier to do a couple later labs.

What you will do:

  0. You'll write the missing code in `get-code.h:has_data_timeout`,
     which returns 1 if there is data (using `boot_has_data`) or 0 if
     it timed-out after a given number of micro-seconds (you can check
     this using `timer_get_usec` in `pi-side/libpi.small/timer.c`.

     The bootloader uses this to send a `GET_PROG_INFO` message 
     every 300ms.

  1. You'll write missing code in `get-code.h:get_code` to get the 
     code and copy it where it should go.

  2. After you compile, copy the resultant `kernel.img` to the SD card
     just as you did in lab 0.

  3. Use the tracing on the unix-side to debug what is going on.
     The traces should be identical to using our bootloader, other
     than the initial number of `GET_PROG_INFO` messages could
     be different.

  4. Again, note, you do have a simple `putk` you can call on the
     `pi-side`.  I would use `putk` judiciously as you incrementally
     develop the code so you can see what state the pi believes it is in.

#### A common pi-side mistake

A key limit of the pi UART hardware is that it only has space to hold 8
bytes in its internal hardware buffer:  if your pi code take too long
between UART reads, the hardware will fill up and then drop bytes.
Thus, whenever the pi side is expecting to receive data, it has to
move promptly.

If you forget this limitation, and (for example) print using `putk`
when a message is arriving, you'll almost certainly input lose bytes,
and also get confused.  Ask me how I know!

--------------------------------------------------------------------
#### Extension: make the bootloader better

If you recall, the unix side of the bootloader has some pretty useless
error messages.  Fix these so they are more helpful.

Similarly: if the pi-side runs into trouble, do something more useful
than just lock up.  For example, if you try to send `GET_PROG_INFO`
some number of times without a response, blink the internal LED (pin 47)
multiple times to show things are in a bad state and reboot.

Also, you can change `boot_putk` to implement `printk` type functionality
of taking a format string rather than a fixed string.

--------------------------------------------------------------------
#### Extensions.

Many possible extensions.  Since this low level code is used by everything we 
want to be absolutely sure it is rock-solid in all situations:

  0. Change the pi-side `get_uint8` calls to timeout if they are stuck for "too long"
     and then reboot.  This is useful for having a stand-alone pi in the field you
     can update.  If it gets stuck you don't have to walk (or drive) over and
     hit a button to restart it.
  1. Set up your tracing (`2-trace`) so you can trace a copy of the bootloader on the pi.
  2. Setup logging on the unix side so you can record the `PUT/GET` operations from
     a given run and then replay them later, so you can check that the
     unix side behaves identically (all output is the same, etc).
  3. Start mutating the traces in (2) and make sure the unix side behaves sensibly.
  4. Adapt your `fake-pi` so that it can run the bootloader and use this to do
     many randomized tests or mutate previous runs.
