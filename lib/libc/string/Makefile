#
# Makefile for C standard library
#

BUILD = ../../../build

SRCS = strcpy.c strcmp.c strlen.c \
       memcpy.c memmove.c memcmp.c memchr.c memset.c
OBJS = $(patsubst %.c,%.o,$(SRCS))

.PHONY:		all install clean

all:		$(OBJS)

install:	$(OBJS)

%.o:		%.c
		$(BUILD)/bin/lcc -A -I../../include -o $@ -c $<

clean:
		rm -f *~ $(OBJS)
