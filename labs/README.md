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


Labs usually have some notes about key concepts --- you must read
these before the lab or you'll be lost.   We also suggest re-reading
some number of labs later after the concepts have sat for a bit
so you get a deeper view.  Note summary (so far):
  - [COMPILATION](1-compile/volatile/README.md)
  - [GPIO](3-gpio/GPIO.md)
  - [DEVICES](3-gpio/DEVICES.md)

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

    ***READING***: our note on [COMPILATION](1-compile/volatile/README.md)
    which describes how compilers exploit observability to optimize code
    and how that can break your device code.

  - [2-trusting-trust](2-trusting-trust): Ken Thompson is arguably our patron
    saint of operating systems --- brilliant, with a gift for simple code that
    did powerful things.   We will reimplement a simplified version of a crazy
    hack he described in his Turing award lecture that let him log into any
    Unix system in a way hidden even from careful code inspection.

---------------------------------------------------------------------
### Part 1: Going down to metal.

The first few labs will writing the low-level code needed to run the
r/pi and using modern techniques to validate it.  Doing so will remove
magic from what is going on since all of the interesting code on both
the pi and Unix side will be written by you:

***We are here*** ===>

  - [3-gpio](3-gpio/):  Two parts.  First, we will give out the 
    hardware and make sure it works (this should be fast).

    Second start getting used to understanding hardware datasheets by
    writing your own code to control the r/pi `GPIO` pins using the
    Broadcom document GPIO description.  You will use this to implement
    your own blink and a simple network between your r/pi's.

    ***READING***: the notes on [GPIO](3-gpio/GPIO.md) and 
    [DEVICES](3-gpio/DEVICES.md) as well as pages 4--7 and 91---96 
    of the broadcom datasheet (`docs/BCM2835-ARM-Peripherals.annot.PDF`)

  - ***cross-check***: you will use read-write logging
    of all loads and stores to device memory to verify that your GPIO
    code is equivalent to everyone else's.  If one person got the code
    right, everyone will have it right.

    A key part of this class is having you write all the low-level,
    fundamental code your OS will need.  The good thing about this
    approach is that there is no magic.  A bad thing is that a single
    mistake can make a miserable quarter.  Thus, we show you modern
    (or new) tricks for checking code correctness.
  
  - ***bootloader***: two of the biggest pieces of code
    we've given you have been the Unix-side and pi-side bootloader code
    (`pi-install` and `bootloader.bin` respectively).  So this lab has
    you implement your own and get rid of ours.

    The most common r/pi bootloader out there uses the `xmodem` protocol.
    This approach is overly complicated.  You will do a much simpler,
    more-likely-to-be-correct protocol.  It has the advantage that it
    will later make it easier to boot over the network.

  - ***uart***: the last key bit of code we've given you is for
    controlling the UART (`uart.o`); so this lab has you write your own
    and remove ours.

    To do so you'll write your first real device driver --- for the UART
    --- using only the Broadcom document.  At this point, all key code
    on the pi is written by you.  You will use the cross checking code
    from lab 2 to verify your implementation matches everyone else's.

---------------------------------------------------------------------
### Part 2: execution: threads, interrupts, exceptions, processes

Execution comes in many forms.  It can be a tricky topic both
because of its thickets of low-level, hardware specific details and
because of how hard mistakes are to debug.  You will build and use four
different versions of execution (threads, interrupts, exceptions and
processes) so that you understand in a depth-perceptive way what is
essential and what is incidental.  We will also do multiple new tricks
using these that work well at finding bugs in novel ways.

After four runs at the architecture manual, you will have a new
comfort with using it.

  - ***threads***: we build a simple, but functional
    threads package.  You will write the code for non-preemptive context
    switching:  Most people don't understand such things so, once again,
    you'll leave lab knowing something many do not.

  - ***interrupts***:
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

  - ***device-interrupts***:
    if you keep doing this kind of work the single most common fancy
    "OS" type thing you'll likely do in the future is to setup GPIO
    pin interrupts so that you can get notified when a hardware device
    has data.  So, in this lab we'll setup GPIO interrupts and tune how
    we do general interrupts.

    Without interrupts, it's difficult to get networking working, since
    our GPIO pins (and our UART options) have limited space and, thus,
    unless our code checks them at exactly the right time, incoming
    messages will vaporize.

  - ***exceptions to catch errors***:
    This lab will give you exposure to exceptions by showing how you can
    abuse debugging hardware in a new way to catch memory corruption
    (such as null pointer bugs) *without virtual memory*.

    As you know too-well by now, a challenge of bare-metal programming
    is that we have not had protection against memory corruption.
    By this point in the quarter, I believe everyone in the class has
    had to waste a bunch of time figuring out what was corrupting some
    random location in their program.  After today's lab you should be
    able to detect such corruption quickly.

  - ***processes + equiv***:
    we will give you a few hundred line, trivial OS --- system calls,
    no virtual memory or file systems --- and you will add user processes
    to it.  Being able to stare at a tiny, complete system will give you
    a concrete place to stand and understand what is going on in an OS.

    In addition: in order to make it so you are surprised if your OS
    code is broken --- you'll implement a novel, wildly useful trick
    for ruthlessly detecting subtle operating system mistakes using the
    debug hardware.  It will prove invaluable next week when we do virtual
    memory, a topic known for its extremely hard-to-track-down bugs.

