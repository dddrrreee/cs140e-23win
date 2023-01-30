Finish implementing the code in:
   - `get-code.c`, which has the routine `get_code` that does the actual loading.
     This is the only file you'll have to modify:

You'll have to look at:
   - `get-code.h`, which has a bunch of helpers.

   - `simple-boot.h`: the values used for the protocol (refer to the lab's `README` for
      how to use them). 
   - `staff-crc32.h`: code to compute the checksum.  You'll have to call this.
   - `main.c`: the trivial driver to call `get_code`.

The other files are from us:
   - `boot-start.S`:  look at how it differs from `libpi:staff-start.S`.
