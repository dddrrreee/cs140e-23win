### Raspberry Pi Setup: make sure r/pi hardware and its toolchain works.

This is relatively trivial, but will make sure your r/pi (Model 1 A+ or Model
Zero) and ARM toolchain works.

- **Make sure you do the [prelab](../PRELAB.md) first!**
- In addition, there is now a [HINTS file](HINTS.md) that lists out many of the
  common solutions to problems that people ran into
- Look at [the checkoff guide](../CHECKOFF.md) to see what the deliverables for
  this portion will be.

There's a lot of fiddly little details in getting a pi working, and this
lab is many people's first time working with hardware, so we break the
lab down into many (perhaps too-many) small steps.

We'll use different variations of a blinky light using GPIO pin 20
(second from bottom on the right):

1. You'll turn on an LED manually;
2. then use a pre-compiled program loaded from the SD card (why not skip 1?);
3. then use a bootloader (why not skip 2?);
4. then install the r/pi tool chain, compile a given assembly
   version and use it (why not skip 3?);
5. then write your own blink program and compile: this is the fun part. It's
   also the longest (why not skip 4?).

Note: you obviously have to do your own work, but please help others if they
get stuck or if you figure out something. This is the kind of class where
talking over things will make everything a lot clearer, and where having
multiple people debug an issue is an order of magnitude more effective than
working alone. If you think of a way of explaining something that's clearer
than the documentation or the README, post it in the newsgroup so others can
benefit from your insight!

---

### Crucial: Life and death rules for pi hardware.

**Always**:

1. Whenever you make a hardware change --- messing with wires,
   pulling the SD card in/out --- **_make sure pi is disconnected
   from your laptop_**. It's too easy to short something and fry
   your hardware. Also, pulling the SD card out while under power
   sometimes causes corruption when some bytes have been written out
   by your laptop and others have not.

2. If anything ever gets hot to the touch --- the serial device,
   the pi --- **_DISCONNECT_**! Sometimes devices have manufacturing
   errors (welcome to hardware), sometimes you've made a mistake.
   Any of these can lead to frying the device or, in the worst case,
   your laptop. So don't assume you have a safety net: it's up to you
   to avert disaster.

3. While the pi has its own unique features, it's also like all other
   computers you've worked with: if it's not responding, "reboot"
   and retry by unplugging it (to remove power) and reconnect.

---

#### 0. Make sure you have everything.

You should have:

1. a R/PI A+ (or Zero);
2. a microSD card and adapter;
3. a CP2102 USB-TTL adapter;
4. a bunch of LEDs
5. a bunch of female-female jumpers;
6. printout of the pi's pins (digital is okay, but a printout will be
   more convenient).

<table><tr><td>
  <img src="images/pi-start.jpg"/>
</td></tr></table>

The pi is oriented with the two rows of pins on the right of the board.
The two pins at the top of the rightmost row are 5v, the top left 3v.
When connecting things I usually try to use the bottom left ground pin,
and the bottom right GPIO 21 pin, since these are impossible to miscount.

---

#### 1. Make sure hardware is working:

Before we mess with software that will control hardware, we first make
sure the hardware works: When doing something for the first time,
you always want to isolate into small pieces.

So as our first step, we will use the USB-TTY to power the pi, and
use the pi's power to directly turn on an LED. This tests some basic
hardware and that you know how to wire.

<table><tr><td>
  <img src="../../docs/gpio.png"/>
</td></tr></table>

<p float="left">
  <img src="images/part1-full.jpg" width="230" />
  <img src="images/part1-devices.jpg" width="230" /> 
  <img src="images/part1-pi.jpg" width="230" />
  <img src="images/part1-pi-power.jpg" width="230" />
</p>

Mechanically:

1. Connect the USB-to-TTL Serial cable's power (red) and ground (black)
   wires to a 5v pin (not the 3v pin) and ground pins on the pi that are next
   to each other (see your printout; upper right corner). You may need to pull
   back or remove the plastic wrapper around the USB-to-TTL.
