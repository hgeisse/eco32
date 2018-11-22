#
# Makefile
#

OUTFLAG = -o

CC = ../../../build/bin/lcc -I$(PORT_DIR) -I. -Wo-nostdinc -Wo-nostdlib \
  -Wo-ldscript=$(PORT_DIR)/stdalone.lnk -Wo-ldmap=coremark.map \
  $(PORT_DIR)/start.s

PORT_SRCS = $(PORT_DIR)/core_portme.c $(PORT_DIR)/ee_printf.c

LOAD = echo "Please set LOAD macro"
RUN = echo "Please set RUN macro"

port_prebuild:

port_postbuild:

port_preload:

port_postload:

port_prerun:

port_postrun:

PORT_CLEAN = coremark coremark.map eco32/*~
