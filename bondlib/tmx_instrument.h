// tmx_instrument.h - times and cash flows
#pragma once
#include <algorithm>
#include <vector>
#include "ensure.h"

namespace tmx {

	// NVI base class for instruments
	template<class U = double, class C = double>
	struct instrument {
		virtual ~instrument()
		{ }

		constexpr bool operator==(const instrument& i) const
		{
			return size() == i.size()
				&& std::equal(time(), time() + size(), i.time())
				&& std::equal(cash(), cash() + size(), i.cash());
		}
		constexpr bool ok() const
		{
			return size() > 0
				&& time()[0] >= 0
				&& std::is_sorted(time(), time() + size(), std::less{});
		}

		size_t size() const
		{
			return _size();
		}
		const U* time() const
		{
			return _time();
		}
		const C* cash() const
		{
			return _cash();
		}
	private:
		virtual size_t _size() const = 0;
		virtual const U* _time() const = 0;
		virtual const C* _cash() const = 0;
	};

	// non-owning instrument view
	template<class U = double, class C = double>
	class instrument_view : public instrument<U, C> {
		size_t m;
		const U* u;
		const C* c;
	public:
		using instrument<U,C>::ok;

		instrument_view()
		{ }
		instrument_view(size_t m, const U* u, const C* c)
			: m{ m }, u{ u }, c{ c }
		{ 
			ensure(ok());
		}
		~instrument_view()
		{ }

		size_t _size() const override
		{
			return m;
		}
		const U* _time() const override
		{
			return u;
		}
		const C* _cash() const override
		{
			return c;
		}
	};

	// instrument value type
	template<class U = double, class C = double>
	class instrument_value : public instrument<U, C> {
		std::vector<U> u;
		std::vector<C> c;
	public:
		using instrument<U,C>::ok;

		instrument_value()
		{ }
		instrument_value(size_t m, const U* u, const C* c)
			: u(u, u + m), c(c, c + m)
		{
			ensure(ok());
		}
		instrument_value(const instrument_value& v) = default;
		instrument_value& operator=(const instrument_value& v) = default;
		~instrument_value()
		{ }

		// add cash flow keeping times sorted
		instrument_value& push_back(U _u, C _c)
		{
			auto iu = std::lower_bound(u.begin(), u.end(), _u);
			auto ic = std::distance(u.begin(), iu);
			if (*iu == _u) {
				// add cash flow to existing time
				c[ic] += _c;
			}
			else {
				u.insert(iu, _u);
				c.insert(c.begin() + ic, _c);
			}

			ensure(ok());

			return *this;
		}

		size_t _size() const override
		{
			return u.size();
		}
		const U* _time() const override
		{
			return u.data();
		}
		const C* _cash() const override
		{
			return c.data();
		}
#ifdef _DEBUG
		static int test()
		{
			{
				constexpr instrument_value<U, C> i;
				static_assert(0 == i.size());
				constexpr instrument_value<U, C> i2{i};
				static_assert(i == i2);
				i = i2;
				static_assert(!(i != i2));

				i.push_back(2, 2);
				static_assert(1 == i.size());
				i.push_back(1, 1);
				static_assert(2 == i.size());
				U u[] = {1, 2};
				C c[] = {1, 2};
				static_assert(i == instrument_value<U, C>(2, u, c));
			}

			return 0;
		}
#endif // _DEBUG
	};

}
