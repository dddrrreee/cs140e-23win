### Interrupts.

***Make sure you understand the readings in the [PRELAB](PRELAB.md)]***: 
  - how the hardware level works for arm exceptions, how to work with 
    modes, how to use the compiler to figure out low level information,
    how to setup interrupts with the broadcom.
  - Go through the code in `timer-int-ex` which is a complete working
    timer interrupt example.

Today's lab is a few small-ish pieces, hopefully that give you a better
view of bare-metal execution, assembly code, interrupts and exceptions.

Big picture:

   0. We'll show how to setup interrupts / exceptions on the r/pi.
      Interrupts and exceptions are useful for responding quickly to
      devices, doing pre-emptive threads, handling protection faults,
      and other things.

   1. We strip interrupts down to a fairly small amount of code.  You will
      go over each line and get a feel for what is going on.

   2. You will use timer interrupts to implement a simple but useful
      statistical profiler similar to `gprof`.  As is often the case,
      because the r/pi system is so simple, so too is the code we need
      to write, especially as compared to on a modern OS.

   3. You'll then implement system calls --- these work using exceptions
      which possibly confusingly use the same interrupt methods. 

Interrupts confuse people.  Often they get implemented in a complicated
way, or get discussed so abstractly it's hard to understand them.
Hopefully by the end of today you'll have a reasonable grasp of at
least one concrete implementation.  If you start kicking its tires and
replacing different pieces with equivalant methods you should get a
pretty firm grasp.

### Deliverables:

Turn-in:

  1. `0-timer-int`: give the shortest time between timer interrupts
     you can make.  Make two small edits to the `0-timer-int` register
     save and restore code (in `interrupts-asm.S`) that make the code
     crash in different locations.  This will help get an intuition for
     how things can go wrong.

  2. `1-gprof`: Implement `gprof.c`.   When you
     run you should see most of the time being spent in `PUT32`, `GET32`
     or the `uart` routines.

  3. `2-syscall`: `make check` passes.   In particular, for `1-syscall.c`
      You can switch to User level, run a system call, and resume.

-----------------------------------------------------------------
#### Background: Why interrupts

As you'll see over the rest of the quarter, managing multiple devices
can be difficult.  Either you check constantly (poll), which means most
checks are fruitless.    Or you do not check constantly, which means most
actions have significant delay since the device has stuff for you to do
much before you check it.  Interrupts will allow you do to your normal
actions, yet get interrupted as soon as an interesting event happens on
a device you care about.

You can use interrupts to mitigate the problem of device input by telling
the pi to jump to an interrupt handler (a piece of code with some special
rules) when something interesting happens.  An interrupt will interrupt
your normal code, run, finish, and jump back.  

Interrupts also allow you to not trust code to complete promptly, by giving
you the ability to run it for a given amount, and then interrupt
(pre-empt) it and switch to another thread of control.  Operating systems
and runtime systems use this ability to make a pre-emptive threads
package and, later, user-level processes.  The timer interrupt we do
for today's lab will give you the basic framework to do this.

