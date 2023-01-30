### Extension: `cmd-watch`

In my opinion, `sublime` and other IDE's have  crippled how people code 
--- after saving a file, often there's 30-60 seconds of switching screens,
mouse clicks, hunting for which `foo.c` you want out of many tabs, etc
before you actually get an edit-compile-run done.  I'd assert that you're
more likely to stay in a flow state and get more done if, as soon as
you save code, all the compile-run happens automatically and immediately.

So we are going to build a simple tool, `cmd-watch`, that that lets you
do so.  It works as follows.

  1. You invoke it:

          cmd-watch ./program-name [arg1 arg2 <...>]

  2. `cmd-watch` will monitor all the `.c`, `.h`, `.S`, `.s` files in the current
     directory and, if they are modified, execute the command `./program-name` with
     any given arguments (e.g., `arg1`, `arg2`, etc.)

  3. For example:

          cmd-watch make

     will run `make` if any of the files `*.[chsS]` change in the
     current directory.

     This means you can be editing any file, save it, and then ---
     without moving a finger --- the code will be compiled and (depending
     on your makefile) run.  IMO, the best coders have everything they
     do either require one keystroke or (better) zero.  This is a good
     step in that direction.

Most of `cmd-watch` uses similar methods as your `unix-side` bootloader.
You can implement `cmd-watch` as follows:

  1. Uses `opendir` to open the current directory and scan all 
     entries using `readdir`.
  2. For any entry that has the suffix we are looking for, uses `stat`
     or `fstat` (`man 2 stat`) to get the `stat` structure associated
     with that entry and check the modification time (`s.st_mtime`).
  3. If the modification time is more recent than the last change in the
     directory, do a `fork-execvp` of the command-line arguments passed to
     `cmd-watch`.
  4. Have the parent process use `waitpid` to wait for the child to
     finish and also get its status.  The parent will print if the child
     exited with an error.

Some gotcha's
  - If nothing changed, go to sleep for a human-scale amount,  otherwise
    you'll burn up a lot of CPU time waiting for some entry to change.
    The claim is that people can't notice below 100ms, so you could sleep
    for 250ms without too much trouble.

  - If you keep opening the directory, make sure you close it after
    (`closedir`) otherwise you'll run out of file descriptors.

  - Even if you find a recent-enough modification time to kick off the
    command, scan the rest of the directory.  Otherwise you can kick off
    spurious executions.

There are a bunch of extensions you can do:
 - Make it easy to use different suffixes or scan in different locations.

 - Run multiple commands, for example `foo \; bar` which will run `foo`
   and then run `bar`.  Note if you use a semi-colon as a separator you
   will likely need to escape it or your shell will interpret it.

