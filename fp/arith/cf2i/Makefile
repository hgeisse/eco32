#
# Makefile for implementation and test of floating-point arithmetic
#

BUILD = ../../../build

# number of test cases
NUM = 10000000
# test level (1 or 2)
LVL = 2

SRC = cf2i.c
EXE = cf2i

GENSRC = gen1.c
GENEXE = gen1

all:		$(EXE) $(GENEXE)

install:	$(EXE) $(GENEXE)

run-few:	$(EXE)
		./$(EXE)

run-special:	$(EXE)
		cat special1.dat | \
		./$(EXE) -s | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore -rminMag f32_to_i32

run-many:	$(EXE)
		../TestFloat-3e/build/Linux-386-GCC/testfloat_gen \
		  -n $(NUM) -level $(LVL) f32 1 | \
		./$(EXE) -s | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore -rminMag f32_to_i32

run-most:	$(EXE) $(GENEXE)
		./$(GENEXE) -most | \
		./$(EXE) -s | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore -rminMag f32_to_i32

run-all:	$(EXE) $(GENEXE)
		./$(GENEXE) -all | \
		./$(EXE) -s | \
		../TestFloat-3e/build/Linux-386-GCC/testfloat_ver \
		  -tininessbefore -rminMag f32_to_i32

$(EXE):		$(SRC)
		gcc -g -Wall -o $(EXE) $(SRC) -lm

$(GENEXE):	$(GENSRC)
		gcc -g -Wall -o $(GENEXE) $(GENSRC)

clean:
		rm -f *~ $(EXE) $(GENEXE)
