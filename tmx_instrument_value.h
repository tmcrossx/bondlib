// tmx_instrument_value.h - Instrument value type
#pragma once
#include <algorithm>
#include <vector>
#include "tmx_instrument_view.h"

namespace tmx::instrument {

	// instrument value type
	template<class U = double, class C = double>
	class value : public instrument::view<U, C> {
		std::vector<U> u;
		std::vector<C> c;
		// Update view with vectors.
		void sync()
		{
			view<U, C>::u = std::span<U>(u.data(), u.size());
			view<U, C>::c = std::span<C>(c.data(), c.size());
		}
	public:
		value(size_t n = 0)
			: u{}, c{}
		{
			u.reserve(n);
			c.reserve(n);
		}
		value(size_t m, const U* u, const C* c)
			: u(u, u + m), c(c, c + m)
		{
			sync();
		}
		value(const std::span <U>& u, const std::span<C>& c)
			: value(u.size(), u.data(), c.data())
		{
			//assert(u.size() == c.size());

			sync();
		}
		template<fms::iterable::input I, class T = typename I::value_type>
		value(I i)
		{
			while (i) {
				u.push_back((*i).u);
				c.push_back((*i).c);
				++i;
			}
			sync();
		}
		value(const value& v)
			: u(v.u), c(v.c)
		{
			sync();
		}
		value& operator=(const value& v)
		{
			if (this != &v) {
				u = v.u;
				c = v.c;
				sync();
			}

			return *this;
		}
		~value()
		{ }

		// add cash flow keeping times sorted
		value& push_back(U _u, C _c)
		{
			if (u.size() > 0 and u.back() == _u) {
				c.back() += _c;
			}
			else {
				//assert(u.size() == 0 or u.back() < _u);

				u.push_back(_u);
				c.push_back(_c);
				sync();
			}

			return *this;
		}

		// add cash flow keeping times sorted
		value& push_front(U _u, C _c)
		{
			if (u.size() > 0 and u.front() == _u) {
				c.front() += _c;
			}
			else {
				//assert(u.size() == 0 or u.front() > _u);

				u.insert(u.begin(), _u);
				c.insert(c.begin(), _c);
				sync();
			}

			return *this;
		}

		// Specify price making present value zero.
		// If p = value::present(instrument, curve) 
		// then 0 = value::present(instrument.price(p), curve)
		value& price(C p)
		{
			if (u.front() == 0) {
				c.front() -= p;
			}
			else {
				u.insert(u.begin(), 0);
				c.insert(c.begin(), -p);
				sync();
			}

			return *this;
		}

#ifdef _DEBUG
		static int test()
		{
			{
				value<U, C> i;
				assert(0 == i.size());
			}
			{
				value<U, C> i;
				assert(0 == i.size());
				value<U, C> i2{i};
				assert(i == i2);
				i = i2;
				assert(!(i != i2));

				i.push_back(1, 1);
				assert(1 == i.size());
				i.push_back(2, 2);
				assert(2 == i.size());
				U u[] = {1, 2};
				C c[] = {1, 2};
				value<U, C> i3(2, u, c);
				assert(i == i3);
			}

			return 0;
		}
#endif // _DEBUG
	};

}
