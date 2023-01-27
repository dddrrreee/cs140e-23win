## Writing a non-preemptive threads package

***MAKE SURE YOU***:
  - Read the [PRELAB](PRELAB.md).
  - Read the [THREAD](THREAD.md) writeup.

Big picture:  by the end of this lab you will have a very simple
round-robin threading package for "cooperative" (i.e., non-preemptive)
threads.

The main operations (see `code/rpi-thread.[ch]`):
  1. `rpi_fork(code, arg)`: to create a new thread, put it on the `runq`.
  2. `rpi_yield()`: yield control to another thread.
  3. `rpi_exit(int)`: kills the current thread.
  4. `rpi_thread_start()`: starts the threading system for the first time.
      Returns when there are threads left to run.

Generally, at a minimum, each thread has its own stack --- which has
to be "large enough" so that it doesn't get overrun --- and the thread
package has to provide an assembly routine to "context switch" from one
thread to another.  Context switching works by:

   1. Saving all callee-registers from the first thread onto its stack
      and storing
      this new stack pointer in the thread control block.

   2. Loading all callee registers for the second from where they
      were saved.

   3. Changing the program counter value to resume the new thread.

The main magic you'll do today is writing this context-switching code and
performing the "brain-surgery" on each newly thread stack so that the
first context switch into it works.  Context-switching doesn't require
much work --- about five instructions using special ARM instructions,
but mistakes can be extremely hard to find.  So we'll break it down into
several smaller pieces.

#### Hints 

While today is exciting, a major sad is that a single bug can lead to
your code jumping off into hyperspace.  As you learned in the interrupt
lab, such bugs are hard to debug.  So before you write a bunch of code:

  1. Try to make it into small, simple, testable pieces.
  2. Print all sorts of stuff so you can sanity check!  (e.g., the value
     of the stack pointer, the value of the register you just loaded).
     Don't be afraid to call C code from assembly to do so.

#### Before you start: make sure everything works.

Run `make checkoff` in the `code-threads/` directory: it should pass.

  - By default the `code-threads/Makefile` will use our staff code in libpi.
  - If you run `make checkoff` in `code-threads` all the tests should pass.
  - Before you start implementing, comment out `USE_STAFF=1` in
    `code-threads/Makefile` so that the Makefile will use your code.
    You can flip back and forth to test.

#### Checkoff:

   - You pass `make checkoff` in the `code-threads/` directory.
   - Various extensions are in: [EXTENSIONS](./EXTENSIONS.md)

----------------------------------------------------------------------
### Part 0: writing code to check machine understanding: `code-asm-checks`

This lab depends on correctly figuring out low-level machine facts.
Does the stack grow up or down?  Does a `push` instruction modify
the stack pointer before it pushes a register or after?

The ARMv6 manual does contain these facts.  However, it's easy to
misunderstand the prose, forget the right answer, just make a mistake.
In this first part of the lab you'll write small pieces of code that can
check your understanding by deriving these facts from machine behavior.

  1. Doing it seperately makes it easy to debug.
  2. It gets you thinking about how to use the compiler to answer
     machine-level questions, which is a lifetime kind of skill.

These will reduce the time you spend chasing thread bugs.  It will also
make you better at actively figuring machine-level facts out:

  - The single biggest obstacle I've seen people make when writing
    assembly is that when they get stuck, they wind up staring passively
    at instructions for a long time, trying to discern what it does or
    why it is behaving "weird."

  - Instead of treating the code passively, be aggressive: your computer
    cannot fight back, and if you ask questions in the form of code
    it must answer.  When you get unsure of something, write small
    assembly routines to clarify.  For example, can't figure out why
    `LDMIA` isn't working?  Write a routine that uses it to store a
    single register.  Can't figure out if the stack grows up or down?
    take addresses of local variables in the callstack print them (or
    look in the `.list` files).

  - Being active will speed up things, and give you a way to
    ferret out the answer to corner case questions you have.

  - Both the [using gcc for asm](../../notes/using-gcc-for-asm/README.md)
    and [caller callee](../../notes/caller-callee.md) give some examples.

