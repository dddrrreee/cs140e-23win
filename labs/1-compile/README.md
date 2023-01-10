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


--------------------------------------------------------------------
#### `volatile`

--------------------------------------------------------------------
#### `make`

Required reading:
  - [simple concrete makefiles](http://nuclear.mutantstargoat.com/articles/make/).
    While the domain name is weird, the `make` examples are concrete,
    simple, useful.

--------------------------------------------------------------------
#### `git`

--------------------------------------------------------------------
#### C traps

Getting `volatile` right and debugging mistakes is subtle enough that
except in very narrow cases where we need low error we do not use it and
instead call out to assembly code to do loads or stores to device memory.
However, doing a dive into the issues will give a big win in terms of
understanding more about what is going on below C code.

We have a fairly narrow concrete issue: when a variable is referenced,
what value(s) is it allowed to have?   At the level of machine code:
when you perform a load, what stores could appear?

Required reading:
  
  - John Regehr useful [volatile blog](https://blog.regehr.org/archives/28)
    writeup.

    He co-authored a [longer technical
    paper](https://www.cs.utah.edu/~regehr/papers/emsoft08-preprint.pdf)
    that is useful (but not required) to go through.


A related issue is how volatil

The pi you use has a single processor, and our main issue is coordinating


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
