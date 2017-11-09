#
# Makefile for C library
#

BUILD = ../../build

SRCS = ctype.c string.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
LIB = libc.a

.PHONY:		all install clean

all:		$(LIB)

install:	$(LIB)
		mkdir -p $(BUILD)/lib
		cp $(LIB) $(BUILD)/lib

$(LIB):		$(OBJS)
		$(BUILD)/bin/ar -cv $(LIB) $(OBJS)

%.o:		%.c
		$(BUILD)/bin/lcc -A -o $@ -c $<

clean:
		rm -f *~ $(OBJS) $(LIB)
