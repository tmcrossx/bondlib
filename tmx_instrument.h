// tmx_instrument.h - times and cash flows
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <concepts>
#include <limits>
#include "tmx_math_limits.h"

namespace tmx::instrument {

	// Cash flow at time u and amount c.
	template<class U = double, class C = double>
		requires std::totally_ordered<U> && std::equality_comparable<C>
	struct cash_flow {
		U u; // time
		C c; // amount

		constexpr cash_flow(U u = math::infinity<U>, C c = 0)
			: u(u), c(c)
		{ }
		constexpr cash_flow(const cash_flow&) = default;
		constexpr cash_flow& operator=(const cash_flow&) = default;
		constexpr ~cash_flow() = default;

		template<class U_, class C_> 
		constexpr bool operator==(const cash_flow<U_,C_>& cf) const
		{
			return u == cf.u && c == cf.c;
		}
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

	// Stream of cash flows.
	template<class U = double, class C = double>
	struct base {
		virtual ~base() { }

		// Detect end of stream.
		explicit operator bool() const
		{
			return op_bool();
		}
		// Current cash flow.
		cash_flow<U, C> operator*() const
		{
			return op_star();
		}
		// Advance to next cash flow.
		base& operator++()
		{
			return op_incr();
		}

	private:
		virtual bool op_bool() const = 0;
		virtual cash_flow<U, C> op_star() const = 0;
		virtual base& op_incr() = 0;
	};

	// A zero coupon bond has a single cash flow.
	template<class U = double, class C = double>
	class zero_coupon_bond : public base<U, C> {
		cash_flow<U, C> uc;
	public:
		constexpr zero_coupon_bond(const U& u = math::infinity<U>, const C& c = 0)
			: uc(u,c)
		{ }
		constexpr zero_coupon_bond(const cash_flow<U,C>& uc)
			: uc(uc)
		{ }
		constexpr zero_coupon_bond(const zero_coupon_bond&) = default;
		constexpr zero_coupon_bond& operator=(const zero_coupon_bond&) = default;
		constexpr ~zero_coupon_bond() override { }

		template<class U_, class C_> 
		constexpr bool operator==(const zero_coupon_bond<U_,C_>& z) const
		{
			return uc == z.uc;
		}
		template<class U_, class C_>
		constexpr bool operator<(const zero_coupon_bond<U_,C_>& z) const
		{
			return uc < z.uc;
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
				assert(z == z);
				assert(!(z < z));
				assert(z);
				assert(*z == cash_flow(1, 2));
				++z;
				assert(!z);
			}

			return 0;
		}
#endif // _DEBUG
	};

	// Combine two streams of cash flows in time order.
	template<class U0 = double, class C0 = double, class U1 = double, class C1 = double,
		class U = std::common_type_t<U0, U1>, class C = std::common_type_t<C0, C1>>
	struct merge : public base<U, C> {
		base<U0, C0>& u0;
		base<U0, C1>& u1;

		constexpr merge(base<U0,C0>& u0, base<U1,C1>& u1)
			: u0(u0), u1(u1)
		{ }

		constexpr bool op_bool() const override
		{
			return u0 || u1;
		}
		constexpr cash_flow<U,C> op_star() const override
		{
			return *u0 < *u1 ? *u0 : *u1;
		}
		constexpr merge& op_incr() override
		{
			if (*u0 < *u1) {
				++u0;
			}
			else {
				++u1;
			}

			return *this;
		}
#ifdef _DEBUG
		static int test()
		{
			{
				zero_coupon_bond z0(1., 2.), z1(3., 4.);
				merge i(z0, z1);
				assert(i);
				assert(*i == cash_flow(1., 2.));
				++i;
				assert(i);
				assert(*i == cash_flow(3., 4.));
				++i;
				assert(!i);
			}
			{
				zero_coupon_bond z0(1., 2.), z1(3., 4.);
				merge i(z1, z0);
				assert(i);
				assert(*i == cash_flow(1., 2.));
				++i;
				assert(i);
				assert(*i == cash_flow(3., 4.));
				++i;
				assert(!i);
			}

			return 0;
		}
#endif // _DEBUG
	};

}
/*
template<class U, class C>
constexpr auto operator,(tmx::instrument::base<U, C>& u...)
{
	return tmx::instrument::merge(u0, u1);
}
*/
