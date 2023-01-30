## Today: Build a bootloader.

Last's weeks labs were intense --- we take a bit of a breather
lab: no assembly, weird bugs from register corruptions, etc.  

Today you'll write the bootloader code.  Both the r/pi side code
that asks for a binary  (`bootloader.bin`) and the Unix side that
sends it (`pi-install`).  

  - The bootloader protocol is defined in: [BOOTLOADER](./BOOTLOADER.md).

After this lab, the main missing piece of the current system will be the
UART driver (`staff-objs/uart.o`) which we will do fairly soon.  At that
point you'll have removed all magic from your r/pi setup by writing all
the code down to the bare metal.  The only code that will run when you
boot (and the only code you run to boot) will either be stuff you wrote,
or simple library routines we give you (which you can also replace!).

The lab has two parts:
  1. [Write the unix side `my-install`](#step-1-write-the-unix-side-my-install)
     (named to differentiate from `pi-install`)
     and make sure that works.
  2. [Write the pi side bootloader](#step-2-write-the-pi-side-bootloader),
     install it on your r/pi's microSD and make sure both work together.

#### Checkoff

The checkoff for this lab is pretty simple: bootload some programs and
check they work the same using your bootloader as they do with the 
staff.  If they don't, at least one of we or you are wrong.  

More specifically:

  - `make check` in `checkoff/output-tests` passes.  This compares the raw output
    of your bootloader to the staff bootloader.

  - `make check` in `checkoff/trace-tests` passes: this is a more-fine
    grained check that traces the `PUT` and `GET` calls between the r/pi
    and your laptop. [checkoff-tests/README](checkoff-tests/README.md)
    gives more detail.

--------------------------------------------------------------------
### 0. How to make coding go better.

There's some simple changes you can do to make this lab go much faster.

#### MUST Have: two code windows for send/receive protocols

You'll be writing two pieces of code that talk to each other through
messages.  If at all possible we strongly suggest you figure out how to
have two code windows side-by-side, one holding the pi code, the other
holding the unix code.  This allows you to easily check that the bytes you
send in one have a matching set of receives on the other.  (We recommend
doing the same thing for any networking send/receive protcol.)

With two coding windows, if you have a good grasp of the [bootloadig
protocol](./BOOTLOADER.md) and of the helper functions, you may be able to
type all this out in about 10-20 minutes.  If not, it could take hours.
Based on past labs, we'd expect this one change to easily halve the
total time.

#### Nice have: Backup known state: use a spare microSD if you can.

You can often dramatically reduce complexity by not sharing stuff.  One
process versus many,  one car on an empty highway versus rush hour, etc.
This lab is no different.  If you have a a second microSD card, it will
be very easy to tell if you have a hardware bug versus a bootloader issue.
Algorithm:
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

     On that note: Ideally, you'd have a second pi. Since we can't do
     that, you *can* simulate that world by swapping with a partner
     (assuming their setup works!).

While the vast majority (or all) of your bugs will be from the bootloader,
it's easy to dislodge a jumper or be unlucky and short your pi, and
then spend an inordinate amount of time debugging code that is not the
distal, hardware problem.  (Making it harder, the code can be buggy as
well of course!)

--------------------------------------------------------------------
### Step 1: write the unix side `my-install` 

Debugging the pi code will be painful, especially since it requires
copying files to the microSD etc.  So we first start with the much nicer
task of replacing the Unix-side bootloader.  If you write it correctly,
it should work seamlessly with the original pi-side.

Where is the code:

  1. The sub-directory `unix-side` has the code that will run on
     your Unix laptop. You'll implement the code missing in
     `simple-boot.c:simple_boot`. 
 
  2. The header file `unix-side/simple-boot.h` has a bunch of useful
     utility routines you should use.     In particular, it has tracing
     versions of `PUT` and `GET` that you can use to print the bytes
     going back and forth.

  3. The header file `../pi-side/simple-boot-defs.h` has the enums
     you should use when sending protocol messages. 

  4. The directory `../../libunix` has a set of Unix
     utility routines.  By now you've implemented two of these:
     `find_ttyusb` and `read_file`.

What to do:

  0. Start working on getting `my-install unix-side/hello.bin` to 
     boot correctly.
  1. You should enable tracing (set `trace_p=1`) for everything
     besides sending the raw code over so you can compare what you do
     to other people.
  2. Make sure you check that the `TRACE:HASH` output matches
     other people: if not, there is a bug in your `read_file`.
  3. Once this works, run the other tests we provide (we will add
     these during the lab).
  4. Copy `my-install` to your `~/bin` and resource your shell. 
     Now you can just use yours! 
  5. `make check BOOTLOADER=my-install` should pass.

##### A common unix-side mistake.

A key feature you have that 140e offerigs did not is the ability to use
`putk` from your pi-side bootloader code.  (Described more in part 2.)
This makes debugging wildly easier.  (Embarrassingly, I only realized
the trick to allow easy printing last year.)  Without output, all bugs
lead to: "my pi isn't responding," which is difficult function to invert
back to root cause.

However, this does lead to a common mistake on the unix side:
  - Make sure you use the `get_op` routine for any word that could
    be a protocol opcode.  Otherwise you won't correctly handle when
    the pi-side sends a `putk` message (see below).


##### Checkoff 

***MAKE THIS CLEARER***: More detailed:

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
### Step 2: write the pi side bootloader

Now you'll write the pi-side.  It should mirror the unix side code and
`PUT` values the unix side has done a `GET` on, and vice-versa.

Where is the code:

  1. `pi-side/get-code.c`: all the code you write will go in here.
  2. `pi-side/get-code.h`: has useful helper routines.
  3. `pi-side/boot-start.S`: it's useful to compare this file to 
     `libpi/staff-start.S` and see what the difference is.
  4. `pi-side/bootloader.c`: this has the `notmain` code that calls
     your `get_code` implementation and then jumps and executes it
     (using `BRANCHTO`).

What you will do:

  1. Implement all the code that is missing in `pi-side/get_code.c`
     by searching for the macro `boot_todo`.

  2. `make` should produce a `kernel.img` that you should copy to your
     pi SD card just as you did in lab 0.

  3. Use the tracing on the unix-side to debug what is going on.
     The traces should be identical to using our bootloader, other
     than the initial number of `GET_PROG_INFO` messages could
     be different.

  4. Again, note, you do have a simple `putk` you can call on the
     `pi-side`.  I would use `putk` judiciously as you incrementally
     develop the code so you can see what state the pi believes it is in.

  5. `make check BOOTLOADER=my-install` should pass and the bootloader
     should print your name.

#### A common pi-side mistake

The r/pi UART hardware only has space to hold 8 received bytes.  If your
pi code take too long between UART reads (`uart_get8`), the hardware will
fill up and then drop bytes.  Thus, whenever the pi side is expecting
to receive data, it has to move promptly.

If you forget this limitation, and (for example) print using `putk` when
a message is arriving, you'll almost certainly lost some arriving bytes,
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