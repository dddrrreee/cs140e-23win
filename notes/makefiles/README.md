## `make`

Simple `make` note.  Could use more prose, but does have 
examples.

### tl;dr: Summary

Each `*-example` directory has an example `Makefile` and a `README.md`
describing how it works.  Typing `make` in this directory should compile
and check all of the examples.  `make clean` should delete them.

Examples:
  - [0-example](0-example): simple, hard-coded Makefile that compiles
    entire program if anything changes.
  - [1-example](1-example): Slightly more fancy Makefile --- rewritten so that we
    only compile those `.c` that have changed or include a header that
    has changed.
  - [2-example](2-example): add a pattern-matching rule to make our code slightly
    more automatic and use the `$<` and `$@` variables.
  - [3-example](3-example):  use wildcards to automatically gather up all source
     and `make` it.  We avoid the use of dependencies by also using
     wildcards to get all header files and stating that every source
     file depends on all header.
  - [4-example](4-example): use the `-MMD` flag to automatically generate
    dependencies rather than hardcode a bunch of stuff (which can go
    wrong in bad way
  - [5-example](5-example): now gather source from many directories and 
     put the generated `.o` and `.d` files in a private build 
     directory.

  - [7-example](7-example): we go entirely the opposite direction and 
    make the simplest possible cross-compiling makefile that just always
    remakes everything.  you should definitely understand everything 
    in it.

### Intro

As projects get large, manually compiling code is a pain.   The Unix
`make` program is one of the oldest ways to automate this process.
`make` has a deserved reputation as an unusually ugly and baroque
input language.  However, it exists everywhere, is used many places,
and has enough power embedded inside that you can often make it do what
you want --- compiling based on what changes with automatic depencies,
producing muiltiple programs or libraries and placing them in
different locations, automatically running regression tests, etc.

We use `make` alot.  Many places you might work or found will as well.
Despite its baroque weirdness, there is a narrow  slice that will do
most of what you need.  We cover much of that slice today by doing
increasingly fancy varions of a `Makefile` that does two things:

  1. Produces an executable file `main` by compiling the C source
     files `a.c`, `b.c`, `c.c`, `main.c`.

     Our makefile structure is driven by two correctness rules.

     Correctness rule 1: any source file must be recompiled if
     it changes.  The `main` must also be re-generated.
  
     Correctness rule 2: All of the example files `#include` a header file
     `header.h` and so must be recompiled if `header.h` changes.

     In general, a source file must be recompiled if anything it depends
     on changes.  For today we only worry about the header `header.h`.
     Of course, the makefile itself is a dependency.  

     In the limit, the operating system installation, compiler
     (assembler, linker, preprocessor) or libc implementations can be
     seen as dependencies as well ---- in some cases you may want to
     recompile if any of these changes.  We do not do this today.

  2. After producing the `main` executable, check that running `main`
     gives the same result as a reference output `main.ref`.

Required reading:
  - [make wikipedia page](https://en.wikipedia.org/wiki/Make_(software)).
  - [simple concrete makefiles](http://nuclear.mutantstargoat.com/articles/make/).
    While the domain name is weird, the `make` examples are concrete,
    simple, useful.

If you understand the required reading, feel free to jump to part 4 and
just implement it.  (You should be able to answer the boldfaced questions
in the other parts.)

---------------------------------------------------------------------------
### 0. Example: A simple-minded makefile: `0-example/Makefile`

As a warm-up, our first makefile in `0-example` hard-codes all dependencies.
If you type:

        % make Makefile

You should get:

        # main: rule
        cc a.c b.c c.c main.c -o main
        # test: rule
        cc a.c b.c c.c main.c -o main
        ./main > out
        diff out main.ref
        makefile: program 'main' passed test
        rm -f out

Stripping out most comments from `0-example/Makefile`:

```make
    # Makefile
    all: main test

    main: a.c b.c c.c main.c header.h
        # the main rule
	    $(CC) a.c b.c c.c main.c -o main

    .PHONY: clean test

    test:
        # test: rule
	    ./main > out
	    diff out main.ref
	    @echo "makefile: program 'main' passed test"
	    rm -f out

    # cleanup remove outputs and temporary files
    clean:
	    rm -f main out *~ *.bak
```

This `Makefile` has four rules:

  - `all:` --- this rule is the first rule in `Makefile`, so is what 
    `make`  will run by default, including any rules it
    recursively depends on.  This first rule does not need to be called `all`.
    Ours states it depends on `main` and `test` rules so `make` will
    also run these rules (in that order).  

  - `main:` --- this rule makes the `main` program by invoking the 
    default compiler (held in the `CC` variable).   It is what we
    will refine in most of the rest of this document.

        main: a.c b.c c.c main.c header.h
	        $(CC) a.c b.c c.c main.c -o main

    It explicitly enumerates all dependencies (`a.c`, `b.c`, `c.c`,
    `main.c` and `header.h`) and will re-execute if any of the
    dependencies change.

  - `test:` --- this rule runs `diff` to compare the result of `main` to
    `out.ref`.

  - `clean:` --- most makefiles define this rule, which conventionally
    removes various automatically produced files.  Typically it is the
    inverse of running `make` by deleting anything `make` produces.
    It often removes any files temporary produced by your editor.
    It does not need to be called `clean`.

Both `test` and `clean` don't produce any ouput file, so we tell `make`
they are `PHONY` targets.   (See the `make` 
[manual pages for why](https://web.mit.edu/gnu/doc/html/make_4.html#SEC31)).
(The main reason we do this is that you'll get weird behavior if there
is a `test` or `clean` file or directory.)

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


  3. The `@` as part of the `test:` rule suppresses output.   Change
     the rule so it only prints if the test passed or failed.

  4. If you do:

            % make -f Makefile.0 clean
            % make -f Makefile.0 test

     What happens and why?  Fix it!

---------------------------------------------------------------------------
### 1. Simple-minded makefile: `1-example/Makefile`

In the previous makefile, we always recompiled all `.c` files even
if only one changed.  For a small project, this doesn't matter
(and does make the makefile simple).  However, for big projects you
don't want to regenerate everything but instead will recompile just
what you need.  A first, sort-of dumb method:


```make
		# Makefile
        all: main  test

        main: a.o b.o c.o main.o 
	        $(CC) a.o b.o c.o main.o -o main
        a.o: a.c header.h
	        $(CC) -c a.c -o a.o
        b.o: b.c header.h
	        $(CC) -c b.c -o b.o
        c.o: c.c header.h
	        $(CC) -c c.c -o c.o
        main.o: main.c header.h
	        $(CC) -c main.c -o main.o

        include common.mk
```

This will re-compile any source file whose dependency changed,
producing an object file (e.g., `a.c` becomes `a.o`).
It will then re-generate `main` using only the `.o` files.
So we don't hvae to keep including the rest of the file, we just
include `common.mk`.

What to do:
  1. Rewrite this makefile so that it uses pattern rules  and there is
     a single rule for how to produce a `.o` from a `.c` file.

     Use the built-in variables `$@` and `$^` --- you can print these
     out using the `echo` command.

---------------------------------------------------------------------------
### Look in the rest of the directories

We're out of time, so you'll have to look in the example directories.
