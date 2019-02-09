#
# Makefile for C standard library
#

BUILD = ../../build

DIRS = ctype errno locale stdio stdlib string syscall
OBJS = $(wildcard $(patsubst %,%/*.o,$(DIRS)))
LIB = libc.a

.PHONY:		all install objects clean

all:		$(LIB)

install:	$(LIB)
		mkdir -p $(BUILD)/lib
		cp $(LIB) $(BUILD)/lib

$(LIB):		objects
		$(BUILD)/bin/ar -cv $(LIB) $(OBJS)

objects:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i all ; \
		done

clean:
		for i in $(DIRS) ; do \
		  $(MAKE) -C $$i clean ; \
		done
		rm -f *~ $(LIB)
