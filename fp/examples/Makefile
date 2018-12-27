#
# Makefile
#

BUILD = ../../build

SRCS = ex01.c ex02.c ex03.c ex04.c ex05.c ex06.c ex07.c ex08.c
ASMS = $(patsubst %.c,%.s,$(SRCS))

all:		$(ASMS)

%.s:		%.c
		$(BUILD)/bin/lcc -S -o $@ $<

clean:
		rm -f *~ $(ASMS)
