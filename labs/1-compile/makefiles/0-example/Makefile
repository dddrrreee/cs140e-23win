# makefile 0.
#
# a very simple-minded Makefile that:
#  1. produces an executable file <main> by compiling
#     C files <a.c>, <b.c>, <c.c>, <main.c> and a header 
#     file <header.h>
#  2. checks that the <main> is the same as a reference
#     <main.ref>
#
# makefile 
#  1. hardcodes all dependencies.
all: main test

# hardcode that <main> depends on <a.c> <b.c> <c.c> <main.c> and
# <header.h>
#
# what this means mechanically: if the modification time of any
# of these files changes so it's more recent than the modification
# time of <main> (or if <main> doesn't exist) the rule will execute.
#
# easy test:
#  % make clean  # removes <main>
#  % make        # produces <main>
#  % make        # should not re-compile <main>
#  % touch header.h
#  % make        # will produce <main>
main: a.c b.c c.c main.c header.h
	# main: rule
	$(CC) a.c b.c c.c main.c -o main

# tell `make` the <test> and <clean> rules don't 
# produce anything.
.PHONY: clean
.PHONY: test

# make sure <main> produces the same result as reference
# output in <main.ref>:
#  1. run <main> and put the output in file <out>;
#  2. check if they differ using <diff>
#
# This will rerun each time you run `make -f Makefile.0`
test: main
	# test: rule
	./main > out
	diff out main.ref
	@echo "makefile: program 'main' passed test"
	@rm -f out

# cleanup remove outputs and temporary files
clean:
	rm -f main out *~ *.bak
