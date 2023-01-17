# makefile based on the makefiles/4-example note.
#
# while there's many more lines, they just do the stuff that is
# already in the 4-example makefile: nothing special.
#
# what's different:
#  - we are cross-compiling, so more steps to go from .elf->.bin.
#  - similarly: there are more tools and make VAR names for them.
#  - we adopt the convention that any file whose name starts with 
#     a number is a test program ("[0-9]-*.c")

PROG_SRC = $(wildcard [0-9]-*.c)
PROG_OBJS = $(PROG_SRC:.c=.o)
PROGS = $(PROG_SRC:.c=.bin)

# we give you these.
LIBPI_OBJS = gpio.o delay.o
# staff start file: must go first for linking.
STAFF_START := ./start.o

# make sure to compute dependencies for all source files.
all_objs = $(PROG_OBJS) $(LIBPI_OBJS) $(STAFF_START)
dep = $(all_objs:.o=.d)

# linker script to lay out the executable (the .bin that runs on the pi)
MEMMAP := ./memmap

# recompile everything if Makefile changes.
DEPS := ./Makefile 

# the different tools we use.
ARM = arm-none-eabi
CC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy
AS  = $(ARM)-as

# The gcc compiler flags
#    -Wall: warn about a bunch of suss things.
#    -Werror = must fix errors
#    -O: optimize (which allows for more -Wall error checking)
CFLAGS = -O -Wall -Wno-unused-variable -Werror \
         -MMD\
          -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99  

# the assembler flags (for start.S)
ASFLAGS = --warn --fatal-warnings -mcpu=arm1176jzf-s -march=armv6zk

all:  $(PROGS)

# rules to generate .elf->.list->.bin
%.elf: %.o $(LIBPI_OBJS) $(STAFF_START) $(MEMMAP) $(DEPS) 
	$(CC) $(CFLAGS) $(STAFF_START) $< $(LIBPI_OBJS) -T $(MEMMAP) -o $@ 

# include the generated dependency rules.
# open up a .d file in your editor to see 
# why this works!
-include $(dep)

#####################################################################
# all the pattern matching rules to make the 
# different file types.

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# rule for assembly.
%.o: %.S $(DEPS)
	$(AS) -c $(ASFLAGS) $< -o $@

# generate a listing from the elf file so can see what the
# machine code looks like
%.list: %.elf
	$(OD) -D  $< > $@

# generate a .bin --- we put the .list as the dependency
# so it gets generated; doesn't actually get used.
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

clean:
	rm -rf *.o *.bin *~ tags *.elf *.list *.d Makefile.bak 

.PHONY: all clean 
# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf %.o
