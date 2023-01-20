### 7-example: simplify labs/3-gpio/code/Makefile
 
Our the makefile in the lab 3 is kind of complicated.  We simplify it
significantly:
  - just get rid of all the .o rules: we are making tiny programs, can
    recompile everything.
  - also remove all the .d computations.

  - use the "FORCE" idiom to force program regeneration for all
    programs (we need this since there are no dependencies :)

  - we could always do it, but we keep the dep computation (you can
    easily nuke this).

  - many makefiles will share the arm toolchain and flags so 
    so seperate it out into an include for makesfiles (`defs.mk`).
