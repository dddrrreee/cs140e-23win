## equiv OS.

Today we'll have a very small, OS that does equivalance checking.
It provides `fork`, `waitpid`, `exit` and `sbrk`.

The good news:  lab today is capped at 2.5 hours.  You should drop
in the code that you can to replace ours from old labs and see that
the tests pass.  As is: `make` in `code` should complete without
panics.

The bad news: the reason its short is that the code is ugly and overly
complex.  It needs another day of hacking to make it about half the
size.  So, all you have to do today is try to drop in a few of your old
implementations and replace mine.  none of the hashes should change.
You can treat the code as a big test case; or you can go into it and
add things.  The absolutely fantastic thing is that equivalance checking
will find your mistakes aggressively (it did mine!).

The big picture:
  - This is a heavily modified version of lab 11 (the equivalance) process
    code, which would context switch on every single program step.

  - This lab adds virtual memory and system calls.  Virtual memory
    uses a hacked up version of pinned memory.  Each process has a
    single pinned entry (each process is 1 MB).  The system itself has
    6 global entries.  There is a single entry used for temporary pins
    (used for copying).

  - The test programs are in `tests`.  There are simple ones for 
    forking, allocation, etc and waitpid.  The full program is 
    `3-do-all.c` which forks everything concurrently.  If this
    code passes, your support code has been tested pretty harshly.

If you look in the `config` structure in `equiv-os.h` you can see
some of the options for flipping.  The main ones that work out of the
box:

  - `config.random_switch`: if you set this to a non-zero the system 
    won't switch on every process, but randomly flip based on the number
    you give (higher number = less often).
 - `config.do_vm_off_on`: this will flip the virtual memory off and on.

The files:
  - `proc.h`: ugly code for allocating and handling processes.
  - `equiv-os.c`: code to implement system calls.
  - `support-code.c`: this is an ugly mix of code that wraps up pinned code
    and does fork and exec.
  - `libos-*`: this is support for user level code.
  - `user-progs/*`: these are the user programs.

### ***WHAT TO DO***:

Any of the following:

  - start going through the `Makefile` and drop in your code for the
    different pieces and make sure that the tests still pass.

Alternatives, or extensions:
 - Alternative: drop in page tables.
 - Alternative: add more interesting equiv hacks.  E.g., enable the 
   icache.  Or, more tricky, add caching. 
   Or, figure out how to reuse ASIDs. 
 - Major great thing: rewrite the code (or some of the code) so it doesn't
   suck.  This is what I will be doing. The great thing about equiv is
   that it's super brutal at finding weird errors.
