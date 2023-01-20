# Observation, code equivalance, and bugs.

Below we have a quick --- very incomplete and rough --- view of how the
C compiler  can break your code because it assumes C code only executes
sequentially.  As a result, many of the transformations the compiler
performs can destroy correctness when they involve memory locations read
or written by threads, interrupt handlers or hardware devices.  (All
things we do in this course.)

This is a good example of how the hardware-level programming we do in
this class is different from what you are used to at application level.

After you read this document, look at [BUG](./BUG.md) for a deep dive
through a subtle, not-uncommon bug.

--------------------------------------------------------------------
### The "As-if" substitution principle.

Bishop Berkeley is the patron saint of computer stuff.  If a tree falls in 
the forest and no one is there to hear it:

   - Then it could be stored in a register.
   - Or it could be cached
   - Or could be skipped.
   - It could fall out of order with other trees since you can't tell.
   - Or rendering could be deferred until someone walks by.

(A famous equivalence-substitution example is the Turing test.  When I
grew up, this was a snickering, "irrelevant" thing to worry about.  Now,
it's going to be interesting to see where the new gen of AI behaves in
an observably equivalent way to humans and can put them out of a job
and on the dole.)

Over-simplifying, the as-if substitution principle is: 

   - Given X and X': if X' has the same observable external behavior
   ("side-effects") as X does --- i.e., X' behaves "as if" it was X ---
   then we say they are equivalent and we can replace X with X'.

   - Equivalence substitution can apply at any level: a single line of
   code (replacing an expensive multiply with a shift for power-of-two
   multiplicands), a procedure (e.g., linear sort with quick sort) or
   even an entire system.    Presumably you have have relied on a rough
   notion of equivalence substitution multiple times where you replace
   an old laptop you can replace an old laptop with a new, faster one
   or an old version of an OS with a newer one, or setup dual booting.

Equivalence substitution is a core systems principle.  We will use it,
exploit it, and sometimes be burned by it this quarter.  It shows up at
every level of the computer systems.  It's the only reasons things work
at all at any kind of speed.

Examples from real life:

  - Compiler optimization: your code is slow; a good chunk of the
    compile is devoted to optimizating it.  The compiler replaces chunks
    of code with faster or smaller equivalants that behave "as if" they
    were the original.  If may even delete the code (dead code removal)
    if an external observer could not tell.

  - Hardware optimization: you may imagine the hardware runnings your
    program's corresponding assembly code in order, and memory holds
    a single, unique version of each variable.  But, of course, modern
    hardware does no such thing.  It aggressively re-orders instructions,
    speculatively executes paths, caches items in multiple locations
    (first level cache, second level cache, write buffers, registers),
    and many many other tricks.

    Memory model is so important: who can observe what, when.  If we
    can't answer this, then we can't reason about if an observer can
    tell if we replaced code.

  - The process abstraction: A crucial step in civilization is that you
    can write your code as if it was sequential, running in isolation.
    Of course, you've never written a true sequential program.  If you run
    "ps aux" on your laptop you'll likely see hundreds of other programs
    running concurrently with your code  (and thousands of interrupts
    happening in the kernel).

    By defining programming language and operating systems interfaces we
    can allow you to write code "as if" it was sequential when, in fact,
    its executing in massively parallel environment.  (How: by writing
    to its own private memory.  Once you start sharing memory --- which
    includes file system state --- then you get all the usual problems).

Weirdly, despite being ubiquitous, it's rarely talked about explicitly.

### Compiler equivalence  substitution

When you write code, you likely reason about how it behaves by looking
at the source code at the source code (loops, variables, function
calls, etc) and pushing the side-effects of these operations forwards
and backwards.  However, obviously, hardware can't run C/C++/Rust source
directly.  Thus the compiler translates your source code down to low-level
"machine code" operations that --- aspirationally --- will behave "as if"
it was the original (sort of: only if your program was well-defined).

As part of this translation, compilers aggressively optimize code by
replacing possibly large chunks with purportedly faster or smaller
equivalents that behave "as if" they were the original.  The compiler
will frequently reorder operations or delete them (dead code removal)
if it believes an external observer could not tell.  A very small set
of examples:

   - inlining.
   - common sub-expression elimination
   - code hoisting.
   - code re-ordering
   - register promotion
   - reorder reads and writes (this is one that really burns us).
   - redundant read elimination (this also burns us)

The key issue for the compiler's game is what an "observer" is.  If the
observer can tell the difference between P and P', then they are not
equivalent. 

#### Example: replace runtime operations with constants

As an example of observation and equivalence, consider the 
contrived code:

        // trivial.c
        int foo(void) {
            int x = 3;
            int y = 4;
            int z = 5;
            int w = 6;
        
            return x*y+z*w;
        }