Traditionally interrupts are used to refer to transfers of control
initiated by "external" events (such as a device or timer-expiration).
These are sometimes called asynchronous events.  Exceptions are
more-or-less the same, except they are synchronous events triggered by the
currently running thread (often by the previous or current instruction,
such as a access fault, divide by zero, illegal instruction, etc.
The framework we use will handle these as well; and, in fact, on the
arm at a mechanical level there is little difference.

Like everything good, interrupts also have a cost:  this will be our
first exposure to race conditions.  If both your regular code and the
interrupt handler read / write the same variables (which for us will be
the common case) you can have race conditions where they both overwrite
each other's values (partial solution: have one reader and one writer)
or miss updates because the compiler doesn't know about interrupt handling
and so eliminates variable reads b/c it doesn't see anyone changing them
(partial solution: mark shared variables as `volatile`).  We'll implement
different ways to mitigate these problems over the next couple of weeks.

Historically interrupts and exceptions are related, but differ
in how they get initiated:
  - Exceptions are traps caused by something the code did, such as
    a divide by zero, a memory protection fault or (maybe confusingly)
    a system call instruction (today).  

  - Interrupts are traps caused by some exogenous event, such as
    a network packet arriving, a GPIO pin triggering from high to low,
    or a timer expiring (today).  They might be related to the currently
    running code, or they might have nothing to do with it or, indeed,
    any process.

In both cases, the hardware does roughly the same thing:
  1. Saves enough of the state of the currently running code that it 
     can be resumed. For the ARMv6: it records the trapped pc (by moving
     it to the `lr`) and preserves the stack pointer by switching to a 
     banked copy.
  2. Jumping to a pre-determined code location, possibly after changing
     processor levels.  On the ARMv6: it switches to the exception or
     interrupt mode and, by default, jumps to a fixed address starting
     at 0.
  3. After the exception/interrupt is handled, the code jumps back to
     the faulting location with the original register values restored.  
     There usually has to be some kind of hardware support that allows 
     this jump and restore to be partially combined since you can't do
     one and then the other.  MIPS reserves two registers that the 
     code can use; ARMv6 provides a special instruction that will
     load the previously saved process status register while 
     simulatenously setting the pc.

As a final point: generally exceptions or interrupts are not recursive
by default.  (If you look at the timer interrupt code, you can probably
see what the challenge would be for taking a second interrupt while
handling the first.)  You can often take steps to make them so (ARMv6
can), but that's begging for destruction.   It is an interesting puzzle,
so can be illuminating, but I would try to avoid in reality.


----------------------------------------------------------------------------
### Part 0: timer interrupts.

Look through the code in `timer-int-ex`, compile it, run it.  Make sure
you can answer the questions in the comments.  We'll walk through it
in class.

-----------------------------------------------------------------------------
### Part 1: Using interrupts to build a profiler.

The nice thing about doing everything from scratch is that simple things
are simple to do.  We don't have to fight a big OS that can't get out
of its own way.

Today's lab is a good example: implementing a statistical profiler.
The basic intuition:
   1. Setup timer interrupts so that you get them fairly often.
   2. At each interrupt, get the address of the interrupted program
      counter and increment a counter associated with it.  
   3. Over time, these counts will build up: the locations with the
      highest count will be where your code spends most of its time.

The implementation will take about 30-40 lines of code in total.


The basic algorithm:

 1. Use `kmalloc` to allocate an array at least as big as the code.
    (We give you  a trivial `kmalloc` to use.)  Compute the code
    size using the labels defined in `libpi/memmap` (we give C
    definitions in `libpi/include/memmap.h`).

 2. In the interrupt handler, use the program counter value to index
    into this array and increment the associated count.  NOTE: its very
    easy to mess up sizes.  Each instruction is 4 bytes, so you'll divide
    the `pc` by 4.

 3. Periodically you can print out the non-zero values in this array
    along with the `pc` value they correspond to.  You should be able to
    look in the disassembled code (`gprof.list`) to see which instruction
    these correspond to.

    NOTE: We do not want to profile our profiling code, so have a way
    to disable counts when doing this printing.

 4. Expected results are most counts should be in `PUT32`, `GET32`,
    and various `uart` routines.  If you see a bunch of your `gprof`
    program counters its b/c of a mistake in step 3.

Congratulations!  You've built something that not many people in the
Gates building know how to do.

----------------------------------------------------------------------------
### Part 2: make a simple system call.

One we can get timer exceptions, we (perhaps surprisingly) have enough
infrastructure to make trivial system calls.   Since we are already
running in supervisor mode, these are not that useful as-is, but making
them now will show how trivial they actually are.  

You will implement two versions:
  - `0-syscall.c`: this just calls system calls at our current process
     level.  If you look in `libpi/staff-start.S` you see we start in
     `SUPER_MODE`, which is the same level system calls run at.  This
     means we already have a live stack pointer and you shouldn't use it.

  - `1-syscall.c`: this is a real system call.  You will implement the
     code to switch to user level, and then handle two trivial system
     calls.


##### `0-syscall.c`

Look in `2-syscall` and write the needed code in:

  1. `interrupts-asm.S`: you should be able to largely rip off the
     timer interrupt code to forward system call.  NOTE: a huge difference
     is that we are already running at supervisor level, so all registers
     are live.  You need to handle this differently.


  2. `0-syscall.c`: finish the system call vector code (should just be
     a few lines).  You want to act on system call 1 and reject all
     other calls with a `-1`.

This doesn't take much code, but you will have to think carefully about which
registers need to be saved, etc.

##### `1-syscall.c`

This is a real system call.  You'll need to:
  1. Make a new interrupt table that uses a different `swi` handler.
     You should copy and paste the existing one and update any labels.
     Admittedly this is mechanical work, but you need to think 
     slightly.
  2. Implement `run_user_code_asm`: this will switch to user mode,
     set the stack pointer register to a given stack value, and
     jump to a give code address.  

     For hints: look at `notes/mode-bugs/bug4-asm.S` for how to roughly
     do what you want at a different level.

  3. Finish implementing `1-syscall.c:syscall_vector`.  This is
     mainly just checking that you are at the right level.

If this works, congratulations!  You have a working user-level
system call.  This small amount of code is really all there is to it.

-----------------------------------------------------------------------------
### lab extensions:

There's lots of other things to do:

  1. Mess with the timer period to get it as short as possible.

  2. To access the banked registers can use load/store multiple (ldm, stm)
    with the caret after the register list: `stmia sp, {sp, lr}^`.

  3. Using a suffix for the `CPSR` and `SPSR` (e.g., `SPSR_cxsf`) to 
	specify the fields you are [modifying]
     (https://www.raspberrypi.org/forums/viewtopic.php?t=139856).

  4. New instructions `cps`, `srs`, `rfe` to manipulate privileged state
   (e.g., Section A4-113 in `docs/armv6.pdf`).   So you can do things like

            cpsie if @ Enable irq and fiq interrupts

            cpsid if @ ...and disable them

-----------------------------------------------------------------------------
### Supplemental documents

There's plenty to read, all put in the `docs` directory in this lab:
 
  1. If you get confused, the overview at `valvers` was useful: (http://www.valvers.com/open-software/raspberry-pi/step04-bare-metal-programming-in-c-pt4)

  2. We annotated the Broadcom discussion of general interrupts and
  timer interrupts on the pi in `7-interrupts/docs/BCM2835-ARM-timer-int.annot.pdf`.
  It's actually not too bad.

  3. We annotated the ARM discussion of registers and interrupts in
  `7-interrupts/docs/armv6-interrupts.annot.pdf`.

  4. There is also the RealView developer tool document, which has 
  some useful worked out examples (including how to switch contexts
  and grab the banked registers): `7-interrupts/docs/DUI0203.pdf`.

  5. There are two useful lectures on the ARM instruction set.
  Kind of dry, but easier to get through than the arm documents:
  `7-interrupts/docs/Arm_EE382N_4.pdf` and `7-interrupts/docs/Lecture8.pdf`.

If you find other documents that are more helpful, let us know!
