#
# Makefile for implementation and test of floating-point arithmetic
#

BUILD = ../../../../build

# number of test cases
NUM_1M = 1000000
NUM_10M = 10000000
NUM_100M = 100000000
NUM_1000M = 1000000000

# test level (1 or 2)
LVL_1 = 1
LVL_2 = 2

INCDIR = ../../test/include
LIB = ../../test/lib/softfloat.a

GEN = ../../test/bin/testfloat_gen
VER = ../../test/bin/testfloat_ver

SRC = cfi.c
EXE = cfi

all:		$(EXE)

install:	$(EXE)

run-round-simple:	$(EXE)
		./$(EXE) -round -simple

run-round-selected:	$(EXE)
		./$(EXE) -round -selected

run-round-intervals:	$(EXE)
		./$(EXE) -round -intervals

run-round-specials:	$(EXE)
		cat special1.dat | \
		./$(EXE) -round -server | \
		$(VER) -rnear_even -exact f32_to_i32

run-round-extremes:	$(EXE)
		cat extreme1.dat | \
		./$(EXE) -round -server | \
		$(VER) -rnear_even -exact f32_to_i32

run-round-1M:		$(EXE)
		$(GEN) -n $(NUM_1M) -level $(LVL_1) f32 1 | \
		./$(EXE) -round -server | \
		$(VER) -rnear_even -exact f32_to_i32

run-round-10M:		$(EXE)
		$(GEN) -n $(NUM_10M) -level $(LVL_2) f32 1 | \
		./$(EXE) -round -server | \
		$(VER) -rnear_even -exact f32_to_i32

run-round-100M:		$(EXE)
		$(GEN) -n $(NUM_100M) -level $(LVL_2) f32 1 | \
		./$(EXE) -round -server | \
		$(VER) -rnear_even -exact f32_to_i32

run-round-1000M:	$(EXE)
		$(GEN) -n $(NUM_1000M) -level $(LVL_2) f32 1 | \
		./$(EXE) -round -server | \
		$(VER) -rnear_even -exact f32_to_i32

run-trunc-simple:	$(EXE)
		./$(EXE) -trunc -simple

run-trunc-selected:	$(EXE)
		./$(EXE) -trunc -selected

run-trunc-intervals:	$(EXE)
		./$(EXE) -trunc -intervals

run-trunc-specials:	$(EXE)
		cat special1.dat | \
		./$(EXE) -trunc -server | \
		$(VER) -rminMag -exact f32_to_i32

run-trunc-extremes:	$(EXE)
		cat extreme1.dat | \
		./$(EXE) -trunc -server | \
		$(VER) -rminMag -exact f32_to_i32

run-trunc-1M:		$(EXE)
		$(GEN) -n $(NUM_1M) -level $(LVL_1) f32 1 | \
		./$(EXE) -trunc -server | \
		$(VER) -rminMag -exact f32_to_i32

run-trunc-10M:		$(EXE)
		$(GEN) -n $(NUM_10M) -level $(LVL_2) f32 1 | \
		./$(EXE) -trunc -server | \
		$(VER) -rminMag -exact f32_to_i32

run-trunc-100M:		$(EXE)
		$(GEN) -n $(NUM_100M) -level $(LVL_2) f32 1 | \
		./$(EXE) -trunc -server | \
		$(VER) -rminMag -exact f32_to_i32

run-trunc-1000M:	$(EXE)
		$(GEN) -n $(NUM_1000M) -level $(LVL_2) f32 1 | \
		./$(EXE) -trunc -server | \
		$(VER) -rminMag -exact f32_to_i32

run-floor-simple:	$(EXE)
		./$(EXE) -floor -simple

run-floor-selected:	$(EXE)
		./$(EXE) -floor -selected

run-floor-intervals:	$(EXE)
		./$(EXE) -floor -intervals

run-floor-specials:	$(EXE)
		cat special1.dat | \
		./$(EXE) -floor -server | \
		$(VER) -rmin -exact f32_to_i32

run-floor-extremes:	$(EXE)
		cat extreme1.dat | \
		./$(EXE) -floor -server | \
		$(VER) -rmin -exact f32_to_i32

run-floor-1M:		$(EXE)
		$(GEN) -n $(NUM_1M) -level $(LVL_1) f32 1 | \
		./$(EXE) -floor -server | \
		$(VER) -rmin -exact f32_to_i32

run-floor-10M:		$(EXE)
		$(GEN) -n $(NUM_10M) -level $(LVL_2) f32 1 | \
		./$(EXE) -floor -server | \
		$(VER) -rmin -exact f32_to_i32

run-floor-100M:		$(EXE)
		$(GEN) -n $(NUM_100M) -level $(LVL_2) f32 1 | \
		./$(EXE) -floor -server | \
		$(VER) -rmin -exact f32_to_i32

run-floor-1000M:	$(EXE)
		$(GEN) -n $(NUM_1000M) -level $(LVL_2) f32 1 | \
		./$(EXE) -floor -server | \
		$(VER) -rmin -exact f32_to_i32

run-ceil-simple:	$(EXE)
		./$(EXE) -ceil -simple

run-ceil-selected:	$(EXE)
		./$(EXE) -ceil -selected

run-ceil-intervals:	$(EXE)
		./$(EXE) -ceil -intervals

run-ceil-specials:	$(EXE)
		cat special1.dat | \
		./$(EXE) -ceil -server | \
		$(VER) -rmax -exact f32_to_i32

run-ceil-extremes:	$(EXE)
		cat extreme1.dat | \
		./$(EXE) -ceil -server | \
		$(VER) -rmax -exact f32_to_i32

run-ceil-1M:		$(EXE)
		$(GEN) -n $(NUM_1M) -level $(LVL_1) f32 1 | \
		./$(EXE) -ceil -server | \
		$(VER) -rmax -exact f32_to_i32

run-ceil-10M:		$(EXE)
		$(GEN) -n $(NUM_10M) -level $(LVL_2) f32 1 | \
		./$(EXE) -ceil -server | \
		$(VER) -rmax -exact f32_to_i32

run-ceil-100M:		$(EXE)
		$(GEN) -n $(NUM_100M) -level $(LVL_2) f32 1 | \
		./$(EXE) -ceil -server | \
		$(VER) -rmax -exact f32_to_i32

run-ceil-1000M:	$(EXE)
		$(GEN) -n $(NUM_1000M) -level $(LVL_2) f32 1 | \
		./$(EXE) -ceil -server | \
		$(VER) -rmax -exact f32_to_i32

$(EXE):		$(SRC)
		gcc -g -Wall -I$(INCDIR) -o $(EXE) $(SRC) $(LIB) -lm

clean:
		rm -f *~ $(EXE)
