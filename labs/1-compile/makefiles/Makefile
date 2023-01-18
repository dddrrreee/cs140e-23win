# example of how to run make in subdirectories and get results
SUBDIRS= 0-example 1-example 2-example 3-example 4-example 5-example 7-example
 
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
