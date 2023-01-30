Simple unix-side `bootloader`:
  1. Driver is in `my-install.c`.
  2. Your code will go in `simple-boot.c`.
  3. You should be able to send over `hello.bin` and see a "hello world" printed out:
     
        my-install hello.bin

Useful routines in `libunix`:
  - send / receive data to and from the pi: `put_uint8`, `put_uint32`, 
    `get_uint8`, `get_uint32`.

  - ttyUSB routines: `open_tty`, `find_ttyusb` (you will build the later).
