// tmx_instrument_view.h - non-owning view of instrument
#pragma once
#include <algorithm>
#include <vector>
#include "tmx_instrument.h"

namespace tmx {

	// non-owning instrument view
	template<class U = double, class C = double>
	class instrument_view : public instrument::base<U, C> {
	protected:
		view<U> u;
		view<C> c;
	public:
		instrument_view()
			: u{}, c{}
		{ }
		instrument_view(size_t m, U* u, C* c)
			: u(m, u), c(m, c)
		{ }
		instrument_view(view<U> u, view<C> c)
			: u{ u }, c{ c }
		{
			ensure(u.size() == c.size());
		}
		instrument_view(const instrument_view& v) = default;
		instrument_view& operator=(const instrument_view& v) = default;
		virtual ~instrument_view()
		{ }

		const view<U> _time() const override
		{
			return u;
		}
		const view<C> _cash() const override
		{
			return c;
		}

#ifdef _DEBUG
		static int test()
		{
			{
				instrument_view<U, C> i;
				assert(0 == i.size());
				instrument_view<U, C> i2{i};
				assert(i == i2);
				i = i2;
				assert(!(i != i2));

				U u[] = {1, 2};
				C c[] = {1, 2};
				instrument_view<U, C> i3(2, u, c);
				assert(i3.size() == 2);
				assert(i3.time() == view<U>(2, u));
				assert(i3.cash() == view<C>(2, c));
			}

			return 0;
		}
#endif // _DEBUG
	};

}
