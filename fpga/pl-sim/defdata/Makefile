#
# Makefile
#

BUILD = ../../../build
BIN2DAT = $(BUILD)/bin/bin2dat

all:		rom00.dat romrand.dat

rom00.dat:
		$(BIN2DAT) -b $(BUILD)/simtest/rom00.bin rom00.dat

romrand.dat:
		$(BIN2DAT) -b $(BUILD)/simtest/romrand.bin romrand.dat

clean:
		rm -f *~ rom00.dat romrand.dat
