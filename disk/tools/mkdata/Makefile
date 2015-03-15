#
# Makefile to generate test data files for file system test
#

BUILD = ../../../build

CC = gcc
CFLAGS = -g -Wall

.PHONY:		all install clean

all:		0x000001

install:	0x000001
		mkdir -p $(BUILD)/data
		cp 0x* $(BUILD)/data

0x000001:	mkdata
		./mkdata

mkdata:		mkdata.c
		$(CC) $(CFLAGS) -o mkdata mkdata.c

clean:
		rm -f *~ mkdata 0x*
