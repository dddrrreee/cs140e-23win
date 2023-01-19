Basic idea: to add code, modify only three places:
  1.  Put your `.c` or `.S` files in `src/`.  
  2. Add these to `SRC` in `Makefile`.  If there is a staff version given in
     `STAFF_OBJS` remove it.

Please do not:
  1. Add files to the `staff-src/`, or `libc` directories, or modify our
     `.h` files.  Files we add later may conflict with yours.
  2. If you want to add extra prototypes put them in `include/your-prototypes.h`
     which we will not touch, so you can change freely.

The rest of the code:
  1. `include`: All the pi specific .h's.
  2. `staff-src/`: All the staff provided r/pi source.   You can write
     your own, but these are not super interesting.
  3. `libc`: `.c` implementation of standard library functions (often
     ripped off from `uClibc`).
  4. `staff-objs`: these are `.o` files we provide. You can always write
     your own.
  5. `objs`: this is where all the .o's get put during make.  you can ignore it.
