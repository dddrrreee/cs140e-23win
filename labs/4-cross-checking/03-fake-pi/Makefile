MAKEFLAGS += --no-print-directory

# can uncomment these for 1-libpi-fake or 2-libpi-fake
# PI_PROGS += test-demand.c
# PI_PROGS += test-assert.c

# use this so we can pass the same set of programs to both makefiles.
PI_PROGS += hello.c null-ptr.c 

FAKE_VERSION = 2-libpi-fake
FAKE_VERSION = 1-libpi-fake

# our initial first version
FAKE_VERSION = 0-libpi-fake

all: fake-pi

pi:
	make -f Makefile.pi "PI_PROGS=$(PI_PROGS)"

fake-pi:
	make -f Makefile.fake-pi "PI_PROGS=$(PI_PROGS)" "FAKE_VERSION=$(FAKE_VERSION)"

both:
	make -C ./ fake-pi
	make -C ./ pi

clean:
	make -s -f Makefile.fake-pi clean
	make -s -f Makefile.pi clean
	make -s -C $(FAKE_VERSION) clean
	make -s -C 0-libpi-fake clean
	make -s -C 1-libpi-fake clean
	make -s -C 2-libpi-fake clean
	rm -f *~ *.bak

.PHONEY: both fake-pi pi clean
