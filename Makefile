CXXFLAGS += -g -std=c++2b -D_DEBUG -Wall -Wno-unknown-pragmas

bondlib: bondlib.cpp fms_iterable.t.cpp

.PHONY: clean

clean:
	rm -rf *.o bondlib

test: bondlib
	./bondlib

check: bondlib
	valgrind --leak-check=yes ./bondlib
