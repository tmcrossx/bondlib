// tmx_instrument.h - cash flows iterable
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <initializer_list>
#include "fms_iterable/fms_iterable.h"

namespace tmx::instrument {

	// Cash flow at time u of amount c.
	template<class U = double, class C = double>
	//requires std::totally_ordered<U> && std::equality_comparable<C>
	struct cash_flow {
		using time_type = U;
		using cash_type = C;

		U u; // time
		C c; // cash

		constexpr cash_flow(U u = U{}, C c = C{})
			: u(u), c(c)
		{ }

		constexpr auto operator<=>(const cash_flow& cf) const = default;
	};


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
		constexpr iterable(IU u, IC c)
			: u(std::move(u)), c(std::move(c))
		{ }
		constexpr iterable(const iterable& i) = default;
		constexpr iterable& operator=(const iterable& i) = default;
		constexpr iterable(iterable&&) = default;
		constexpr iterable& operator=(iterable&&) = default;
		constexpr ~iterable() = default;

		constexpr bool operator==(const iterable& i) const = default;

		IU time() const
		{
			return u;
		}
		IC cash() const
		{
			return c;
		}

		// Last cash flow at maturity/termination/expiration.
		value_type last() const
		{
			return cash_flow(*--u.end(), *--c.end()); // TODO: fms::iterable::last(u)...
		}

		iterable begin() const
		{
			return *this;
		}
		iterable end() const
			requires fms::iterable::has_end<IU> && fms::iterable::has_end<IC>
		{
			return iterable(u.end(), c.end());
		}
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
		constexpr iterable& operator--()
		{
			--u;
			--c;

			return *this;
		}
		constexpr iterable operator--(int)
		{
			auto i = *this;

			--this;

			return i;
		}
	};

	// Single cash flow c at u.
	template<class U = double, class C = double>
	constexpr auto zero_coupon_bond(U u, C c)
	{
		return iterable(fms::iterable::single(u), fms::iterable::single(c));
	}

} // namespace tmx::instrument	
