#
# Makefile to build the mvstrt program
#

BUILD = ../../../../build

.PHONY:		all install clean

all:		mvstrt.bin

install:	mvstrt.bin
		mkdir -p $(BUILD)/run/fs-Linux
		cp mvstrt.bin $(BUILD)/run/fs-Linux

mvstrt.bin:	mvstrt.o
		$(BUILD)/bin/ld -h -rc 0xC0400000 -o mvstrt.bin mvstrt.o

mvstrt.o:	mvstrt.s
		$(BUILD)/bin/as -o mvstrt.o mvstrt.s

clean:
		rm -f *~ mvstrt.o mvstrt.bin
