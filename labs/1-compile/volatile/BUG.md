### Bug

We have been claiming that device bugs are intimittent and an interesting
example showed up when doing the lecture in the first class.

We had the following simplified code that tries to initialize the
r/pi framebuffer by:

  1. Filling in a message structe `cp`.
  2. After (1) completes: send the message by setting assigning its
     address to the mailbox `write` field.
     
We had the code:

```cpp

typedef struct {
  unsigned read;
  unsigned padding[3];
  unsigned peek;
  unsigned sender;
  unsigned status;
  unsigned configuration;
  unsigned write;
} mailbox_t;

typedef struct {
        unsigned width;
        unsigned height;
        unsigned virtual_width;
        unsigned virtual_height;
        unsigned pitch;
        unsigned depth;
        unsigned x_offset;
        unsigned y_offset;
        unsigned pointer;
        unsigned size;
} __attribute__ ((aligned(16))) fb_config_t;

#define MAILBOX_EMPTY  (1<<30)
#define MAILBOX_FULL   (1<<31)

static fb_config_t cp;

void write_mailbox_x(mailbox_t *mbox, unsigned channel) {
    // wait for mailbox to not be full.
    while(mbox->status & MAILBOX_FULL)
        ;

    // 1: initialize the message fields in <cp>
    cp.width = cp.virtual_width = 1280;
    cp.height = cp.virtual_height = 960;
    cp.depth = 32;
    cp.x_offset = cp.y_offset = 0;
    cp.pointer = 0;

    // 2. send the <cp> message by writing its address
    //    to mbox->write.
    mbox->write = ((unsigned)(&cp) | channel | 0x40000000);
}

```

In class the claim was that because `gcc` didn't know there was a device
ordering requirement between step 1 and step 2 that at higher optimization
levels some of the assignments to `cp` would trickle down below the send.
(This reordering is perfectly legal in C: modifying the contents of a
storage location does not change its address.)