If we compile this to machine code (so that the compiler optimizes it)
and then disassemble the machine code (so we can understand it) we can
see exactly what the compiler did:

    # compile with reasonable optimization using the 
    # arm gcc compiler for this class
    % arm-none-eabi-gcc -O2 -c trivial.c
    # disassemble
    % arm-none-eabi-objdump -d trivial.o

    Disassembly of section .text:
    00000000 <foo>:
        0:	e3a0002a 	mov	r0, #42	; 0x2a
        4:	e12fff1e 	bx	lr


Here the compiler views the only "observation" that occurs as the value
returned from `foo`.  Since all of `foo`'s local variables are constants, 
the compiler can compute the final result at compile-time ("statically")
and replace all these calculations with 
a single constant `42` and return it (on ARMv6,
an integer return result is put in register `r0`).

A couple notes:
  1. You should get used to looking at machine code! 
  2. Even if you don't know how to write assembly code, or
     even understand all (most) of of the instructions, it's possible
     to get a sense for key events such as what locations are being read
     or written, what values returned, etc.

     A common question: we often want to know the order of reads or
     writes to device memory locations and if any were removed.


  4. Further, since the code for `trivial.c` is well-defined, we can
     compile ito for other machine architectures and yet still see
     similar behavior.  

     For example, even though your laptop almost certainly uses a
     different CPU than the pi, we can still compile code for the pi on it
     ("cross-compilation").  Using the native `gcc` and `objdump`
     on my 64-bit x86 laptop:

            % gcc -O2 -c trivial.c
            % objdump -d trivial.c
            0000000000000000 <foo>:
                0:	f3 0f 1e fa          	endbr64 
                4:	b8 2a 00 00 00       	mov    $0x2a,%eax
                9:	c3                   	retq   

     Here, too, the compiler has computed 42 (`0x2a`) and returned it.
     Though, unsurprisingly, the machine code looks very different.

   5. Detail: The reason the code addresses start at 0 is that we have
      not linked the code.  We'll discuss this in later labs.

### Tradeoffs in observer power

If you think about it, a strong observer is good for correctness but
bad for speed, and vice versa.  The more all-seeing and all-knowing the
observer is:

  1. The easier it is for us as programmers to reason about what the
     code does (since the observations are what we are allowed to
     rely on).  If code can behave in exactly one way, it is easier to
     reason about.

     For example, the easiest language definition for programmers to
     reason about:  every each operation executes exactly as written
     and a series of operations run in the order they were written
     (sequential consistency).  What does the code do?  Exactly what it
     says, in exactly the order it's written, with no changes.  What you
     see is what you get.

     On the other hand if the code is allowed to have different possible
     results (e.g., if it has a non-sequential memory model) it's harder
     to figure out what is going on.

  2. But the more changes the observer can detect,
     and the less substitution can occur.

     Observers with complete information --- such as your CPU, a debugger,
     or even a disassembler (which will see any code transformation)
     --- can see exactly how code differs.  If we had to satisfy the
     equivalence judgement of these observers, we could never optimize
     anything, because they could detect any change.  For example,
     if you replaced a multiply with a shift or reordered two stores,
     any disassembler or debugger could see this.

Since a weak observer can be fooled the most number of ways and allows
the most number of substitutions the game we play is to try to make the
observer as weak as possible in every every way that "does not matter"
(e.g., the exact order ALU operations execute) while making it as strong
as possible in ways that do matter (e.g., the exact order that memory
operations complete).  Of course, the kicker is "does not matter": a
human looking at the code must be able to look at (observe) the program
and reason about what it does.  Humans already can't do simple, reliably
--- adding non-deterministic behavior (e.g., look up "memory models")
can easily make correctness hopeless.


In the big world, the rough place many systems settle on is that they
ignore "how" a result is computed (what instructions actually run on
the CPU), and instead only require that externally observable actions
(such as network messages, disk writes, output statements, exceptions,
or launching a rocket) happen in the order they are written, without
duplication or eliminating. As long as these observations remain the
same, they treat code itself as a black box and (very roughly speaking)
do not concern themselves with the internal computation.  As an example:
An entire program could be replaced with a lookup table if that gives
externally equivalent side-effects.  It could even be deleted entirely
if it has no output.

This is a subtle topic so we'll largely just ignore it 
and get to examples.

### How as-if lets the compiler wtf-you.

To bring our discussion back to the concrete , low-level of this class:
roughly speaking, the C compiler `gcc` that we will use  assumes that
only the source code you write can affect storage locations, and only
do so within a single path of execution that runs sequentially.  It does
not know about threads, interrupts, or hardware devices.

