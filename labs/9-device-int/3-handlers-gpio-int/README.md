### implementing gpio interrupt handlers.

For this part you write the handlers to deal with GPIO and 
also do actual tests.

All the code you write will be in:
  - `test-interrupts.c`:
  - you'll write handlers for rising and falling edges, and timer
    intrrupts (just steal the handler code you need from lab 5).

The header:
  - `test-interrupts.h` has calls and wrappers for the tests.
  - `interrupts-asm.S` just has the timer interrupt trampoline.

The tests:
  - `1-*` do a single event.
  - `2-*` do multiple types of events, once each.
  - `3-*` do multiple types of events, many times each.

The Makefile currently uses:
  - the staff libpi `../staff-objs/libpi.a`: 
  - uncomment this if you want to use your own.
