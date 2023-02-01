@ based on dwelch's start for bootloader.

// To keep this in the first portion of the binary.
.section ".text.boot"

@ this is the whole purpose for this <start.S>: so we can 
@ have a gap at the start so we can copy the sent code and
@ not have it hit the bootloader code.
.globl _start
_start:
    b skip
.space 0x200000-0x8004,0
skip:
    mov sp,#0x08000000
@    bl notmain
    bl _cstart
hang: b rpi_reboot

@ if we don't pull these in, we get conflicts.

.globl put32
.globl PUT32
put32:
PUT32:
    str r1,[r0]     @ store r1 into address held in r0
    bx lr           @ return

@ get32(addr)
@
@ c-code equivalent:
@   - get32(addr) { return *(uint32_t*)addr; }
@
@ load value of <addr> passed in first argument register (r0).
@
.globl get32
.globl GET32
get32:
GET32:
    ldr r0,[r0]     @ load address held in r0 into r0
    bx lr           @ return

@ cannot overwrite this code or anything after it --- make sure
@ sure you check  that the code sent to the bootloader
@ does not collide with the address of <BRANCHTO>
.globl BRANCHTO
BRANCHTO:
    bx r0


.globl put8
.globl PUT8
put8:
PUT8:
    strb r1,[r0]
    bx lr

