#
# Makefile
#

BUILD=../../../build

all:
		$(BUILD)/bin/lcc -o test00.s -S test00.c
		$(BUILD)/bin/lcc -o test01.s -S test01.c
		$(BUILD)/bin/lcc -o test02.s -S test02.c
		$(BUILD)/bin/lcc -o test03.s -S test03.c
		$(BUILD)/bin/lcc -o test04.s -S test04.c

clean:
		rm -f *~ *.s
