# trivial "fake pi" makefile: for compiling our r/pi hello on unix with some
# wrappers

SRC := $(wildcard ./*.c)
OBJS := $(SRC:.c=.o)
HDRS = $(wildcard ./*.h)
LIBNAME = libpi-fake.a

INC +=  -I$(CS140E_2023_PATH)/libpi/include
INC +=  -I$(CS140E_2023_PATH)/libpi/libc
INC +=  -I$(CS140E_2023_PATH)/libpi/staff-src

CFLAGS = $(INC) -O -Wall -Wno-unused-function -Wno-unused-variable

all: $(LIBNAME)

$(LIBNAME): $(OBJS)
	ar cr $(LIBNAME) $(OBJS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *~ *.o Makefile.bak   $(LIBNAME)

.PHONY: all clean 
