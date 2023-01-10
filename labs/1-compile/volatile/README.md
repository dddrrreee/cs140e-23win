Today we look at some quick examples to give a feel for how hardware-level
(operating system) programming is different from at application level.

Today is the only non-lab class. 
   1. We give a broad, high-level view of the course: `intro.pdf`.
   2. We try to give a feel for the type of thinking in the class by
      doing a narrow deep-ish dive into the issue of how a compiler
      can cause extreme grief by optimizing code that controls hardware.

### The "As-if" substitution principle.

Bishop Berkeley is the patron saint of computer stuff.  If a tree falls in 
the forest and no one is there to hear it:

   - Then it could be stored in a register.
   - Or it could be cached
   - Or could be skipped.
   - It could fall out of order with other trees since you can't tell.
   - Or rendering could be deferred until someone walks by.

(A famous equivalance-substitution example is the Turing test.  When I
grew up, this was a snickering, "irrelevant" thing to worry about.  Now,
it's going to be interesting to see where the new gen of AI behaves in
an observably equivalent way to humans and can put them out of a job
and on the dole.)

Over-simplifying, the as-if substitution principle is: 

   - Given a program P, if program P_better has the same observable
   behavior ("side-effects") as P does --- i.e., it behaves "as if"
   it was P ---  then we say they are equivalent and, for example,
   we can replace P with P_better.

   Equivalance substitution can apply at any level: a single line of
   code (replacing an expensive multiply with a shift for power-of-two
   multiplicands), a procedure (e.g., linear sort with quick sort) or
   even an entire system.    Presumably you have have relied on a rough
   notion of equivalance substitution multiple times where you replace
   an old laptop you can replace an old laptop with a new, faster one
   or an old version of an OS with a newer one, or setup dual booting.

Equivalance substitution is a core systems principle.  We will use it,
exploit it, and sometimes be burned by it this quarter.  It shows up at
every level of the computer systems.  It's the only reasons things work
at all at any kind of speed.

Weirdly, despite being ubiquitous, it's rarely talked about explicitly.

### Some general rules for equivalance  substitution


When you write code, you likely reason about how it behaves by looking
at the source code at the source code (loops, variables, function
calls, etc) and pushing the side-effects of these operations forwards
and backwards.  However, obviously, hardware can't run C/C++/Rust
source directly.  Thus the compiler translates your source code down
to low-level operations that --- aspirationally --- will behave "as if"
it was the original (sort of: only if your program was well-defined).

As part of this translation, compilers aggressively optimize code
by replacing possibly large chunks with puportedly faster or smaller
equivalants that behave "as if" they were the original.  The compiler
will frequently reorder operations or delete them
(dead code removal) if it believes an external
observer could not tell.  A very small set of examples:

   - inlining.
   - common sub-expression elimination
   - code hoisting.
   - code re-ordering
   - register promotion
   - reorder reads and writes (this is one that really burns us).
   - redundant read elimination (this also burns us)

The key issue for the compiler's game is what an "observer" is.  If the
observer can tell the difference between P and P', then they are not
equivalant. 

If you think about it, there is a tension between how much the observer
sees and:
  1. How easy it is for us to reason about the code (since the observations 
     are what we are allowed to rely on.
  2. How much the compiler or system can substitute.

If we define every single operation as occuring in the order its written
(sequential consistency) exactly as written, this is is easiest for us
as programmers to reason about.  What does the code do?  Exactly what
it says, in exactly the order it's written, with no changes.  What you
see is what you get.

Of course, an all-seeing, all-knowing observer with complete information
(such as your CPU, a debugger, or even a disassembler) can see exactly
how code differs --- if we had to satisfy the equivalency judgement of
these observers, we could never optimize anything, because they could
detect any change.  For example, if you replaced a multiply with a shift
or reordered two stores, any disassembler or debugger could see this.

The game we play is to --- perhaps counte-intuitively --- try to make
the observer as weak as possible (while still being useful), since a weak
observer can be fooled the most number of ways.  Of course, the kicker
is "as possible": a human looking at the code must be able to look at
(observe) the program and reason about what it does.  Humans already
can't do simple, reliably --- adding non-deterministic behavior (e.g.,
look up "memory models") can easily make correctnes hopeless.

