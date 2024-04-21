// fms_iterable.t.cpp - test fms::iterable
#include <cassert>
#include "fms_iterable.h"
#include "fms_time.h"

using namespace fms::iterable;

int test_constant = []() {
	constant c(1);
	constant c2(c);
	c = c2;
	assert(c == c2);
	assert(!(c2 != c));

	assert(c);
	assert(*c == 1);
	++c;
	assert(c);
	assert(*c == 1);

	return 0;
}();

int test_once = []() {
	{
		once o(1);
		once o2(o);
		o = o2;
		assert(o == o2);
		assert(!(o2 != o));

		assert(o);
		assert(*o == 1);
		++o;
		assert(!o);
	}

	return 0;
}();

int test_iota = []() {
	{
		iota i(2);
		assert(i);
		auto i2(i);
		i = i2;
		assert(i == i2);
		assert(!(i2 != i));

		assert(*i == 2);
		++i;
		assert(i);
		assert(*i == 3);
	}

	return 0;
}();

int test_power = []() {
	{
		power p(2);
		assert(p);
		auto p2(p);
		p = p2;
		assert(p == p2);
		assert(!(p2 != p));

		assert(*p == 1);
		++p;
		assert(p);
		assert(*p == 2);
		++p;
		assert(p);
		assert(*p == 4);
		++p;
		assert(p);
	}

	return 0;	
}();

int test_factorial = []() {
	{
		factorial f;
		assert(f);
		auto f2(f);
		assert(f == f2);
		f = f2;
		assert(!(f2 != f));

		assert(*f == 1);
		assert(*++f == 1);
		assert(*++f == 2);
		assert(*++f == 6);
	}

	return 0;
}();

int test_apply = []() {
	{
		apply a([](int x) { return 0.5 * x; }, iota<int>{});
		auto a2(a);
		assert(a == a2);
		a = a2;
		assert(!(a2 != a));

		assert(a);
		assert(*a == 0.);
		++a;
		assert(a);
		assert(*a == 0.5);
		++a;
		assert(a);
		assert(*a == 1.);
	}

	return 0;
	}();

int test_until = []() {
	{
		until a([](double x) { return x <= std::numeric_limits<double>::epsilon(); }, power(0.5));
		auto a2(a);
		assert(a == a2);
		a = a2;
		assert(!(a2 != a));

		auto len = length(a);
		assert(len == 52);
	}

	return 0;
}();

int test_filter = []() {
	{
		filter a([](int i) { return i % 2; }, iota<int>{});
		auto a2(a);
		assert(a == a2);
		a = a2;
		assert(!(a2 != a));

		assert(a);
		assert(*a == 1);
		++a;
		assert(a);
		assert(*a == 3);
		++a;
		assert(a);
		assert(*a == 5);
	}

	return 0;
	}();

int test_fold = []() {
	{
		fold f(std::plus<int>{}, iota<int>{});
		auto f2(f);
		assert(f == f2);
		f = f2;
		assert(!(f2 != f));

		assert(f);
		assert(*f == 0);
		++f;
		assert(f);
		assert(*f == 0);
		++f;
		assert(f);
		assert(*f == 1);
		++f;
		assert(f);
		assert(*f == 3);
	}
	{
		fold f(std::plus<int>{}, iota<int>{});
		fold g(std::plus<int>{}, iota<int>{});
		assert(f == g);
		++g;
		assert(f != g);
	}
	{
		fold f(std::multiplies<int>{}, iota<int>(1), 1);
		// 1 * 1 * 2 * 3
		f = skip(f, 3);
		assert(*f == 6);
	}
	{
		fold f(std::multiplies<int>{}, iota<int>(1), 1);
		// 1 * 1 * 2 * 3
		auto f_ = last(take(f, 4));
		assert(*f_ == 6);
	}

	return 0;
}();

int test_pointer = []() {
	int i[] = {1, 2, 3};
	pointer p(i);
	pointer p2(p);
	assert(p == p2);
	p = p2;
	assert(!(p2 != p));

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
	{
		int i[] = { 1, 2, 0 };
		null_terminated_pointer p(i);
		auto i2(i);
		assert(i == i2);
		//i = i2; // int[3] not assignable
		//assert(!(i2 != i));

		assert(p);
		assert(*p == 1);
		++p;
		assert(p);
		assert(*p == 2);
		++p;
		assert(!p);
	}
	{
		char i[] = "ab";
		null_terminated_pointer s(i);
		assert(s);
		assert(*s =='a');
		++s;
		assert(*s == 'b');
		++s;
		assert(!s);
	}

	return 0;
}();

int test_take = []() {
	int i[] = { 1, 2, 3 };
	{
		pointer p(i);
		take<pointer<int>,int> t(p, 3);
		auto t2(t);
		t = t2;
		assert(t == t2);
		assert(!(t2 != t));

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

int test_exp = []() {
	{
		double x = 1;
		const auto eps = [](double x) { return x + 1 == 1; };
		auto expx = sum(until(eps, power(x)/factorial()));
		auto len = length(expx);
		len = len; // 19
		double exp1 = std::exp(1.);
		double diff;
		diff = *last(expx) - exp1;
		diff = diff;
		auto dt1 = fms::time<std::chrono::microseconds>([]() { std::exp(1.); }, 10'000);
		auto dt = fms::time([expx]() { *last(expx); }, 10'000);
		auto dd = 1000*dt / dt1; // ~250
		dd = dd;
	}

	return 0;
}();
