# Checkoff Checklist

Make sure you have all of these things working for checkoff!

Unfortunately, due to the size of the class this year we won't be able to give
you debugging help during checkoff if there are others waiting.

## Setup

For the setup portion of the lab, show us that you have:

1. You have a working `blink` compiled using the toolchain from
   `setup/part2/blink-pin20.s` and started using the bootloader.
2. Bonus: write up a short example problem you ran into (or someone you helped
   ran into) and how you solved it. Post this to the newsgroup so we can start
   accumulating examples.

## GPIO

If you've never used a Pi (baremetal) before, to get credit for the lab show
the following:

1. You can blink a single LED on Pin 20.

2. You can blink two LEDs on Pins 20 and 21, such that they're always in
   opposite states (i.e., if 20 is on, 21 should be off and vice-versa) (see
   `code/2-blink.c`).

3. You can turn on an LED connected to Pin 20 whenever a touch sensor on Pin 21
   is touched (`code/3-input.c`).

4. You can blink two LEDs in different `FSEL` groups, e.g., Pins 16 and 20.

5. You can do two out-of-sync things at once, e.g., blinking one LED at
   a constant rate while controlling a second LED with a button.

6. You've reasonably validated that your code is correct. We'll ask you to
   explain how; "it passed the test cases" is insufficient.

7. If you've used a Pi before (e.g., in 107e), also implement `gpio_pullup` and
   `gpio_pulldown`. Ideally you also do some extensions.

## Extensions

See more in the [extensions doc](EXTENSIONS.md).
