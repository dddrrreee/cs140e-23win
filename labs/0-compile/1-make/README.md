## `make`

As projects get large, manually compiling code is a pain.   The Unix
`make` program is one of the oldest ways to automate this process.
`make` has a deserved reputation as an unusually ugly and baroque
input language.  However, it exists everywhere, is used many places,
and has enough power embedded inside that you can often make it do what
you want --- compiling based on what changes with automatic depencies,
producing muiltiple multiple programs, libraries, and placing them in
different locations, running regression tests etc.

We use `make` alot.  Many places you might work will as well.  While it
is huge and sprawling,there is a narrow  slice that will do most of what
you need.  We cover most of that slice today by doing increasingly fancy
varions of a `Makefile` that does two things:

  1. Produces an executable file <main> by compiling the C files `a.c`,
     `b.c`, `c.c`, `main.c`.

     Correctness rule 1: All of these files include a header file
     `header.h` and so must be recompiled if `header.h` changes.

     Correctness rule 2: if any C file changes, or anything it depends
     on changes, it must be re-compiled and `main` regenerated.

     Note: we only worry about the header.   Of course the Makefile
     itself is a dependency.  Strictly speaking, the operating system
     installation, compiler (assembler, linker, preprocessor) or libc
     implementations can be seen as dependencies as well ---- in some
     cases you may want to recompile if any of these changes.  We do
     not do this today.

  2. After producing the `main` executable, checking that running `main`
     gives the same result as a reference output `main.ref`.

Required reading:
  - [simple concrete makefiles](http://nuclear.mutantstargoat.com/articles/make/).
    While the domain name is weird, the `make` examples are concrete,
    simple, useful.


If you understand the required reading, feel free to jump to part 4
and just implement it.  (You should be able to answer the questions
in the other parts.)

---------------------------------------------------------------------------
### 0. A simple-minded makefile: `Makefile.0`

Our first makefile `Makefile.0` hard-codes all dependencies.  Stripping
out most comments:

```make
    # Makefile.0
    all: main test

    main: a.c b.c c.c main.c header.h
	    $(CC) a.c b.c c.c main.c -o main

    .PHONY: clean test

    test:
	    ./main > out
	    diff out main.ref
	    @echo "makefile: program 'main' passed test"
	    rm -f out

    # cleanup remove outputs and temporary files
    clean:
	    rm -f main out *~ *.bak
```

This `Makefile` has four rules:

  - `all` --- this rule is the first rule in the makefile so is what the
    only rule `make`  will run by default, including any rules it
    recursively depends on.  `make` will ignore all other rules (e.g.,
    `clean`).    This first rule does not need to be called `all`.
    Ours states it depends on `main` and `test` rules so `make` will
    also run these rules (in that order).  

  - `main` --- this rule makes the `main` program by invoking the 
    default compiler (held in the `CC` variable).   It is what we
    will refine in most of the rest of this document.

        main: a.c b.c c.c main.c header.h
	        $(CC) a.c b.c c.c main.c -o main

    It explicitly enumerates all dependencies (`a.c`, `b.c`, `c.c`,
    `main.c` and `header.h`) and will re-execute if any of the
    dependencies change.

  - `test` --- this rule runs `diff` to compare the result of `main` to
    `out.ref`.

  - `clean` --- this common rule removes various automatically produced
    files.  It does not need to be called `clean`.  The general behavior
    is to be the inverse of running `make` by deleting anything `make`
    produces.  It typically includes any files produced by your editor.
    If your editor produces others, add them!

Both `test` and `clean` don't produce any ouput file, so we tell `make`
they are `PHONY` targets.   (See the `make` 
[manual pages for why](https://web.mit.edu/gnu/doc/html/make_4.html#SEC31)).
(The main reason we do this is that you'll get weird behavior if there
is a `test` or `clean` file / directory.)

***Questions***:

  1. What happens if we changed the `all` rule as follows?

        all: test main

     When does this work the same?  Differently?

     What happens if we changed it as follows?

        all: main


  2. What happens if you delete some or all of the `main` rule
     dependencies?  For example:

        main: 
	        $(CC) a.c b.c c.c main.c -o main

     When will this work?  What is an example of when it will break?


