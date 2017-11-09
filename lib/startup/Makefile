#
# Makefile for C startup
#

BUILD = ../../build

SRCS = crt0.s
OBJS = $(patsubst %.s,%.o,$(SRCS))

.PHONY:		all install clean

all:		$(OBJS)

install:	$(OBJS)
		mkdir -p $(BUILD)/lib
		cp $(OBJS) $(BUILD)/lib

%.o:		%.s
		$(BUILD)/bin/as -o $@ $<

clean:
		rm -f *~ $(OBJS)
