# radically simplify labs/3-gpio makefile:  
#   1. we only compile tiny programs: so always remake everything, always.
#      no dependency calculations, no .o rules, etc.
#   2. the arm toolchain names are shared across multiple makefiles so 
#      we pull them into a seperate include (similar to C)
#   3. merge all the program generation rules for .list .bin into a
#      single recipe.

# anything that has a name starting with digits is a driver
# program (has a notmain)
PROG_SRC = $(wildcard [0-9]-*.c)
PROGS = $(PROG_SRC:.c=.bin)

# all the shared source.  we seperate out start for linking.
LIB_SRC = gpio.c delay.c
START := ./start.S  # sets up environment
MEMMAP := ./memmap  # linker script

# tool chain names and flags.
include defs.mk

all: $(PROGS)

# dumb: recompile everything, all the time.
%.bin:  FORCE
	$(CC) $(CFLAGS) $(START) $*.c $(LIB_SRC) -T $(MEMMAP) -o  $*.elf 
	$(OD) -D  $*.elf > $*.list
	$(OCP) $*.elf -O binary $*.bin

# this idiom forces the rule to always run.  can be called something
# else:
#   https://www.gnu.org/software/make/manual/html_node/Force-Targets.html
FORCE:

clean:
	rm -rf *.o *.bin *~ tags *.elf *.list *.d Makefile.bak 

.PHONY: all clean 
