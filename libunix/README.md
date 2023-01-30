This directory contains a bunch of useful Unix utility routines.  We also
mix in some pi-specific unix-side routines since it's easier to keep
them in one place.
  - Put all your unix utilities routines in here, one per file.  
  - The prototypes are in `libunix.h` and `staff-libunix.h`.
  - The `Makefile` will automatically gather them up and put them in `libunix.a`.
  - The `Makefile` should handle dependencies: if it does not, let me know!

You can modify (and not get `git` conflicts):
  - `libunix.h`
  - `Makefile`
  - add any file you want.

Don't modify (since you'll get conflicts):
  - `staff-libunix.h`
  - `staff-rules.mk`

