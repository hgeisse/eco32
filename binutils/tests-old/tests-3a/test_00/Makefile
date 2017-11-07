#
# Makefile for test
#

BUILD=../../../build

TEST=test_00

all:
		$(BUILD)/bin/as -o $(TEST).o $(TEST).s
		$(BUILD)/bin/dof -a $(TEST).o >$(TEST).lst
		@if cmp -s $(TEST).lst $(TEST).ref ; then \
		  echo "$(TEST) result: ok" ; \
		else \
		  echo "$(TEST) result: error" ; \
		fi

clean:
		rm -f *~ $(TEST).o $(TEST).lst
