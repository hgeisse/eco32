#
# Makefile for implementation and test of floating-point arithmetic
#

BUILD = ../../../build

# number of test cases
NUM = 10000000
# test level (1 or 2)
LVL = 2

SRC = cmp.c
EXE = cmp

all:		$(EXE)

install:	$(EXE)

run-few:	$(EXE)
		./$(EXE)

run-special:	$(EXE)
		cat special2.dat | \
		./cmp -s_eq | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore f32_eq
		cat special2.dat | \
		./cmp -s_le | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore f32_le
		cat special2.dat | \
		./cmp -s_lt | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore f32_lt

run-many:	$(EXE)
		../TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 2 | \
		./cmp -s_eq | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore f32_eq
		../TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 2 | \
		./cmp -s_le | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore f32_le
		../TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 2 | \
		./cmp -s_lt | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore f32_lt

$(EXE):		$(SRC)
		gcc -g -Wall -o $(EXE) $(SRC) -lm

clean:
		rm -f *~ $(EXE)
