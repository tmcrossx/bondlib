// fms_iterable.t.cpp - test fms::iterable
#include <cassert>
#include "fms_iterable.h"

using namespace fms::iterable;

int test_constant = []() {
	constant c(1);
	constant c2(c);
	c = c2;
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
	pointer p2(p);
	p = p2;
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
	null_terminated_pointer p(i);
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
		take<pointer<int>,int> t(p, 3);
		auto t2(t);
		t = t2;
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
	{
		auto t = array(i);
		assert(equal(t, t));
		assert(length(t) == 3);
		assert(length(skip(t, 1), length(t)) == 5);
	}

	return 0;
}();

int test_concatenate = []() {
	int i[] = { 1, 2, 3 };
	int j[] = { 4, 5, 6 };
	{
		auto p = array(i);
		auto q = array(j);
		auto c = concatenate(p, q);
		assert(c);
		assert(*c == 1);
		++c;
		assert(c);
		assert(*c == 2);
		++c;
		assert(*c == 3);
		++c;
		assert(c);
		assert(*c == 4);
		++c;
		assert(*c == 5);
		++c;
		assert(*c == 6);
		++c;
		assert(!c);
	}

	return 0;
}();

int test_merge = []() {
	int i[] = { 1, 3, 5 };
	int j[] = { 2, 4, 6 };
	{
		auto p = array(i);
		auto q = array(j);
		auto c = merge(p, q);
		assert(c);
		assert(*c == 1);
		++c;
		assert(c);
		assert(*c == 2);
		++c;
		assert(*c == 3);
		++c;
		assert(c);
		assert(*c == 4);
		++c;
		assert(*c == 5);
		++c;
		assert(*c == 6);
		++c;
		assert(!c);
	}
	{
		auto c = merge(array(i), array(j));
		assert(c);
		assert(*c == 1);
		++c;
		assert(c);
		assert(*c == 2);
		++c;
		assert(*c == 3);
		++c;
		assert(c);
		assert(*c == 4);
		++c;
		assert(*c == 5);
		++c;
		assert(*c == 6);
		++c;
		assert(!c);
	}

	return 0;
}();