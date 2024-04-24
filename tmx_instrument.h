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
		requires std::totally_ordered<U> && std::equality_comparable<C>
	struct cash_flow {
		using time_type = U;
		using cash_type = C;

		U u; // time
		C c; // amount

		constexpr cash_flow(U u = math::infinity<U>, C c = 0)
			: u(u), c(c)
		{ }

		template<class U_, class C_> 
		constexpr bool operator==(const cash_flow<U_,C_>& cf) const
		{
			return u == cf.u && c == cf.c;
		}
		// ordered by time
		template<class U_, class C_>
		constexpr bool operator<(const cash_flow<U_,C_>& cf) const
		{
			return u < cf.u;
		}
	};
#ifdef _DEBUG
	static_assert(cash_flow(1, 2) == cash_flow(1, 2));
	static_assert(cash_flow(1, 2) == cash_flow(1., 2.));
	static_assert(cash_flow(1., 2) == cash_flow(1, 2.));
	static_assert(!(cash_flow(1, 2) < cash_flow(1, 2)));
	static_assert(!(cash_flow(1., 2) < cash_flow(1, 2.)));
#endif // _DEBUG

	// A fixed income instrument is a sequence of cash flows.
	template<class U = double, class C = double>
	using interface = fms::iterable::interface<cash_flow<U, C>>;

	template<class U = double, class C = double>
	concept input = fms::iterable::input<interface<U, C>>;


	// A zero coupon bond has a single cash flow.
	// Equivalent to singleton(cash_flow(u,c)).
	template<class U = double, class C = double>
	class zero_coupon_bond : public fms::iterable::interface<cash_flow<U, C>> {
		cash_flow<U, C> uc;
	public:
		constexpr zero_coupon_bond(const U& u = math::infinity<U>, const C& c = 0)
			: uc(u,c)
		{ }

		zero_coupon_bond* clone() const override
		{
			return new zero_coupon_bond(*this);
		}
		void destroy() override
		{
			delete this;
		}

		constexpr bool op_bool() const override
		{
			return uc.u != math::infinity<U>;
		}
		constexpr cash_flow<U, C> op_star() const override
		{
			return uc;
		}
		constexpr zero_coupon_bond& op_incr() override
		{
			uc = cash_flow<U, C>{};

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				zero_coupon_bond z(1, 2);
				assert(z);
				assert(*z == cash_flow(1, 2));
				++z;
				assert(!z);
			}

			return 0;
		}
#endif // _DEBUG
	};

}
