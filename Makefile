CXXFLAGS += -g -std=c++2b -D_DEBUG -Wall -Wno-unknown-pragmas

bondlib: bondlib.cpp

.PHONY: clean test check

clean:
	rm -rf *.o bondlib

test: bondlib
	./bondlib

check: bondlib
	valgrind --leak-check=yes ./bondlib
