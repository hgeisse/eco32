#
# Makefile for building a hosted executable
#

BUILD = ../../../../../build
INC = ../../include
LIB = ../../lib
CMD = ../../cmd
TXT = ../../txt

CC = $(BUILD)/bin/lcc
CFLAGS = -A -Wo-nostdinc -Wo-nostdlib -I$(INC)
LD = $(BUILD)/bin/ld
LDFLAGS = -c 0xC0100000
CRT0 = $(LIB)/crt0.o
LDLIBS = $(LIB)/libc.a

SRC = chkdsk.c
OBJ = $(patsubst %.c,%.o,$(SRC))
EXE = chkdsk

all:		$(EXE)
		mkdir -p $(CMD)
		cp $(EXE) $(CMD)
		mkdir -p $(TXT)
		cp $(EXE).txt $(TXT)

$(EXE):		$(OBJ)
		$(LD) $(LDFLAGS) -o $(EXE) $(CRT0) $(OBJ) $(LDLIBS)

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ -c $<

clean:
		rm -f *~ $(OBJ) $(EXE)
