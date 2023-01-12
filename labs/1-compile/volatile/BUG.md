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

        [*** all the initialization starts here *** ]

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

        [*** all the initialization ends here *** ]

        [this is where the assignment (store) occurs]
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

You get:

        ...
        mbox->write = ((unsigned)(&cp) | channel | 0x40000000);
        18:   e1831001        orr     r1, r3, r1
        1c:   e3811101        orr     r1, r1, #1073741824     ; 0x40000000
            cp.depth = 32;
        20:   e3a0c020        mov     ip, #32
            cp.pointer = 0;
        24:   e5832020        str     r2, [r3, #32]
     ==> this is the write to mbox->write
        mbox->write = ((unsigned)(&cp) | channel | 0x40000000);

     ==> all initializations afterwards occur after send!
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
