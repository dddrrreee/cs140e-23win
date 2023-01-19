This is a simple test that you have your `CS140E_2023_PATH` path defined.

It demonstrates both our usual `Makefile` (you just define a few variables
and it does all the work) as well as different debuggng macros from
`libpi/libc/demand.h`.

if you type `make` it should run all the programs.

  - hello.c	

            hello world
            DONE!!!

  - test-interfile.c

            about to call foo()
            foo.c:foo:5:in foo
            DONE!!!

  - test-assert.c  

            test-assert.c:notmain:4:assert should fail:
            test-assert.c:notmain:6:ERROR: Assertion `x < 4` failed.

  - test-demand.c  

            test-demand.c:notmain:4:demand should fail
            test-demand.c:notmain:6:ERROR: Demand `x < 4` failed: testing demand: this should fail

  - test-panic.c

        PANIC:test-panic.c:notmain:4:checking that panic works: should die



