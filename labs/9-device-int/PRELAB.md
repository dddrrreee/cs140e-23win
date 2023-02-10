## Lab 8: device interrupts

We're going to setup the pi so you can get GPIO interrupts, speed up
the interrupt handling code, and finish building your sw-uart.

----------------------------------------------------------------------
### ARM exception handler relocation.

Having exceptions starting at address 0 causes all sorts of problems.
The general ARMv6 documents talk about how you can relocate the handlers
to a fixed address in high memory, but this requires using virtual memory
(since we don't have enough physical to reach that high).  

However, it turns out that the `arm1176` provides a coprocessor
instruction to move these anywhere in memory.  I wish I had known about
this years ago!  Among other things it would make it easy to mark the
0 page as no-access, trapping null pointer reads and writes.  

We're going to implement this approach today.   

Read:
  - 3-121 in `../../docs/arm1176.pdf`: this let's us control where the
    exception jump table is!
  - In fact, whenever you get some time and inclination, I'd read this
    whole chapter (ideally print it out and use a pen) --- there are
    all sorts of interesting tricks you can play using the special
    instructions in it.

If you can, write (in assembly):
   - `void arm_vector_set(void *base)`: set the exception vector
     base to `base`.
   - `void *arm_vector_get(void)`: get the exception vector base.

To get you started doing inline assembly:
  - [simple introduction](http://199.104.150.52/computers/gcc_inline.html)
  - [gcc arm inline assembly cookbook](../../docs/ARM-GCC-Inline-Assembler-Cookbook.pdf)

----------------------------------------------------------------------
### Background reading for assembly / interrupts

For reading:
  1. `../../docs/hohl-book-interrupts.annot.pdf`: if you were confused
     about the interrupts, this is a good book chapter to read.
  2. `../../docs/subroutines.hohl-arm-asm.pdf`: this is a good review
     of ARM assembly as used by procedure calls: stack allocation, 
     caller and callee saved registers, parameter passing, etc.
  3. `../../docs/IHI0042F_aapcs.pdf`: this gives you a detailed view
     of the procedure call standard for the ARM.  

  4. Reread the note on [Using C to figure out useful things](../../notes/using-gcc-for-asm/README.md).  You will likely need these tricks.

----------------------------------------------------------------------
### GPIO Interface background

If you keep hacking on embedded stuff, the single most common activities
your code will do is (1) setup a hardware device and (2) then configure
device interrupts over GPIO pins (e.g., in response to setting them high,
low, transition, etc).  So today's lab will involve doing the latter:
set the GPIO pins you are doing SW-uart reception over to give you an 
interrupt when they go from low-to-high and also from high-to-low.  You
can use these transitions to detect when bits are being transmitted.

Before lab, implement four functions (prototypes are in `gpio.h` in `libpi`):

  - `gpio_int_rising_edge(pin)`: enabling rising edge detection for `pin`.  Page 97
    discusses it and the address to write to is on page 90.

  - `gpio_int_falling_edge(pin)`: enabling rising edge detection for
    `pin`.  Page 99 discusses it and the address to write to is on page 90.

  - `gpio_event_detected(pin)`: returns 1 if an event was detected (p96).

  - `gpio_event_clear(pin)`: clears the event set on `pin` (p96: write the same
    address as `gpio_event_detected`).

Don't panic.  These are pretty much like all the other GPIO functions
you've built: get an address, write a bit offset.   No big deal.  

However, just as with timer interrupts, enabling is a two-step process: we first
enable the specific GPIO interrupt(s) we care about using the first two routines
above, and then tell the pi that we care about GPIO interrupts generally.  If you 
look on page 113:
  - We want to enable `gpio_int[0]`, which is `49` in terms of general
    interrupts.  (we use `gpio[0]` since we are using a GPIO pin that
    is less than 32).

  - To set this, we bitwise-or a 1 to the `(49-32)` position in
    `Enable_IRQs_2` (which covers interrupts `[32-64)`).
     (see `libpi/includes/rpi-interrupts.h`)

This appears magic, especially since the documents are not clear.  But,
mechanically, it's just a few lines.   You should look at the timer
interrupt code to see how to set the general interrupts.

At this point you'll have interrupts enabled.

The interrupt handler works by checking if an event happened, and then using the 
`pin` value to determine if it was a rising or falling edge:
    
    if(a GPIO event was detected for pin) 
       if <pin> = 0, then
           it was a falling edge (a transition from high to low, i.e., 1 -> 0).  
       else 
           it was a rising edge (a transition from low to high).
      clear the event

As with timer interrupts, you need to clear the event that generated
the interrupt (using `gpio_event_clear`).

To make your code cleaner, I'd suggest using something like the following to
make it easier to do your `read-modify-write` of enabled pin events:

        static void or32(volatile void *addr, uint32_t val) {
            device_barrier();
            put32(addr, get32(addr) | val);
            device_barrier();
        }
        static void OR32(uint32_t addr, uint32_t val) {
            or32((volatile void*)addr, val);
        }

More complete discussion of the routines is in `gpio.h` but to make sure you
don't miss it the comments are cut-and-paste below:

    // gpio_int_rising_edge and gpio_int_falling_edge (and any other) should
    // call this routine (you must implement) to setup the right GPIO event.
    // as with setting up functions, you should bitwise-or in the value for the 
    // pin you are setting with the existing pin values.  (otherwise you will
    // lose their configuration).  you also need to enable the right IRQ.   make
    // sure to use device barriers!!
    int is_gpio_int(unsigned gpio_int);
    

    // p97 set to detect rising edge (0->1) on <pin>.
    // as the broadcom doc states, it  detects by sampling based on the clock.
    // it looks for "011" (low, hi, hi) to suppress noise.  i.e., its triggered only
    // *after* a 1 reading has been sampled twice, so there will be delay.
    // if you want lower latency, you should us async rising edge (p99)
    void gpio_int_rising_edge(unsigned pin);
    
    // p98: detect falling edge (1->0).  sampled using the system clock.  
    // similarly to rising edge detection, it suppresses noise by looking for
    // "100" --- i.e., is triggered after two readings of "0" and so the 
    // interrupt is delayed two clock cycles.   if you want  lower latency,
    // you should use async falling edge. (p99)
    void gpio_int_falling_edge(unsigned pin);
    
    // p96: a 1<<pin is set in EVENT_DETECT if <pin> triggered an interrupt.
    // if you configure multiple events to lead to interrupts, you will have to 
    // read the pin to determine which caused it.
    int gpio_event_detected(unsigned pin);
    
    // p96: have to write a 1 to the pin to clear the event.
    void gpio_event_clear(unsigned pin);

----------------------------------------------------------------------
### UART interrupt background.

For the extension doing UART interrupts --- you should
read the miniUART chapter again, looking for how to:
  0. Enable miniUART interrupts at all: you should look at how the timer code
     from last lab set things up --- this will be similar to how you do 
     the UART.
  1. Enable TX and RX interrupts;
  2. How to see if TX or RX was triggered (and under what conditions).
  3. How to clear TX or RX (note: TX is weird).

##### Various hints from past years:

A major thing that people seem to forget: Interrupts are pre-emptive
threads in that your code can context switch into the interrupt handler at
any time (unless you have disabled interrupts).  This causes two problems:

  1. If you are modifying state the interrupt handler will also modify,
     then you will get bad results.  For example, since your interrupt
     handler calls `cq_pop` on the putc circular queue, then you better
     not call `cq_pop` in your non-interrupt code unless you disable
     interrupts first.

  2. Since our interrupt handlers are not tuned, when they run they can
     add large timing overheads (1000+ cycles wouldn't be unexpected).
     Thus, when you do operations that require tight timings (such as
     your software uart), you probably need to have interrupts disabled.
     
Low level mistakes I've seen:
  1. People forget that `system_disable_interrupts` and
     system_enable_interrupts` are not "recursive" in that they do not
     record the nesting level.  The first enable will enable them:

        system_disable_interrupts();
        ...
        system_disable_interrupts();
        x++;
        system_enable_interrupts();  // <--- interrupts are now enabled!
        ...
        system_enable_interrupts();  

      You can (and we should) provide a way to nest interrupts, either
      using a counter, or by returning a flag on each call which you
      can then use to restore them to their previous state.

  2. If you look in the errata for page 12, the bits for `tx` and `rx`
     are reversed.

  3. We are using the mini-uart, not the full uart.  Thus, we need to enable
     the AUX interrupts (which control the mini-uart, as you recall from 
     your uart driver), which in the table on page 115 is 29.  This will
     require setting a bit in `Enable_IRQs_1`.

  4. The `TX` interrupt is a bit weird: once the hardware FIFO queue is
     empty, the `TX` interrupt will continue triggering until you either
     write a character to the fifo queue or disable `TX` interrupts
     entirely.

     The easiest approach seems to be: (1) disabling the `TX` interrupts
     in the interrupt handler if your `putc_int` circular queue is empty
     and (2) on every `uart_putc_int` re-enbling the `TX` interrupts
     *if* the queue is not empty after you flush as much of it as you
     can to the hardware FIFO.  MAKE SURE you do this non-empty check
     when interrupts are disabled or you will have a very difficult to
     figure out race condition.

  5. Your interrupt handler should call `uart_putc` and `uart_getc` to
     flush the characters since these already work.    You should also
     use loops to push as many characters as you can to the `TX` FIFO
     and from the `RX` FIFO.
