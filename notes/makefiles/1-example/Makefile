CFLAGS = -O -Wall

all: main test


main: main.o a.o b.o c.o 
	$(CC) a.o b.o c.o main.o -o main

# compile the .o's individually.
# we still manually specify each rule.
a.o: a.c header.h
	$(CC) $(CFLAGS) -c a.c -o a.o
b.o: b.c header.h
	$(CC) -c b.c -o b.o
c.o: c.c header.h
	$(CC) -c c.c -o c.o
main.o: main.c header.h
	$(CC) -c main.c -o main.o

.PHONY: clean
.PHONY: test

test: main
	# test: rule
	./main > out
	diff out main.ref
	@echo "makefile: program 'main' passed test"
	rm -f out

# cleanup remove outputs and temporary files
clean:
	rm -f main out *~ *.bak *.o *.d

