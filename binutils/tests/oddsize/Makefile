#
# Makefile for as/ld odd segment size tests
#

BUILD = ../../../build

all:
	$(BUILD)/bin/as -o src1.o src1.s
	$(BUILD)/bin/as -o src2.o src2.s
	$(BUILD)/bin/ld -s standalone.lnk -o oddsize src1.o src2.o
	$(BUILD)/bin/dof -a oddsize >oddsize.dump

clean:
	rm -f *~ src1.o src2.o oddsize oddsize.dump
