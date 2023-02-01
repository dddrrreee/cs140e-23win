## Checkoff tests

The final boss check: `make checkoff` in this directory should pass.
It only runs the two trace tests. You should make sure the regressions
pass before.  If they don't, your quarter will suck.

Different levels of tests:
  1. Simple regression: The two directories `0-lab4-tests`
     `0-lab5-tests` have the tests from labs 4 and 5.  You should be able
     to go into those directories and run `make check` and have it pass.

  2. `1-trace-control`: compares the bootloader control messages. `make check`
     should pass.
  3. `1-trace-all`: traces all bootloader messages (including the many bytes
     of code).  `make check` should pass.
  
As a comparison, when I run:

        % my-install --trace-control hello.bin

I get:

    TRACE:simple_boot: sending 3372 bytes, crc32=cf4943ae
    BOOT:waiting for a start
    TRACE:GET32:11112222 [GET_PROG_INFO]
    TRACE:PUT32:33334444 [PUT_PROG_INFO]
    TRACE:PUT32:8000 [UNKNOWN]
    TRACE:PUT32:d2c [UNKNOWN]
    TRACE:PUT32:cf4943ae [UNKNOWN]
    TRACE:GET32:55556666 [GET_CODE]
    TRACE:GET32:cf4943ae [UNKNOWN]
    TRACE:PUT32:77778888 [PUT_CODE]
    DEBUG:PRINT_STRING:pi sent print: <<STAFF>: success: Received the program!>
    TRACE:GET32:9999aaaa [BOOT_SUCCESS]
    BOOT:bootloader: Done.
    hello world
    DONE!!!