However, when I ran our r/pi compiler and looked at the result the
claimed bug did not show up:  

        % arm-none-eabi-gcc -g -O3 -c 4-fb.c
        % arm-none-eabi-objdump -S -d 4-fb.o


     ====> all the initialization starts here.

        10:	e3a02020 	mov	r2, #32
            cp.width = cp.virtual_width = 1280;
        14:	e59f303c 	ldr	r3, [pc, #60]	; 58 <write_mailbox_x+0x58>
        void write_mailbox_x(mailbox_t *mbox, unsigned channel) {
        18:	e92d0030 	push	{r4, r5}
            cp.width = cp.virtual_width = 1280;
        1c:	e3a04c05 	mov	r4, #1280	; 0x500
        20:	e3a05d0f 	mov	r5, #960	; 0x3c0
            cp.depth = 32;
        24:	e5832014 	str	r2, [r3, #20]
            cp.x_offset = cp.y_offset = 0;
        28:	e3a02000 	mov	r2, #0
            cp.pointer = 0;

            mbox->write = ((unsigned)(&cp) | channel | 0x40000000);
        2c:	e1831001 	orr	r1, r3, r1
        30:	e3811101 	orr	r1, r1, #1073741824	; 0x40000000
            cp.width = cp.virtual_width = 1280;
        34:	e8830030 	stm	r3, {r4, r5}
        38:	e5834008 	str	r4, [r3, #8]
        3c:	e583500c 	str	r5, [r3, #12]
            cp.pointer = 0;
        40:	e5832020 	str	r2, [r3, #32]
            cp.x_offset = cp.y_offset = 0;
        44:	e583201c 	str	r2, [r3, #28]
     ====> all the initialization ends here.

     ====> this is where the assignment (store) occurs.
        48:	e5832018 	str	r2, [r3, #24]
            mbox->write = ((unsigned)(&cp) | channel | 0x40000000);
        4c:	e5801020 	str	r1, [r0, #32]

While embarassing, after some poking around, it is also interesting.

To jump over some steps, it turns out that if you reorder some of the
assignments and recompile the bug *does show up*.   My current guess
is that there is an interaction between register assignments and a low
level "peephole" optimization pass later in the compiler that reorders
instructions.

In any case, if you change the assignmnts from:

```cpp
  cp.width = cp.virtual_width = 1280;
  cp.height = cp.virtual_height = 960;
  cp.depth = 32;
  cp.x_offset = cp.y_offset = 0;
  cp.pointer = 0;
```


To:

```cpp
 cp.height = cp.virtual_height = 960;
 cp.x_offset = cp.y_offset = 0;
 cp.depth = 32;
 cp.width = cp.virtual_width = 1280;
 cp.pointer = 0;
```


And then run the compiler on the result (`5-fb.c`):


        % arm-none-eabi-gcc -g -O3 -c 5-fb.c
        % arm-none-eabi-objdump -S -d 5-fb.o

You get:


        mbox->write = ((unsigned)(&cp) | channel | 0x40000000);
        18:   e1831001        orr     r1, r3, r1
        1c:   e3811101        orr     r1, r1, #1073741824     ; 0x40000000
            cp.depth = 32;
        20:   e3a0c020        mov     ip, #32
            cp.pointer = 0;
        24:   e5832020        str     r2, [r3, #32]
     ==> this is the write to mbox->write
        mbox->write = ((unsigned)(&cp) | channel | 0x40000000);

     ==> BUG: all these initializations happen after send!
        28:   e5801020        str     r1, [r0, #32]
            cp.width = cp.virtual_width = 1280;
        2c:   e3a00c05        mov     r0, #1280       ; 0x500
        30:   e3a01d0f        mov     r1, #960        ; 0x3c0
            cp.x_offset = cp.y_offset = 0;
        34:   e583201c        str     r2, [r3, #28]
            cp.width = cp.virtual_width = 1280;
        38:   e8830003        stm     r3, {r0, r1}
        3c:   e5830008        str     r0, [r3, #8]
        40:   e583100c        str     r1, [r3, #12]
            cp.x_offset = cp.y_offset = 0;
        44:   e5832018        str     r2, [r3, #24]


Some points:

  1. This bug shows up because the device has stronger assumptions 
     about device "memory" than the C language requires (i.e., that
     all writes to the storage location complete before its address is
     assigned to a different device address).   
     
  2. This bug will be extraordinary intermitent.  Not only will it
     only be triggered erratically because its timing dependent,
     it will come and go with compiler versions and also come
     and go with different statement orderings.


#### How to fix?

The problem with the code above is that it has an invisible requirement
on memory stores: all stores to the `cp` message structure must complete
before its address is written to the `mbox->write` "message send" address.

It's not uncommon for programs to need a guarantee that memory access are
not re-ordered.  To prevent language level re-orderings,
most compilers will provide various "memory barriers".

For `gcc` this is an inline assembly statement:

        asm volatile ("" : : : "memory")

Where:

   - `asm` indicates this is "inline assembly" (you'll be using this
     in a bunch of places this quarter).
   - `volatile` tells `gcc` not to move this statement.
   - and the `memory` annotation states the empty assembly instruction `""`
     can read or write all memory.
     
Operationally, this gives two guarantees:
  1. No load or store above the statement can be pushed below it.
  2. No load or store below it can be pushed above it.

This is enough to guarantee our sequence.

Note:
  - It only prevents the compiler from reordering memory accesses
    across the barrier.

  - This barrier is *not* a hardware memory barrier. It *does not*
    guarantee that previous loads or stores complete at the hardware level
    (e.g., all the way to memory, or to other CPU caches etc)

    As one example: If you were sharing memory between threads on a modern
    multi-processor (i.e., multiple CPUs) you would need to do hardware
    memory barriers to ensure that the memory was synchronized between
    these CPUs.  Even if you used our compiler memory barrier to ensure
    that loads and stores would occur in order, modern hardware systems
    do not provide "sequential consistency" (where a load is guaranteed
    to get the value of the last store) but instead only give consistency
    after a (typically expensive) hardware memmory barrier of some kind
    is performed.


With all that said, we can fix this code using a barrier as follows:

```cpp
    cp.height = cp.virtual_height = 960;
    cp.x_offset = cp.y_offset = 0;
    cp.depth = 32;
    cp.width = cp.virtual_width = 1280;
    cp.pointer = 0;
    asm volatile ("" : : : "memory");
    mbox->write = ((unsigned)(&cp) | channel | 0x40000000);
```


And things get fixed:

        24:	e59f302c 	ldr	r3, [pc, #44]	; 58 <write_mailbox_x+0x58>
            cp.width = cp.virtual_width = 1280;
        28:	e8830030 	stm	r3, {r4, r5}
        2c:	e5834008 	str	r4, [r3, #8]
        30:	e583500c 	str	r5, [r3, #12]
            cp.x_offset = cp.y_offset = 0;
        34:	e583201c 	str	r2, [r3, #28]
        38:	e5832018 	str	r2, [r3, #24]
            cp.pointer = 0;
        3c:	e5832020 	str	r2, [r3, #32]
            cp.depth = 32;
        40:	e583c014 	str	ip, [r3, #20]
            asm volatile ("" : : : "memory");
            mbox->write = ((unsigned)(&cp) | channel | 0x40000000);
        44:	e1833001 	orr	r3, r3, r1
        48:	e3833101 	orr	r3, r3, #1073741824	; 0x40000000
        4c:	e5803020 	str	r3, [r0, #32]

Other methods:

  - You could also make the `cp` structure `volatile`.  (But note 
    this adds much more ordering requirements!)

  - You can use `put32` to store all the `struct` fields or write
    and assembly routine that took the `cp` address and stored it
    to `mbox->write`.

In any case: worth playing around with.  This is not a fake problem
and shows up whenever operating systems interact with devices.
