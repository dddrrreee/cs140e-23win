

.globl put8
.globl PUT8
put8:
PUT8:
    strb r1,[r0]
    bx lr

.globl get8
.globl GET8
get8:
GET8:
    ldrb r0,[r0]
    bx lr


