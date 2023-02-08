SUBDIRS= 0-circular-example 1-vector-base 2-gpio-int 3-handlers-gpio-int 4-logic-analyzer

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
	make -C 1-vector-base$(STAFF)/ checkoff
	make -C 2-gpio-int$(STAFF)/ checkoff
	make -C 3-handlers-gpio-int$(STAFF)/ checkoff
