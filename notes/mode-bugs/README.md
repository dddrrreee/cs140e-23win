## Some different mode bugs.

We'll talk through some different bugs that can show up when switching
modes.  This does two things:
  1. very concrete examples for how the modes and banked registers work.
  2. sharpens intuition / pattern matching for what the bugs look like.

### Mode review

Note:  this discussion overlaps with some other Notes.   Skim if you
already know it.

We have 18 registers:
  - 16 general purpose registers `r0` --- `r15`.  These often have
    "symbolic" names that are intended to make them easier to remember,
    but can also confuse things.   The most common we use are the 
    stack pointer, `sp` (r13), the link register, `lr` (r14) and 
    the program counter, `pc`, (r15).

  - `cpsr` --- the "current program status register", which holdes
    the current mode (its lowest 5 bits), whether interrupts are enabled,
    and other things that we don't care that much about atm.

  - `spsr` --- the saved `cpsr` from the last fault.  If hardware switches
    from one mode to another via a hardware fault (e.g., a system call,
    breakpoint instruction, interrupt, memory faults) the `cpsr` when
    this fault occurs is saved to the `spsr` and the new mode is put
    in the `cpsr`.  Without doing this, we wouldn't know what mode to
    resume to after handling the fault.

As shown on page A2-3 of the ARMv6 document,  there are 7 modes; four
that we use in this lab:

 - User (`0b10000`) : this is user mode, no privileges.  Privileged
   instruction gives a fault.
 - IRQ (`0b10001`) : mode set on interrupt, privileged
 - Supervisor (`0b10011`) : supervisor mode, privileged
 - System (`0b11111`) : system, privileged

Some key things:

 - All modes other than User and System have their own "shadow" or
   "banked" copies of two general purpose registers: the stack pointer
   and the link register as well as their own copy of the `spsr`.

   These banked registers are persistent on mode switches: if you set
   them, switch to another mode, do stuff, and then switch back, they
   retain their values.  For example, if we are in Supervisor mode,
   set the stack pointer to `8`, switch to System mode, do some stuff,
   and then switch back to Supervisor, `sp` will still hold `8`.

   However, this persistence places no obligation on you to use the
   previous value --- you can always set them how you want.

 - System and User share all registers.  However, all others have banked
   copies so do not interfer with them.  A nasty bug is to forget this
   sharing and trash User mode registers when in System mode.  This shouldn't
   show up today.

 - With the exception of User mode, any other mode can manually switch
   to any other by using the privileged `msr` instruction to forcibly
   set the mode.  User mode has no privileges so cannot use these
   instructions.

   In contrast, the method User mode uses to switch to privileged
   execution is to initiate a system call (using the `swi` instruction)
   which is treated just as any other exception in that it jumps to a
   known location, sets the mode (A2-20) to Supervisor and starts running

In any case, the basic idea isn't that complicated:

  - There are different modes.  
  - These modes have their own copies of `sp`, `lr`, and `spsr`.  
  - They generally share the other registers: switching a mode 
    does not change the value of `r0` or `pc`.  
  - We can switch between privileged modes as we want using `msr` 
    instructions (i.e., in all modes other than User).

Banked registers aren't that subtle.  However,  that doesn't prevent
many bugs in practice:

  - Depending on what your code does, it can create bugs by
    forgetting that it shares registers with other modes (e.g.,
    r0-r12, all unbanked) and trashing them.

  - Your code can *also* have bugs by forgetting it has
    private copies of `sp` or `lr` and attempting to use the previous
    values.

We show some examples below.

--------------------------------------------------------------------
### Bug 1: try to switch to Super mode and keep running

We use variations on the following simple program:
  1. `bug1-driver.c`: C code that gets the current mode from `cpsr`
     and prints out some information.
  2. It then calls buggy assembly (`bug1-asm.S`)
     to change from the current Supervisor mode to System.

The C code:
```cpp
// bug1-driver.c
void notmain(void) {
    uint32_t cpsr = cpsr_get();
    printk("cpsr = <%b>\n", cpsr);

    uint32_t mode = cpsr&0b11111;
    printk("     mode = <%b>\n", mode);
    assert(mode == SUPER_MODE);

    printk("    interrupt = <%s>\n",
                ((cpsr >> 7)&1) ? "off" : "on");

    switch_to_system_bug();
    printk("switched to system\n");
}

```

