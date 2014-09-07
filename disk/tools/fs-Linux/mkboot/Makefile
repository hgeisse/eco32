#
# Makefile to build the boot record
#

BUILD = ../../../../build

.PHONY:		all install clean

all:		br.bin

install:	br.bin
		mkdir -p $(BUILD)/run/fs-Linux
		cp br.bin $(BUILD)/run/fs-Linux

br.bin:		br.o
		$(BUILD)/bin/ld -h -rc 0xC0010000 -o br.bin br.o

br.o:		br.s
		$(BUILD)/bin/as -o br.o br.s

clean:
		rm -f *~ br.o br.bin
