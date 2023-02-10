PROG_SRC = $(wildcard ./[67]*.c)
SRC = fake-pi.c  fake-random.c  pi-random.c
SRC +=  gpio-int.c  

LPP = $(CS140E_2023_PATH)/libpi
CFLAGS += -DRPI_UNIX -I$(LPP)/include

include $(CS140E_2023_PATH)/libunix/mk/Makefile.unix.fake

LIBNAME = libfake.a

# i'm not sure this actually makes things better.
run: $(PROGS)
	make -f ./Makefile.test runall "TESTS=$(PROGS)"
check: $(PROGS)
	make -f ./Makefile.test checkall "TESTS=$(PROGS)"
cksum:  $(PROGS)
	@make -f ./Makefile.test cksumall "TESTS=$(PROGS)"

checkoff: $(PROGS)
	make -f ./Makefile.test checkall "TESTS=$(PROGS)"
	@printf "total progs =: ";
	@ls $(PROGS) | wc -l
	cksum *.out | sort -n | cksum

