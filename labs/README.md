## The labs

***We are doing a big re-organization compared to last years, so the
labs below are tentative.  There will likely be some fussing about.
Take them as a suggested roadmap***

Below describes where we're going and why.   The labs themselves have a
lot more prose on each topic.  There is a pointer to where we currently
are: the labs after this point can still see significant revisions.

I would strongly recommend reading the labs carefully before starting.
And then, once you finish, going back and reading them for things you
missed the first time, and also looking closely at your code --- for
every line, you should understand why we needed it or what it does.
And on second reading, you'll likely find some lines that we could have
cut out!

---------------------------------------------------------------------
### Part 0: non-pi hacking

Unlike all subsequent labs, our first two don't use hardware.  They should
give a good feel for whether the class works for you without requiring
a hardware investment.

  - [0-intro](0-intro): the intro (non-lab) lecture.

  - [1-compile](1-compile): This short lab focuses on what happens when
    you compile code.  How to automate compilation with `make`.  How to
    see how the compiler translated your C code by examining the machine
    code it produced.  And some some of the subtle rules for how the
    compiler can do this translation.  What we cover will apply to every
    lab this quarter.

We are here ===>

  - [2-trust](2-trusting-trust): Ken Thompson is arguably our patron
    saint of operating systems --- brilliant, with a gift for simple
    code that did powerful things.   We will reimplement a simplified
    version of a crazy hack he described in his Turing award lecture
    that let him log into any Unix system in a way hidden even from
    careful code inspection.


---------------------------------------------------------------------
### Part 1: Getting some experience with hardware

The first few labs will writing the low-level code needed to run the
r/pi and using modern techniques to validate it.  Doing so will remove
magic from what is going on since all of the interesting code on both
the pi and Unix side will be written by you:

  - [3-blink](3-blink/): get everyone up to speed and all
    necessary software installed.  This should be a fast lab.

  - [4-gpio](4-gpio/): start getting used to understanding hardware
    datasheets by writing your own code to control the r/pi `GPIO` pins
    using the Broadcom document GPIO description.  You will use this to
    implement your own blink and a simple network between your r/pi's.

  - [5-cross-check](2-cross-check/): you will use read-write logging
    of all loads and stores to device memory to verify that your GPIO
    code is equivalent to everyone else's.  If one person got the code
    right, everyone will have it right.

    A key part of this class is having you write all the low-level,
    fundamental code your OS will need.  The good thing about this
    approach is that there is no magic.  A bad thing is that a single
    mistake can make a miserable quarter.  Thus, we show you modern
    (or new) tricks for checking code correctness.

---------------------------------------------------------------------
### Part 2: Getting into bare-metal execution

  - [6-interrupts](6-interrupts/):
    you will walk through a simple, self-contained implementation of
    pi interrupts (for timer-interrupts), kicking each line until you
    understand what, how, why.  You will use these to then implement
    a simple system call and a version of `gprof` (Unix statistical
    profiler) in about 30 lines.

    Perhaps the thing I love most about this course is that because we
    write all the code ourselves, we aren't constantly fighting some
    large, lumbering OS that can't get out of its own way.  As a result,
    simple ideas only require simple code.  This lab is a great example:
    a simple idea, about twenty minutes of code, an interesting result.
    If we did on Unix could spend weeks or more fighting various corner
    cases and have a result that is much much much slower and, worse,
    in terms of insight.
