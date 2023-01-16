# Prelab: Setup and GPIO

### Always obey the first rule of PI-CLUB

- **_IF YOUR PI GETS HOT TO THE TOUCH: UNPLUG IT_**
- **_IF YOUR PI GETS HOT TO THE TOUCH: UNPLUG IT_**
- **_IF YOUR PI GETS HOT TO THE TOUCH: UNPLUG IT_**
- **_IF YOUR PI GETS HOT TO THE TOUCH: UNPLUG IT_**
- **_IF YOUR PI GETS HOT TO THE TOUCH: UNPLUG IT_**

You likely have a short somewhere and in the worst-case can fry your laptop.

---

## TL;DR

This is broken down some more below, but please make sure:

1. You've done the reading (see below), especially the Broadcom reading.
2. You've done the prelab questions on
   [Gradescope](https://www.gradescope.com/courses/488143/assignments/2568938).
3. You've installed the `arm-none-eabi-gcc` toolchain (see
   [software](setup/SOFTWARE.md)).
4. You have a way to read/write a microSD card and connect a USB-A device to
   your computer.
5. You've added `~/bin` to your `$PATH`.

There's more information below. Since this is the first lab and people have
varying levels of experience, we figured it's better to over-explain than
under-explain; future labs will be more succinct.

## Reading

1. Read through the [GPIO](./GPIO.md) and [device memory](./DEVICES.md) crash
   courses. You should have the [Broadcom
   document](../../docs/BCM2835-ARM-Peripherals.annot.PDF) open so you can go
   through the examples in the crash course
   (`../../docs/BCM2835-ARM-Peripherals.annot.PDF`).

2. After doing so, read through pages 4--7 and 91---96 of the broadcom
   document to see what memory addresses to read and write to get the GPIO pins
   to do stuff. The specific stuff we want to do: configure a pin as an output
   and turn it on and off, or configure it as an input and read its level.

   This is a low-level hardware document. It's okay if it's confusing! Just
   skim what you don't understand and try to pull out what you can. We will
   cover the necessary pieces in class.

3. Look through the `code` directory. You'll be implementing the routines in
   `gpio.c` which is used by three simple programs: `1-blink.c` `2-blink.c` and
   `3-input.c`. You only modify `gpio.c`. We provide prototypes (in `rpi.h`)
   and some trivial assembly routines in `start.S`.

4. Note: where the broadcom document uses addresses `0x7E20xxxx`, you'll use
   `0x2020xxxx`. The reason is complicated, but you can find a diagram
   explaining it on page 5 of the manual.

---

## Lab Info

1. You'll turn on an LED manually;
2. Then copy a pre-compiled program to an micro-SD and boot up your pi;
3. Then use a bootloader to ship the program to the pi directly;
4. Then install the r/pi tool chain, compile a given assembly
   version and use it;
5. Then write your own `blink` program, compile and run it;

Parts 1-4 are in `setup`, part 5 is in `code`.

You don't need to submit answers, but you should be able to answer the
following questions:

1. What is a bug that is easier to find b/c we broke up steps 1 and 2?
2. What is a bug that is easier to find b/c we broke up steps 2 and 3?
3. What is a bug that is easier to find b/c we broke up steps 3 and 4?
4. What is a bug that is easier to find b/c we broke up steps 4 and 5?

Differential debugging: Your pi setup in step 2 is not working.
Your partner's does. You plug your setup into their laptop.

1. Yours works in their laptop: what do you know?
2. Yours does not work in their laptop: what do you know?

---

## Setup

For this lab, we'll use different variations of a blinky light to check
that parts of your toolchain / understanding are working.

You'll likely forget this link, but if at any point you get confused
about some basic concept, cs107e has a good set of guides on electricity,
shell commands, pi stuff in general:

- [CS107E repo] (http://cs107e.github.io/guides/)

Before lab, you should do the following:

1.  Figure out how to add a local `~/bin` directory in your home directory
    to your shell's `PATH` variable.
2.  Have a way to mount a micro-SD card on your laptop.
3.  Make sure you know how to use common unix commands such as: `ls`,
    `mkdir`, `cd`, `pwd`.
4.  Install software as necessary based on the [software guide](setup/SOFTWARE.md).

---

## Micro-SD card stuff

In order to run code on the pi, you will need to be able to write to a
micro-SD card on your laptop:

1.  Get/bring a micro-SD card reader or adaptor if its not built-in
    to your laptop. The [CS107E guide on SD
    cards](http://cs107e.github.io/guides) is a great description
    (with pictures!).

2.  Figure out where your SD card is mounted (usually on MacOS it is in
    `/Volumes` and on linux in `/media/yourusername/`, some newer linuxes
    might put it in `/run/media/yourusername/`). Figure out
    how to copy files to the SD card from the command line using
    `cp`, which is much much faster and better than using a gui (e.g.,
    you can put it in a
    `Makefile`, or use your shell to redo a command). For me,

          % cp kernel.img /media/engler/0330-444/
          % sync

    will copy the file in `kernel.img` in the current directory to the
    top level directory of the mounted SD card, the `sync` command forces
    the OS to flush out all dirty blocks to all stable media (SD card,
    flash drive, etc). At this point you can pull the card out.

    Pro tip: **_do not omit either a `sync` or some equivalant file
    manager eject action_** if you do, parts of the copied file(s)
    may in fact not be on the SD card. (Why: writes to stable storage
    are slow, so the OS tends to defer them.)
