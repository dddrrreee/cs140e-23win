# simple, single file tests.
#
# "make runall" will run all the programs listed in TEST_SRC (below)
# "make emitall" will put their output in output files.  "foo" will be in "foo.out"
#    etc
# "make checkall" will check their output against the out files of previous runs.
# 
# to compare your output against other people, just do "cksum *.out" and use the
# checksum / byte count.  you can post to the newsgroup

# when you want to swap in, specify the path to your gpio.c implementation
# GPIO_OBJ = ./gpio.o

# if you want to run a single program:
TEST_SRC = 0-gpio-read.c

# if you want to run all of the ones that don't need hardware
# TEST_SRC := $(wildcard ./[0123]-*.c) $(wildcard ./prog-*.c) 

# to run all the simple tests
TEST_SRC := $(wildcard ./0-*.c)

# the trace flags --- if you override notmain, would need to add it.
WRAP = --wrap=PUT32 --wrap=GET32

# shut up make
MAKEFLAGS += --no-print-directory

# make them local to this directory
TESTS := $(patsubst %.c, %.bin, $(TEST_SRC))
# remove path from names
TEST_PROGS := $(foreach src, ./$(TESTS), $(notdir $(src)))

# pull out all the directories in the test files so make knows 
# where to look for everything.
# note: if tests have the same name, you'll get a collision.
VPATH := $(sort $(dir $(TESTS)))

ifndef CS140E_2023_PATH
$(error CS140E_2023_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

include $(CS140E_2023_PATH)/libpi/defs.mk
CFLAGS += -Wno-unused-function -Wno-unused-variable  -I..

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(wildcard ./*.h) ./Makefile $(LIBTRACE)

LIBTRACE = ../libtrace.a
LIBS = $(LIBTRACE) $(LPI)

all: libtrace $(GPIO_OBJ) $(TEST_PROGS)

libtrace:
	make -C ..

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S $(DEPS)
	$(CC) -c $(CPP_ASFLAGS) $< -o $@
 
%.elf: %.o $(DEPS)  $(LIBTRACE) $(GPIO_OBJ)
	$(LD) $(WRAP) $(START) $< $(GPIO_OBJ) -T $(MEMMAP) -o $@  $(LIBS)

%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

# i'm not sure this actually makes things better.
emit: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test emitall "TESTS=$(TEST_PROGS)"
run: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test runall "TESTS=$(TEST_PROGS)"
check: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test checkall "TESTS=$(TEST_PROGS)"
cksum: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test cksumall "TESTS=$(TEST_PROGS)"

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs]

clean:
	rm -rf $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d
cleanall:
	rm -rf *.out
	make -C ./ clean

.PHONY: libs all tags clean emitall runall checkall cksumall

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf  %.o