The assembly code:

        @ bug1-asm.S
        #include "rpi-asm.h"
        
        @ use the mrs instruction to get the current program
        @ status register value and return it in r0 
        .global cpsr_get
        cpsr_get:
            mrs  r0, cpsr
            bx lr

        @ switch to SYSTEM mode (0b11111)
        @ and return. 
        @
        @ the way we do it doesn't work of course.
        @ hint: banked registers. 
        .global switch_to_system_bug
        switch_to_system_bug:
            mov r1,  #0b11111
            orr r1,r1,#(1<<7)    @ disable interrupts.
            @ use msr to set the current mode
            msr cpsr, r1
            @ do a prefetch flush to make sure the msr is done
            @ (similar to a memory barrier)
            prefetch_flush(r1)
            @ done: should be in SYSTEM mode, return.
            bx lr


Some quick review:
  - The first four integer / pointer values are passed in `r0`, `r1`,
     `r2`, and `r3` registers.
  - We return an integer / pointer result in `r0`.
  - When a routine is done we return back to the caller using `bx lr`:
    the `lr` register holds the return address.
  - We can trash any caller-saved register we want without saving it.

Now if we run the code, what happens?
On my r/pi if I compile and run, I get:

            % make BUG=bug1
            ...
            bootloader: Done.
            listening on ttyusb=</dev/ttyUSB0>
            cpsr = <1000000000000000000000110010011>
                mode = <10011>
                interrupt = <off>

The driver code never returns from the `switch_to_system_bug` call and 
the code hangs with no output after the last line.

<details>
  <summary>What is going on?</summary>

<br>
Hint: think about the banked registers.  What is the value of `lr` that we
are jumping back to?  (I assume `lr` holds the value 0 so we are jumping
to address 0 and running, which is going to cause some random problems.)

</details>

--------------------------------------------------------------------
### Bug 2: try to fix by passing `lr` to System

Ok, so our fix is to try to pull the `lr` from our initial Super mode
to the new System mode:

        @ bug2-asm.S
        @
        @ we pull the SUPER mode lr to SYSTEM.
        @ this doesn't fix the problem.
        @ why?  (hint: even more banked registers)
        .global switch_to_system_bug
        switch_to_system_bug:
            @ save lr to a non-banked register
            mov r0, lr

            mov r1,  #0b11111
            orr r1,r1,#(1<<7)    @ disable interrupts.
            msr cpsr, r1
            prefetch_flush(r1)
        
            @ jump to the saved version.
            @
            @ why don't we need to restore lr?
            blx r0

