@ this location is linked at 0x8000 using the trivial linker 
@ script <memmap>
@
@ this address is defined in the rpi <config.txt> file as
@ the start location (you can change it) that you 
@ copied onto the SD card.
@ 
@ if you look at blink.list (which is a disassembled version of
@ blink.bin) you can verify that this is true.
@
@ trivial trampoline to start the "C environment" --- in our case,
@ just loading an address of physical memory into the stack pointer
@ register and jumping to <notmain>.
.globl _start
_start:
    mov sp, #0x8000000 
    bl notmain
hang: b hang

@ trivial routine to write a 32-bit passed as the second argument
@ (register r1) to the address passed as the first argument (register 
@ r0)
.globl put32
.globl PUT32
put32:
PUT32:
    str r1,[r0]
    bx lr

@ trivial routine to do a 32-bit load of the address passed as
@ the first argument.
.globl get32
.globl GET32
get32:
GET32:
    ldr r0,[r0]
    bx lr

@ do nothing: put the call here so the compiler cannot optimize.
.globl nop
nop:
    bx lr
