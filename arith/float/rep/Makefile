#
# Makefile for implementation and test of floating-point arithmetic
#

BUILD = ../../../build

SRC = rep.c
EXE = rep

all:		$(EXE)

install:	$(EXE)

run:		$(EXE)
		./$(EXE)

$(EXE):		$(SRC)
		gcc -g -Wall -o $(EXE) $(SRC) -lm

clean:
		rm -f *~ $(EXE)
