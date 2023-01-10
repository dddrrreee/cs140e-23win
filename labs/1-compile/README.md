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

  2. Write good makefiles for `examples-volatile` and `examples-pointer`.
     These should use wildcards and automatic dependencies.  After
     compiling a `.c` into a `.o` they should disassemble the result.



Required reading:
  - [simple concrete makefiles](http://nuclear.mutantstargoat.com/articles/make/).
    While the domain name is weird, the `make` examples are concrete,
    simple, useful.


--------------------------------------------------------------------
#### `git`

Useful reading:
  - [Wikipedia volatile]
    (https://en.wikipedia.org/wiki/Volatile_(computer_programming))
  - [How gcc treats volatile](https://gcc.gnu.org/onlinedocs/gcc/Volatiles.html).
  - Linux's [case against volatile]:
    (https://github.com/spotify/linux/blob/master/Documentation/volatile-considered-harmful.txt)

  - [Long thread on volatile and threads]
    (https://groups.google.com/g/comp.lang.c++.moderated/c/_O9XxTmkLvU).
  - Another [against volatile](https://sites.google.com/site/kjellhedstrom2/stay-away-from-volatile-in-threaded-code)
  - [Linux memory barriers](https://www.kernel.org/doc/Documentation/memory-barriers.txt).
