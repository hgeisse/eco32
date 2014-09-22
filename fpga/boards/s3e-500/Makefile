#
# Makefile for ECO32 on Digilent S3E starter kit board
#

BUILD = ../../../build

.PHONY:		all install clean

all:		build/eco32.bit

install:	build/eco32.bit
#		$(BUILD)/bin/bit2exo 0x180000 \
#		  xsa-xst-3/eco32.bit xsa-xst-3/eco32.exo
#		mkdir -p $(BUILD)/eco32/xsa-xst-3
#		cp xsa-xst-3/eco32.bit $(BUILD)/eco32/xsa-xst-3
#		cp xsa-xst-3/eco32.exo $(BUILD)/eco32/xsa-xst-3

clean:
		mv build/eco32.xise .
		mv build/eco32.bit .
		rm -rf build/*
		mv eco32.xise build
		mv eco32.bit build
		rm -f *~
