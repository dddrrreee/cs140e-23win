## Extensions

changing this.

--------------------------------------------------------------------
### Medium extension: implement `cmd-watch`.

You'll use the Unix routines you made for the prelab to make a simple
program `cmd-watch` that will run a command whenever something changes
in the current directory (optional: list of directories).

So:

          % cmd-watch make

will run `make` whenever any file in the current directory changes.
This makes code development much more flowing, since you don't
have to stop and do anything.

Good chance you use this program for years.  I do.  It's not that
complicated to make.

The [`cmd-watch` README](extension-cmd-watch/README.md) describes
in more detail.  You should tune the interface however works best
for your method.

--------------------------------------------------------------------
### Medium extension: implement a `PI_PATH` extension.

Hunting around and finding different pi binaries is a pain.  You can
do the same hack your shell does for binaries and have the user (you)
define a `PI_PATH`  variable with a colon-seperated list of all the
different locations to look in for a binary.

Put your code in `libunix/resolve-pi-path.c:find_pi_binary`.
  - `extension-find-pi/` has a simple test driver.

The code:
  - use `getenv(PI_PATH)` to get any value associated with `PI_PATH`.
  - scan these `:` seperated locations for the given binary.
  - return the full path if you find it.
 
This routie is useful for the rest of the quarter.  Will save you a
bunch of time `cd`-ing around.

You can of course do your own hacks.  One potentially useful one is to 
go up one level, and search all subdirectories for a binary.  

--------------------------------------------------------------------
#### Extension: make the bootloader better

If you recall, the Unix side of the bootloader has some pretty useless
error messages.  Fix these so they are more helpful.

Similarly: if the pi-side runs into trouble, do something more useful
than just lock up.  For example, if you try to send `GET_PROG_INFO`
some number of times without a response, blink the internal LED (pin 47)
multiple times to show things are in a bad state and reboot.

Also, you can change `boot_putk` to implement `printk` type functionality
of taking a format string rather than a fixed string.

--------------------------------------------------------------------
#### Extensions.

Many possible extensions.  Since this low level code is used by everything we 
want to be absolutely sure it is rock-solid in all situations:

  0. Change the pi-side `get_uint8` calls to timeout if they are stuck for "too long"
     and then reboot.  This is useful for having a stand-alone pi in the field you
     can update.  If it gets stuck you don't have to walk (or drive) over and
     hit a button to restart it.

  1. Set up your tracing (`2-trace`) so you can trace a copy of the bootloader on the pi.
  2. Setup logging on the unix side so you can record the `PUT/GET` operations from
     a given run and then replay them later, so you can check that the
     Unix side behaves identically (all output is the same, etc).
  3. Start mutating the traces in (2) and make sure the Unix side behaves sensibly.
  4. Adapt your `fake-pi` so that it can run the bootloader and use this to do
     many randomized tests or mutate previous runs.

- do a PI_PATH exenstion that looks for pi binary in a list of locations.
  so if you add your bin/ directory and do 
     "my-install hello.bin" should just work.

- write up the cmd-watch extension.
