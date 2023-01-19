# trivial "fake pi" makefile: for compiling our r/pi hello on unix with some
# wrappers

SRC := $(wildcard ./*.c)
OBJS := $(SRC:.c=.o)
HDRS = $(wildcard ./*.h)
LIBNAME = libpi-fake.a

CFLAGS = -O -Wall -Wno-unused-function -Wno-unused-variable

all: $(LIBNAME)

$(LIBNAME): $(OBJS)
	ar cr $(LIBNAME) $(OBJS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *~ *.o Makefile.bak $(LIBNAME)

.PHONY: all clean 
