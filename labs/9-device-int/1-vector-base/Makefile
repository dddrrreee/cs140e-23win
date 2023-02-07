# comment out if you want to use your libpi
LPI = ../staff-objs/libpi.a

# include staff for everything so can run completely.
PROGS := 0-test-checks.c 1-test-run.c
COMMON_SRC := interrupt-asm.S

# uncomment if you want it to automatically run.
# RUN=1

# we use a higher optimization level for timings.
OPT_LEVEL = -O2

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2

checkoff:
	make -f ./checkoff.mk
