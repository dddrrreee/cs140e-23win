## 5-example

Previously, we've only grabbed source from the directory we are in.
Now we grab source code from other directories, compile it,
and:

  1. Put the generated object files in a private build 
     directory (`obj`).
  2. Put the automatic dependencies in the build directory, too.
  3. Automatically compute the directories involved and add them 
     to `VPATH` (so the `make` knows where to look).
  4. Also use these directories to genereate `-I` flags passed
     to the compiler so it knows where to search for include files.

This Makefile is a bit much.  On the other hand, it's generally the
fanciest you'll need and you may be able to copy it and tweak it for
much of your career.

As an exercise: simpler by getting rid of the build directory option.
This should remove a bunch of stuff.  Between `4-example`, `5-example`
and your simplified Makefile, you should be covered.

