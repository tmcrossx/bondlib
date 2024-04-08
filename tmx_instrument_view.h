// tmx_instrument_view.h - non-owning view of instrument
#pragma once
#include <algorithm>
#include <vector>
#include "tmx_instrument.h"

namespace tmx::instrument {

	// non-owning instrument view
	template<class U = double, class C = double>
	class view : public tmx::instrument<U, C> {
	protected:
		std::span<U> u;
		std::span<C> c;
	public:
		view()
			: u{}, c{}
		{ }
		view(size_t m, U* u, C* c)
			: u(u, m), c(c, m)
		{ }
		view(std::span<U> u, std::span<C> c)
			: u{ u }, c{ c }
		{
			//ensure(u.size() == c.size());
		}
		view(const view& v) = default;
		view& operator=(const view& v) = default;
		virtual ~view()
		{ }

		explicit op_bool() const override
		{
			return u.size() > 0;
		}
		std::pair<U, C> op_star() const override
		{
			return std::make_pair(u[0], c[0]);
		}
		view& op_incr() override
		{
			u = u.subspan(1);
			c = c.subspan(1);

			return *this;
		}

#ifdef _DEBUG
		static int test()
		{
			{
				view<U, C> i;
				assert(0 == i.size());
				view<U, C> i2{i};
				assert(i == i2);
				i = i2;
				assert(!(i != i2));

				U u[] = {1, 2};
				C c[] = {1, 2};
				view<U, C> i3(2, u, c);
				assert(i3.size() == 2);
				assert(i3.time() == std::span<U>(u, 2));
				assert(i3.cash() == std::span<C>(c, 2));
			}

			return 0;
		}
#endif // _DEBUG
	};

}
