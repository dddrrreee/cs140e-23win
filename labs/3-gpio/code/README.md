### Implement the routines in `gpio.c` to control the pi's GPIO pins.

Files:

- `gpio.c` the only place you will write code.
- `rpi.h` has the definitions of simple routines we provide to
  read/write device memory and the prototypes for the `gpio.c` routines.
- `start.S`: the trivial routines we provide (written in assembly).

Part 1:

- `1-blink.c` blinks an LED on pin 20.
- `2-blink.c` blinks two LEDs oppositely, one on 20, one on 21.
  Note: its easy to make a mistake in how you read the broadcom
  document!

Part 2:

- `3-input.c` the driver for part 2: blinks an LED on pin 20
  whenever the input to GPIO pin 21 is high. If you have a touch
  sensor you can hook it up, or just touch connect the ground of an
  LED to pin 21 and touch its positive leg to 3v.
