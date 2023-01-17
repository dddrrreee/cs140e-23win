## 0 example

Very simple makefile.  Manually specify and compile 
everything.  Advantages:
  - Explicit, manual dendencies = impossible to miss what is going on.
  - Just recompiing everything = don't need any `.o` rules etc.

To run:

        % make test

Should see:

        # main: rule
        cc a.c b.c c.c main.c -o main
        # test: rule
        ./main > out
        diff out main.ref
        makefile: program 'main' passed test

If we go though and touch each of the `.c` files, 
should see `main` recompiled:

        % touch a.c
        % make main
        # main: rule
        cc a.c b.c c.c main.c -o main
        % make main
        make: 'main' is up to date.


        % touch b.c
        % make main
        # main: rule
        cc a.c b.c c.c main.c -o main
        % make main
        make: 'main' is up to date.

        % touch c.c
        % make main
        # main: rule
        cc a.c b.c c.c main.c -o main
        % make main
        make: 'main' is up to date.

        % touch header.h
        % make main
        # main: rule
        cc a.c b.c c.c main.c -o main
        % make main
        make: 'main' is up to date.

