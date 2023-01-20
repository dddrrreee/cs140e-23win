NOTE:
    - For the hardware loopback test you will need to run a jumper between 20 and 21.

NOTE: These work similarly to the tests in `1-fake-pi`.

Different tests of your tracing, from simplest (start here) to most complicated:

  - `0-*.c`: simple tests that call one GPIO operation.  These are fast, 
    simple, easy to debug: get these right first!

  - `1-*.c`: trace N GPIO operations.   A bit more complicated.
  - `2-*.c`: even more complicated.
  - `progs-*.c`: complete programs (slowest).  Do these last.

If you run:

  - You can define the tests to run in `Makefile` by modifying the `TEST_SRC`
    variable to list all the tests you want to run.  You can use wildcards.
    For example, to run all of the `0-*.c` tests:


        TEST_SRC := $(wildcard ./0-*.c) 

  - `make runall`: will run all the tests you specified, with the output going to 
    the screen.
  - `make emitall`: will run all the tests you specified, with output prefixed
    with `TRACE:` being put in a `.out` file (`foo.bin` output will be in `foo.out`).
  - `make checkall`: will run all the tests you specified, and compare its output
    to that of the last `emitall` (stored in its `.out` file).

Workflow.  For each program, from easy to hardest:

  - Make sure it can run: `make runall`.
  - Emit the output files: `make emitall`
  - Run cksum on the result: `cksum *.out`
  - Compare to other people.
  - Repeat.
