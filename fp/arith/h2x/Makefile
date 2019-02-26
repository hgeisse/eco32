#
# Makefile for Hauser-to-hex converter
#

BUILD = ../../../build

SRC = h2x.c
EXE = h2x

all:		$(EXE)

install:	$(EXE)

$(EXE):		$(SRC)
		gcc -g -Wall -o $(EXE) $(SRC)

clean:
		rm -f *~ $(EXE)
