### Setting vector base

You should implement:
  - <vector-base.h>:  You'll write the inline assembly to set the 
    vector base.  See:

    - 3-121 in `../../docs/arm1176.pdf`
  - <interrupt-asm.S>: do the single line change to switch to a branch
    that calls `sys_plus1_handler`.  You should see a speedup!

  - When the tests pass, move the `vector-base.h` file to `libpi/src` and make
    sure they still work.
