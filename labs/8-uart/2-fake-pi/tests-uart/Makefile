# TEST_SRC are all the files that get run.
#    - you should start with the easiest first --- just do a single
#      one of the 1-* files (these do single call)
#    - then do all of them and see that you get the same "make checkoff"
#      as other people
#    - then do 2
#    - then do 5
#    - then do prog-*
MAKEFLAGS += --no-print-directory

### use this for checkoff: hardest b/c is everything.
TEST_SRC := $(wildcard ./[1-5]-*.c) $(wildcard ./prog-*.c) 

# easier: tell makefile to make the second batch of tests
TEST_SRC := $(wildcard ./[2]-*.c)
# easiest: tell makefile to make the first batch of tests.
TEST_SRC := $(wildcard ./[1]-*.c)
# just run one test.
TEST_SRC += 0-uart-putc.c

TEST_SRC = 0-uart-getc.c
TEST_SRC += 0-uart-init.c
TEST_SRC := $(wildcard ./0-*.c)

# no other source code
SRC = 
# fake libpi
LIBPI_FAKE = ../libpi-fake.a

TEST_OUT := $(TEST_SRC:.c=.out)
FAKE_PROGS := $(TEST_SRC:.c=.fake)
FAKE_RUN   := $(TEST_SRC:.c=.run)

INC  =  -I../  -I.
INC +=  -I$(CS140E_2023_PATH)/libpi/include
INC +=  -I$(CS140E_2023_PATH)/libpi/libc
INC +=  -I$(CS140E_2023_PATH)/libpi/staff-src
CFLAGS = $(INC) -g -Wall -Wno-unused-function -Wno-unused-variable

DEPS = libpi-fake $(FAKE_PROGS) 

all: libpi-fake $(FAKE_PROGS) #  $(FAKE_RUN)

libpi-fake:
	make -C .. 

print:
	# FAKE_PROGS = <$(FAKE_PROGS)>
	# FAKE_RUN = <$(FAKE_RUN)>
	# LIBPI_FAKE = <$(LIBPI_FAKE)>


%.fake: %.c  $(SRC) $(LIBPI_FAKE)
	$(CC) $(CFLAGS) $< $(SRC) -o $*.fake $(LIBPI_FAKE)

#%.run: %.fake
#	@echo "------------------------------------------------------ "
#	@echo "       running our fake pi program: <$*.fake>         "
#	./$*.fake

clean:
	rm -rf *.fake *~ *.o Makefile.bak   *.d

cleanall:
	make -C ./ clean
	make -C ../ clean
	rm -f *.out


# i'm not sure this actually makes things better.
emit: $(DEPS)
	make -f ./Makefile.test emitall "TESTS=$(FAKE_PROGS)"
run: $(DEPS)
	make -f ./Makefile.test runall "TESTS=$(FAKE_PROGS)"
check: $(DEPS)
	make -f ./Makefile.test checkall "TESTS=$(FAKE_PROGS)"
cksum: $(DEPS)
	@make -f ./Makefile.test cksumall "TESTS=$(FAKE_PROGS)"

checkoff: $(DEPS)
	@ls $(TEST_OUT)
	@echo -n "total files = ";
	@ls $(TEST_OUT) | wc  -l
	@echo -n "checksum of cksum = ";
	@ls $(TEST_OUT) | sort -n | cksum 



.PHONY: all clean  run  print