A few notes:
  - The non-banked registers are essentially shared memory across
    all modes --- if you want to pass a few values between modes, the
    easiest method is to just put them in non-banked registers (typically
    caller saved ones so they don't have to be saved and restored).

  - So, we can pass the `lr` from Super mode to System mode by sticking 
    it in `r0`.

When I do:

        % make BUG=bug2

I get an infinite amount of garbage (you could get a different result).
Not really the look we were going for.

<details>
  <summary>What is the bug?</summary>

<br>

Well it's banked registers again.

`lr` is not the only banked register: `sp` is banked, too.
We switched modes and didn't set `sp`.  Not-brilliant.  I'm betting in
this case that `sp`'s initial value is 0 (null) so we are reading and
writing low memory.  The result is hard to say.  It could spew infinite
garbage, as it did here.  It could also do some other random stuff.
For example, it printed an infinite number of `U` characters a few
minutes ago for me and changed when I modified a `printk` format string.

Oddly, this infinite string of `UUUUUUUUU.....` seems to show up in many
situations where we have stack corruption with exceptions and threads.
Unclear why.  Seems to be the golden ratio of register save/restore.

</details>


--------------------------------------------------------------------
### Bug 3: switch to Super with a new stack

In this variant, we change the assembly routine to take in a new stack:
when we switch from Super mode to 
System mode, we set the System `sp` to this new stack
and return to the caller.

```cpp
// bug3-driver.c
enum { N = 1024 * 64 / 8 };
// we used unsigned long long so is 8 byte aligned.
static unsigned long long stack[N];

void notmain(void) {
    uint32_t cpsr = cpsr_get();
    printk("cpsr = <%b>\n", cpsr);
    printk("    mode = <%b>\n", cpsr&0b11111);
    printk("    interrupt = <%s>\n",
                ((cpsr >> 7)&1) ? "off" : "on");


    uint32_t base = (uint32_t)&stack[0];
    demand(base % 8 == 0, "stack %p is not 8 byte aligned!\n", base);

    // switch to System mode using the given stack.
    switch_to_system_w_stack(stack);
    printk("switched to system: sp = %x\n", sp_get());
}
```

Where our assembly code is:

        @ bug3-asm.S
        @ switch to stack address passed in r0
        @
        @ bug: why?  hint: where is the previous 
        @ stuff in caller stored?
        .global switch_to_system_w_stack
        switch_to_system_w_stack:
            @ save lr to non banked.
            mov r1, lr
            
            @ set SYSTEM mode + disable interrupts
            msr cpsr, #(SYS_MODE | (1<<7))
            prefetch_flush(r2)
        
            @ load sp from first argument
            mov sp, r0
            @ jump back to original lr
            blx r1


When I compile and run this code:

        % make BUG=bug3

I get an infinite set of:

        cpsr = <1000000000000000000000110010011>
            mode = <10011>
            interrupt = <off>
        switched to system: sp = 0xx xx
        cpsr = <1000000000000000000000110010011>
        ...

<details>
  <summary>What is the bug in the assembly code?</summary>


<br>
There are actually two bugs, but let's just talk about the conceptual
one in the assembly routine specification.  At a mechanical level we do
correctly do what the comments say:

   1. We correctly pass the return address held in `lr` to 
      System mode by moving it to the unbanked `r1`.
      This means the last `blx r1` does jump back to the
      right location (the original `lr`).

   2. We also correctly set the passed in stack address (held
      in `r0`) to the stack pointer for System mode by performing `mov
      sp, r0` after the `msr/prefetch flush`.  (Doing before
       would modify the wrong `sp`.)

So what goes wrong?    Well, if you look at the `bug3-driver.list` file
right after the call to `switch_to_system_w_stack` you can see a hint:

            // machine code in notmain
            8074:   eb000011    bl  80c0 <switch_to_system_w_stack>
            8078:   eb00000e    bl  80b8 <sp_get>
            807c:   e1a01000    mov r1, r0
            8080:   e59f0020    ldr r0, [pc, #32]   ; 80a8 <notmain+0x70>
            8084:   eb000013    bl  80d8 <printk>
        uh oh, problem ==>
            8088:   e8bd8010    pop {r4, pc}

The code will start popping registers off of the stack.  Oops.
These registers were stored to the previous, original stack 
--- we then switched to a new stack, which means these pops
are loading garbage values.

It's hard to believe but a bug like this cost us a few hours in the 
initial cs240lx offering.

</details>

--------------------------------------------------------------------
### Bug 4: run a routine in Super mode with a new stack

Ok, last case.  Here we are trying to run a procedure at System mode
with a new clean stack, and never return.  We pass in the routine 
(`r0`) and a stack address (`r1`) to use:

```cpp


    void hello(void) {
        uint64_t *sp = sp_get();
        printk("hello: running with stack: sp = %x, &stack[N]=%x\n",
                sp, &stack[N]);
    
        assert(sp < &stack[N]);
        assert(sp >= &stack[0]);
    }

    void notmain(void) {
    ...
        run_fn_at_system(hello, stack);

```

Where the (AFAIK) correct assembly is:

        @ SYSTEM mode + disable interrupts
        msr cpsr, #(0b11111 | (1<<7))
        prefetch_flush(r2)

        mov sp, r1
        blx r0

        @ if we return, reboot
        bl  clean_reboot


Unfortunately if we compile and run we get a weird assertion failure
message:

    % make BUG=bug4
    listening on ttyusb=</dev/ttyUSB0>
    cpsr = <1000000000000000000000110010011>
        mode = <10011>
        interrupt = <off>
    hello: running with stack: sp = 0x8ab8, &stack[N]=0x 8ad 
    bug4-driver.c:hello:  :ERROR: Assertion `sp >= &stack[0]` failed.

Where the `assert` fails but the message has some garbage (no line number).
This weird output (and corrupted memory in general) is a common pattern
with register mistakes.

<details>
  <summary>What is the bug?</summary>

<br>
Here the bug is not in the assembly but in the caller: stacks grow down
(you should see how to write a piece of C code to test this).  

However, we have passed the base of the `stack` array in as the stack
pointer.

As usual, the code compiles, and kinda runs.  We just get a weird result.
This is common.  You should be able to start pattern matching on weird
errors like this and recognize what a mistake in your register save /
restore code looks like.

(Of course, the  problem is that this also looks like normal memory 
corruption.  You'll have to get good at crossing that off by 
simplifying your code.)

</details>

--------------------------------------------------------------------
### what to do

I'd strongly suggest writing:
  1. A simple pi program to verify stacks grow down.  It's 
     good to get in the habit of writing small C or assembly programs
     to test different hypotheses.  You should try to figure them
     out from the architecture manual or from first-ish principles.
     But as the proverb goes: "Trust, but verify."

  2. A simple routine that will run a routine at a different privilege
     level with the *same* stack, and upon return switch back.  This
     will be a synthesis of the above examples.
