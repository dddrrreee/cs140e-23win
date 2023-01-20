
What to do:

   0. Set your `CS140E_2023_PATH` variable (see below).  You should also
      look through the `libpi` that's been added to the top level of
      your directory.  it provides a small number of C library routines
      (`libpi/libc`) some pi specific routines (`libpi/staff-src`)
      and some staff object files.  You'll be adding your code to this
      library over the quarter so it gets more interesting.

   1. `00-hello`: compile and make sure this code runs on your pi.
      look through the .c files to see how to use `demand` `assert` etc.

   2. `01-tracing-pi`: run and look through this code.  it's a
      simple example of how to override routines using the linker.
      You will use this to trace the PUT/GET calls when run on the
      actual pi hardware and do the same comparison to everyone else,
      and compare to when you replace our staff `gpio.o` with your own.

   3. `02-tracing-linux` if you're on linux you can also run similar code
      for step 2.  AFAIK MacOS's linker  does not support this option.

   4. `03-fake-pi`: this shows increasingly fancy examples of taking
      r/pi code and tricking it into running on your laptop (i.e.g.,
      not the pi hardare) by making fake versions of different r/pi
      library routines.

      It is setup to run both on the pi and your laptop.   You should
      look through the subdirectories (`0-libpi-fake` `1-libpi-fake`
      `2-libpi-fake`) and try running them by changing the
      `03-fake-pi/Makefile` to use the different version.

   5. `code-fake-pi` : a more full fledged but still fairly
      trivial user-level emulator for your pi code.  You will use this
      approach to cross-check your implementation against everyone else's
      to ensure everyone is equivalant.  Thus, if one person is correct,
      everyone is correct.


If you're going to modify these, please copy the directory --- I'm
still modifying and playing around with the code, so you'd get conflicts
otherwise.


To pop up a few levels, the raw tracing of PUT/GET enables some
interesting tricks:

  1. Check fidelity of your fake-pi emulator:  You can store the PUT/GET
     calls emitted from a raw run to a file (a log) and then change the
     fake pi so it optionally reads from this log: when you replay them
     the result should be identical.  (Including when you log the PUT/GET
     done during the replay, emit that to a log, replay that, etc.)

  2. Partial evaluation: You can also write a program that reads this
     log and emits a pi program that executes just these exact calls ---
     you can use this trick to replace the original program them with this
     (often much simpler) low-level sequence of PUTs and GETs.

-----------------------------------------------------------------
#### Part 0: Setup your  CS140E_2023_PATH variable.

Similar to how you added your local `~/bin` directory to your shell's path
on the first lab, today you're going to edit your shell configuration file
(e.g., `.tcshrc`, `.bash_profile`, etc) to set set an environment variable
`CS140E_PATH` to contain where you have your repository.

E.g., for me, since I'm running `tcsh` I edit my `.tcshrc` file and add:


    setenv CS140E_2023_PATH /home/engler/class/cs140e-23win/

To the end of it.  If you use `bash` you'll do an:

    export CS140E_2023_PATH=<path to your repo>

And, as before, don't forget to source your configuration file.

As a simple test:

        % cd 2-cross-check/00-hello
        % make
        # can also do 'make run'
        % pi-install hello.bin

Should print:

        ... bunch of stuff ...
        hello: things worked!
        DONE!!!
