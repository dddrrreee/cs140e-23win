## `make`

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
### 0. Example: A simple-minded makefile: `Makefile.0`

As a warm-up, our first makefile `Makefile.0` hard-codes all dependencies.
If you type:

        % make -f Makefile.0

You should get:

        # main: rule
        cc a.c b.c c.c main.c -o main
        # test: rule
        cc a.c b.c c.c main.c -o main
        ./main > out
        diff out main.ref
        makefile: program 'main' passed test
        rm -f out

Stripping out most comments from `Makefile.0`:

```make
    # Makefile.0
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

  - `all:` --- this rule is the first rule in `Makefile.0`, so is what 
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
### 1. A simple-minded makefile: `Makefile.1`
