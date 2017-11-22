BUILD = ../../../build

SRC = start.s main.c
EXE = twotasks-2
BIN = twotasks-2.bin
MAP = twotasks-2.map
EXO = twotasks-2.exo

all:		$(BIN) $(EXO)

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN) -a 0x10000

$(EXO):		$(BIN)
		$(BUILD)/bin/bin2exo -S2 0x10000 $(BIN) $(EXO)

$(BIN):		$(EXE)
		$(BUILD)/bin/load -p $(EXE) $(BIN)

$(EXE):		$(SRC) task1.dump task2.dump
		$(BUILD)/bin/lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=stdalone.lnk \
		  -Wo-ldmap=$(MAP) -o $(EXE) $(SRC)

task1.dump:
		../dump/dump ../task1/task1 task1.dump

task2.dump:
		../dump/dump ../task2/task2 task2.dump

clean:
		rm -f *~ task1.dump task2.dump
		rm -f $(EXE) $(BIN) $(MAP) $(EXO)
