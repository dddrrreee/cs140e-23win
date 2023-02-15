## Single step equivalance checking

Today you're going to do a wildly useful trick for ruthlessly detecting
subtle operating system mistakes.  It will prove invaluable next week
when we do virtual memory --- a topic that has extremely hard-to-track-down
bugs if you just use "normal" testing of running programs and checking
print statements and exit codes.

There's not much code, we build it up in small pieces, and it tests
itself, so that's a nice improvement over some labs.

Goals:
  1. Be able to pre-emptively switch between user processes.
  2. Hash every single register on every single instruction 
     executed so that no difference can ever be missed.

Roadmap:
  0. Read and write the banked user registers using the `ldm` and `stm`
     idiom. Test: `0-user-sp-lr.c`. Code to write: `0-user-sp-lr-asm.S`.
  1. Implement the code to use `srs` and `rfe`.
     Test: `1-srs-rfe.S`.  Code to write in: `1-srs-rfe-asm.S`.
  2. Setup simple single stepping.   Put your code in `breakpoint.c`.
     It should implement the routines in `breakpoint.h`.    Test: 
     `2-simple-single-step.c` and `2-simple-single-step-asm.S`.
     When run on the `nop_10` it should trace the expected instructions.
  3. Implement the assembly to save all registers in ascending order
     into a single block of memory and pass that to the system call
     exception handler.  Test: `4-reg-restore.c`.  Code to implement:
     `4-reg-restore-asm.S`.

     This is the first step of getting to full processes.   
  4. Implement the assembly to take an array saved in the previous step
     and restore it, including mode switching.   Test: `4-reg-restore.c`
     Code to write: `4-reg-restore-asm.S`.
  5. Wrap up the code from the previous step in a macro that you put
     in `regs-save-restore.h` (provided) and use it to implement both
     the `swi_trampoline` and `single_step_trampoline`.
     The test: `5-single-step-one.S`.  Code to implement:
     `5-single-step-one-asm.S`.
  6. Implement the single step handler in `6-single-step-proc.c`.  Your
     output should match when just running the `nop` routines (`printk`
     can change.

  7. Pull the pieces of code you need into a small two file system in
     a seperate directory and do some kind of interesting test.  (E.g.,
     add interrupts; change optimization levels, etc)

We'll add more discussion as the lab goes on.

The high bit of all of this:
  1. You have pre-emptive save/restore code that can switch between
     user processes.
  2. I'd say this code has been verified.
  3. No one does (1) and (2) in this way.  I'd wager you have the smallest
     system ever written with this kind of power.
  4. From this point forward we can do series of small steps and make sure
     that *nothing* ever changes the hashes.  This will make the rest of
     the quarter much smoother when it comes to VM or other stuff.

### Checkoff: 

  - You can run multiple processes and your hashes pass and match other
     people.
