# input is harder.
PROGS = $(wildcard ./input-test-0-small-input.c)
PROGS = $(wildcard ./input-test*.c)

PROGS = $(wildcard ./output-test*.c)
# easiest output
PROGS = $(wildcard ./output-test-0-hello.c)

COMMON_SRC = cstart.c
# if you commment this out, will be using our libpi version: should pass!
COMMON_SRC += uart.c

# uncomment if you want it to automatically run.
# RUN=1

BOOTLOADER = my-install
CAN_EMIT = 0

EXCLUDE = grep -v simple_boot
include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2

# we check that echo'ing input works.  gross hack b/c we didn't make it
# easy to filter all bootloader input (very annoying) so we grep out 
# the leading number that our inputs are tagged w/
input-test-1-echo.check: input-test-1-echo.bin
	my-install $< < ../test-input/trivial.c  > trivial.tmp  2>&1 
	@grep "[0-9][0-9][0-9][0-9]:" trivial.tmp | grep -v BOOT: > trivial.test
	@diff trivial.test ../test-input/trivial.c
	
	my-install $< < ../test-input/switch.c  > switch.tmp  2>&1 
	@grep "[0-9][0-9][0-9][0-9]:" switch.tmp | grep -v BOOT: > switch.test
	@diff switch.test ../test-input/switch.c
	
	@echo "about to echo the first 1000 lines of unix source: will take awhile";
	my-install $< < ../test-input/1000-unix-source.c  > unix-source.tmp  2>&1 
	@grep "[0-9][0-9][0-9][0-9]:" unix-source.tmp | grep -v BOOT: > unix-source.test
	@diff -Bb unix-source.test ../test-input/1000-unix-source.c
	@rm -f *.tmp

input-test-0-small-input.check: input-test-0-small-input.bin 
	my-install $< < input-hello.txt

# if this hangs make sure your uart.c has stuff in it.
checkoff:
	make -C ./ COMMON_SRC="uart.c cstart.c" PROGS="$(wildcard *-test*.c)" check
	@echo "checking that not using staff uart: if you see output is an error:"
	arm-eabi-none-nm *.elf  | grep staff_uart
