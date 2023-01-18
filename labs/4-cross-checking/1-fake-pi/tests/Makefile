# trivial makefile: can define the SRC here or pass it in.
# note: this is useful or dangerous: we strip out all the 
# directory stuff so we can put the executable here.

MAKEFLAGS += --no-print-directory

# makefile variables: last writer wins (just like C), so 
# you don't need to comment out assignments.

# hardest; tell makefile to make everything.
TEST_SRC := $(wildcard ./*.c) 

# easier: tell makefile to make the second batch of tests
TEST_SRC := $(wildcard ./[2]-*.c) 

# easiest: tell makefile to make the first batch of tests.
TEST_SRC := $(wildcard ./[1]-*.c) 
TEST_SRC := 1-gpio-read.c


# make them local to this directory
OBJS := $(patsubst %.c, %.o, $(TEST_SRC))
OBJS := $(foreach obj, $(OBJS), ./$(notdir $(obj)))

TEST_PROGS := $(patsubst %.o, %, $(OBJS))

dep := $(OBJS:.o=.d)  # one dependency file for each source


# pull out all the directories in the test files so make knows 
# where to look for everything.
# note: if tests have the same name, you'll get a collision.
VPATH := $(sort $(dir $(TESTS)))

LIBPI_FAKE = ../libpi-fake.a
CC = gcc

# where the libpi includes are
CFLAGS =  -Wall -Wno-unused-function -Wno-unused-variable  -g
CFLAGS += -I..

DEPS = ./Makefile $(LIBPI_FAKE) 

all: libs $(TEST_PROGS) 

libs:
	make -C ../  

## we put all the executables here: how to remove?
./%: %.o  $(DEPS) 
	$(CC) $(CFLAGS) $< -o $@  $(LIBPI_FAKE)

-include $(dep)  # include dep files

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@


# i'm not sure this actually makes things better.
emit: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test emitall "TESTS=$(TEST_PROGS)"
run: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test runall "TESTS=$(TEST_PROGS)"
check: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test checkall "TESTS=$(TEST_PROGS)"
cksum: $(DEPS) $(TEST_PROGS)
	@make -f ./Makefile.test cksumall "TESTS=$(TEST_PROGS)"

clean:
	rm -rf $(TEST_PROGS) *~ tags *.o Makefile.bak *.d

cleanall:
	@make -s -f ./Makefile clean
	@make -s -f ./Makefile.test cleanall TESTS=x

.PHONY: all clean libs run check emit
