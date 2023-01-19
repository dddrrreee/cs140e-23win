This shows an example of how to use the GNU linker `ld` to interpose on
function calls by using the `--wrap` flag.

We will use this in lab to interpose on `GET32` and `PUT32`:

  1. When linking the a program, add a `--wrap=GET32` argument to the linker command.
  2. The linker will replace all calls to `GET32` with calls to `__wrap_GET32`.  
  3. The linker will also rename the definition of `GET32` as `__real_GET32`.

Then, in order to trace all `GET32` operations you would implement a
`__wrap_GET32(unsigned addr)` routine which calls `__real_GET32(addr)`
to get the value for `addr` and prints both.

The example overrides the definition of `foo` in `trace-foo.c`:

  1. `Makefile` uses the `--wrap=foo` flag to override `foo`.
  2. `trace-foo.c` defines a `__wrap_foo` that calls `__real_foo`.

If you run this on your pi it should print:

        % pi-install trace-foo.bin 
        .. a bunch of stuff ...
        about to call foo(1,2)
        real foo: have arguments (1,2)
        returned  3
        DONE!!!
