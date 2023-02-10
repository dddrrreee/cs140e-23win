### A quick and dirty guide to making a r/pi simulator.

The file `fake-pi.c` implements a trivial fake r/pi system that will
allow you to debug your `gpio.c` code on your laptop.  You can compile it
along with `gpio.c` and whatever driver you want to produce an executable
that you can run on your laptop.   (Note: there are some additional files
for implementing our own pseudo-random number generator so everyone will
have a consistent source of randomness.)

Before class:
   1. Run the code (see below) and see what it does.  You might get
      different results because your `gpio.c` has differences from ours.

   2. Look through `fake-pi.c` to understand how it works.  You should
      see how to extend it to other addresses as well as better interfaces
      to fake memory that makes it easier for fake devices to take over
      parts of the address space.

#### Compile and run the code

To compile:

        % cd 4-cross-check/1-code-fake-pi
        % cp ../../3-gpio/code/gpio.c 
        % make
        % ls
        1-blink  2-blink  3-input  ...


Note, you can also compile by hand if it's easier:

        # symlink to make easier
        % ln -s ../../1-gpio/code code  
        % gcc -g -Wall -Icode code/gpio.c code/1-blink.c  fake-pi.c -o 1-blink


You can then run the programs on your laptop.  They will print a sequence of
`PUT` and `GET` calls:

        % ./1-blink
        calling pi code
        ... initialization ...
        TRACE:1: GET32(0x20200008) = 0x66334873
        TRACE:2: PUT32(0x20200008) = 0x66334871
        TRACE:3: PUT32(0x2020001c) = 0x100000
        ... bunch of PUT32/GET32's ...
        TRACE:21: PUT32(0x2020001c) = 0x100000
        TRACE:22: PUT32(0x20200028) = 0x100000
        TRACE: pi exited cleanly: 7 calls to random


#### Testing

The directory `tests` has some simple tests, roughly broken up by difficulty. 
   - The tests with a `1-*.c` tests are the simplest and call a given
     routine a single time.  E.g., `1-gpio-set-input.c` calls
     `gpio_set_input` once.  These should be the easiest tests to ensure
     equivalence.

   - The `2-*.c` tests do multiple calls, possibly with illegal inputs.
   - Higher number tests are more complicated.


To run a given set of tests:
   0. Modify the `TEST_SRC`  variable in `tests/Makefile` to list out the
     tests.  E.g., to run all the `1-` tests:

            TEST_SRC := $(wildcard ./[1]-*.c)

     To run all the tests:

            TEST_SRC := $(wildcard ./*.c)

   1. `make run` will simply run the tests indicated by `TEST_SRC`.
   2. `make cksum` will run (just like 1) and compute a checksum (hash)
      on the output using the standard `cksum` Unix utility.  This makes it
      easier to compare your output to other people.
   3. `make emit` will execute like (1) but emit the results to output files 
      (program `foo` will generate `foo.out`).   You can then inspect more easily
      and (see below) check subsequent runs behave identically.
   4. `make check` will re-run and check against a previously emitted `out` file.
      This is very useful for checking that modifications that should not change
      functionality should not.

Workflow:
  1. make sure things compile and run by doing `make run`.
  2. start working through the first set of tests, making sure you get equivalant
    results with other people.
  3. `make emit` to save these results.
  4. Go to the next batch of tests  and repeat (2).  
  5. Then check that the older tests give you the same results.
  6. when you run all the test, compute a `cksum` of all the output files 
     to get a single number:

            % cksum *.out | cksum

#### Discussion

Some nice things:

    1. You can run the "pi" program using a debugger;
    2. You have memory protection, so null pointer writes get detected.
    3. You can run with tools (e.g., valgrind) to look for other errors.
    4. Finally: By comparing the actual put/get values can check your
       code against other people (today's lab).  If your sequence of is 
       identical to theirs, that proves your code is equivalant (on that run)
       to theirs, despite the two implementations looking very different.

While `fake-pi.c` is laughably simple and ignores more than it handles,
it can still get enough accuracy (fidelity) with how the code behaves
on the r/pi to be useful.  How:

   1. Most of your code is just straight C code, which runs the same on
      your laptop and the r/pi.
   2. While there is some ARM assembly (in `start.S`), which we cannot run
      it's small/simple enough we can replace it with C versions.
      If you look in `fake-pi.c` you'll see implementations of `PUT32`,
      `GET32` as well as the code that runs before `notmain`.
   3. The main pi specific thing we need to handle is what exactly
      happens when you read/write GPIO addresses.   Given how simple
      these are, we can get away with just treating them as memory, where
      the next read returns the value of the last write.  For fancier
      hardware, or for a complete set of possible GPIO behaviors we
      might have to do something more elaborate.  But for our GPIO usage,
      this is enough.

      NOTE: this problem of how to model devices accurately is a big
      challenge both for virtual machines and machine simulators.

It's good to understand what is going on here.  both why it works,
and when you can use this kind of trick in other places.

   - If you step back and think, one intersting thing leaps out
     immediately: we can transparently take code that you wrote explicitly
     to run bare-metal on the pi ARM cpu, and interact with the weird
     broadcom chip and run it on your laptop, which has neither, *without
     making any change!*

   - Another interesting hack: if you keep everything the same, but
     replace the `PUT32` and `GET32` with code that sends `PUT32` and
     `GET32` messages over to the pi using the UART, you can have a tiny
     driver on the pi wait for these and then act on them.  This lets you
     control the pi hardware "for real" just as you would when running
     on the pi, while still having all your code run on your laptop,
     with its nice environment.  

     We will do a hack like this in an upcoming lab.  It's especially cool
     when you do this over a network, and broadcast the messages to many
     pi's that operate in unison.  (E.g., for large light installations.)
