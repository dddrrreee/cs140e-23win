### mini-UART cheat sheet

mini-UART Readings:
  1. Sections 1, 2, 2.1, 2.2, 6.2 (p 102) in the Broadcom document:
     `docs/BCM2835-ARM-Peripherals.annot.PDF` to figure out where, what,
     and why you have to read/write values.

     The main reading is pages 8---19.  I had to read the sections about
     10 times (not kidding).   It's do-able.  Lab will ensure we can
     get through it to a working program.

     Note that you need to setup the TX and RX GPIO pins; 6.2 says how.
  2. [errata](https://elinux.org/BCM2835_datasheet_errata) has the
     errata associated with the Broadcom.
  3. [8n1](https://en.wikipedia.org/wiki/8-N-1)
  4. [UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter).

#### Register cheat sheet 

All page numbers are from the Broadcom document.

AUX:
  - `AUXIRQ`: p 9.  Is interrupt pending?
  - `AUXENB`: p 9. Set (write a 1) using R-M-W.
    Until set, no access to peripheral.

mini-UART:
  - `IO`: p 11, tx, rx FIFO.
  - `IER`: p 12, turn off interrupts.
  - `IIR`: p 13, clear FIFOs.
  - `LCR`: p 14, data size, errata!
  - `MCR`: p 14
  - `LSR`: p 15, tx empty or has space, rx empty.
  - `MSR`: p 15, 
  - `SCRATCH`: 16
  - `CNTL`: 16, tx, rx enable.
  - `STAT`: 18, tx full, rx has data.
  - `BAUD`: 19.  Baud rate calculation: p 11.

#####  Incomplete Notes

General notes on initializing the mini-UART:

  1. You need to turn on the UART in `AUX`.  Make sure you 
     read-modify-write --- don't kill the `SPIm` enables.
  2. Immediately disable tx/rx: you don't want to send garbage while
     configuring.
  3. Figure out which registers you can ignore (e.g., `IO`, p 11).
     Many devices have many registers you can skip.
  4. Find and clear all parts of the UART state we use (e.g., FIFO queues)
     since we are not absolutely positive they do not hold garbage.
     In particular: we know the UART was used by the bootloader, so
     we don't know it's current state, other than it won't have it's
     reset values.
  5. Disable interrupts!  
  6. Configure: 115200 Baud, 8 bits, 1 start bit, 1 stop bit.  No flow
     control.
  7. Final step: Enable tx/rx.  It should be working!

Notes on sending (TX) and receiving (RX) data:
  - The smallest unit of sending and receiving is a single byte.
  - As is generally the case for hardware UART has fixed sized internal
    buffers for sending (TX) and receiving (RX) data.  In our specific case they
    are very small.
  - Sending (TX): a fixed-size TX buffer (called a FIFO since it's first
    in first out) means we must always first check to see if there
    is space before adding another byte.  If we don't we will
    non-deterministically lose data.

    Since we initially configure the UART to use a slow baud rate (much
    slower than the maximum rate the r/pi CPU can sustain) we will often
    be spinning, waiting for the TX fifo to get space.

  - Receiving (RX): similarly, a fixed size RX FIFO means we have to 
    be prompt about pulling data out of it --- running too slowly means 
    we will non-deterministically lose received bytes when a burst of 
    input arrives faster than we drain it.

  - For simplicity: today we will handle RX/TX by *polling* --- 
    explicitly looping and either waiting for RX data to arrive or 
    for TX space to open up.

    In a subsequent lab we will change to using *interrupts*.  With
    interrupts we configure the UART and the hardware to jump to a code
    location ("interrupting" whatever code is currently running) whenever
    an "interesting" event happens (e.g., space opens up on the TX FIFO,
    or data arrives on the RX FIFO).

    Most hardware will provide ways to get interrupts on interesting
    transitions.  As you manage more and more devices and/or have more 
    and more tasks to do, polling can have more and more problems and
    interrupts can become increasingly attractive.  (Think about your 
    own life.)

    However, its usually a good idea to use polling first for simplicity
    --- for example, it's much easier to test sequential code.  If your
    code has limited functionality, you can even leave it as a polling
    system.

Specific notes:

  1. As with the GPIO, the upper bits of the Broadcom addresses are
  incorrect for the pi and you will have to use different ones (for
  the mini-UART: `0x2021`).

  2.  We don't want: `RTS`, `CTS`, auto-flow control, or interrupts, so ignore
  these.

  3. You'll want to explicitly disable the mini-UART at the beginning to
  ensure it works if `uart_init(void)` is called multiple times (as can
  happen when we use a bootloader, or `reboot()`).

  4. Similarly, you will often want to fully enable the mini-UART's ability
  to transmit and receive as the very last step after configuring it.
  Otherwise it will be on in whatever initial state it booted up in,
  possibly interacting with the world before you can specify how it
  should do so.

  5. Since the device can read or write memory invisibly to the
  compiler, we'll manipulate it using `GET32` and `PUT32` (as we
  did with GPIO) rather than raw pointer manipulations.

  6. You'll notice the mini-UART "register" memory addresses are
  contiguous (table on page 8, section 2.1).  Rather than attempt to type
  in each register's address correctly, just make a large structure of
  `unsigned` fields (which are 32-bits on the pi), one for each entry
  in the table, and cast the mini-UART address to a pointer to this
  struct type.  Since you will be using `GET32` and `PUT32` (as before),
  you don't need `volatile`.

  7.  If a register holds received / transmit data you will almost
  certainly want to clear it before enabling the device.  Otherwise it
  may hold whatever garbage was in there at boot-up.

  8.  For `uart_tx_is_empty` you want it to be both empty and idle.

  9. p 10: "GPIO pins should be setup first before enabling the UART."

#### Questions.

Incomplete set of questions to test understanding (be careful about
the errata!):

  0. Give three different register / fields to check if there is at least
     one byte in the RX FIFO.
  1. Give three different register / fields to check if there is at
     least one byte of space in the TX FIFO.
  2. Write the expression to read a byte from the RX FIFO (assume
     you don't have to check if data is there)
  3. Write the expression to write a byte to the TX FIFO  (assume you
     don't have to check if there is space for more data).
  4. What value do you write to what register (and its address) to
     clear the RX and TX FIFOs?
  5. What register and value to write to set the UART data size to 8-bits.

  6. Compute the value to write to the `AUX_MU_BAUD` register to set
     the UART baudrate to 115200.  Show your work solving for this value.
     Assume a 250Mhz clock (250,000,000 cycles per second).
