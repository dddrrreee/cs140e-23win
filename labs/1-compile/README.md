## Volatile + Makefiles

Unlike all subsequent labs, our first two don't use hardware.  They should
give a good feel for whether the class works for you without requiring
a hardware investment.

  - Today's short lab focuses on what happens when you compile code.
    How to automate compilation with `make`.  How to see how the compiler
    translated your C code by examining the machine code it produced.
    And some some of the subtle rules for how the compiler can do this
    translation.  What we cover will apply to every lab this quarter.

  - The next lab will dive into subtle inductive magic tricks that
    arise when you compile a compiler using itself.  FWIW: Last year,
    this lab was by far the favorite in the class post mortem.

What to do:
  0. Check out the class repository from github.

  1. Read through [volatile writeup](volatile/README.md)
      and work through the `examples-volatile` and `examples-pointer`.


Checkoff:

  1. Write good makefiles for `examples-volatile` and `examples-pointer`.
     These should use wildcards and automatic dependencies.  After
     compiling a `.c` into a `.o` they should disassemble the result.

-------------------------------------------------------------------

### install the toolchain

For this class you need to compile bare-metal r/pi programs on your
computer, which is most likely not a bare-metal r/pi itself.  Thus we
need to set up the tools needed to `cross-compile` r/pi programs on
your computer and to r/pi binaries.

Install the toolchain:
   -  For a mac use the [cs107e install notes](http://cs107e.github.io/guides/install/mac/).  Note: do not
      install the python stuff.

   -  For [ubuntu/linux](https://askubuntu.com/questions/1243252/how-to-install-arm-none-eabi-gdb-on-ubuntu-20-04-lts-focal-fossa), ARM recently
      changed their method for distributing the tool change.   Now you
      must manually install.  As of this lab, the following works:

            wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2

            sudo tar xjf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C /usr/opt/

      Then either add symlinks to these:

            sudo ln -s /usr/opt/gcc-arm-none-eabi-10.3-2021.10/bin/* /usr/bin/

      Or, cleaner, add `/usr/opt/gcc-arm-none-eabi-10.3-2021.10/bin` to your
      `path` variable in your shell configuration file (e.g., `.tchsrc`
       or `.bashrc`), save it, and `source` the configuration.  When you run:


            arm-none-eabi-gcc
            arm-none-eabi-ar
            arm-none-eabi-objdump

      You should not get a "Command not found" error.


      You may also have to add your username to the `dialout` group.

      If gcc can't find header files, try:

           sudo apt-get install libnewlib-arm-none-eabi



### write good makefiles for `volatile/examples-*`

If you look at the makefiles in these subdirectories, they are awful. You will fix them.

Rewrite both `volatile/examples-pointer/Makefile` and `volatile/examples-volatile/Makefile` from scratch using the methods from:

  - [simple concrete makefiles](http://nuclear.mutantstargoat.com/articles/make/).

At the end of this lab, when you type `make` in either `volatile/examples-pointer` or `volatile/examples-volatile`, all `.c`'s should be compiled into `.o`'s and corresponding disassembled `.dis`'s should be generated. 

In particular:
  - Use wildcard patterns to get all the `.c` files from 
    the current directory and generate `.o` files from them.
  - Have a `CFLAGS`, `DIS` and `CC` variables so you can 
    switch between compiler, disassembly, and compiler options.
      - `CC` should be the name of your compiler (either `gcc`, `clang`, or `arm-none-eabi-gcc`)
      - `DIS` should be the name of your disassembler (either `objdump` or `arm-none-eabi-objdump`)
  - In the custom `%.o` rule you define, disassemble the resultant
    `.o`.
  - Use the built in make variables `$@` and and `$<`
  - Install the ARM compiler and make sure your makefile can correctly use it.

Note: `make` has some automatic rules to make `.o` files from `.c` files; make sure it's using your rules and not the built-in ones.

Additional reading:
  - [The wikipedia page gives a reasonable overview](https://en.wikipedia.org/wiki/Make_(software))
  - [cheatsheet](https://devhints.io/makefile)

Once you think you're done, please check in with one of the CA's (this is typical for labs--at the end of each lab you'll want to "check off" your work with a CA).
