# The ARM Thumb Instruction Set

**Note:** This is all **optional**, and not part of checkoff. If you finish lab
early and want to experiment some more, you can try this.

Many instruction sets, including ARM, support "compressed" instructions as
a way to reduce code size. On ARM, this takes the form of the "Thumb"
instruction set, an alternate instruction set which each instruction is only 16
bits (2 bytes) long, as opposed to the usual 32 bits (4 bytes). As you might
imagine, the Thumb instruction set is much smaller and much less powerful than
the full ARM instruction set (after all, there are 65536x fewer possible
instructions); however, it's still good enough for many things. You can see
a full listing of supported Thumb instructions on page 1-44 of the
[ARM1176JZF-S documentation](../../docs/arm1176.pdf).

Note that, if you're using Google to search for more info, you'll want to look
up "Thumb 1". There's a newer version called "Thumb 2", but our Pis don't
support them.

ARM and Thumb are considered different instruction sets, so you need to:

1. Tell the assembler to emit a certain function as Thumb instructions, rather
   than ARM instructions.
2. Tell the CPU you want to switch states to Thumb when you're calling a Thumb
   function, or back to ARM when you're calling an ARM function.

   Note that a "state" is distinct from a "mode". The former deals with
   instruction sets, the latter deals with permissions.

Luckily, when you're writing C code, the compiler will take care of most of
this for you.

## Thumb Limitations

Thumb's two main limitations are:

1. A smaller set of valid instruction opcodes; most basic arithmetic
   instructions are there, as are memory loads/stores, but some less-used
   instructions like coprocessor accesses aren't possible in Thumb.
2. A smaller set of registers. As described on page 2-21 of the ARM1176JZF-S
   manual, the registers `{r0-r7, sp, lr, pc}` and the CPSR are usable in
   Thumb. The other registers are accessible indirectly, using special `MOV`,
   `CMP`, or `ADD` instructions, but can't be used for most instructions.

## Declaring a Thumb Function

### Assembly

In assembly, you need to provide a few directives to the assembler to tell it
to generate a Thumb function.

First, you need to switch the assembler into the Thumb state:

```gas
.thumb
```

Once you've switched states, every instruction will be emitted as a Thumb
instruction (and you'll get an assembler error if you try to use an instruction
which doesn't exist in Thumb). You can define multiple Thumb functions in a row
after using `.thumb` once. You can switch back to the ARM state using `.arm`.

Second, you need to tell the _linker_ that you're declaring a Thumb function.
This is because the linker needs to emit different instructions to call a Thumb
function than an ARM function (see below).

You can declare a thumb function as follows:

```gas
.thumb_func
```

For example, a Thumb implementation of memcpy could look like this:

```gas
.thumb                     ; switch the assembler to thumb state
.thumb_func                ; declare a thumb function
.globl thumb_asm_memcpy    ; make the function global (callable from C)
thumb_asm_memcpy:          ; actually define the function
  push {r4}
  mov r3, r0
  mov r4, r1
  b 1f
  .align 4
0:
  ldrb r1, [r4]
  strb r1, [r3]
  add r3, #1
  add r4, #1
1:
  sub r2, #1
  cmp r2, #0
  bpl 0b
  pop {r4}
  bx lr
```

### C

Declaring a Thumb function from C is a lot easier.

```C
__attribute__((target("thumb")))
void *thumb_c_memcpy(void *p, void *q, int n) {
  for (int i = 0; i < n; i++) {
    ((char *)p)[i] = ((char *)q)[i];
  }
  return p;
}
```

I'd recommend defining this macro to make things less verbose:

```C
#define THUMB __attribute__((target("thumb")))
```

## Calling a Thumb Function

**TLDR:** Just call the function normally! The linker figures it all out for
you. Just make sure to label your assembly thumb functions with `.thumb_func`.

Internally, calling a Thumb function from assembly is slightly different than
calling an ARM function. Normally, we'd do a `bl` or `blx` to the address of
the function; however, that wouldn't work because the CPU would still be in the
ARM state. Instead, we do a `bl` or `blx` to `address | 1`. For example, if
a function was defined at address `0x84b0`, we'd jump to address `0x84b1`.
Since this address is invalid, the CPU switches to the Thumb state and goes to
address `0x84b0`.

However, `gcc` (and `ld`) are smart, and deal with all of this internally! You
can just call a Thumb function as usual from ARM state (or vice-versa), and the
linker will automatically adjust the code for you. It does this by calling an
auto-generated "trampoline", named like `__thumb_memset_from_arm`, which does
the adjustment and calls the correct Thumb function. This all happens
automatically, so you don't need to worry about it at all when coding, but you
might see a slight performance hit due to the extra function call.

## Things to Try

1. Look at a `.list` file including Thumb code, and look at the disassembly of
   a Thumb function. How is it different than a C function.
2. Write the same C function twice, but mark the second one as a Thumb
   function. How are the two versions different in the `.list` file? Which one
   is more instructions? Which is more bytes?
3. Using Thumb, write the smallest possible blink program in bytes. Try
   in either C or assembly.
