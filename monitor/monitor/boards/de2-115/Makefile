#
# Makefile for ECO32 ROM monitor
#

BUILD = ../../../../build

SRC = start.s \
      ../../common/main.c \
      ../../common/command.c \
      ../../common/instr.c \
      ../../common/asm.c \
      ../../common/disasm.c \
      ../../common/load.c \
      ../../common/boot.c \
      ../../common/cpu.c \
      ../../common/mmu.c \
      ../../common/getline.c \
      ../../common/romlib.c \
      ../../common/keyboard.s \
      ../../kbdtbls/kbdtbls.s \
      ../../common/display.s \
      ../../common/serial.s \
      ../../common/dskctl.s \
      ../../common/dskser.s \
      ../../common/dsksdc.c

.PHONY:		all install clean

all:		monitor.bin

install:	monitor.bin
		mkdir -p $(BUILD)/monitor/de2-115
		cp monitor.bin $(BUILD)/monitor/de2-115

monitor.bin:	prelim.bin
		cat ../../copy/copy.bin prelim.bin >monitor.bin

prelim.bin:	prelim
		$(BUILD)/bin/load -p prelim prelim.bin

prelim:		$(SRC)
		$(BUILD)/bin/lcc -A \
		  -Wo-nostdinc -Wo-nostdlib \
		  -Wo-ldscript=standalone.lnk \
		  -Wo-ldmap=monitor.map \
		  -o prelim $(SRC)

clean:
		rm -f *~
		rm -f prelim prelim.bin monitor.map monitor.bin
