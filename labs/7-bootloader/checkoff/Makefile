SUBDIRS= 0-lab4-tests  1-trace-all   0-lab5-tests  1-trace-control

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

checkoff:
	make -C 1-trace-control check
	make -C 1-trace-all check
