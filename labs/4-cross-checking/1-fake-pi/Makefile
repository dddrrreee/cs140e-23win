LIBNAME = libpi-fake.a

# grab stuff from libpi
LPP = $(CS140E_2023_PATH)/libpi/
SRC  = $(LPP)/libc/printk.c
SRC  += $(LPP)/libc/putk.c
SRC  += $(LPP)/libc/putchar.c
SRC  += $(LPP)/staff-src/clean-reboot.c
SRC  += $(wildcard ./*.c)

# or copy your GPIO here.
ifndef CS140E_STAFF
SRC  += ../../3-gpio/code/gpio.c 
else
SRC  += $(LPP)/staff-private/gpio.c 
endif

INC +=  -I$(CS140E_2023_PATH)/libpi/include
INC +=  -I$(CS140E_2023_PATH)/libpi/libc
INC +=  -I$(CS140E_2023_PATH)/libpi/staff-src
INC +=  -I.
CFLAGS = $(INC) -O -Wall -Wno-unused-function -Wno-unused-variable -MMD -DRPI_UNIX

# all the directories we get code from.
dirs := $(sort $(dir $(SRC)))

# tell make where to search for prereqs.
VPATH := $(dirs)

# strip all the directories
reloc := $(foreach o, $(SRC), $(notdir $o))
OBJS := $(reloc:.c=.o)

dep := $(reloc:.c=.d)

all: $(LIBNAME)

$(LIBNAME): $(OBJS)
	ar cr $(LIBNAME) $(OBJS)

-include $(dep)

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

check:
	@make -s -C tests check
run:
	@make -s -C tests run
emit:
	@make -s -C tests emit
cksum:
	@make -s -C tests cksum
checkoff:
	@make -s -C tests checkoff

clean:
	rm -rf *.o *~ *.o Makefile.bak   $(LIBNAME) *.d

.PHONY: all clean 
