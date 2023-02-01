## Today: Build a bootloader.

<p align="center">
  <img src="images/robot-pi.png" width="450" />
</p>


Last's weeks labs were intense --- we take a bit of a breather
lab: no assembly, weird bugs from register corruptions, etc.  

Today you'll write the bootloader code.  Both the r/pi side code
that asks for a binary  (the `bootloader.bin` from lab 0) and the 
Unix side that sends it (`my-install`).  

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

#### Standard checkoff

The standard checkoff for this lab is pretty simple: bootload some
programs and check they work the same using your bootloader as they do
with the staff.  If they don't, at least one of we or you are wrong.

More specifically:

  - `make checkoff` in `checkoff` passes.  This makes sure old regression
    tests from previous labs pass and that the traced bootloader messages
    match.

  - [EXTENSIONS](./EXTENSIONS.md) gives a list of extensions.

#### Hard checkoff

Because this lab is organized around a network protocol, it's pretty
simple to completely ignore our code, and build the parts for each side
entirely on your own.  You'll learn a lot!  (We did when we first build
all of this :).  

To be interoperable you will need:
  1. The opcodes defined in `pi-side:boot-defs.h`.
  2. The CRC code given in `pi-side:boot-crc32.h`.

You might want to take the `my-install.c` driver just so you don't have
to mess around with matching argument parsing and setting the TTY speed.

Note: if you are going this way, please let us know first so we can 
cheer you on.

--------------------------------------------------------------------
### 0. How to make coding go better.

There's some simple changes you can do to make this lab go much faster.

#### MUST Have: two code windows for send/receive protocols

You'll be writing two pieces of code that talk to each other through
messages.  If at all possible we strongly suggest you figure out how to
have two code windows side-by-side, one holding the pi code, the other
holding the Unix code.  This allows you to easily check that the bytes you
send in one have a matching set of receives on the other.  (We recommend
doing the same thing for any networking send/receive protocol.)

With two coding windows, if you have a good grasp of the [bootloading
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
### Step 1: write the Unix side `my-install` 

Debugging the pi code will be painful, especially since it requires
copying files to the microSD etc.  So we first start with the much nicer
task of replacing the Unix-side bootloader.  If you write it correctly,
it should work seamlessly with the original pi-side.

Make sure you read [the bootloader protocol](./BOOTLOADER.md).

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
  1. You can toggle tracing on and off from the command line.
     `my-install --trace-control` will emit the bootloader 
     control messsages.  `my-install --trace-all` will emit all
     bootloader messages (including the code)
     so you can compare what you do
     to other people.
  2. Make sure you check that the `TRACE:simple_boot:` hash output matches
     other people: if not, there is a bug in your `read_file`.
  3. Once this works, run the tests in the `checkoff` directory.
     The [checkoff README](checkoff/README.md) describes them in 
     more detail.
  4. Copy `my-install` to your `~/bin` and resource your shell. 
     Now you can just use yours! 
  5. `make check` in the different checkoff directories should pass.

##### A common Unix-side mistake.

A key feature you have that 140e offerings did not is the ability to use
`putk` from your pi-side bootloader code.  (Described more in part 2.)
This makes debugging wildly easier.  (Embarrassingly, I only realized
the trick to allow easy printing last year.)  Without output, all bugs
lead to: "my pi isn't responding," which is difficult function to invert
back to root cause.

However, this does lead to a common mistake on the Unix side:
  - Make sure you use the `get_op` routine for any word that could
    be a protocol opcode.  Otherwise you won't correctly handle when
    the pi-side sends a `putk` message (see below).


##### Example of `my-install --trace-control`

As a comparison, when I run:

        % my-install --trace-control hello.bin

I get:

    TRACE:simple_boot: sending 3372 bytes, crc32=cf4943ae
    BOOT:waiting for a start
    TRACE:GET32:11112222 [GET_PROG_INFO]
    TRACE:PUT32:33334444 [PUT_PROG_INFO]
    TRACE:PUT32:8000 [UNKNOWN]
    TRACE:PUT32:d2c [UNKNOWN]
    TRACE:PUT32:cf4943ae [UNKNOWN]
    TRACE:GET32:55556666 [GET_CODE]
    TRACE:GET32:cf4943ae [UNKNOWN]
    TRACE:PUT32:77778888 [PUT_CODE]
    DEBUG:PRINT_STRING:pi sent print: <<STAFF>: success: Received the program!>
    TRACE:GET32:9999aaaa [BOOT_SUCCESS]
    BOOT:bootloader: Done.
    hello world
    DONE!!!


--------------------------------------------------------------------
### Step 2: write the pi side bootloader

Now you'll write the pi-side.  It should mirror the Unix side code and
`PUT` values the Unix side has done a `GET` on, and vice-versa.

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

  5. `make check` in the `checkoff` directory should pass and the
     bootloader
     should print your name.

#### A common pi-side mistake

The r/pi UART hardware only has space to hold 8 received bytes.  If your
pi code take too long between UART reads (`uart_get8`), the hardware will
fill up and then drop bytes.  Thus, whenever the pi side is expecting
to receive data, it has to move promptly.

If you forget this limitation, and (for example) print using `putk` when
a message is arriving, you'll almost certainly lost some arriving bytes,
and also get confused.  Ask me how I know!
