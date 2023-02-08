### Implementing `gpio-int.c`

You only have to implement the code in `gpio-int.c`

***NOTE: you cannot use raw GPIO addresses: each has to have an 
  enum***

`make check` should pass.

You probably want to work on one routine at a time.
  - the `6-*` are the easiest.
  - `7-*` should be a formality if 6 pass.

For matches:
  0. We left the `.out` files.  
  1. I checked pins were <= 32.   I did this before issuing a `dev_barrier`.
  2. I enabled any GPIO address before the Broadcom interrupt controller.
  3. The interrupt controller is a different device from GPIO so we need
     dev barriers between them.
  4. We don't know what we were going before the GPIO calls, so always 
     do a dev barrier before and after.
  5. `gpio_has_interrupt` and `gpio_event_detected` call 
     `DEV_VAL32` on their result before returning.