You should complete the five small programs in `code-asm-checks` that will
give you answers to the following questions you need for your threads:

  1. Does the stack grow up or down?  Otherwise you won't know 
     whether to give the start of an allocated block as the stack
     or the end.

     Write the code in `1-stack-dir.c` to determine this by running code:
     it should run and print `SUCCESS`.

  2. Your context-switching code will save registers
     using the `push` instructions, which pushes a list of registers
     onto the stack.

     The question here: does `push` write to the stack before or after
     changing the stack pointer?  If you can't answer this question, you
     won't know the exact first address to initialize the stack pointer
     to.  A mistake will lead to hard-to-debug memory corruption bugs.

     Look in `2-where-push.c` to see what we need to check this, and 
     then implement `check_push_asm` in `asm-checks.S`.
  
  3. When you `push` multiple registers, what is the order they are written
     out? (Or, equivalently: where is each one placed?)

     Based on the architecture manual, the bulk register save and restore
     instructions store the smallest registers so that the smallest is
     at the lowest address.  (See: the armv6 document, or 
     [ARM doc](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0473m/dom1359731152499.html) or [this](https://www.heyrick.co.uk/armwiki/STM)).

     Add a small assembly routine to the and a caller that will validate
     that registers are pushed from smallest to largest.

     Note: for `push` and `pop` don't include the stack pointer in the
     register list!   

  4. Finish the code in `4-callee-regs.c` that when it runs
     validates that the given registers are callee or callee saved.
     Search for `todo` --- you don't have to write much code, but the
     cool this will be when it runs that you will know *for sure* if
     you need to save a register or not.

     The code abuses the C preprocessor enthusiastically to generate
     the functions we need.   This method of code generation will be
     useful in many cases where we need to repetitively generate inline
     assembly routines.

  5. Finally, how to write out the registers we need to save?  

     For context switching we need to save (1) the callee-saved registers,
     (2) the return register, and finally, (3) write the stack pointer
     to a storage location (`saved_sp` in our thread block).

     Look in `5-write-regs.c` to see what we need to check this, and then
     implement `write_regs_to_stack` in `asm-checks.S`.  When you run
     the test, each printed register other than the stack pointer should
     match its expected value (the location for `r4` should hold `4` etc).

     This pattern of doing something in assembly, then calling C code
     to print the results and then exit (because our execution state is
     messed up) is a useful one to use throughout the lab.

Don't be afraid to go through the ARM manual (`docs/armv6.pdf`) or the
lectures we posted in `6-threads/docs`.

Now you have code that can mechanically validate the key low-level facts
you need, time to write the thread code.

----------------------------------------------------------------------
### Part 1: getting run-to-completion threads working (20 minutes)

We've tried to break down the lab into isolated pieces so you can test
each one.  The first is to get a general feel for the `rpi_thread`
interfaces by getting "run-to-completion" (RTC) threads working: these
execute to completion without blocking.

RTC threads are an important special case that cover a surprising number
of cases.  Since they don't block, you can run them as deferred function
calls.   They should be significantly faster than normal threads since:
  1. you don't need to context switch into them.
  2. you don't need to context switch out of them.
  3. you won't have to fight bugs from assembly coding.
Most interrupt handlers will fall into this category.

What to do:
  1. `rpi_fork` create a thread block, set the `fn` and `arg` field
     to the values passed into `rpi_fork` and put the thread on the 
     run queue.
  2. `rpi_thread_start` remove threads in FIFO order using `Q_pop`,
     and call each function and when it returns free the thread block.
     When there are no more threads, return back to the caller.
  3. Panic if code reaches `rpi_exit` (since we don't handle it).

  4. Make sure you keep the `trace` calls in the thread code: we use these
     for testing!

What you do not have to do:
  1. Write any assembly.
  2. Do anything with the thread stack.

Tests: `code-threads/1-*` 
  - either set `PROGS` manually in the `Makefile`:

            PROGS = $(wildcard ./1-test*.c)
    and run:

            % make check

    OR do:

            % make PART=1 check

    Whichever method you do should pass.
  - `1-test-run-one.c`: run a single thread to completion.
  - `1-test-run-N.c`: run N threads to completion.

Note:
  - If a test fails, you can look at the associated reference 
    files `.raw` (full output) and `.out` (reduced output) and 
    compare them to the `.test` file generated by your code.

    For example, if `1-test-thread.bin` fails, `1-test-thread.test`
    contains the output from your code, and `1-test-thread.raw` the
    full reference output and `1-test-thread.out` just the reference
    trace statements.  You can also run `1-test-thread.bin` manually.

----------------------------------------------------------------------
### Part 2 and Part 3: these do not exist

Skip.


----------------------------------------------------------------------
### Part 4: building `rpi_fork` and `rpi_start_thread`

Given your have done state saving both in the interrupt labs and in Part 1
above you should be able to implement `rpi_cswitch` without too much fuss:

  - Put your `cswitch` code into `rpi_cswitch` in `thread-asm.S`
    This will be based on your code 
    `code-asm-checks/asm-checks.S:write_regs_to_stack` 
    (from part 1),
    except that you will add the code to restore the registers as well.
    For today: only save the callee saved registers.  Since we are doing
    non-pre-emptive threads, the compiler will have saved any live caller
    registers when calling `rpi_yield`.

After context-switching, the main tricky thing left to figure out is how
to setup a newly created thread so that when you run context switching
on it, the right thing will happen (i.e., it will invoke to `code(arg)`).

  - The standard way to do this: during thread creation (`rpi_fork`)
    manually store values onto the thread's stack (sometimes called
    "brain-surgery") so that when the thread's state is loaded via
    `rpi_cswitch` control will jump to a trampoline routine (written in
    assembly) with `code` with `arg` in known registers.   The trampoline
    will then branch-and-link (using the `bl` instruction) to the `code`
    address with the value of `arg` in `r0` (note: you will have to
    move it there).
    
    As you'll see in part 6: The use of a trampoline lets us handle
    the problem of missing `rpi_exit` calls.

First, write `rpi_fork` :

  1. `rpi_fork` should write the address of trampoline
     `rpi_init_trampoline` to the `lr` offset in the newly
      created thread's stack (make sure you understand why!)  and the
      `code` and `arg` to some other register offsets (e.g., `r4` and
      `r5`) --- the exact offsets don't matter.

  2. Implement `rpi_init_trampoline` in `thread-asm.S` so that
     it loads arg` from the stack (from Step 1) into `r0`, 
     loads `code` into another register that it then uses to 
     do a branch and link.

  3. To handle missing `rpi_exit`: add a call to `rpi_exit` at the end
     of `rpi_init_trampoline`.

  4. To help debug problems: you can initially have the
     trampoline code you write (`rpi_init_trampoline`) initially just
     call out to C code to print out its values so you can sanity check
     that they make sense.

Second, write `rpi_start_thread`:

  - `rpi_start_thread` will context switch into the first thread it
    removes from the run queue.  Doing so will require creating a
    dummy thread (store it in the `scheduler_thread` global variable)
    so that when there are no more runnable threads, `rpi_cswitch` will
    transfer control back and we can return back to the main program.
    (If you run into problems, first try just rebooting when there are
    no runnable threads.)

What to do for `rpi_start_thread`:
  1. If the runqueue is empty, return.
  2. Otherwise: allocate a thread block, set the `scheduler_thread`
     pointer to it and contxt switch into the first runable thread.
  3. You contxt-switch out of the scheduler thread once and into it once
     when runque is empty.

What you *do not* do with the `scheduler_thread` thread:
 - Initialize its stack.
 - The scheduler thread is never on the runqueue.

Tests:
  - `4-test-one-fork.c`: There is a trivial program that
    forks a single thread, runs it, and then reboots.  I.e., you do not
    need to have `rpi_exit` working.  This makes it easier to debug if
    something is going on in your context switching.

----------------------------------------------------------------------
### Part 5: implement `rpi_exit` and `rpi_yield`

First, write `rpi_exit`: 
  - If it can dequeue a new runnable thread, context switch into it and
    free the old one.

Tests:
  - `5-test-exit.c`: forks `N` threads that all explicitly call `rpi_exit`.
  - `5-test-restart.c`: restarts the threads package over and over.

     If the run queue is empty: context switch into the initial
     start thread created by `rpi_start_thread` (stored in the
     `scheduler_thread` variable).


Second, write `rpi_yield`:
  - It should yield to the first thread on the run-queue
    (if any) or simply return if the run-queue is empty.

Tests:
  - `5-test-yield.c`: forks `N` threads that all explicitly call `rpi_yield`
     and then call `rpi_exit`.
  - `5-test-yield-fail.c`: yields with an empty run queue.

----------------------------------------------------------------------
### Part 6: Handle missing `rpi_exit` calls.

Note: If the thread code does not call `rpi_exit` explicitly but instead
returns, the value in the `lr` register that it jumps to will be nonsense.
Our hack: have our trampoline (from part 4) that calls the thread code
simply call `rpi_exit` if the intial call to `code` returns.

If your trampoline in part 4 works as expected, this should "just work".

   - `6-test-implicit-exit.c`: should run and print `SUCCESS`.
   - `6-test-implicit-exit-run-N.c`: this is the same program as part 1,
     but the behavior will now change with a single print from 
     `rpi_exit`. 
   - `6-test-implicit-exit-run-one.c`: same program as `1-test-run-one.c`
     but will now have a single print from `rpi_exit`.

----------------------------------------------------------------------
### Part 7: Running the larger programs.

Checking:
   - `7-test-thread.c` should work and print `SUCCESS`.
   - `7-test-ping-pong.c` should work and print `SUCCESS`.

If you want to get fancy, you should be able to run two LEDs in 
   - `7-test-yield.c`

Congratulations!  Now you have a simple, but working thread implementation
and understand the most tricky part of the code (context-switching)
at a level most CS people do not.