In the big world, the rough place many systems settle on is that they
ignore "how" a result is computed, and instead only require that external
actions ---- e.g., network messages, disk writes, output statements,
exceptions, or launching a rocket --- happen in the order they are
written, without duplication or eliminating (not always).  As long as
these observations remain the same, they treat code itself as a black
box and (very roughly speaking) do not concern themselves with what
actual CPU operations performed.  An entire program could be replaced
with a lookup table if that gives equivalant results, or even deleted
entirely if it has no output.

### How as-if lets the compiler wtf-you.

To bring our discussion back to the concrete , low-level of this class:
roughly speaking, the C compiler `gcc` that we will use  assumes that only
the source code you write can affect values, and only do so sequentially.
It does not know about threads, interrupts, or hardware devices.

It tries to preserve side-effect equivalance with a "sequentially
consistent" program where every read returns the value of the last write.

Thus, if we have code that does:

    y = 1                   
    x = 2;

Since there is no read of `y` or `x` in this code --- no observertation
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

Thus, this hardware is both a strict observer and a mutator of these
special locations.  Unfortunately, `gcc` has no idea these locations
are magic and falsely assumes:

  1. The only way to read a location is via a load within the program.

     The compiler does not realize there is a concurrent hardware device
     that can see exactly what is written as soon as the change occurs.

     An easy way to break:

        device->enable_interrupt = 0;
        device->enable_device = 1;

     Given that there are no reads, the compiler could potentially
     reorder this as:

        device->enable_device = 1;
        device->enable_intterrupt = 0;

  2. The only way to change the value of a location is a store 
     within a program.

    The compiler does not realize that that there is an external
    device that can spontenously change the value without any 
    visible prodding.

that there is an external
agent that can see or affect these values, and so its optimizations
can totally destroy the intended semantics.   (You cannot detect such
problems without looking at the machine code it generates; which is a
good reason to get in the habit of doing so!)

One method to handle this problem is to mark any shared memory location as
`volatile`.  The rough rule for `gcc` is that it will not remove, add,
or reorder loads and stores to volatile locations.  

A bit more precisely, from the very useful [blog](https://blog.regehr.org/archives/28):

 - Use `volatile` only when you can provide a precise technical
 justification.  `volatile` is not a substitute for thought

 - If an aggregate type is `volatile`, the effect is the same as making all
 members `volatile`.

 - The C standard is unambiguous that `volatile` side effects must
 not move past sequence points

As a more cynical counter-point from someone who should know:

    "The keywords 'register' 'volatile' and 'const'are recognized
    syntactically but are semantically ignored.  'Volatile' seems to have
    no meaning, so it is hard to tell if ignoring it is a departure from
    the standard.

            Ken Thompson, "A New C Compiler"

It's easy to forget to add `volatile` in each place you need it.
Even worse, if you forget, often the code will almost-always work and
only occasionally break.  Tracking down the problem is a nightmare (add
a `printf`?   Problem goes away.  Remove some code?  Same.  Add some
code?  Same.)  As a result, for this class we only ever read/write
device memory using trivial assembly functions `get32` and `put32`
(disussed in lab `1-gpio`) since this defeats any attempt of current
compilers to opimize the operations they perform.   


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

### Examples

For today we will:
  - Go through the examples in the `volatile/` directory.

  - Get a feel for compiler observation games, by going through the 
    `pointer` directory examples.  Each dereference of a pointer is a
      observation --- the compiler can only optimize if it's sure no pointer
      dereference can catch it.
  - To refresh your view of C, look through the `c-traps` directory.

    It's worth thinking about how you actually reason about what your
    code does.  This likely involves working forwads or backwards
    following a sequence of how/what each storage location (variable,
    heap) is assigned, what is read, and what side-effects occur.
    You are likely informally computing what "happens before" to determine
    what causes what.  Your reasoning is likely sequential and tries to
    ignore stuff that is not (hopefully) not relevant, such as the 100+
    other programs executing on your computer when this one is.
