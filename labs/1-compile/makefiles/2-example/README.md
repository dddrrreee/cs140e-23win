## 2-example

Now we add a pattern-matching rule to make our code slightly more
automatic and use the `$<` and `$@` variables.

        %.o: %.c header.h
            # doing %.o=$@ [this is $$@] and and %.c=$< [this is $$<]
            $(CC) $(CFLAGS) -c $<  -o $@

Here the comment will print out the values of these two variables
when the rule runs.


A common mistake we see:
  - when people get stuck, they sit there and stare at the Makefile.
  - But a `Makefile` is just code: treat it like that.  If you can't
    figure out what variables are --- print them out!  You can use
    `echo` or just put them in a comment.
  - If you're confused by how rules are getting followed you can 
    also use `make -d`

Everything else should be the same!  You should test it.
