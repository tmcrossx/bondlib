// fms_iterable.t.cpp - test fms::iterable
#include <cassert>
#include "fms_iterable.h"

using namespace fms::iterable;

int test_constant = []() {
	constant c(1);
	assert(c);
	assert(*c == 1);
	++c;
	assert(c);
	assert(*c == 1);

	return 0;
}();
int test_pointer = []() {
	int i[] = {1, 2, 3};
	pointer p(i);
	assert(p);
	assert(*p == 1);
	++p;
	assert(*p == 2);
	++p;
	assert(*p == 3);
	++p; // *p is undefined

	return 0;
}();

int test_zero_pointer = []() {
	int i[] = { 1, 2, 0 };
	zero_pointer p(i);
	assert(p);
	assert(*p == 1);
	++p;
	assert(p);
	assert(*p == 2);
	++p;
	assert(!p);

	return 0;
}();

int test_take = []() {
	int i[] = { 1, 2, 3 };
	{
		pointer p(i);
		take t(p, 3);
		assert(t);
		assert(*t == 1);
		++t;
		assert(t);
		assert(*t == 2);
		++t;
		assert(*t == 3);
		++t;
		assert(!t);
	}
	{
		auto t = array(i);
		assert(t);
		assert(*t == 1);
		++t;
		assert(t);
		assert(*t == 2);
		++t;
		assert(*t == 3);
		++t;
		assert(!t);
	}

	return 0;
}();