CFLAGS = -O -Wall

all: main test

main: main.o a.o b.o c.o 
	$(CC) $(CFLAGS) a.o b.o c.o main.o -o main

# we print out what we are doing just so we can see it.
%.o: %.c header.h
	# doing %.o=$@ [this is $$@] and and %.c=$< [this is $$<]
	$(CC) $(CFLAGS) -c $<  -o $@

.PHONY: clean
.PHONY: test

test: main
	./main > out
	diff out main.ref
	@echo "makefile: program 'main' passed test"
	rm -f out

clean:
	rm -f main out *~ *.bak *.o *.d
