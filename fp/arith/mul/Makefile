#
# Makefile for implementation and test of floating-point arithmetic
#

BUILD = ../../../build

# number of test cases
NUM = 100000000
# test level (1 or 2)
LVL = 2

SRC = mul.c
EXE = mul

all:		$(EXE)

install:	$(EXE)

run-simple:	$(EXE)
		./$(EXE)

run-special:	$(EXE)
		cat special2.dat | \
		./mul -s | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore f32_mul

run-full:	$(EXE)
		../TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 2 | \
		./mul -s | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore f32_mul

$(EXE):		$(SRC)
		gcc -g -Wall -o $(EXE) $(SRC) -lm

clean:
		rm -f *~ $(EXE)
