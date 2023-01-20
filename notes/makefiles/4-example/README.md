## 4-example

We now use the `-MMD` flag to automatically generate dependencies rather
than hardcode a bunch of stuff (which can go wrong in bad ways).

This makefile is one of the few you'll need in your whole career:
  - compile all the `.c` files in a directory
  - into an executable in the same directory
  - with automatic dependencies.

You shoudl test that:
  - `touch a.c`
  - `touch b.c`
  - `touch c.c`
  - `touch header.h`
All work as expected.

Also: look in the `.d` files to see why they work (it should be obvious
how you'd write them by hand, since that is what we did in `0-example`
and `1-example`).
