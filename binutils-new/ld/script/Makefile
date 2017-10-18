#
# Makefile for scriptfile parser
#

CC = gcc
CFLAGS = -g -Wall -Wno-unused

SRCS = readscript.c utils.c parser.tab.c lex.yy.c script.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
LIB = libreadscript.a

all:		$(LIB)

$(LIB):		$(OBJS)
		ar -crs $(LIB) $(OBJS)

%.o:		%.c
		$(CC) $(CFLAGS) -o $@ -c $<

parser.tab.h:	parser.y
		bison -d parser.y

parser.tab.c:	parser.y
		bison -d parser.y

lex.yy.c:	scanner.l parser.tab.h
		flex scanner.l

-include depend.mak

depend.mak:	parser.tab.c lex.yy.c
		$(CC) $(CFLAGS) -MM $(SRCS) >depend.mak

clean:
		rm -f *~ *.o $(LIB)
		rm -f parser.tab.h parser.tab.c lex.yy.c depend.mak