2. Plug the USB-TTY into your laptop's USB port.
3. Connect your LED to 3V and ground through the touch sensor. If you connect
   it directly to power and ground, your LED may explode! (it may also work
   anyway, the LED production quality is very low)

   If the LED doesn't go on, reverse its connections. You'll note that
   one leg of the LED is longer than the other. This is used to
   indicate which one is for power, which is for ground. You can
   empirically determine which is which.

   If still doesn't go on, plug someone else's working version into your your
   computer. If that doesn't work, ask.

   If it still doesn't go on, try with your other Pi and/or another LED. If
   that doesn't work, ask.

   (Note: the color of the wire does not matter for electricity,
   but it makes it much easier to keep track of what goes where:
   please use red for power, black for ground.)

4. Try another LED and try some of the other ground and power pins.

(EE folks: We don't use a breadboard b/c it's bulky; we don't use
resistors for the same reason + the LED is big enough we generally don't
fry it.)

What can go wrong:

1. If your USB-to-TTL starts heating up, disconnect! It appears that 1 in
   8 is defective.
2. If your pi starts heating up, now or ever, disconnect! If you have a
   short, where a pin with power feeds right into ground, you'll fry it.
3. If you see smoke, disconnect! Smoke means something has fried, and the
   longer you leave it plugged in the more things will get destroyed.

Success looks like the following photos:

<p float="left">
  <img src="images/part1-success.jpg" width="450" />
  <img src="images/part1-ttl.jpg" width="450" /> 
</p>

---

#### 2. Make sure you're able to install firmware, etc:

Now you'll run a precompiled program (`part1/blink-pin20.bin`) on the
pi and make sure it can blink pin 20.

Note: when you develop your own remote bootloader (see next step) you
step) you have to use this SD card method repeatedly to load new versions,
so pay attention to how you do it on your computer!

Mechanically:

