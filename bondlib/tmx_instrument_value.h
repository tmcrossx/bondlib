// tmx_instrument_value.h - Instrument value type
#pragma once
#include <algorithm>
#include <vector>
#include "ensure.h"
#include "tmx_instrument_view.h"

namespace tmx::instrument {

	// instrument value type
	template<class U = double, class C = double>
	class value : public instrument::view<U, C> {
		std::vector<U> u;
		std::vector<C> c;
		void update()
		{
			view<U, C>::u = view<U>(u.begin(), u.end());
			view<U, C>::c = view<C>(c.begin(), c.end());
		}
	public:
		value()
			: u{}, c{}
		{ }
		value(size_t m, const U* u, const C* c)
			: u(u, u + m), c(c, c + m)
		{
			update();
		}
		value(const value& v)
			: u(v.u), c(v.c)
		{
			update();
		}
		value& operator=(const value& v)
		{
			if (this != &v) {
				u = v.u;
				c = v.c;
				update();
			}

			return *this;
		}
		// TODO: test these
		value(value&& v) = default;
		value& operator=(value&& v) = default;
		~value()
		{ }

		// add cash flow keeping times sorted
		value& push_back(U _u, C _c)
		{
			if (u.size() > 0 and u.back() == _u) {
				c.back() += _c;
			}
			else {
				ensure(u.size() == 0 or u.back() < _u);

				u.push_back(_u);
				c.push_back(_c);
				update();
			}

			return *this;
		}
		// Make price zero
		value& price(C p)
		{
			u.insert(u.begin(), 0);
			c.insert(c.begin(), -p);
			update();

			return *this;
		}

#ifdef _DEBUG
		static int test()
		{
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