Let's say it again, since one sentence will be the root of many of
your problems:

  - C does not know about threads, interrupts, or hardware devices.
  - C does not know about threads, interrupts, or hardware devices.
  - C does not know about threads, interrupts, or hardware devices.
  - C does not know about threads, interrupts, or hardware devices.
  - C does not know about threads, interrupts, or hardware devices.
  - C does not know about threads, interrupts, or hardware devices.

How this breaks your code:

  1. The C compiler is allowed to do any transformation that
     preserves side-effect equivalence with a "sequentially consistent"
     program where every read returns the value of the last write.

  2. But any memory location shared between threads, interrupts or
     hardware devices can potentially be read or written in ways that
     the compiler assumes are "impossible".   These reads or writes will
     break your code.

Trivial example: if we have code that does:

    y = 1                   
    x = 2;

Since there is no read of `y` or `x` in this code --- no observation
of either variable --- `gcc` can reorder these as:

    x = 2;
    y = 1                   

It can even delete both if it sees no reads of them elsewhere.  Or if
there are no writes, replace these reads with the constants.

All well and good.  Unfortunately, if there is another thread or
an interrupt handler running concurrently that can read `x` and `y`
they see if these reorderings or deletions occur.  This can cause real
problems.  Or consider a bit fancier code where `x` was called `lock`
and was supposed to protect `y` --- if the compiler reorders writes to
`lock` and `y` it's broken your critical section.

We'll now show some example bugs that have burned us in this class.

--------------------------------------------------------------------
### A specific way the compiler will break your code this quarter

For this class, often you will communicate with hardware devices
by reading and writing magic values to special memory locations.
While these look like normal loads and stores in your program, they
essentially get turned into messages that are sent to and received from
the hardware device.

Thus, unsurprisingly, the order of reads and writes to these locations
matters very much.  (One way to see: take any signal or whatsapp message
chain between two people that are dating and see how many breakups you
could have caused by re-ordering just one or two messages.)

