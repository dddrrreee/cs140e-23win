M = Makefile.3

# compile using Makefile.0
all:  main test

main:
	make -f $(M)

# tell `make` these rules don't produce anything.
.PHONY: test clean main

# make sure <main> produces the same result.
test: 
	./main > out
	diff out main.ref
	@echo "makefile: program 'main' passed test"
	rm -f out

clean:
	rm -f main out *~ *.o
