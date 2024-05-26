CXXFLAGS += -g -std=c++2b -D_DEBUG -Wall -Wno-unknown-pragmas

bondlib.t: bondlib.t.cpp

.PHONY: clean

clean:
	rm -rf *.o bondlib.t

test: bondlib.t
	./bondlib.t

tidy: bondlib.t.cpp
	run-clang-tidy bondlib.t.cpp

check: bondlib.t
	valgrind --leak-check=yes ./bondlib.t
