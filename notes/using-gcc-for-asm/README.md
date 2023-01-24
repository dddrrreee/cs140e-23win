## Using gcc to figure out assembly.

Many people in class haven't used or even seen assembly before.   That
makes sense --- it's much lower level than C, error prone, not portable.

The reason we use it is both for the former and the latter:

  1. There are many machine-specific instructions, especially those
     that deal with privileged state, that have no corresponding high
     level construct.     If we need their functionality, we have to
     write assembly code.   

     This issue will frequently come up for us in virtual memory, where
     we use assembly code to flush TLB and caches.  C, nor any other
     mainstream language has a "flush TLB" operator,  so we have to
     issue it ourselves.

     For interrupts and threads it comes up because we need to save and
     restore registers (possibly including shadow, "banked" registers)
     in a way that the compiler won't really help us with (*).

  2. To guarantee that normally legal instructions do not occur in
     restrited circumstances.  For example, in the virtual memory
     labs:  the arm1176 CPU we use forbids the use of branch or return
     instructions during certain virtual memory updates.  If we write C
     code, we can't be absolutely sure that the compiler does not emit
     these --- with assembly we know exactly what is written.

  3. Occasionally for low-error or speed: the compiler 
     makes optimization decisions without awareness of your actual
     usage.  In rare cases you may want to tune every last cycle, either
     to reduce error rate or to get absolute speed.  (E.g., to switch
     a GPIO pin from low-to-high as close to 100ns as possible.)
     It's easy to waste hours fighting with the optimizer because small
     changes to the C code can lead it to make unfortunate decisions
     in emitted code.   In these cases it can be easier to just write
     everything in assembly (sometimes assembly you generate at runtime!).

     Note: an easy way to start this process is to write C, use `gcc -S`
     to convert it to assembly, and fuss with that.

In any case, you'll need to write some assembly here and there.  However,
because it's so different people often get stuck.  You can often avoid
paralysis-by-idk-what's-going-on by writing C code and either looking
at the machine code or running it to answer questions.  While these may
not be proofs, they are certainly potential counter-examples so even if
you believe you know what is going on, it's worth doing.


### Some examples of using c to figure stuff out.

We often have questions about which registers are callee or caller
(see the [callee/caller note](../callee-caller/README.md), where
arguments are passed, and where results are returned.  This is 
easy to figure out with C code.

For example:

 1.  What register is a non-float result returned in?   To figure
     this out, just write a simple routine that returns something and
     see which register it uses:

            % cat foo.c
            int foo(void) { return 1; }
            % arm-none-eabi-gcc -O2 -march=armv6 -c foo.c
            % arm-none-eabi-objdump -d foo.o
            00000000 <foo>:
                0:  e3a0000d    mov r0, #13
                4:  e12fff1e    bx  lr

     In this case: `r0`.

 2. What register are the first four arguments passed in?

            % cat args.c
            typedef unsigned u32;

            u32 args1(u32 arg1) {
                return arg1;
            }
            u32 args2(u32 arg1, u32 arg2) {
                return arg2;
            }
            u32 args3(u32 arg1, u32 arg2, u32 arg3) {
                return arg3;
            }
            u32 args4(u32 arg1, u32 arg2, u32 arg3, u32 arg4) {
                return arg4;
            }


            % arm-none-eabi-gcc -O2 -march=armv6 -c args.c
            % arm-none-eabi-objdump -d args.o
            00000000 <args1>:
                0:  e12fff1e    bx  lr

            00000004 <args2>:
                4:  e1a00001    mov r0, r1
                8:  e12fff1e    bx  lr

            0000000c <args3>:
                c:  e1a00002    mov r0, r2
                10: e12fff1e    bx  lr

            00000014 <args4>:
                14: e1a00003    mov r0, r3
                18: e12fff1e    bx  lr


    As you can see: the first argument is in `r0`, the second in `r1`,
    the third in `r2` and the fourth in `r3`.

  3. How do to a 32-bit or 8-bit load of a value at either a 
     constant or register offset from a base pointer?

            % cat offset.c
        
            typedef unsigned u32;
            typedef unsigned char u8;
            
            // load 32-bits at byte offset 16
            u32 ld32_off(u32 *ptr) {
                return ptr[16/4];
            }
            // load 32-bits at byte offset <off>
            u32 ld32_reg(u8 *ptr, u32 off) {
                return *(u32*)(ptr+off);
            }
            // load 8 bits at offset 16
            u8 ld8_off(u8 *ptr) {
                return ptr[16];
            }
            // load 8-bits at byte offset <off>
            u8 ld8_reg(u8 *ptr, u32 off) {
                return ptr[off];
            }

     And looking at the machine code:

            % arm-none-eabi-gcc -O2 -march=armv6 -c offset.c
            % arm-none-eabi-objdump -d offset.o
            00000000 <ld32_off>:
                0:	e5900010 	ldr	r0, [r0, #16]
                4:	e12fff1e 	bx	lr
            
            00000008 <ld32_reg>:
                8:	e7900001 	ldr	r0, [r0, r1]
                c:	e12fff1e 	bx	lr
            
            00000010 <ld8_off>:
                10:	e5d00010 	ldrb	r0, [r0, #16]
                14:	e12fff1e 	bx	lr
            
            00000018 <ld8_reg>:
                18:	e7d00001 	ldrb	r0, [r0, r1]
                1c:	e12fff1e 	bx	lr


     So, 32-bit uses `ldr` and 8-bit uses `ldrb` and both a constant
     and a register can be put in the bracketed index expression.
    

There's many other questions you'll likely need that you can imagine
answering --- how are structures passed,  does the stack grow up or down,
how to load or store multiple operations.  Or how to encode assembly
instructions as machine code so you can generate them at runtime.
Emitting and then examining is a pretty strong lens.  You can also
automate it for automatic reverse engineering.

In any case, if you get stuck or nervous about how to figure something out,
try taking a few runs at writing C code and seeing how ti gets translated
or how it runs.    Active exploration can easily beat thinking real hard,
especially when you have incomplete information.

As a note, if you like figuring this kind of stuff out, retargeting a
compiler is fun --- do all the integer ALU ops, then memory, then calling
conventions, then float.   Each problem is big enough to be interesting,
but not so big you get tired.  Pretty soon the sun comes up.
