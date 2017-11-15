#
# Makefile to build the "hello world" test
#

BUILD = ../../../build

ROM_SIZE_KB = 64

BIN = test_100.bin
DAT = test_100.dat

all:		$(DAT)

install:	$(DAT)

run:		$(BIN)
		$(BUILD)/bin/sim -i -s 1 -t 0 -r $(BIN)

$(DAT):		$(BIN)
		$(BUILD)/bin/bin2dat $(BIN) $(DAT) $(ROM_SIZE_KB)

$(BIN):		copy/copy.bin hello/hello.bin
		cat copy/copy.bin hello/hello.bin >$(BIN)

copy/copy.bin:	size.s
		$(MAKE) -C copy all

size.s:		hello/hello.bin
		/bin/echo -e ";" >size.s
		/bin/echo -e "; set number of bytes to be copied" >>size.s
		/bin/echo -e ";" >>size.s
		/bin/echo -e -n "\t.set\tsize," >>size.s
		stat -c %s hello/hello.bin >>size.s
		/bin/echo -e "\t.export\tsize" >>size.s

hello/hello.bin:
		$(MAKE) -C hello all

clean:
		rm -f *~ size.s $(BIN) $(DAT)
		$(MAKE) -C copy clean
		$(MAKE) -C hello clean
