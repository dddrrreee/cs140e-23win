# you should not need to modify this.
CC =gcc
CFLAGS = -Og -g -Wall

all: quine-gen

check: quine-gen
	@echo "--------------------------------------------"
	@echo "checking that the quine generates itself\n"
	@echo "1. Generating a quine"
	./quine-gen < seed.c > quine.c
	$(CC) quine.c -o quine
	@echo "2. Using quine to generate itself"
	./quine > quine-out.c
	@echo "3. Checking generated quine matches quine"
	diff quine.c quine-out.c
	@echo "**** Success: Quine matches its output ***"
	@echo "--------------------------------------------\n"

quine-gen: quine-gen.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f quine-gen quine-out.c quine.c quine *~

.PHONEY: clean check