1.  Unplug the USB-TTY.
2.  Plug SD card into your computer and figure out where it's mounted.
3.  As discussed in the `PRELAB`, copy all the files from class
    `firmware` directory onto the SD card. Then copy
    `part1/blink-pin20.bin` to the SD card as `kernel.img`. Then type
    `sync` and then eject the SD card (don't just pull it out! data may
    not be written out.)

    For me, this is:

           #  from the 0-blink directory
           % cp ../../firmware/* /media/engler/0330-444/
           % cp part1/blink-pin20.bin /media/engler/0330-444/kernel.img
           % sync

    On linux 20.04: the cards are often shipped with a corrupt FAT32 file
    system. Linux seems unable to repair this and will mount read-only.
    I had to mount it on a windows machine and format it.

4.  With your pi disconnected from your laptop,
    reconnect the LED power from the 3v pin to gpio pin 20.
5.  Plug the SD card into your pi -- you should feel a "click" when
    you push it in.
6.  Plug in the USB-TTY to your USB to power the pi. The pi will jump
    to whatever code is in `kernel.img`.

The LED should be blinking. If you get this working, please help anyone
else that is stuck so we all kind of stay about the same speed.

Troubleshooting:

0.  First try to trouble shooting from part 1.
1.  If it's not blinking, swap in someone else's card that is working.
2.  If that works, compare their SD card to yours.
3.  If that doesn't work, try your card in their rpi.

Success looks like:

<table><tr><td>
  <img src="images/part2-success.jpg"/>
</td></tr></table>

---

#### 3. Send a new pi program from your computer rather than SD card.

You might need drivers, see the [software guide](./SOFTWARE.md).

As you've noticed, running new programs on the pi using the SD card
method is tedious. We now set up your system so you can send programs
directly from your computer to a plugged-in pi.

Method: install a program (which we somewhat inaccurately call a
"bootloader") on the SD card (as before) that spins in a loop,
waiting for your laptop to send a program over the UART-TTL. If the
bootloader successfully receives a program, it copies it into pi memory,
and then jumps to it. We currently give you a pre-compiled version
(`firmware/bootloader.bin`). The next lab will have you implement
your own.

<table><tr><td>
  <img src="images/assembled.jpg"/>
</td></tr></table>

Mechanically:

0.  Unplug your pi. Don't modify your current wiring.
1.  Did you unplug your pi?
2.  Copy `firmware/bootloader.bin` on your SD card as `kernel.img` (see a
    pattern?), `sync`, and plug back into the pi.

            # from the 0-blink directory
            % cp ../../firmware/bootloader.bin /media/engler/0330-444/kernel.img
            % sync
            # now safe to unplug the SD card

3.  Hook the TX and RX wires up to the pi. Do you TX/TX and RX/RX or
    switch them? (Hint: Think about the semantics of TX (transmit)
    and RX (receive).)
4.  Copy `bin/pi-install.linux` or `bin/pi-install.*.macos` (`arm` or `x86_64`) to your
    local `bin/pi-install`. Make sure when you type `pi-install`
    something happens! If not, make sure your local `~/bin/` directory
    is in your path, and that you have sourced your shell startup file.

    On `tcsh` on my linux machine, after plugging the pi back in:

           % cp ../../bin/pi-install.linux ~/bin/pi-install
           % rehash  # so the shell refreshes its cache
           % pi-install part1/blink-pin20.bin
           # the pi is now blinking.

5.  Your LED should be blinking, just like before.
6.  If you unplug your pi and re-plug it in, you should be able to
    run a hello program:

          %  pi-install part1/hello.bin
          opened tty port </dev/ttyUSB0>.
          pi-install: tty-usb=</dev/ttyUSB0> program=<part1/hello.bin>
          ...
          listening on ttyusb=</dev/ttyUSB0>
          hello world
          DONE!!!

    It exits in such a way that you can rerun it multiple times.

There are some troubleshooting hints in the [hints file](./HINTS.md)

---

#### 4. Make sure your r/pi toolchain is working.

For this class you need to compile bare-metal r/pi programs on your
computer, which is most likely not a bare-metal r/pi itself. Thus we
need to set up the tools needed to `cross-compile` r/pi programs on
your computer and to r/pi binaries.

Install the toolchain: [software guide](./SOFTWARE.md).

Now test that the toolchain works and produces a runnable binary:

1.  Reset your pi: unplug the TTY-USB then plug it back in to your laptop.
2.  Compile and bootload `part2/blink-pin20.s` using the shell script.

          % cd part2
          % sh make.sh     # compile blink-pin20.s to blink-pin20.bin
          % ls
          blink-pin20.bin  blink-pin20.s	make.sh  README.md
          % pi-install blink-pin20.bin    # send it to the pi.

3.  If everything worked, your LED light should be blinking. Congratulations!

---

#### 5. Minor Extension: Run two pi's at once

Because of supply chain issues, we don't have enough pi's (yet?) to give
out two. However, if you are working in a group, it's worth getting two
pi's running at the same time --- this will clarify issues and also make
it easier to do networking.

Configuring your second pi is a great way to re-enforce the steps above.
Also you're going to want two working systems at all times. It will make
it much easier to isolate what a problem is by swapping pi's and then
(if needed) swapping components (e.g., switching SD cards). Finally,
you will need two pi's for the networking labs.

The steps:

1.  Configure your second pi as above. The microSD will likely have a different
    name.

           # from the 0-blink directory
           % cp ../../firmware/* /media/engler/sdd1/
           % cp ../../firmware/bootloader.bin  /media/engler/sdd1/kernel.img
           % sync
           # unplug the microSD from your laptop and plug into your pi

           # as a quick test: run hello since it doesn't need wiring
           % pi-install part1/hello.bin
           find-ttyusb.c:find_ttyusb:55:FOUND: </dev/ttyUSB0>
           opened tty port </dev/ttyUSB0>.
           ... stuff ...
           hello world
           DONE!!!

2.  In two different terminal windows, run `blink-pin20.bin` on each.
    You will need to specify each distinct TTL-USB device on the
    `pi-install` command line, otherwise `pi-install` will try to load
    the same one (the last mounted device).

    For me (again: Linux. MacOS will have a different device mount
    point.):

           # in one terminal
           % pi-install /dev/ttyUSB0 part1/blink-pin20.bin

           # in another terminal
           % pi-install /dev/ttyUSB1 part1/blink-pin20.bin

    Success looks like:

<p float="left">
  <img src="images/part5-succ-far.jpg" width="350" />
</p>

To see where the devices are loaded, you can do `ls -lrt /dev/` as above.
On many Unix systems you can also look at the end of the kernel log.
For example, on Linux:

        % tail -f /var/log/kern.log
        # plug in the device
        [105660.736891] usb 1-2: new full-speed USB device number 22
        ... lots of stuff ...
        [105660.933050] cp210x 1-2:1.0: cp210x converter detected
        [105660.935793] usb 1-2: cp210x converter now attached to ttyUSB0

We can see it's connected to `ttyUSB0`. Using `tail -f` lets us see
immediately as the log file changes (e.g., when you plug in or pull out
your TTL-usb).

Your system likely has an even better way; so it's worth searching online.
