#
# Makefile for hex-to-Hauser converter
#

BUILD = ../../../build

SRC = x2h.c
EXE = x2h

all:		$(EXE)

install:	$(EXE)

$(EXE):		$(SRC)
		gcc -g -Wall -o $(EXE) $(SRC)

clean:
		rm -f *~ $(EXE)