---------------------------------------------------------------------
### Part 3: virtual memory

We now build virtual memory which will let us add (1) general memory
protection and (2) user processes that can safely run with their own
private address space.

Conceptually, virtual memory is simple: it just involves building an
integer function (usually using a table lookup; see: "page table") that
can replaces one integer (a virtual address) with another (a physical
address).  However, since this replacement occurs on every memory access,
it must be fast.  This need for speed makes modern hardware for virtual
memory wildly complicated.

As a result, this topic will require the most reading --- about 100
pages of the ARMv6 manual.  (You should start now.)  There's just no way
around this complexity.  But, after you understand how to build virtual
memory there's nothing more complicated, so anything else should be
relatively easy.

  - ***pinned virtual memory***: you will build a simple virtual memory
    system from ARM documents.   The interesting thing about this will
    be how little code is required.  You will exploit a neat, novel
    hack that lets you implement virtual memory without page tables,
    removing a major source of complexity.    (Your system will be a
    few hundred lines of code, which is not the typical size.)

    You also add exception handling to catch protection and
    missing translation faults.   You will be able to use this add
    both user-level and kernel-level protection.

  - ***page table virtual memory***: you will add page tables to
    your previous system, making it fully general.  After both labs you
    should have a strong grasp of what, exactly, is going on.

  - ***virtual memory coherence***: The previous lab defined the main
    noun in the virtual memory universe
    (the page table); this lab does the main verbs used to set up the
    VM hardware, including how to synchronize hardware, translation,
    and page table state (more subtle than it sounds).  At the end you
    should be able to delete all our starter code.

---------------------------------------------------------------------
### Part 4: file systems

So far, if you do anything useful on your pi, this work evaporates when
you unplug it.  We will implement files and directories so that you can
make data persistent.  (In a sense, networking sends data across distance
and files/directories/memory sends it across time.)

This code will likely be the most  substantial of the quarter.  

On the plus side: from a purely mercenary angle, if you do an embedded
startup, you'll need a FAT32 file system that has no license restrictions
and these labs will get you much of the way there.

  - ***fuse***:
    In this lab you will use the FUSE file system to wrap up your pi
    as a special file system and mount it on your laptop, where you can
    use standard utilities (and your normal shell) to interact with it.
    You can then control the pi by reading / writing to special files:
    e.g., echoing a `1` to `pi/reboot` to cause a reboot, echoing a
    program to `/pi/run` to run it.

    This lab is a great example of the power of Unix's simple, powerful
    OO-interface that lets you package a variety of disparate things as
    files, directories, links and interact with them using a uniform set
    of verbs (e.g., `open()-read()-write()-close()`).

  - ***fat32 read-only***: The SD and virtual memory are the biggest
    unknowns in our universe, so we'll bang out quick versions of each,
    and then circle back around and make your system more real.

    You will write a simple read-only FAT32 file system that can read from
    your SD card. You will use this to do a very OS-style "hello world":
    use your FAT32 to read `hello-fixed.bin` from last lab from your SD
    card, jump to it and run it.

  - ***fat32 write***: you will extend your FAT32 file system to add
    writes.  You'll use a novel model-checking trick to ensure that no
    write will corrupt the system, no matter where it crashes.

---------------------------------------------------------------------
### Part 5: Networking

Having one pi that can only talk to itself is much less useful even two
pi's that can talk to each other.   So we'll do some simple networking.

   - ***nrf transmit/receive***: your system gets much more useful
     if it can control computation remotely.  To this end we write a
     simple bi-directional network implementation using the common,
     cheap NRF24L01+ RF trancievers.   As an extension you will build
     a network bootloader.

---------------------------------------------------------------------
### Part 6: Putting it all together.

For the last labs we put everything all together:

  - ***complete OS***:
    you will combine user-processes, virtual memory, system calls,
    and threads into a capstone implementation.
