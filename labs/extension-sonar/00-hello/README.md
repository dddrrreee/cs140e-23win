
Trivial test to check that:
  1. Your `CS49N_2021_PATH` is defined correctly.
  2. Your `libpi` can build and works.

Note:
   - if you have to manually specify your usb device to `pi-install`,
     set `TTYUSB`  to the device path in `Makefile`.  E.g.,

            TTYUSB = /dev/ttyUSB0

To check run:

        # should complete without errors
        % make
        # if you have to specify your usb device, see below.
        % make run

        ...
        TRACE:hello world from the pi
        DONE!!!
    
        Saw done
    
        bootloader: pi exited.  cleaning up


If you run:

        % make check

The check should pass:

        checking <hello>:  Success: <hello.test> matched reference <hello.out>!

