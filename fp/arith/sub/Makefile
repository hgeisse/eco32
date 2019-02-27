#
# Makefile for implementation and test of floating-point arithmetic
#

BUILD = ../../../build

# number of test cases
NUM = 100000000
# test level (1 or 2)
LVL = 2

SRC = sub.c
EXE = sub

all:		$(EXE)

install:	$(EXE)

run-simple:	$(EXE)
		./$(EXE)

run-special:	$(EXE)
		cat spec2.dat | \
		./sub -s | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_sub

run-full:	$(EXE)
		../TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 2 | \
		./sub -s | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver f32_sub

$(EXE):		$(SRC)
		gcc -g -Wall -o $(EXE) $(SRC) -lm

clean:
		rm -f *~ $(EXE)
