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


     Note, the  `srs` instruction is a little weird: it only works with
     the `sp` register and you have to specify which mode `sp` to use:

            srs sp, #SUPER_MODE

     so in our case you'll have to copy the `sp` to another register,
     move `r0` into it, do the `srs` then restore the `sp`.

     Test: `1-srs-rfe.c`.  Code to write in: `1-srs-rfe-asm.S`.

  2. Setup simple single stepping.   Put your code in `breakpoint.c`
     (there is a `staff-breakpoint.o` you can use).
     It should implement the routines in `breakpoint.h`.    

     Test: `2-simple-single-step.c` and `2-simple-single-step-asm.S`.

     When run on the `nop_10` it should trace the expected instructions.
     You can verify this from the `.list` file to double check.

  3. Implement the assembly to save all registers in ascending order
     into a single block of memory on the exception stack and pass that
     to the system call exception handler.  
     This is the first step of getting to full processes.   

     Test: `4-reg-restore.c`.  Code to implement: `4-reg-restore-asm.S`.

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
     a seperate directory and do some kind of interesting test.  For 
     example, add interrupts (note you will have to modify the hashed
     `cpsr`), do overclocking and see at what point hashes change,
     recompile libpi with higher optimization (note: the hashes
     for any C routines will change from previous runs).

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

Workflow using this trick:
  1. Run processes one at a time, get their hashes and store them.
  2. As long as you run the processes at the same address with the
     same stack, these values should not change.
  3. You can then re-run these processes concurrently, over and over
     and make sure nothing changes. (See caveat below.)
  4. You can also add features and check that the hashes remain the same.
     Assuming you have a seperation between the "user" code and the kernel,
     then turning virtual memory off or on, adding interrupts, changing
     compiler options should not change anything.  

The caveat for today is that since we bundled all the code in the same binary,
any change that affects code layout can change the hash.   For us, any
assembly routine in `code/staff-start.S` won't change layout (since its
linked first in the binary and is assembly code) so you can mix and match
these as you like.  Any routine that requires a stack will change if you 
allocate multiple since the second will be at a different location.  Similarly,
if the code interacts with the UART because of timing differences.

When we go to full user processes that are seperated from the kernel
(there's not much needed!) then these restrictions go away.  We did
things the way we did to reduce the number of moving parts in the lab.  But
it is weird.



### Checkoff: 

  - You can run multiple processes and your hashes pass and match other
     people.
