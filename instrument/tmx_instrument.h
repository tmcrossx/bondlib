// tmx_instrument.h - times and cash flows
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <initializer_list>
#include "fms_iterable/fms_iterable.h"
#include "tmx_cash_flow.h"

namespace tmx::instrument {

	template<class IU, class IC,
		class U = typename IU::value_type, class C = typename IC::value_type>
	class iterable {
		IU u;
		IC c;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = cash_flow<U, C>;
		using reference = value_type&;
		using difference_type = std::ptrdiff_t;

		constexpr iterable() = default;
		constexpr iterable(IU u, IC c) // TODO: default
			: u(u), c(c)
		{ }
		constexpr iterable(const iterable& i) // TODO: default
			: u(i.u), c(i.c)
		{ }
		constexpr iterable& operator=(const iterable& i ) // TODO: default
		{
			if (this != &i) {
				u = i.u;
				c = i.c;
			}

			return *this;
		};
		constexpr iterable(iterable&&) = default;
		constexpr iterable& operator=(iterable&&) = default;
		constexpr ~iterable() = default;

		constexpr bool operator==(const iterable& i) const = default;

		iterable begin() const
		{
			return *this;
		}
		///*
		iterable end() const
		{
			return iterable(u.end(), c.end());
		}
		//*/
		constexpr explicit operator bool() const
		{
			return u && c;
		}
		constexpr value_type operator*() const
		{
			return cash_flow(*u, *c);
		}
		constexpr iterable& operator++()
		{
			++u;
			++c;

			return *this;
		}
		constexpr iterable operator++(int)
		{
			auto i = *this;

			++this;

			return i;
		}

		IU time() const
		{
			return u;
		}
		IC cash() const
		{
			return c;
		}
	};

	// Single cash flow c at u.
	template<class U = double, class C = double>
	constexpr auto zero_coupon_bond(U u, C c)
	{
		return iterable(fms::iterable::single(u), fms::iterable::single(c));
	}

} // namespace tmx::instrument	
