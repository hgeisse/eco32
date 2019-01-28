#
# Makefile for C math library
#

BUILD = ../../build

SRCS = sin.c cos.c tan.c asin.c acos.c atan.c atan2.c sinh.c cosh.c tanh.c \
       exp.c log.c log10.c pow.c sqrt.c ceil.c floor.c fabs.c ldexp.c \
       frexp.c modf.c fmod.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
LIB = libm.a

.PHONY:		all install clean

all:		$(LIB)

install:	$(LIB)
		mkdir -p $(BUILD)/lib
		cp $(LIB) $(BUILD)/lib

$(LIB):		$(OBJS)
		$(BUILD)/bin/ar -cv $(LIB) $(OBJS)

%.o:		%.c
		$(BUILD)/bin/lcc -A -I../include -o $@ -c $<

clean:
		rm -f *~ $(OBJS) $(LIB)
