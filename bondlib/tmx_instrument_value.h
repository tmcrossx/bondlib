// tmx_instrument_value.h - Instrument value type
#pragma once
#include <algorithm>
#include <vector>
#include "ensure.h"
#include "tmx_instrument_view.h"

namespace tmx {

	// instrument value type
	template<class U = double, class C = double>
	class instrument_value : public instrument_view<U, C> {
		std::vector<U> u;
		std::vector<C> c;
		void update()
		{
			instrument_view<U, C>::u = view<U>(u.begin(), u.end());
			instrument_view<U, C>::c = view<C>(c.begin(), c.end());
		}
	public:
		instrument_value()
			: u{}, c{}
		{ }
		instrument_value(size_t m, const U* u, const C* c)
			: u(u, u + m), c(c, c + m)
		{
			update();
		}
		instrument_value(const instrument_value& v)
			: u(v.u), c(v.c)
		{
			update();
		}
		instrument_value& operator=(const instrument_value& v)
		{
			if (this != &v) {
				u = v.u;
				c = v.c;
				update();
			}

			return *this;
		}
		instrument_value(instrument_value&& v) = default;
		instrument_value& operator=(instrument_value&& v) = default;
		~instrument_value()
		{ }

		// add cash flow keeping times sorted
		instrument_value& push_back(U _u, C _c)
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
		instrument_value& price(C p)
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
				instrument_value<U, C> i;
				assert(0 == i.size());
				instrument_value<U, C> i2{i};
				assert(i == i2);
				i = i2;
				assert(!(i != i2));

				i.push_back(1, 1);
				assert(1 == i.size());
				i.push_back(2, 2);
				assert(2 == i.size());
				U u[] = {1, 2};
				C c[] = {1, 2};
				instrument_value<U, C> i3(2, u, c);
				assert(i == i3);
			}

			return 0;
		}
#endif // _DEBUG
	};

}
