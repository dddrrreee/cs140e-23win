## Prelab for user-level processes.

We now do our third variant on execution: user-level processes.  First,
we did interrupts as a way to handle high-priority events that are
difficult to anticipate.  Second, we then did non-preemptive threads for
managing independent but cooperating executions ("threads of control")
in the same address space.   Finally, we use user-level processes to
manage independent executions that you want to isolate from each other
(so a crash in one does not crash another) and do not trust enough to
run with full kernel privileges.

### Quick and dirty overview

Processes can have a large number of moving parts:
  - all the code from threading.
  - full-fledged system calls.
  - virtual memory.
  - other stuff we'll defer.

Doing a complex, new thing with many moving parts is a tautology for
awful.  Fortunately, we can play some tricks to break it down into much
simpler pieces.
  1. The arm 1176 chip used on the r/pi A+ (and the ARMv6 architecture
     in general) has the very odd feature that you can can run code at
     user-level *without* having virtual memory.  This lets us entirely
     skip one of the most error prone subsystems.
  2. From cs240lx: we will do a powerful and fairly simple hack using
     the r/pi debugging hardware so that we can immediately catch if the
     user-level code deviates by even a single-bit in a single register
     from its expected behavior.  I cannot imagine writing OS code without
     this trick, and no one else knows about it besides us, so this should
     be fun. 

For the lab we will:
  1. Run a single process at user-level (i.e., without kernel privileges),
     but without virtual memory.
  2. Have a simple kernel that just services system calls (no interrupts).
  3. Write simple, but true system calls that go between user and kernel
     space.   (We previously handled the system call exception, but there
     are some additional details to deal with the fact that we do not trust
     the user's stack, etc.)
  4. Use single-stepping hardware to ensure there is absolutely no
     deviation for each change we make.  (More in the lab).
  5. Extend the system to two processes.

### Big picture readings.

There's lots of options for what to read.

For the big picture, from the OSTEP book (these are locally cached in
`10-process/docs`):
   - [processes](https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-intro.pdf).
   - [process API](https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-api.pdf)
   - [direct execution](https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-mechanisms.pdf): 
     this is sort of what we'll be doing in lab.

As always, Mazieres' [lecture notes from
CS140](http://www.scs.stanford.edu/20wi-cs140/notes/) are a great,
albeit dense resources.  You should look through all of them 
for good discussion.   Specific high-lights for today:
  - [processes and threads](http://www.scs.stanford.edu/20wi-cs140/notes/processes.pdf).
  - [scheduling](http://www.scs.stanford.edu/20wi-cs140/notes/scheduling.pdf) -- we
    are not going to do any real scheduling, but it's good to see what is not happening.

### A brief general note: revist labs!

A positive of the labs is that they are highly concrete.  A downside is
that it is easy to lose the big picture, and just focus on getting the
next fetchquest object.  In general, you are strongly, strongly encouraged
go back through the labs a few days later and reread both the description
and the code (and, ideally, the tests --- how could you test better?)

The threads lab from last time is good example.  Not all that much code,
but some of it can be fairly subtle.  You are strongly encouraged to
print out the code (`rpi-thread.h`, `rpi-thread.c`, and `thread-asm.S`)
and look through it all, again, making sure you understand every line ---
why are we doing what we are doing?  Is it correct?  Can you check it?
Is there another way to do it?

Also, to help re-affix the concepts, it can be worth rereading the general
overview from the OSTEP book:

  - [threads intro](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)
  - [threads API](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf)

Also, to go further on the last lab:
   - [concurrency](http://www.scs.stanford.edu/20wi-cs140/notes/concurrency.pdf).
   - [synchronization I](http://www.scs.stanford.edu/20wi-cs140/notes/synchronization1.pdf)
   - [synchronization II](http://www.scs.stanford.edu/20wi-cs140/notes/synchronization2.pdf)

In general, the more you understand each concept from each lab, the
more you can synthesize into your own world view, and the firmer the
ground you'll have to stand on for the rest of the course and beyond.
Most people do not understand how computation really works; it's hard
to imagine your grasp of it not paying off in compounding ways.
