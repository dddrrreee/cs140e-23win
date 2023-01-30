## Lab 2: build a bootloader.

***NOTE: Make sure you start with the [PRELAB](PRELAB.md)!***

The last two labs have been intense --- we take a bit of a breather
lab: no assembly, weird bugs from register corruptions, etc.  

Today you'll write the bootloader code.  Both the r/pi side code
that asks for a binary  (`bootloader.bin`) and the Unix side that
sends it (`pi-install`).  
  - The bootloader protocol is defined in: [BOOTLOADER](./BOOTLOADER.md).

After this lab, the main missing piece of the current system will be the
UART driver (`staff-objs/uart.o`) which we will do fairly soon.  At that
poit you'll have removed all magic from your pi setup by writing all
the code down to the bare metal.  The only code running when you boot
(and the only code you run to boot) will either be stuff you wrote,
or simple library routines we give you (which you can also replace!).

The lab has two parts:
  1. Write the unix side `my-install` (named to differentiate from `pi-install`)
     and make sure that works.
  2. Write the pi side `bootloader.bin`, install it on your pi and
     make sure both work together.  

#### Checkoff

The checkoff for this lab is pretty simple: bootload some programs
and check they work the same.  More specifically:

  - `make check` in `tests/output-boot-trace` should pass.
  - [checkoff-tests/README](checkoff-tests/README.md) gives more decription.

fix this.  what is going on here?

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

--------------------------------------------------------------------
### 0. How to make coding go better.

There's some simple changes you can do to make this lab go much faster.

#### MUST DO: two code windows for send/receive protocols

You'll be writing two pieces of code that talk to each other through
messages.  If at all possible we strongly suggest you figure out how to
have two code windows side-by-side, one holding the pi code, the other
holding the unix code.  This allows you to easily check that the bytes
you send in one have a matching set of receives on the other.

With two coding windows, if you have a good grasp of the protocol below
and of the helper functions, you may be able to type all this out in
about 10-20 minutes.  If not, it could take hours.  Based on past labs,
this one change can halve the total time.

We recommend doing the same thing for any networking send/receive
protcol.

#### Backup known state: use a spare microSD if you can.

You can often dramatically reduce complexity by not sharing stuff.  One
process versus many,  one car on an empty highway versus rush hour, etc.
This lab is no different.  If you have a a second microSD card, it will
be very easy to tell if you have a hardware bug versus a bootloader issue.
  1. Set aside the clean, working microSD you are currently using.
  2. Setup a second microSD that you will write all pi-side modifications to.
     Leave the first one alone.  
  3. If your pi stops working during the lab, you can quickly swap back
     to the original microSD (with the original bootloader) and rerun.
     If it also does not work, you likely have a hardware issue.

     This method gives you an easy, quick way to check during the lab if
     a hardware problem has come up (e.g., a loose wire) or if it's just
     a bug in your bootloader code: just swap them and run `hello.bin`.

     Several people last year wasted hours trying to fix their software
     when in fact it was a hardware problem.  You always want a quick,
     robust way to do a binary-search, differential analysis to get at
     root causes.

     On that note: Ideally, you have a second pi.   You can simulate
     that world by swapping with a partner (assuming their setup works!).

While the vast majority (or all) of your bugs will be from the bootloader,
it's easy to dislodge a jumper or be unlucky and short your pi, and then 
spend an inordinate amount of time debugging code that is not the distal
problem.  (Making it harder, the code can be buggy as well of course!)

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

  4. The directory `../../libunix` has a set of Unix
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
