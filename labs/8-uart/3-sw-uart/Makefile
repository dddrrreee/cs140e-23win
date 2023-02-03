# first make sure hello.c works.

# "make checkoff" should run everything and check
# easiest output: do this first with the sw-uart-veneer.c
PROGS = $(wildcard ./tests/output-test-0-hello.c)

# then this
PROGS = $(wildcard ./tests/output-test*.c)
# do this first to develop
PROGS = hello.c

#once your sw-uart works: add sw-uart-veneer.c
# COMMON_SRC += sw-uart-veneer.c
COMMON_SRC += sw-uart.c

# can use the staff code to see that things work.
# STAFF_OBJS := staff-sw-uart.o

# uncomment if you want it to automatically run.
# RUN=1

BOOTLOADER = my-install

EXCLUDE = grep -v simple_boot
include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2

checkoff:
	@echo "you should have copied sw-uart.o into your libpi/src directory";
	@echo "and added it to your makefile";
	make -C ./ COMMON_SRC="sw-uart-veneer.c" PROGS="$(wildcard tests/output-test*.c)" check

clean::
	make -C tests clean
