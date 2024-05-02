// tmx_instrument.h - times and cash flows
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <concepts>
#include "tmx_math_limits.h"
#include "fms_iterable.h"

namespace tmx::instrument {

	// Cash flow at time u of amount c.
	template<class U = double, class C = double>
		//requires std::totally_ordered<U> && std::equality_comparable<C>
	struct cash_flow {
		using time_type = U;
		using cash_type = C;

		U u; // time
		C c; // cash

		constexpr cash_flow() = default;
		constexpr cash_flow(U u, C c)
			: u(u), c(c)
		{ }

		cash_flow operator()(U u_, C c_) const
		{
			return cash_flow(u_, c_);
		}

		std::pair<U, C> pair() const
		{
			return { u, c };
		}

		constexpr bool operator==(const cash_flow& cf) const = default;

		// ordered by time
		constexpr bool operator<(const cash_flow& cf) const
		{
			return u < cf.u;
		}
	};
#ifdef _DEBUG
	static_assert(cash_flow(1, 2) == cash_flow(1, 2));
	static_assert(!(cash_flow(1, 2) < cash_flow(1, 2)));
	static_assert(!(cash_flow(1., 2.) < cash_flow(1., 2.)));
#endif // _DEBUG

	// A fixed income instrument is a sequence of cash flows.
	template<class U = double, class C = double>
	using interface = fms::iterable::interface<cash_flow<U, C>>;

	// Make instrument from time and cash flow iterables.
	template<fms::iterable::input U, fms::iterable::input C>
	class iterable : public interface<typename U::value_type, typename C::value_type>
	{
		U u;
		C c;
	public:
		iterable(const U& u, const C& c)
			: u(u), c(c)
		{ }

		bool op_bool() const override
		{
			return u && c;
		}
		cash_flow<typename U::value_type, typename C::value_type> op_star() const override
		{
			return cash_flow(*u, *c);
		}
		iterable& op_incr() override
		{
			++u;
			++c;

			return *this;
		}	

	};

	// A zero coupon bond has a single cash flow.
	// Equivalent to once(cash_flow(u,c)).
	template<class U = double, class C = double>
	inline auto zero_coupon_bond(const U& u, const C& c)
	{
		return fms::iterable::once(cash_flow(u,c));
	}
#ifdef _DEBUG
	inline int zero_coupon_bond_test()
	{
		{
			auto z = zero_coupon_bond(1, 2);
			assert(z);
			assert(*z == cash_flow(1, 2));
			++z;
			assert(!z);
		}

		return 0;
	}
#endif // _DEBUG

}

// Add cash flows having the same time.
template<class U, class C>
inline tmx::instrument::cash_flow<U, C> operator+(const tmx::instrument::cash_flow<U, C>& uc1, const tmx::instrument::cash_flow<U, C>& uc2)
{
	return uc1.u == uc2.u ? tmx::instrument::cash_flow(uc1.u, uc1.c + uc2.c) : tmx::instrument::cash_flow{};
}