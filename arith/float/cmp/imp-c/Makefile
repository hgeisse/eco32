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

SRC = cmp.c
EXE = cmp

all:		$(EXE) cmpl

install:	$(EXE) cmpl

run-eq-simple:		$(EXE)
		./$(EXE) -eq -simple

run-eq-selected:	$(EXE)
		./$(EXE) -eq -selected

run-eq-intervals:	$(EXE)
		./$(EXE) -eq -intervals

run-eq-specials:	$(EXE)
		cat special2.dat | \
		./$(EXE) -eq -server | \
		$(VER) f32_eq

run-eq-extremes:	$(EXE)
		cat extreme2.dat | \
		./$(EXE) -eq -server | \
		$(VER) f32_eq

run-eq-1M:	$(EXE)
		$(GEN) -n $(NUM_1M) -level $(LVL_1) f32 2 | \
		./$(EXE) -eq -server | \
		$(VER) f32_eq

run-eq-10M:	$(EXE)
		$(GEN) -n $(NUM_10M) -level $(LVL_2) f32 2 | \
		./$(EXE) -eq -server | \
		$(VER) f32_eq

run-eq-100M:	$(EXE)
		$(GEN) -n $(NUM_100M) -level $(LVL_2) f32 2 | \
		./$(EXE) -eq -server | \
		$(VER) f32_eq

run-eq-1000M:	$(EXE)
		$(GEN) -n $(NUM_1000M) -level $(LVL_2) f32 2 | \
		./$(EXE) -eq -server | \
		$(VER) f32_eq

run-ne-simple:		$(EXE)
		./$(EXE) -ne -simple

run-ne-selected:	$(EXE)
		./$(EXE) -ne -selected

run-ne-intervals:	$(EXE)
		./$(EXE) -ne -intervals

run-ne-specials:	$(EXE) cmpl
		cat special2.dat | \
		./$(EXE) -ne -server | \
		./cmpl | \
		$(VER) f32_eq

run-ne-extremes:	$(EXE) cmpl
		cat extreme2.dat | \
		./$(EXE) -ne -server | \
		./cmpl | \
		$(VER) f32_eq

run-ne-1M:	$(EXE) cmpl
		$(GEN) -n $(NUM_1M) -level $(LVL_1) f32 2 | \
		./$(EXE) -ne -server | \
		./cmpl | \
		$(VER) f32_eq

run-ne-10M:	$(EXE) cmpl
		$(GEN) -n $(NUM_10M) -level $(LVL_2) f32 2 | \
		./$(EXE) -ne -server | \
		./cmpl | \
		$(VER) f32_eq

run-ne-100M:	$(EXE) cmpl
		$(GEN) -n $(NUM_100M) -level $(LVL_2) f32 2 | \
		./$(EXE) -ne -server | \
		./cmpl | \
		$(VER) f32_eq

run-ne-1000M:	$(EXE) cmpl
		$(GEN) -n $(NUM_1000M) -level $(LVL_2) f32 2 | \
		./$(EXE) -ne -server | \
		./cmpl | \
		$(VER) f32_eq

run-le-simple:		$(EXE)
		./$(EXE) -le -simple

run-le-selected:	$(EXE)
		./$(EXE) -le -selected

run-le-intervals:	$(EXE)
		./$(EXE) -le -intervals

run-le-specials:	$(EXE)
		cat special2.dat | \
		./$(EXE) -le -server | \
		$(VER) f32_le

run-le-extremes:	$(EXE)
		cat extreme2.dat | \
		./$(EXE) -le -server | \
		$(VER) f32_le

run-le-1M:	$(EXE)
		$(GEN) -n $(NUM_1M) -level $(LVL_1) f32 2 | \
		./$(EXE) -le -server | \
		$(VER) f32_le

run-le-10M:	$(EXE)
		$(GEN) -n $(NUM_10M) -level $(LVL_2) f32 2 | \
		./$(EXE) -le -server | \
		$(VER) f32_le

run-le-100M:	$(EXE)
		$(GEN) -n $(NUM_100M) -level $(LVL_2) f32 2 | \
		./$(EXE) -le -server | \
		$(VER) f32_le

run-le-1000M:	$(EXE)
		$(GEN) -n $(NUM_1000M) -level $(LVL_2) f32 2 | \
		./$(EXE) -le -server | \
		$(VER) f32_le

run-lt-simple:		$(EXE)
		./$(EXE) -lt -simple

run-lt-selected:	$(EXE)
		./$(EXE) -lt -selected

run-lt-intervals:	$(EXE)
		./$(EXE) -lt -intervals

run-lt-specials:	$(EXE)
		cat special2.dat | \
		./$(EXE) -lt -server | \
		$(VER) f32_lt

run-lt-extremes:	$(EXE)
		cat extreme2.dat | \
		./$(EXE) -lt -server | \
		$(VER) f32_lt

run-lt-1M:	$(EXE)
		$(GEN) -n $(NUM_1M) -level $(LVL_1) f32 2 | \
		./$(EXE) -lt -server | \
		$(VER) f32_lt

run-lt-10M:	$(EXE)
		$(GEN) -n $(NUM_10M) -level $(LVL_2) f32 2 | \
		./$(EXE) -lt -server | \
		$(VER) f32_lt

run-lt-100M:	$(EXE)
		$(GEN) -n $(NUM_100M) -level $(LVL_2) f32 2 | \
		./$(EXE) -lt -server | \
		$(VER) f32_lt

run-lt-1000M:	$(EXE)
		$(GEN) -n $(NUM_1000M) -level $(LVL_2) f32 2 | \
		./$(EXE) -lt -server | \
		$(VER) f32_lt

$(EXE):		$(SRC)
		gcc -g -Wall -I$(INCDIR) -o $(EXE) $(SRC) $(LIB) -lm

cmpl:		cmpl.c
		gcc -g -Wall -o cmpl cmpl.c

clean:
		rm -f *~ $(EXE) cmpl
