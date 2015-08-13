#
# Makefile to build the test
#

BUILD = ../../../build

ROM_SIZE_KB = 64

BIN = test_054.bin
DAT = test_054.dat

all:		$(DAT)

install:	$(DAT)

$(DAT):		$(BIN)
		$(BUILD)/bin/bin2dat $(BIN) $(DAT) $(ROM_SIZE_KB)

$(BIN):		copy/copy.bin hellodsp/hellodsp.bin
		cat copy/copy.bin hellodsp/hellodsp.bin >$(BIN)

copy/copy.bin:	size.s
		$(MAKE) -C copy all

size.s:		hellodsp/hellodsp.bin
		/bin/echo -e ";" >size.s
		/bin/echo -e "; set number of bytes to be copied" >>size.s
		/bin/echo -e ";" >>size.s
		/bin/echo -e -n "\t.set\tsize," >>size.s
		stat -c %s hellodsp/hellodsp.bin >>size.s
		/bin/echo -e "\t.export\tsize" >>size.s

hellodsp/hellodsp.bin:
		$(MAKE) -C hellodsp all

clean:
		rm -f *~ size.s $(BIN) $(DAT)
		$(MAKE) -C copy clean
		$(MAKE) -C hellodsp clean
