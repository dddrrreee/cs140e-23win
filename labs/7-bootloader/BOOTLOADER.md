## The bootloader protocol.

At a high level, the protocol works as follows: 
  1. pi: spin in a loop, periodically asking for the program to run;
  2. unix: send the program code;
  3. pi: receive the program code, copy it to where it should go, and
     jump to it.
  4. To detect corruption, we use a checksum (a hash) that we can
     compare the received data too.

This is a stripped down version (explained more below):

     =======================================================
             pi side             |               unix side
     -------------------------------------------------------
      boot_put32(GET_PROG_INFO)+ ----->
      [resend every 300ms 
          if no response]

                                      boot_put32(PUT_PROG_INFO);
                                      boot_put32(ARMBASE);
                                      boot_put32(nbytes);
                             <------- boot_put32(crc32(code));

      boot_put32(GET_CODE)
      boot_put32(crc32)      ------->
                                      <check crc = the crc value sent>
                                      boot_put32(PUT_CODE);
                                      foreach b in code
                                           boot_put8(b);
                              <-------
     <copy code to addr>
     <check code crc32>
     boot_put32(BOOT_SUCCESS)
                              ------->
                                       <done!>
                                       start echoing any pi output to 
                                       the terminal.
     =======================================================

More descriptively:

  1. The pi will repeatedly signal it is ready to receive the program by
     sending `GET_PROG_INFO` requests every 300 milliseconds. 

     (Q: Why can't the pi simply send a single `GET_PROG_INFO` request?)

  2. When the unix side receives a `GET_PROG_INFO`, it sends back
     `PUT_PROG_INFO` along with three other 32-bit words: the constant
     to load the code at (for the moment, `ARMBASE` which is `0x8000`),
     the size of the code in bytes,  and a CRC (a collision resistant
     checksum) of the code.

     (Q: Why not skip step 1 and simply have the unix side start first
     with this step?)

     Since the pi could have sent many `GET_PROG_INFO` requests before
     we serviced it, the Unix code will attempt to drain these out.
     Also, when the Unix code starts, it discards any garbage left in
     the tty until it hits a `GET_PROG_INFO`.  
    
     IMPORTANT: the UART connection could have a bunch of garbage ---
     so do the initial reads to drain the connection using 8-bit reads,
     otherwise we could miss the true start of the protocol.

  3. The pi-side checks the code address and the size, and if OK (i.e.,
     it does not collide with its own currently running code) it sends a
     `GET_CODE` request along with the CRC value it received in step
     2 (so the server can check it).  Otherwise is sends an error
     `BAD_CODE_ADDR` and reboots.

  4. The unix side sends `PUT_CODE` and the code.

  5. The pi side copies the received code into memory starting at the
     indicated code address using `PUT8`(from step 2).  It then computes
     `crc32(addr,nbytes)` over this range and compares it to the
     expected value received in step 2.  If they do not match, it sends
     `BOOT_ERROR`.  If so, it sends back `BOOT_SUCCESS`.

  6. Once the Unix side receives `BOOT_SUCCESS` it simply echoes all
     subsequent received bytes to the terminal.

  7. If at any point the pi side receives an unexpected message, it
     sends a `BOOT_ERROR` message to the unix side and reboots.
     If at any point the Unix side receives an unexpected or error
     message it simply exits with an error.

The use of send-response in the protocol is intended to prevent the
Unix-side from overrunning the pi sides finite-sized UART queue.  The CRC
and other checks guard against corruption.

I would start small: do each message, check that it works, then do
the next.  If you go in small, verifiable steps and check at each point,
this can go fairly smoothly.  If you do everything all-at-once and then
have to play wack-a-mole with various bugs that arise from combinations
of mistakes, this will take awhile.

And don't forget: there are a bunch of useful error checking macros in
`libunix/demand.h` and uses in `libunix/*.c`.
