#
# Makefile for library test
#

BUILD = ../../../build

ELEFUNT = ../../../fp/tests/elefunt

TSTS = tmacha.c tsqrt.c talog.c texp.c tpower.c \
       tsin.c ttan.c tasin.c tatan.c tsinh.c ttanh.c
AUX1 = macharf.c storef.c sran.c srandl.c ipowf.c
AUX2 = initseed.c maxtest.c
SRCS = elefun.c $(TSTS) $(AUX1) $(AUX2)

EXE = elefun
BIN = elefun.bin
EXE_LINUX = elefun-linux

all:		$(BIN) $(EXE_LINUX)

install:	$(BIN) $(EXE_LINUX)

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN)

run-linux:	$(EXE_LINUX)
		./$(EXE_LINUX)

$(BIN):		$(EXE)
		$(BUILD)/bin/load $(EXE) $(BIN)

$(EXE):		$(SRCS)
		$(BUILD)/bin/lcc -A -o $(EXE) $(SRCS) f2d.c -lm

$(EXE_LINUX):	$(SRCS)
		gcc -Wall -o $(EXE_LINUX) $(SRCS) -lm

tmacha.c:	elefunt
		cp elefunt/c/sp/tmacha.c .
		cp elefunt/c/sp/tsqrt.c .
		cp elefunt/c/sp/talog.c .
		cp elefunt/c/sp/texp.c .
		cp elefunt/c/sp/tpower.c .
		cp elefunt/c/sp/tsin.c .
		cp elefunt/c/sp/ttan.c .
		cp elefunt/c/sp/tasin.c .
		cp elefunt/c/sp/tatan.c .
		cp elefunt/c/sp/tsinh.c .
		cp elefunt/c/sp/ttanh.c .

macharf.c:	elefunt
		cp elefunt/c/sp/macharf.c .
		cp elefunt/c/sp/storef.c .
		cp elefunt/c/sp/sran.c .
		cp elefunt/c/sp/srandl.c .
		cp elefunt/c/sp/ipowf.c .

initseed.c:	elefunt
		cp elefunt/c/common/initseed.c .
		cp elefunt/c/common/maxtest.c .

elefunt:
		tar -xf $(ELEFUNT)/elefunt.tar.gz

clean:
		rm -rf elefunt
		rm -f $(TSTS) $(AUX1) $(AUX2)
		rm -f *~ $(EXE) $(BIN) $(EXE_LINUX)