Thus, device hardware is both a strict observer and a mutator of these
special locations.  Unfortunately, `gcc` has no idea these locations
are magic and falsely assumes:


  1. The only way to change the value of a storage location is mutation
     within the program source (e.g., an assignment).

     The compiler does not realize that that there is an external device
     that can spontaneously change the value without any corresponding
     source code.

     A cliched example of how this breaks, consider some typical code
     that waits until a status field says a hardware FIFO is not full:

            #define MAILBOX_FULL   (1<<31)

            // status field of r/pi mailbox
            static uint32_t *status =  (void*)(0x2000B880+4*7);

            // wait until mailbox is not full
            while(*status & MAILBOX_FULL)
                ;
            return;

     In this case, the compiler sees that the `while` loop contains no
     store that can affect `status`.  Because it assumes sequential
     execution, it decides it can replace the loop with an if-statement that
     checks the loop condition once, and if it is true, infinite loops
     without reading the location again (this is not what we want).
     Otherwise falls through (this is fine).
     To see this:

            % arm-none-eabi-gcc -O3 -c wait-not-full.c 
            % arm-none-eabi-objdump -d  wait-not-full.o 

            00000000 <wait_not_full>:
            # load <status> address
            0:	e59f300c 	ldr	r3, [pc, #12]
            # load <status> value
            4:	e593389c 	ldr	r3, [r3, #2204]
            # compare if high bit set
            8:	e3530000 	cmp	r3, #0
            # if not: return
            c:	a12fff1e 	bxge	lr
            # if so: inf loop
            10:	eafffffe 	b	10 <wait_not_full+0x10>
            # status address: hack b/c can't load w
            # one instruction.
            14:	2000b000 	.word	0x2000b000

  2. The compiler also assumes the only way to read a location is via
     an access  within the program source.  The compiler does not realize
     there is a concurrent hardware device that can see exactly what is
     written as soon as the change occurs.
  
     As with the previous point, this issue also arises with interrupt
     or exception handlers which can interleave with code that modifies
     a location.

     A contrived example of how this can be bad, assume we have an
     `initialized` variable and a `cnt` variable shared with an
     interrupt handler.

            int initialized;
            unsigned *cnt;

            // assume: will run whenever interrupts trigger
            void int_handler(void) {
                if(!initialized)
                    return;
                *cnt = *cnt + 1;
            }

            // initialization code
            cnt = malloc(sizeof *cnt);
            *cnt = 0;
            initialized = 1;

     Given that the two assignments to `cnt` and `initialized` are
     independent for sequential code, the compiler could potentially
     reorder this as:

            initialized = 1;
            cnt = malloc(sizeof *cnt);
            *cnt = 0;

     Which would cause memory corruption or reads of garbage values if the
     interrupt handler triggered during this process.


Note: in both cases usually the code would work.  As a result, the bugs
do not deterministically show up each time they run (``bohr bugs'') but
rather are intermittent ``Heisenbugs'' that come and go.  Add a `printk`?
Goes away.  Run again?  Goes away.   Ship the code to a million customers?
10 get hit, sometimes, but not when you send a kernel genius to go debug
the problem.


To summarize on repeat: threads, interrupt handlers, and devices are
all external agent that can see or affect program values in ways that
violate the compilers assumptions.  Thus, since it does transformations
without an accurate notion of what can see the result, its optimizations
can totally destroy the intended semantics.  (It can be very hard to
detect such problems without looking at the machine code it generates;
which is a good reason to get in the habit of doing so!)

--------------------------------------------------------------------
### volatile

One method to handle this problem is to mark any shared memory location
as `volatile` which, crudely, tells the compiler this marked location is
magic and can spontaneously change or instantaneously, continuously be
read.  The rough rule for `volatile` in `gcc` is that it will not remove,
add, or reorder loads and stores to volatile locations.  (Even rougher:
`volatile` has whatever semantics  is needed so the the linux kernel
roughly works when compiled with `gcc`.)

Some high level rules from the very useful
[blog](https://blog.regehr.org/archives/28):

 - Use `volatile` only when you can provide a precise technical
   justification.  `volatile` is not a substitute for thought

 - If an aggregate type is `volatile`, the effect is the same as
 making all
   members `volatile`.

 - The C standard is unambiguous that `volatile` side effects must
   not move past sequence points

As a more cynical counter-point from someone who should know:

    "The keywords 'register' 'volatile' and 'const'are recognized
    syntactically but are semantically ignored.  'Volatile' seems to have
    no meaning, so it is hard to tell if ignoring it is a departure from
    the standard.

            Ken Thompson, "A New C Compiler"

#### We almost never use `volatile` in this class.

It's easy to forget to add `volatile` in each place you need it.
Even worse, if you forget, often the code will almost-always work and
only occasionally break.  Tracking down the problem is a nightmare (add
a `printf`?   Problem goes away.  Remove some code?  Same.  Add some
code?  Same.)  

As a result, for this class we only ever read/write device memory
using trivial assembly functions `get32` and `put32` (discussed in lab
`3-gpio`) since this defeats any attempt of current compilers to optimize
the operations they perform.

A contrived example:

        // status holds pointer to a busy device "register"
        uint32_t *busy = (void*)0x12345678;

        ...
        // wait til device not busy.
        while(get32(busy))
            ;


The ARM assembly for these:

        @ put32(addr, val)
        @   store 32-bit value <val> (passed in second argument 
        @   register r1) into memory address <addr> (passed in 
        @   first argument register, r0).  
        @
        @ C code equivalent:
        @   put32(volatile uint32_t *addr, uint32_t val) { 
        @       *addr = val; 
        @   }
        put32:
            str r1,[r0]     @ store r1 into address held in r0
            bx lr           @ return

        @ get32(addr)
        @ load value of <addr> passed in first argument register (r0).
        @
        @ C code equivalant:
        @   uint32_t get32(volatile uint32_t *addr) { 
        @       return *addr; 
        @   }
        get32:
            ldr r0,[r0]     @ load address held in r0 into r0
            bx lr           @ return


Now, while we will usually take the safe/simple route above, writing
snippets of code that use `volatile` and seeing how the compiler does
stuff you don't want is a great, concrete way to see the gap between
the fake abstraction of a programming language and what the machine
actually runs.  (One of the goals of this class is to frequently have you
rip back the lies of abstractions and see something closer to reality.)

--------------------------------------------------------------------
### What to do now

We have only covered a sliver of the issues.   In the interests of
time, rather than have a lot more prose, we just throw a bunch of 
examples at the problem.  What to do now:

  - Go through the volatile examples in the `examples-volatile/` and
    figure out (1) what the problem is and (2) how to fix it.

  - Get a feel for compiler observation games, by going through the
    `pointer` directory examples.  Each dereference of a pointer is
    a observation --- the compiler can only optimize if it's sure no
    pointer dereference can catch it.
    
  - Rewrite Makefiles in examples-volatile and examples-pointer to use
    wildcards.

Useful volatile reading:
  - [Wikipedia volatile]
    (https://en.wikipedia.org/wiki/Volatile_(computer_programming))
  - [How gcc treats volatile](https://gcc.gnu.org/onlinedocs/gcc/Volatiles.html).
  - Linux's [case against volatile]:
    (https://github.com/spotify/linux/blob/master/Documentation/volatile-considered-harmful.txt)

  - [Long thread on volatile and threads]
    (https://groups.google.com/g/comp.lang.c++.moderated/c/_O9XxTmkLvU).
  - Another [against volatile](https://sites.google.com/site/kjellhedstrom2/stay-away-from-volatile-in-threaded-code)
  - [Linux memory barriers](https://www.kernel.org/doc/Documentation/memory-barriers.txt).
