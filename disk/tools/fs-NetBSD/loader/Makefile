#
# Makefile to build the NetBSD system loader
#

BUILD = ../../../../build

.PHONY:		all install clean

all:		loader

install:	loader
		mkdir -p $(BUILD)/run/fs-NetBSD
		cp loader $(BUILD)/run/fs-NetBSD

loader:		c0.o loader.o auxlib.o biolib.o c1.o
		$(BUILD)/bin/ld -h -rc 0xC0300000 -o loader \
		  c0.o loader.o auxlib.o biolib.o c1.o

c0.o:		c0.s
		$(BUILD)/bin/as -o c0.o c0.s

loader.o:	loader.c
		$(BUILD)/bin/lcc -A -c -o loader.o loader.c

auxlib.o:	auxlib.c
		$(BUILD)/bin/lcc -A -c -o auxlib.o auxlib.c

biolib.o:	biolib.s
		$(BUILD)/bin/as -o biolib.o biolib.s

c1.o:		c1.s
		$(BUILD)/bin/as -o c1.o c1.s

clean:
		rm -f *~ c0.o loader.o auxlib.o biolib.o c1.o loader
