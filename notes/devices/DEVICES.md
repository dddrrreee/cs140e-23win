## Crash course: devices and datasheets (using GPIO as an example)

***NOTE: still editing: send any feedback***

This note gives a give a crash course in thinking about devices and
their datasheets.  It's both incomplete and yet somewhat repetitive with
both itself and the [GPIO](./GPIO.md) writeup.  But hopefully better
than nothing.

Table of Contents:
- [Crash course: Devices](#crash-course)
  - [What is a device driver](#device-driver-whats-that)
  - [The first rule of devices: errata](#errata-everything-is-broken)
  - [Expectations and heuristics for datasheets](#expectations-and-heuristics-for-datasheets)
  - [Device access = remote procedure calls](#device-accesses--remote-procedure-calls)
  - [Device configuration is not instantaneous](#device-configuration-is-not-instantaneous)
  - [Interrupts versus polling for device events](#interrupts-versus-polling-for-device-events).
  - [Device memory + compiler optimization = bugs](#device-memory--compiler-optimization--bugs)
  - [Some concrete rules for writing device code](#some-concrete-rules-for-writing-device-code)

------------------------------------------------------------
### Device driver: What's that.

Each hardware device an operating system supports requires a "device
driver" that can configure and use the device.  Since there are thousands
of devices, there are thousands of drivers.  Because of device numbers
and device complexity, about 90% of Linux or BSD is device drivers.

 - If you have a few minutes you can verify these claims yourself:
   download a Linux kernel tar-ball, unpack it, and look in the `drivers/`
   directory --- you'll see thousands of subdirectories containing many
   millions of lines of code.

However, weirdly, not much gets written about driver code other than to
sneer at its high number of bugs or low elegance.  While you can find
tens of thousands of research papers and hundreds of book chapters on
"core" operating system topics such as virtual memory, processes or file
systems, comparatively little gets written about devices despite them
making up the monster proportion of the code.

Partly this is because devices are necessarily ad hoc.  Among other
things, this makes it hard to distill them down to general principles.
Partly it is because the status of hacking on the core kernel is higher;
drivers are where you pawn off a newbie.

In any case, for this class: alot of the action is in devices.  A big win
of writing a small system from scratch on a hardware platform like the
r/pi is that we can easily connect devices and do things with them that
a general-purpose system such as your laptop can't.  Especially true
if the device has tight real time requirements.  For example: Try to
plug accelerometer into your laptop, or get consistent small nanosecond
accurate timings on MacOS (hahaha, sorry, that's mean), or build any of
the wearable devices you might buy, etc.

Hopefully by the By the end class you'll have the superpower that if you
see some cool new device, you can quickly grab its datasheet, code up a
driver for it without panic or much fuss, and then be able to exploit
whatever interesting power it gives.  This ability will also make it
easy for you to have an interesting idea and then back solve for how to
build it by looking at the the massive catalogue of devices out there,
secure in the knowledge that you can write any code you need.

Partly you'll develop this superpower because after you get through about
N datasheets and architecture manual chapters the N+1th just isn't that
bad. (I'd say N=10 is a reasonable guess, but it could take fewer.)
Partly it is also because there are general patterns you can exploit
and get used to; this note will talk about some of them.

So, let me tell you about devices.

--------------------------------------------------------------------
#### Errata: Everything is broken

The first rule of software is that everything is broken.  Since hardware
is just software run through a fab, it has the same rule, too.

Check for datasheet errata! Datasheets often have errors.  The Broadcom
document is no different. Fortunately it has been worked over enough that
it has a [nice errata](https://elinux.org/BCM2835_datasheet_errata).
Sometimes you can figure out bugs from contradictions, sometimes
from errata. One perhaps surprisingly good source is looking at the
Linux kernel code, since Linux devs often have back-channel access
to manufacturers, who are highly incentivized to have Linux work with
their devices.

With that said, if the device doesn't work, the problem is almost
certainly in your code or your understanding.  Did you hook up to the
right power?  Did you off-by-one miscount a pin or use a loose jumper?
Did you mistype an address?  A register name?  Not wait for initialization
to complete?  Write a bunch of code all-at-once and expect it to work?
For some reason, many people immediately blame hardware.  Hardware does
go bad.  But it's a good chance the bug is in your code, just like all
the preceding yesterdays,


--------------------------------------------------------------------
### Expectations and heuristics for datasheets

If expectation management makes happiness, then the key to getting
through datasheets is a clear-eyed view of the ways they often suck,
just expecting it, and doing enough that their surprises become routine
and you can flow through without a lot of drama.

The following comments also generally apply to architecture manuals,
though these often get vetted much more heavily.

Some crude patterns for datasheets:

  - To start off on a positive note: typically 15-20% (or more)
    of a datasheet can be skipped because it deals with packaging
    dimensions, or spends a bunch of time defining a common device
    protocol (e.g., SPI or I2C).  It's a happy feeling to flip past
    these pages; enjoy.

  - When you read a datasheet you mainly want to know "how":  how to
    configure a device to do what you want, how to know when data is
    ready, how to read its data, how to transmit output.

    Unfortunately, most datasheets are written in the wrong voice:
    passively, with pages and pages of "what" and "where" definitions but
    not much on "how".   In a sense they are a dictionary defining the
    device language, whereas you need to know how to speak device
    sentences for configuration and use.

    The most extreme example of non-operational, definitions-only prose
    I recall is the I2C device protocol "chapter" in the HiFive1 RISC-V
    manual: they simply give a table of device addresses with the sentence
    that you should go "read the the I2C spec."

  - Related: fields are generally defined linearly --- e.g., in order
    of their memory address --- rather than grouped operationally.
    so you'll may have to read the datasheet  all the way through and
    try to infer what fields have to be done at same time

  - In addition, since many devices are made overseas, you can have
    the added challenge that the passive voice prose is being remixed by
    a non-English speaker.  It's just the way it is.  Often devices most
    attractive at a cost level have the hardest to understand datasheets.
    (Of course: if you can read the native language of the manufacturer, don't
    forget to check for a natively written datasheet!)

  - While the primary user goal is, definitionally, to use
    the device, weirdly few datasheets actually give examples for how
    to configure and use their device.

    As a result, you may have to spend hours trying to figure out
    something that would only take minutes given a simple example.
    Bizarre.  Clearly they have such examples (otherwise how would
    they test?): so put some simple ones in!  Show a "hello world".
    Show best practices.  Show some common mistakes.

    I'd say the single biggest, easy change that device manufacturers
    could make is to put examples.  IMO, it should be an ANSI standard
    requirement.

    In any case: known problem.  It's just the way it is.   One of
    your skills will be to get used to taking a bunch of "what" prose,
    and transmuting it into a working "how".  Part of this will be
    inferring implicit rules for how to combine or sequence multiple
    device actions.  Do you have to write A before B?  Do you have to
    clear X before configuring the device?

    On the plus side, a bad datasheet is a moat keeping your competition
    out and (hopefully) this class makes you able to swim, fast.

  - To contradict the previous points: some of the better manufacturers
    will put out examples and suggestions.  However, these are often
    in something often called an "Application Note" rather than the
    datasheet itself.

    Mechanical suggestion: if you have a device X, search "application
    note X".  Likely useful.

  - Datasheets and hardware manuals often have crucial restrictions
    or important rules buried in the middle of a chapter, in the middle of
    a bunch of other prose.  You have to expect this plot device and get
    in the habit of reading everything carefully.  If you device doesn't
    work: there are worse algorithms than carefully re-reading everything.

    A good example of such a "buried lede" is from our upcoming "debug
    hardware" lab where we use the machine's single stepping execution
    method to validate code correctness.

    If you look at the bottom of page 13-16 in the ARM1176 architecture
    manual (the middle of the debug hardware configuration chapter),
    you see this major rule for single stepping:

      - "[if a] mismatch occurs while the processor is running in a
          privileged mode ... it is ignored"

    I.e., if you try to single-step in privileged mode rather than
    user-mode, nothing happens.  The preceding chapter 13 pages have
    a bunch of prose on configuring the debug hardware --- nowhere do
    they imply processor state matters.  It's very easy to miss this
    unflagged sentence and try to do single stepping of kernel code,
    have it not work, and not be able to figure out what is going on. (I
    wasted some hours.)

    Again, major restriction buried in prose with no fore-shadowing.

  - More for architecture manuals:  just because something is in there
    and your CPU chip claims that it implements that version, it does
    not mean you have that functionally!

    An imperfect safety net for chips: scan the manual for architecture
    feature registers that you can read to get the list of features the
    specific implementation provides.  They may be able to drop large
    features you didn't realize were optional.

    One example from this class: if you read the arm1176 manual it has a
    bunch of different performance counters.  An interesting trick these
    counters give you is that you can set them to an arbitrary small
    constant, they will count down on each event (cache miss, cycle)
    and when the counter reaches zero, throw an exception.  However,
    even though the manual specifies how to do this and configuration
    appears to work and both the r/pi A+ and pi zero provide working
    counters, neither has this exception connected --- if a counter hits
    0, nothing happens.  Not mentioned in the document; we had to find
    it discussed in some forum posts.

    Second example: there is a whole chapter in the arm1176 on how to put
    code and data in "fast memory" (sort of like pinning in a cache).
    But, as with counter exceptions, the arm1176 chip used for the A+
    and zero silently does not provide this feature.  Making matters
    worse, when you "initialize" the functionality the configuration
    registers do change, and kind-of look right so when things don't
    work you think you misunderstood the manual.  I wasted a couple of
    days on this for the first 240lx offering.  The only place I found
    any comment was an old forum post.  (But in this case, it's possible
    I missed something in the manual --- but that is a lesson too!)

----------------------------------------------------------------
#### Device accesses = remote procedure calls.

While the device operations are initiated using memory loads and stores,
what actually occurs "behind the scenes" is much closer to an arbitrarily
complex remote procedure calls where:

 1. The network is the I/O or memory bus the CPU and device use to
    talk to each other.

 2. The register source value being stored "to memory" is
    in fact a message that is sent to the device.  This message
    can trigger complicated and/or slow device actions, especially
    for configuration  and, thus, take much longer than a 
    true memory store.  One way to see this is to use a cycle   
    counter to measure the cost of different device operations.

 3. The destination value loaded "from memory" is a message
    sent from the device to the CPU.     If this load was part of a
    "send-reply" pattern where the code first stored and then waited
    for a result, this store-load pair can take an arbitrary long
    time to complete.

##### Device memory ordering problems.

One consequence of the fact that device memory accesses are fake is
that when you are doing loads and stores to multiple Broadcom devices,
you can get weird ordering bugs.

   - Again, as discussed above, both loads and stores to device memory
     can take much longer than true memory accesses, since they are closer
     to arbitrarily complex procedure calls rather than memory accesses.

   - In addition, while your CPU has machinery to enforce an order on true
     memory accesses (this order is called a "memory consistency model"),
     device accesses may happen partially or entirely outside of its
     control.

For the Broadcom devices on our r/pi system:

  1. Reading or writing  to a single device works as you would naively
     expect it to --- the Broadcom chip (which controls all devices)
     forces loads and stores to the device to complete in the order they
     were issued ("sequential consistency") irrespective of their cost.

  2. However, the Broadcom chip does not enforce an order for
     memory operations *between multiple devices*.

  3. The official Broadcom datasheet rule: after accessing one device
     (via a load or store), you must manually insert a *hardware memory
     barrier instruction* before you can access a different device.

     A hardware memory barrier (over-simplifying) guarantees all previous
     loads and stores have completed before execution goes beyond the
     barrier and no memory operations below the barrier can propagate
     above it (e.g., from out-of-order execution).

     But, to repeat: a sequence of accesses to the same device do not
     need memory barriers.

One easy one to understand ordering bug: if you have to enable device
A before you can turn on device B, then just because your final store
instruction to set A's "enable" field was "completed" from the point
of view of the CPU, which has started running the instructions after:
***this does not mean device A is up and running!***  This issue comes up
in the UART lab, where the Broadcom document states you have to enable the
AUX device before turning on UART.  

Note: even when two devices do not have this type of sequencing rule the
Broadcom document states that any set of reads and writes to different
devices must be separated by memory barriers.  It appears (not sure)
that this is partly due to the memory values not being tagged by their
associated address, and so if they take different times, the values
can get reordered but the CPU cannot tell.  Thus, for this class: we
require you use a memory barrier (`dev_barrier()`) before and possibly
after accessing a device.

Note:

  - the hardware barrier should remind you of `gcc`'s compiler barrier,
    which has the same semantics for language level memory accesses.
    But, note: one is not a superset of the other.  It's worth thinking
    about why!

----------------------------------------------------------------
#### Device configuration is not instantaneous

Device enable can be extremely expensive.  Complex devices (such as
gyroscope, accelerometer, network transceiver) are not going to be
instantaneously active.  It's not uncommon for such devices to need 10
or more milliseconds after "enable" to be in a legal state (million of
cycles on the pi).

Identically to memory ordering, just because your final store instruction
to set the device's enable field was "completed" from the point of view
of the CPU, that doesn't mean the device is good to go.  A too-soon
subsequent read or write may get (or even cause) undefined results.

While datasheets generally state how long initialization takes this
information may be buried deep in the center of the PDF or the wording
kind of weird.  In any case, for any complex device, it's not going to
be instantaneously active: you'll want to do a close read looking for
any table in the datasheet that says how long to wait.  Like device
examples I'd say this should be collected on the first few pages.

Similarly, if you're looking at device code and you don't see some kind
of delay after initialization, the code is likely broken.  If you do
see a delay, but it doesn't have a specific comment or datasheet page
number, I'd say it's also likely broken.  E.g., people flying blind may
just stick in a 30ms delay or so "just in case."

----------------------------------------------------------------
#### Interrupts versus polling for device events

When the device has a result or performed an action: how do you know
about it?  You can either poll (explicitly check) or use interrupts (so
the device signals the CPU, which then jumps to an interrupt routine).
We discuss common patterns below.

For some devices, device-to-CPU messages can be triggered because of
some external event, rather than being a reply to a request explicitly
sent by code running on the CPU.  The correctness problem is that if
the driver code does not read the event soon enough, a second event
will either (silently) overwrite it or get discarded (e.g., microphone,
gyroscope or accelerometer readings).

As a partial solution, some devices provide a FIFO queue to buffer
results.  This helps, but doesn't solve the issue: FIFOs are finite,
so waiting too long still leads to discarded results.  This problem is
not some fake thing I'm making up; you'll see the issue pretty soon:

  - When you implement your UART code, the UART hardware has a 8-byte
    receive FIFO --- if bytes arrive when this FIFO is full, the hardware
    will silently discard.

  - Similarly the NRF transceiver we use for networking has a 4 message
    receive FIFO: if you wait too long and a fifth message arrives,
    one gets dropped.  

These bugs suck.  They often won't show up when the system is ticking
along slowly or with few nodes or few tasks (e.g., as during testing)
but only sporadically when you go full speed or add more nodes or tasks
and then things will just lock up occasionally.  (Compounding the problem:
"are you sure it's not a hardware bug?")

To avoid dropping device results, the code must either sit in a "fast
enough" loop *polling* for these events (e.g., by checking a status
location over and over: "is there anything?")  or use *interrupts* so
that its device handling code runs soon-enough when there is something
to do.  Polling is simple, but makes it hard to do other things ---
do too many or take too long doing one and you missed your window.
Interrupts make it easier to do other things, but are a great way to
break your system since the interrupt routine can get
invoked on any instruction, begging for race conditions.

In this class we will always build device code using polling first and
only switch to interrupts reluctantly.

It may seem too cute, but I think this is completely isomorphic: You
already understand interrupts versus polling because of your phone.
If you turn off the ringer, you'll have to manually go and repeatedly
check if a message arrived.  The more time-critical the message is (e.g.,
if you have to reply before the sender gets mad) the more frequently you
have to check.  On the other hand, you can instead turn on the ringer so
you get notified (interrupted) when something comes in and just go work on
other things.  You also see issues with context switching.  Similarly with
buzzers on washer or dryer machines, doorbells, your mom yelling.

As a postscript: For reasons I don't quite understand a lot of people
think you have to immediately use interrupts if you do a device.  This
is false.  You can always poll.  And, in fact, polling is the superior
solution in many of the use cases we will see.  E.g., if you are building
a single-purpose device, rather than a general purpose OS, you can sit
in a loop pretty easily checking all the different devices you have to
see if something interesting occurred.  A polling loop makes it feasible
to much more thoroughly test your system --- maybe even to the point
that you are surprised if it breaks.  However, if you add interrupts,
I'd say you now almost-certainly made you system impossible-to-test ---
the interrupt routine can run at any time, 
exponentially blowing up your code's state space.  You can't exhaust
this space with brute force testing.  (There are other tricks you can
play, but operating systems people almost never know about them; we will
do a few in cs240LX but that doesn't help us now.)

----------------------------------------------------------------
#### Device memory + compiler optimization = bugs

As discussed in the [COMPILE](../1-compile/volatile/README.md) note:
The C compiler does not know about hardware devices nor does it know
that device memory is "special" and can spontaneously change without
visible any store in the program text. Thus, the compiler can (and
often will) optimize pointers to device memory in a way that breaks
the intent of your code. For example, assume `status` is a pointer
to a device status register and we want to spin until it holds the
value `1` before using the device:

        unsigned *status = (void*)0x12345678;
        ...
        while(*status != 1)
            ;

The compiler will look at the loop, see that there is no write to
`status` and potentially rewrite it internally as code that looks
like:

        if(*status != 1) {
            // infinite loop
            while(1)
                ;
        }

This is very different than what you intended. As discussed in
[COMPILE](../1-compile/volatile/README.md) the traditional method OSes
have used for this problem is to mark device pointers with the `volatile`
type qualifier, which (roughly) tells the compiler the pointed to location
can spontaneously change and thus the compiler should not add or remove
any `volatile` load or store or reorder it with either its own accesses
or with those of other `volatile` loads or stores (regular, non-volatile
access have no guarantees).

----------------------------------------------------------------
#### Some concrete rules for writing device code

Heuristics for writing device code, not necessarily in order:

  - For external devices: ***Before you connect anything figure
    out the device input and output voltage***.  This generally is in
    the first table or set of bullets in the datasheet.  Always make
    sure you get this right!

    For the r/pi: obviously, do not connect a 3v device to a 5v pi pin!
    But, also don't connect a 5v device to a 3v pi pin.  If you're
    lucky this will cause the device to do nothing (e.g., all device
    reads return `0`). But it might cause it to almost work -- sort-of
    working stuff is hard to debug since it causes Heisenbugs, error
    that come and go non-deterministically.

    Similarly: Never connect a device directly to a pi input pin if it's
    not putting out around 3v.  Higher and the r/pi pins will shut down
    (or the pi will fry); lower, the pi will not register it.

  - Do a quick internet search for the device name and "Application Note"
    (for examples and suggestions) and "errata" (for bugs).

    I'd also suggest redoing it by adding "forum" or "bare metal" to pull
    up more obscure issues --- especially for hardware restrictions not
    mentioned in any official document.

  - When you set values, you have to determine if you must preserve
    old values (so must read-modify-write) or can ignore them and just
    set what you want (the GPIO `SET` and `CLR` registers).

  - Check your work: When you write a value to the device for a field
    that is both read and write, read it back and make sure that that
    you wrote is what you get back (note: some device registers will
    have bits that can change spontaneously).  You can add this check
    to your device write procedure so it always occurs.

    Trivial, but this hack has detected many bugs for me over the years.
    (E.g., when the SPI protocol speed was wrong, when a jumper was
    loose.)  If nothing else, it increases your confidence.

  - Find the values that must be true on reset.  Read the device after
    reboot and see that these are what you get.  If you don't the
    device could be broken (it happens, especially these days with
    so many counterfeits).  Or you code could be broken --- e.g., the
    addresses you use.

  - If things don't work: it's likely to be your code.  However, hardware
    does go bad.

    Buy at least two devices: if the first doesn't work, try swapping in
    the second.  Also try switching pi's --- sometimes pins get fried,
    or jumps are too loose.

  - Avoid dumb counting mistakes by picking GPIO pins to use
    where you simply cannot miscount.  E.g., use the lowest ground pin on
    the left side (all the way on the bottom) or for GPIO pin 21 (all the
    way on the bottom right) since you can't miscount either.  Every year
    a non-trivial number of people get burned by mis-connecting.

  - For devices that have multi-step configurations, usually you will
    set the device to an "on" but disabled state when you (re-)configure
    it.  Otherwise, it may start sending garbage out the world when you
    are halfway done with configuration.

  - Before access a built-in Broadcom device (GPIO, UART, SPI, I2C
    etc), make sure you issue a hardware memory barrier (for us: call
    `dev_barrier()`).

    Similarly, you have to use a memory barrier when you write a routine
    that accesses a device, but cannot *guarantee* all callers of the
    routine either were using the same device or issued a memory barrier.

    A big example of this: interrupt handlers, where likely have to do
    a device barrier before and after.


  - If you are doing networking: in our experience, sending is pretty
    robust, but receiving --- where an antennae has to cleanly decode
    signals --- can be very sensitive to dirty power (e.g., from your
    laptop USB) and cause receive to fail.  This can be hard to figure
    out since the fact send "works" means you will assume there is no
    hardware problem.

  - If there are FIFO queues you almost certainly want to clear them so
    that after a re-config you don't send or receive garbage.  An easy
    race condition: enable the device and then clear the FIFOs ---
    a message from a previous session could arrive.  You want to clear
    with the device disabled.

  - A minor point but I have made this mistake: a follow-on device
    that adds functionality to its predecessor may only be denoted with
    with a "+" rather than a big red "version 2" or a different device
    number.  If you're careless it's easy to buy the older version (e.g.,
    because its getting liquidated at good prices) but pick up the later
    datasheet and spend a ton of time writing code to use functionality
    that simply does not exist on the device you have.


Code suggestions:

  - When you write the code, add the datasheet page numbers for
    why you did things.  You won't remember.  Plus, it let's someone
    (like a TA) look at your code and see why you did something and if
    it makes sense.

  - If you have two devices that communicate, set up your pi in a
    "loop back" configuration where it can send and receive to itself.
    This has a larger initial cost to setup, but having a single pi that
    can send and receive to itself makes development vastly simpler and
    faster.   Obviously, you don't have to fuss with multiple pi's and the
    doubling of problems that comes from 2x the hardware.  More subtly,
    correctness checking gets simpler: one pi talking to itself always
    knows ground truth and can compare that to what is being communicated.

  - Use `put32` and `get32` to read or write the device locations
    so that the compiler optimization does not blow up your code.

  - Start by using polling to get device results rather than jumping
    right into interrupts.

  - Arguable, but keep in mind: While devices have values they should
    be reset to on restart, it's usually better not to assume them ---
    either check that's the actual value, or set it explicitly (so,
    for example, configure can be done twice with different values).


  - Just because your code ran doesn't mean your code is correct.
    Many device errors are subtle timing issues or not-handling corner
    cases that happen under high load.  Or simply produce results that
    you won't know how to check (is `1724` correct?  I don't know.)

    A piece of code that ran and didn't crash doesn't at all imply
    the device code is correct.  You'll have to read the datasheet
    carefully, read your code carefully, put in assertions, and check
    as many known examples as possible.  (This algorithm also doesn't
    guarantee correctness, but at least it is less stupid.)

