# Makefile to build or clean all labs.
SUBDIRS= 0-compile

# 0-trusting-trust 0-blink 1-gpio 2-cross-check 3-bootloader 4-uart 5-replay 6-interrupts 7-device-interrupts 8-threads 9-debug-hardware

.PHONY: all check clean
all check clean: $(SUBDIRS)

all: TARGET=all
check: TARGET=check
clean: TARGET=clean

# No, you can't do TARGET=$@, or at least I don't know how to.

# recursive call to make
$(SUBDIRS): force
	$(MAKE) -s -C $@ $(TARGET)

clean:
	rm -f *~ *.bak

.PHONY: force
	force :;
