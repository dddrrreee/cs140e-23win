## 1-example

Slightly more fancy Makefile --- rewritten so that we only compile those
`.c` that have changed or include a header that has changed.

Two behavior changes:

  - If you `touch` an individual `.c` only that file will be compiled and
    then the `main` program produced by linking all the `.o`:

            % make main
            make: 'main' is up to date.
            % touch a.c
            % make main
            cc -O -Wall -c a.c -o a.o
            cc a.o b.o c.o main.o -o main

  - If you `touch` the `header.h` then everything will be recompiled:

            % make main
            make: 'main' is up to date.
            % touch header.h
            % make main
            cc -c main.c -o main.o
            cc -O -Wall -c a.c -o a.o
            cc -c b.c -o b.o
            cc -c c.c -o c.o
