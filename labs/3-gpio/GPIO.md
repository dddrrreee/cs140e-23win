## Setting up GPIO on the r/pi

This note describes how to configure and use the r/pi GPIO pins.
The GPIO device is a good "hello world" case study for getting used to
reading a device datasheet description and writing driver code to control
the device. We will do more advanced ones later in the quarter (e.g.,
UART and the NRF24L01P RF transceiver).

Primary sources:

- All GPIO device information is from the Broadcom BCM2835 datasheet:
  `/docs/BCM2835-ARM-Peripherals.annot.pdf`.

- [Errata](https://elinux.org/BCM2835_datasheet_errata).

  Always check for datasheet errata! Datasheets often have
  errors. The Broadcom document is no different. Fortunately
  it has been worked over enough that it has a [nice
  errata](https://elinux.org/BCM2835_datasheet_errata). Sometimes you
  can figure out bugs from contradictions, sometimes from errata. One
  perhaps surprisingly good source is looking at the Linux kernel code,
  since Linux devs often have back-channel access to manufacturers,
  who are highly incentivized to have Linux work with their devices.

  The hardest task is to be the first one to boot up a system or
  configure a device. Any datasheet mistake can easily prevent all
  progress. Isolating the root cause using differential analysis is
  hard because a datasheet error in any of the multiple setup steps
  causes the same effect: the device doesn't work. Multiply the
  above by: are you sure both your code and your understanding of the
  datasheet is correct?

Also read:

- The [DEVICES.md](DEVICES.md) note to get a crash course on general
  device patterns and heuristics. Most datasheets suck, but they
  suck in similar ways, so you can improve your life by understanding
  what to expect and how to work with it.

---

#### How devices work on the r/pi

The r/pi, like most processors has a bunch of different devices it can
control. For example: the GPIO pins you use in lab 0 and lab 1. You
can see a bunch of these devices by skimming the index of the Broadcom
document: UART (you'll write a driver for this), I2C, SPI, the SD card
reader, etc. Obviously, to use these devices, the pi must have a way
to communicate with them.

An old-school, obsolete approach (circa 1950s)
for device communication is to have special assembly instructions for each
device and its operations. (You can see what ARM assembly looks like
by looking at any of the `.list` files our `Makefile`s generate during
compilation.) This method sucks, since each new device needs its own set
of instructions: the hardware designer would have to anticipate all of
these or awkwardly back patch them later (yuck) and these instructions
would take up precious space in the instruction set encodings (more on
this in a different note).

Instead, almost all systems use the following hack:

1. They give each device its own chunk of the physical address space;
   loads or stores to these addresses are forwarded to the device.
2. The device specifies which addresses control which device "registers"
   (typically not literally registers, only linguistically) down to the
   semantics of each bit.
3. Programs control a device by performing loads or stores to its
   associated memory
   locations using magic, device-specific values. (Example below.)

This method works better than device-specific instructions because:

1. Modern address spaces are big enough you can give a chunk to devices
   without much downside.
2. Encoding device commands as opaque, uninterpreted integers (32-bits
   on the ARM) that the CPU does not interpret in any way makes it easy
   for devices to express any semantics you want.

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

---

#### GPIO on our r/pi

GPIO is an acronym for [General Purpose Input Output](https://en.wikipedia.org/wiki/General-purpose_input/output).

On the r/pi: physically GPIO pins are the double row of 40 metal pins
sticking on the side of the PCB. ![r/pi pins](https://learn.microsoft.com/en-us/windows/iot-core/media/pinmappingsrpi/rp2_pinout.png) You'll use these to
connect your r/pi different devices. Some of these have have a dedicated
purpose such as providing 5v or 3v power or ground. The rest can be
configured for output (producing 0 or 3.3v), input (where they convert
3.3v or less than 3.3v to a digital 1 or 0 respectively) and a range of
more esoteric features (you will do some).

GPIO is a descriptor for a pin that has no inherent dedicated purpose.
Instead, it can be programmatically toggled between input/output mode,
and high/low signals in order to control some electronic device. 26 out
of the 40 RPi pins are GPIO pins, while the others have assigned purpose
(e.g. 5v, ground pins, etc...). We can use the pi to control these GPIO
pins as needed.

#### How to turn GPIO pin 20 "on"?

For our first hardware lab you'll want to turn pin 20 on and off. Pin
20 is the second pin from the bottom-right, if you orient the r/pi
with the pins on the right. To figure out how to do so we look in the
Broadcom document:

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
        PUT32(0x2020001C, (1 << 20));

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

---

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
