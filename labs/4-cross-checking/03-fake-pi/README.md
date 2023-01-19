Simplest possible "fake pi"  where we just redefine printk and
clean_reboot in a private rpi.h  (`libpi-fake/rpi.h`) and include it
into a pi program.

Key thing:
  - you can take code written to run on an r/pi on bare metal and 
    redefine primitives so it can run on your laptop.

  - by running on your laptop you get memory protection, debugging,
    other tools.

Rough organization:

 - have one makefile to compile for the pi (`Makefile.pi`)

        % make pi
        ...
        bootloader: Done.
        listening on ttyusb=</dev/ttyUSB0>
        hello world
        DONE!!!
        
 - have one makefile to compile on unix (`Makefile.fake-pi`)

        % make fake-pi
        running our fake pi program
        ./hello.fake
        hello world
        DONE!!!
