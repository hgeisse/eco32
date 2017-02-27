#
# Makefile for ECO32 ROM monitor, running from ROM
#

BUILD = ../../../../build

SRC = start.s \
      main.c \
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
      ../../common/end.s

.PHONY:		all install clean

all:		monitor.bin

install:	monitor.bin
		mkdir -p $(BUILD)/monitor/sim-rom
		cp monitor.bin $(BUILD)/monitor/sim-rom

monitor.bin:	$(SRC)
		$(BUILD)/bin/lcc -A -Wo-rom -I../../common \
		  -Wl-rc -Wl0xE0000000 \
		  -Wl-rd -Wl0xC000C000 \
		  -Wl-m -Wlmonitor.map \
		  -o monitor.bin $(SRC)

clean:
		rm -f *~
		rm -f monitor.map monitor.bin
