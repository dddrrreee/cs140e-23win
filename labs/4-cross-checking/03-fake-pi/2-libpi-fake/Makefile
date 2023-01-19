# trivial "fake pi" makefile: for compiling our r/pi hello on unix with some
# wrappers


# grab stuff from libpi
LPP = $(CS140E_2023_PATH)/libpi/
SRC  = $(LPP)/libc/printk.c
SRC  += $(LPP)/libc/putk.c
SRC  += $(LPP)/libc/putchar.c
SRC  += $(LPP)/staff-src/clean-reboot.c
SRC  += $(wildcard ./*.c)

# all the directories we get code from.
dirs := $(sort $(dir $(SRC)))

# tell make where to search for prereqs.
VPATH := $(dirs)

# strip all the directories
reloc := $(foreach o, $(SRC), $(notdir $o))
OBJS := $(reloc:.c=.o)

dep := $(reloc:.c=.d)

LIBNAME = libpi-fake.a

INC +=  -I$(CS140E_2023_PATH)/libpi/include
INC +=  -I$(CS140E_2023_PATH)/libpi/libc
INC +=  -I$(CS140E_2023_PATH)/libpi/staff-src

CFLAGS = $(INC) -O -Wall -Wno-unused-function -Wno-unused-variable -MMD

all: $(LIBNAME)

print:
	# SRC = <$(SRC)>
	# OBJS = <$(OBJS)>
	# dirs = <$(dirs)>
	# dep = <$(dirs)>

$(LIBNAME): $(OBJS)
	ar cr $(LIBNAME) $(OBJS)

-include $(dep)

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *~ *.o Makefile.bak   $(LIBNAME) *.d

.PHONY: all clean 
