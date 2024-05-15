// tmx_instrument.h - times and cash flows
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include "fms_iterable/fms_iterable.h"
#include "tmx_cash_flow.h"

namespace tmx::instrument {

	template<class U = double, class C = double>
	using interface = fms::iterable::interface<cash_flow<U, C>>;

	// Fixed income instrument from time and cash flow iterables.
	template<fms::iterable::input IU, fms::iterable::input IC,
		class U = typename IU::value_type, class C = typename C::value_type>
 	class iterable : public interface<U, C>
	{
		IU u;
		IC c;
	public:
		//???
		using value_type = cash_flow<U, C>;

		iterable(const IU& u, const IC& c)
			: u(u), c(c)
		{ }

		bool operator==(const iterable&) const = default;

		IU time() const
		{
			return u;
		}
		IC cash() const
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
		return iterable(cash_flow<U,C>(u, c));
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
