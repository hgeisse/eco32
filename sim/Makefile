#
# Makefile for ECO32 simulator
#

BUILD = ../build

CC = gcc
CFLAGS = -g -Wall -I./getline -I/usr/X11R7/include
LDFLAGS = -g -L./getline -L/usr/X11R7/lib -Wl,-rpath -Wl,/usr/X11R7/lib
LDLIBS = -lgetline -lX11 -lpthread -lm

SRCS = main.c console.c error.c except.c command.c \
       instr.c asm.c disasm.c cpu.c trace.c fpu/fpu.c \
       fpu/add.c fpu/sub.c fpu/mul.c fpu/div.c \
       fpu/sqrt.c fpu/i2f.c fpu/f2i.c \
       mmu.c icache.c dcache.c ram.c rom.c io.c \
       timer.c dsp.c kbd.c serial.c disk.c sdcard.c \
       output.c shutdown.c graph1.c graph2.c mouse.c \
       bio.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
BIN = sim

.PHONY:		all install clean

all:		$(BIN)

install:	$(BIN)
		mkdir -p $(BUILD)/bin
		cp $(BIN) $(BUILD)/bin

getline/libgetline.a:
		$(MAKE) -C getline

$(BIN):		$(OBJS) getline/libgetline.a
		$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LDLIBS)

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ -c $<

depend.mak:
		$(CC) -MM -MG $(CFLAGS) $(SRCS) >depend.mak

-include depend.mak

clean:
		$(MAKE) -C getline clean
		rm -f *~ fpu/*~ $(OBJS) $(BIN) depend.mak
