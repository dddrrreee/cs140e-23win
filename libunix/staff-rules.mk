# DO NOT MODIFY THIS FILE!   You will get conflicts.
#
# WARNING:  the way this is setup, you cannot have files with the same 
#           name in different directories --- when the dependencies
#           are thrown into the BUILD_DIR they will collide.   to fix
#           this we'd have to move the directory structure in there,
#           or do some kind of name flatting.
#
#          you might want to leave the dependencies in the different
#          source directories, but this doesn't work if they are changed
#          by different build targets.  one approach might be to move
#          the makefile into the subdir rather than having it use
#          conditional compilation.
#
#  maybe this?
#      https://stackoverflow.com/questions/1139271/makefiles-with-source-files-in-different-directories

TARGET := libunix.a
BUILD_DIR ?= ./objs

# if you have a space it breaks everything.
LU := $(CS140E_2023_PATH)/libunix
CFLAGS += -DLIB_UNUX -I$(LU)

# note: if tests have the same name, you'll get a collision.
OBJS := $(patsubst %.c, %.o, $(SOURCES))
OBJS := $(foreach o, $(OBJS), $(BUILD_DIR)/$(notdir $o))

# Get all depend files by replacing extensions
CC_DEPS := $(OBJS:.o=.d) 
DEPS := $(LU)/Makefile $(LU)/staff-rules.mk

all: $(TARGET)

# we bundle start.s with the library: just make different ones for each
# different start so that the library is internally consistent.
$(TARGET): $(OBJS)  $(DEPS)
	@mkdir -p $(BUILD_DIR)
	ar cr $(TARGET) $(OBJS) 

# XXX: We seem to need this rule to stop compilation with an error if the 
# .c file has one: otherwise make does not stop compilation when generating 
# .d.  If compilation goes correctly, is not triggered.
# $(BUILD_DIR)/%.o : %.c
$(BUILD_DIR)/%.o : %.c $(BUILD_DIR)/%.d
	@mkdir -p $(BUILD_DIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

# XXX: I think we need this to catch errors in .S as well.
# $(BUILD_DIR)/%.o : %.S
$(BUILD_DIR)/%.o : %.S $(BUILD_DIR)/%.d
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPP_ASFLAGS) $<

# Rules to automatically generate dependencies and put in build directory
# We want to recompile everything if the makefile changes.
$(BUILD_DIR)/%.d: %.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -MT $@ -MMD -MP -MF $@ $< -o $(BUILD_DIR)/$(notdir $*).o

$(BUILD_DIR)/%.d: %.S $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPP_ASFLAGS) -c -MT $@ -MMD -MP -MF $@ $< -o $(BUILD_DIR)/$(notdir $*).o

# -MF  write the generated dependency rule to a file
# -MG  assume missing headers will be generated and don't stop with an error
# -MM  generate dependency rule for prerequisite, skipping system headers
# -MP  add phony target for each header to prevent errors when header is missing
# -MT  add a target to the generated dependency

# I don't get why we need this.  check the mad-make guy.
#   http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
$(BUILD_DIR)/%.d: ;
.PRECIOUS: $(BUILD_DIR)/%.d

# *unbelievable*: make clean doesn't skip this include even though the 
# target is after.
#   https://www.gnu.org/software/make/manual/html_node/Goals.html
ifneq ($(MAKECMDGOALS),clean)
-include $(CC_DEPS)
endif

ctags: 
	ctags *.[chSs] */*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(TARGET) *~ tags *.o

.PHONY: clean all ctags print
