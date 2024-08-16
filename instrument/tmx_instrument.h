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


	template<class IU, class IC>
	class iterable {
		IU u;
		IC c;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = cash_flow<std::iter_value_t<IU>, std::iter_value_t<IC>>;
		using reference = value_type&;
		using pointer = value_type*;
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
			return cash_flow(*fms::iterable::last(u), *fms::iterable::last(c)); // TODO: fms::iterable::last(u)...
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
	constexpr auto zero_coupon_bond(U u, C c = (U)1)
	{
		using namespace fms::iterable;

		return iterable(single(u), single(c));
	}
#ifdef _DEBUG
	static_assert(*zero_coupon_bond(2.) == cash_flow(2., 1.));
	static_assert(*zero_coupon_bond(2., 3.) == cash_flow(2., 3.));
	static_assert(!++zero_coupon_bond(2.));
#endif // _DEBUG

	// Simple bond with coupon/freq payments at freq times per year and unit notional at maturity.
	template<class U = double, class C = double>
	constexpr auto simple(U maturity, C coupon, unsigned freq = 2)
	{
		using namespace fms::iterable;

		const auto u = sequence(1./freq, 1./freq);
		const auto c = constant(coupon / freq);
		ptrdiff_t n = static_cast<ptrdiff_t>(maturity * freq);

		return concatenate(take(iterable(u, c), n), zero_coupon_bond(maturity));
	}
#ifdef _DEBUG
	constexpr auto x0 = simple(1., .05);
	static_assert(equal(simple(1., .05), concatenate(
		zero_coupon_bond(0.5, 0.025),
		zero_coupon_bond(1., 0.025), 
		zero_coupon_bond(1., 1.))));
#endif // _DEBUG

} // namespace tmx::instrument	
