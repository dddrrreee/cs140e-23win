### A sort-of ELI5 crash course on device memory: GPIO

#### Introduction: What is GPIO?

GPIO: [General Purpose Input Output](https://en.wikipedia.org/wiki/General-purpose_input/output).

GPIO is a descriptor for a pin that has no inherent dedicated purpose.
Instead, it can be programmatically toggled between input/output mode,
and high/low signals in order to control some electronic device.
26 out of the 40 RPi pins are GPIO pins, while the others have
assigned purpose (e.g. 5v, ground pins, etc...). We can use the pi
to control these GPIO pins as needed.

#### So _how_ do we use the pi to configure the pins?

(Note: all page numbers refer to the Broadcom BCM2835 document: `/docs/BCM2835-ARM-Peripherals.annot.pdf`. Errata [here](https://elinux.org/BCM2835_datasheet_errata).)

The r/pi, like most processors has a bunch of different devices it can
control. For example: the GPIO pins you use in lab 0 and lab 1. You
can see a bunch of these devices by skimming the index of the Broadcom
document: UART (you'll write a driver for this), I2C, SPI, the SD card
reader, etc. Obviously, to use these devices, the pi must have a way
to communicate with them.

An old-school, obsolete approach for device communication is to have
special assembly instructions for each device and its operations.
(You can see what ARM assembly looks like by looking at any of the `.list`
files our `Makefile`s generate during compilation.) This method sucks,
since each new device needs its own set of instructions: the hardware
designer would have to anticipate all of these or awkwardly back patch
them later (yuck) and these instructions would take up precious space
in the instruction set encodings (more on this in a different note).

Instead, modern systems use the following hack:

1. They give each device its own chunk of the physical address space;
2. Programs control a device by reading or writing its associated memory
   locations using magic, device-specific values. (Example below.)

This method works better than device-specific instructions because:

1. Modern address spaces are big enough you can give a chunk to devices
   without much downside.
2. Encoding device commands as integers (32-bits on the ARM) that the
   CPU does not interpret in any way makes it easy for devices to
   express any semantics you want.

Of course, everything has a cost. A couple of downsides to this approach:

1. The magic values have no obvious meaning, so require looking at a
   (correct!) datasheet. This can make debugging hard. You'll have
   experience with this in the GPIO and UART labs. Comments that give
   page numbers will save you if you ever need to look at months from
   now (or even tomorrow!)

2. As we discuss later, this approach can lead to nasty bugs
   since, obviously, it is a lie and these operations are not truly
   loads or stores.

Great. Lets do an example.

#### How to turn GPIO pin 20 "on"?

For lab 1 you will want to turn pin 20 on and off. Pin 20 is the second pin from the bottom-right, if you orient the r/pi with the pins on the right. To figure out how
to do so we look in the Broadcom document:

1.  On page 95 it states that a write to the ith bit of `GPSET0`
    will set the i-th GPIO pin on (for pins 0 to 31).

2.  Using the table on page 90 we see `GPSET0` is located at address
    `0x7E20001C` (note: a constant prefixed with `0x` means it is
    written in hex notation.)

3.  Finally, just to confuse things, we know after staring at the
    diagram on page 5 that "CPU bus addresses" at `0x7Exx xxxx`
    are mapped to physical addresses at `0x20xx xxxx` on the pi.
    Thus the actual address we use for `GPSET0` is `0x2020001C`
    rather than `0x7E20001C`.

    Note: such ad hoc, "you just have to know" factoids are wildly
    common when dealing with hardware, which is why this is a lab class.
    Otherwise you can get stuck for weeks on some uninteresting fact
    you simply do not know. Hopefully, after this class you operate
    robustly in the face of such nonsense.)

The result of all this investigation is the following sleazy C code:

        *(volatile unsigned *)0x2020001C = (1 << 20);

Which does the following:

1.  Casts the location `0x2020001C` to a `volatile unsigned`
    pointer.

    Why `volatile`? We need to tell the compiler this pointer is
    magic and don't optimize its use away (big, subtle topic: more on
    this later).

    Why `unsigned`? We use `unsigned` because the `GPSET0` location is
    32-bits (Figure 6-8 on page 95) and on the pi `unsigned` is 32-bits.

    Note: `int` (signed integer) is _also_ 32-bits but using signed
    integers when doing bit manipulation can lead to very nasty bugs
    (in general: not just for device memory). In this class: all
    device operations should always use `unsigned` rather than `signed`.

2.  Constructs a 32-bit value that has a `1` as its 20th bit and `0`
    everywhere else: (`1 << 20`).

3.  Assigns the constant (2) to location (1).

In this specific case, the above pointer cast is morally fine, despite
what some people might tell you. However, empirically, it is very easy
to forget a `volatile` type qualifier, which will cause the compiler to
(sometimes!) silently remove and/or reorder reads or writes. In this
class we will _never_ directly read or write device memory, instead
we will call the procedures `get32` and `put32` for `volatile` device
pointers (or the identical `GET32` or `PUT32` for raw integer address
values) to read and write addresses with 32-bit values.

For example, we would rewrite above as:

        // *(volatile unsigned *)0x2020001C = (1 << 20);
        PUT32(0x2020202, (1 << 20));

The call `PUT32(...)` will jump to assembly code in another file (`gcc`
currently cannot optimize this) which writes the value of the second
argument to the address specified by the first argument.

If we already have a named pointer to the location (which can be easier
to track and document) we can use the equivalent `put32`:

        volatile unsigned *gpset0 = (void*)0x2020001C;
        ...
        put32(gpset0, (1 << 20));

In addition to correctness, this method of using `put` and `get`
routines makes it trivial for us to write code that monitors, records,
or intercepts all read and writes to device memory. This trivial
bit of infrastructure makes it easy for you to do a bunch of surprisingly
powerful tricks:

1. Instead of performing them on the local device memory we can
   send them over the network and control one or many remote r/pi's.

2. We can record the reads and writes that are done to device memory
   and then use this for testing, or automatically construct a
   bare-bones program replaces our original and simply replays them.

   Lab 3 uses this trick to sort-of-prove that your code is correct.
   It works by running your code and checking that it does the same
   reads and writes in the same order with the same values as everyone
   else. This makes it easy to show your code is equivalent to everyone
   else's code --- despite the fact that it will look very different.
   If one person is correct, all must be correct.

3. Many many others!

#### Bigger picture: controlling devices

Generally, whenever you need to control a device, you'll do something
like the following:

0. Get the datasheet for the device (i.e., the oft poorly-written PDF that describes it).
1. Figure out which memory locations control which device actions.
2. Figure out what magic values have to be written to cause the device to do something.
3. Figure out when you read from them how to interpret the results.

Fairly common patterns:

1. Devices typically require some kind of initialization, so often
   the first thing you'll do is turn the device on and then do a
   sequence of writes to set the device in a specific configuration.
   In some cases, rather than turning it on, you'll disable it so it
   doesn't interact with the outside world while being setup.

   The tables in the Broadcom give the initial value the device memory
   will have when powered on (the "reset" column).

   This part of the process is often the most complicated, with several
   steps: its made worse because getting any step wrong leads to the
   same result: the device doesn't work.

2. When you're waiting for results there will often be some kind of
   status or "ready" bit you have to check to see that there is data.
   Similarly, before you write data (e.g., to send a character using
   the UART) you will often have to wait for the device to have space.

3. Similarly, for results there is often a way to setup interrupts (we
   will do this in week 3) so that a piece of code runs rather than
   requiring you to spin checking if there is data (or space).

4. When you set values, you have to determine if you must preserve
   old values or can ignore them.

#### Example: How to set GPIO pin 20 to output?

To reiterate: usually you can't just start using devices --- even
one as simple as a GPIO pin --- without first doing some setup.

To make this concrete, we return to our GPIO example. GPIO pins can
have different functions. From Table 6-4 on page 93 defining `GPFSEL2`
(the device "register" that controls pins 20 through 29) you can see
that pin 20 can have 8 different functions (controlled by bits 0-2,
also denoted `FSEL20`): input, output and then a bunch of other opaque
functions we'll get to in later labs.

Looking a bit deeper at the table:

1.  You see that on reset all bits in `GPFSEL2` are set to `000`, which
    from the Table corresponds to every pin being an `input` pin (i.e.,
    we can only read from it, not set it.

2.  If we want to set pin 20 on and off, we need to change pin 20 to
    be an output pin. We do this by writing `001` to `FSEL20` (bits
    0--2) while preserving the original value of all other bits
    in `GPFSEL2` (from bit 3 up to bit 31). We must preserve them
    because in general we won't know what they have been set to.

While this all may sound complicated, and the Broadcom document is not
particularly friendly (sadly: it is better than most!), you will see
that what is actually going on is pretty simple: there is just a bunch
of jargon, a few loads or stores to weird addresses, and with a few
lines of code you can start to control some pretty neat stuff.

In our case, to set bits 0-2: we would:

1. Read the original value of `GPFSEL2` using `GET32`. From table 6-1
   on page 90 we see the address of `GPFSEL2` is given as `0x7E200008`
   which after remapping is: `GET32(0x20200008)`.

2. Since in general, we won't know their initial value, clear bits
   bits 0-2 (so they are 0) using C bit operations.

3. Do a bitwise-or of (2) with `0b001` (the "input" bit pattern).
4. Write (3) back to `GPFSEL2` using `PUT32`.

Something like:

        // a true constant in C: cannot write to.
        enum { GPFSEL2  = 0x20200008 };
        ...
        PUT32(GPFSEL2, bit_clear(GET32(GPFSEL2), 0,2) | 0b001)

To make a more general set of GPIO operations:

1.  Figure out what location to write to set a pin to input or output.
2.  Implement `gpio_write` to write the value of an output pin (LED).
3.  Implement `gpio_read` to read the value of an input pin (touch sensor).

#### Correctness notes

We discuss a few of the more tricky issues below:

- Check for datasheet errata! Datasheets often have errors.
  The Broadcom document is no different. Fortunately it has been
  worked over enough that there is a [nice errata](https://elinux.org/BCM2835_datasheet_errata)
  that you can see --- the newsgroup already had a nice example of this.
  Sometimes you can figure out bugs from contradictions, sometimes
  from errata. One good source is actually looking at the Linux kernel
  since they often have back-channel access to manufacturers, who are
  highly incentivized to have Linux work with their devices.

  The hardest task is to be the first one to boot up a system since any
  such datasheet mistake can really prevent any progress. We'll see
  an example of such a crazy bug when we do the UART device driver.

- While the device operations are initiated using memory loads and
  stores, what actually occurs "behind the scenes" is typically far
  more complex and expensive than a memory operation. (While they
  look like memory operations they are closer to arbitrarily complex
  procedure calls.) Thus, they will take much longer than a normal
  memory operation. Their cost leads to the following hard-to-debug
  problem (I had one that literally took two days a few years back.)

  On many machines, including the pi, when you perform a store
  to a device, the store can return before the device operation
  completes. This hack can give a big speed improvement by, among
  other things, allowing you to pipeline operations to the same device.
  However, it also can lead to subtle ordering mistakes.

  The pi does guarantee that all reads and writes to the same device
  occur in order (these operations are "sequentially consistent"
  w.r.t. each other). However it _does not_ guarantee that a write to
  one device A followed by a second write to device B will complete in
  that order. If they are entirely independent, this may not matter.
  However, if the device operations were supposed to happen in that
  order (A then B), the code is broken.

  The way we handle this is to put in a "memory barrier" that
  (over-simplifying) guarantees that all previous loads and stores
  to memory or devices have completed before execution goes beyond
  the barrier. You can use these to impose ordering. We will discuss
  them at length later. There are some very subtle issues, especially
  when dealing with virtual memory hardware. We don't worry about
  this for the current lab.

- The C compiler does not know about hardware devices nor does it know
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

This is very different than what you intended. As we discuss in
another note, the traditional method OSes have used for this problem
is to mark device pointers with the `volatile` type qualifier, which
(roughly) tells the compiler the pointed to location can spontaneously
change and thus the compiler should not add or remove any `volatile`
load or store or reorder it with either its own accesses or with
those of other `volatile` loads or stores (regular, non-volatile
access have no guarantees).
