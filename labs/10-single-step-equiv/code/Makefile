# SUPPORT_OBJS := debug-fault.o

# comment this out to use your code
USE_STAFF=1

ifeq ($(USE_STAFF),1)
# STAFF_OBJS := staff-rpi-thread.o  staff-thread-asm.o
else
# SUPPORT_OBJS += rpi-thread.o  thread-asm.o
endif

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = my-install

# uncomment if you want it to automatically run.
RUN=1

# part 1
PROGS := $(wildcard ./2-*.c)
PROGS := $(wildcard ./1-*.c)
PROGS := $(wildcard ./0-*.c)

CFLAGS += -Wno-unused-function -Wno-unused-variable 

# to run all the tests.
# TEST_SRC := $(wildcard ./[0-9]*.c)

# should not have to change below

GREP_STR := 'TRACE:\|THREAD:\|ERROR:\|PANIC:' 

# CAN_EMIT = 1

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template

# override the default rules.  should have a cleaner solution
2-brkpt-test.elf:  2-brkpt-test.o $(DEPS) $(START) $(LIBS)
	$(LD) $(START)  $< -T $(MEMMAP) -o $@  $(LIBS)
1-watchpt-test.elf:  1-watchpt-test.o $(DEPS) $(START) $(LIBS)
	$(LD) $(START)  $< -T $(MEMMAP) -o $@  $(LIBS)


clean::
	rm -f rpi-thread.o thread-asm.o
	rm -f 2-write-regs-asm.o 2-where-push-asm.o 
