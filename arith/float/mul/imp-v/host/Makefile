#
# Makefile for implementation and test of floating-point arithmetic
#

BUILD = ../../../../../build

SERIAL = /dev/ttyUSB0

# test level (1 or 2)
LVL = 1
#LVL = 2

# number of test cases
NUM = 100000
#NUM = 10000000

GEN = ../../../test/bin/testfloat_gen
VER = ../../../test/bin/testfloat_ver

SRC = multest.c
EXE = multest

all:		$(EXE)

install:	$(EXE)

run-specials:	$(EXE)
		cat special2.dat | \
		./multest $(SERIAL) | \
		$(VER) -rnear_even -tininessbefore f32_mul

run-extremes:	$(EXE)
		cat extreme2.dat | \
		./multest $(SERIAL) | \
		$(VER) -rnear_even -tininessbefore f32_mul

run-full:	$(EXE)
		$(GEN) -level $(LVL) -n $(NUM) f32 2 | \
		./multest $(SERIAL) | \
		$(VER) -rnear_even -tininessbefore f32_mul

run-simulate:	$(EXE)
		$(GEN) -level $(LVL) -n $(NUM) f32 2 | \
		./multest -simulate | \
		$(VER) -rnear_even -tininessafter f32_mul

$(EXE):		$(SRC)
		gcc -g -Wall -o $(EXE) $(SRC) -lm

clean:
		rm -f *~ $(EXE)
