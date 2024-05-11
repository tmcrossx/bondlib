// tmx_instrument.h - times and cash flows
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include "tmx_math_limits.h"
#include "fms_iterable.h"
#include "tmx_cash_flow.h"

namespace tmx::instrument {

	// Fixed income instrument from time and cash flow iterables.
	template<fms::iterable::input U, fms::iterable::input C>
	class iterable : public fms::iterable::interface<cash_flow<typename U::value_type, typename C::value_type>>
	{
		U u;
		C c;
	public:
		//???
		using value_type = cash_flow<typename U::value_type, typename C::value_type>;

		iterable(const U& u, const C& c)
			: u(u), c(c)
		{ }

		bool operator==(const iterable&) const = default;

		U time() const
		{
			return u;
		}
		C cash() const
		{
			return c;
		}

		bool op_bool() const override
		{
			return u && c;
		}
		value_type op_star() const override
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
#ifdef _DEBUG
	inline int iterable_test()
	{
		{
			auto u = std::vector({ 1,2,3 });
			auto c = std::vector({ 2,3,4 });
			auto i = iterable(fms::iterable::container(u), fms::iterable::container(c));
			assert(i);
			assert(*i == cash_flow(1, 2));
			++i;
			assert(i);
			assert(*i == cash_flow(2, 3));
			++i;
			assert(i);
			assert(*i == cash_flow(3, 4));
			++i;
			assert(!i);
		}
		{
			auto u = std::vector({ 1,2,3 });
			auto c = std::vector({ 2,3,4 });
			auto ii = iterable(fms::iterable::container(u), fms::iterable::container(c));
			auto i = iterable(ii.time(), ii.cash());
			assert(i);
			assert(*i == cash_flow(1, 2));
			++i;
			assert(i);
			assert(*i == cash_flow(2, 3));
			++i;
			assert(i);
			assert(*i == cash_flow(3, 4));
			++i;
			assert(!i);
		}

		return 0;
	}
#endif // _DEBUG

	// A zero coupon bond has a single cash flow.
	// Equivalent to once(cash_flow(u,c)).
	template<class U = double, class C = double>
	inline auto zero_coupon_bond(const U& u, const C& c)
	{
		return iterable(fms::iterable::once(u), fms::iterable::once(c));
	}
#ifdef _DEBUG
	inline int zero_coupon_bond_test()
	{
		{
			auto z = zero_coupon_bond(2024y / 1 / 1, 2);
			assert(z);
			assert(*z == cash_flow(2024y / 1 / 1, 2));
			++z;
			assert(!z);
		}

		return 0;
	}
#endif // _DEBUG

	// TODO: forward_rate_agreement
}
