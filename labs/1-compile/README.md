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

### write good makefiles for `volatile/examples-*`

If you look at the makefiles in these subdirectories, they are awful.
Rewrite them from scratch using the methods from:

  - [simple concrete makefiles](http://nuclear.mutantstargoat.com/articles/make/).

In particular:
  - Use wildcard patterns to get all the `.c` files from 
    the current directory and generate `.o` files from them.
  - Have a `CFLAGS`, `DIS` and `CC` variables so you can 
    switch between compiler, disassembly, and compiler options.
  - In the custom `%.o` rule you define, disassemble the resultant
    `.o`.
  - Use the built in make variables `$@` and and `$<`
  - Install the ARM compiler and make sure your makefile can correctly use it.

Additional reading:
  - [The wikipedia page gives a reasonable overview](https://en.wikipedia.org/wiki/Make_(software))
  - [cheatsheet](https://devhints.io/makefile)

