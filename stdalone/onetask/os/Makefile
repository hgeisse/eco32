BUILD = ../../../build

SRC = start.s main.c
EXE = onetask
BIN = onetask.bin
MAP = onetask.map
EXO = onetask.exo

all:		$(BIN) $(EXO)

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -l $(BIN) -a 0x10000

$(EXO):		$(BIN)
		$(BUILD)/bin/bin2exo -S2 0x10000 $(BIN) $(EXO)

$(BIN):		$(EXE)
		$(BUILD)/bin/load -p $(EXE) $(BIN)

$(EXE):		$(SRC) task.dump
		$(BUILD)/bin/lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=stdalone.lnk \
		  -Wo-ldmap=$(MAP) -o $(EXE) $(SRC)

task.dump:
		../dump/dump ../task/task task.dump

clean:
		rm -f *~ task.dump $(EXE) $(BIN) $(MAP) $(EXO)
