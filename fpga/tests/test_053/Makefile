#
# Makefile to build the test
#

BUILD = ../../../build

ROM_SIZE_KB = 64

BIN = test_053.bin
DAT = test_053.dat

all:		$(DAT)

install:	$(DAT)

$(DAT):		$(BIN)
		$(BUILD)/bin/bin2dat $(BIN) $(DAT) $(ROM_SIZE_KB)

$(BIN):		copy/copy.bin loop/loop.bin
		cat copy/copy.bin loop/loop.bin >$(BIN)

copy/copy.bin:	size.s
		$(MAKE) -C copy all

size.s:		loop/loop.bin
		/bin/echo -e ";" >size.s
		/bin/echo -e "; set number of bytes to be copied" >>size.s
		/bin/echo -e ";" >>size.s
		/bin/echo -e -n "\t.set\tsize," >>size.s
		stat -c %s loop/loop.bin >>size.s
		/bin/echo -e "\t.export\tsize" >>size.s

loop/loop.bin:
		$(MAKE) -C loop all

clean:
		rm -f *~ size.s $(BIN) $(DAT)
		$(MAKE) -C copy clean
		$(MAKE) -C loop clean
