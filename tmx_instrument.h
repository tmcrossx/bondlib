// tmx_instrument.h - times and cash flows
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <initializer_list>
#include "fms_iterable/fms_iterable.h"
#include "tmx_cash_flow.h"

namespace tmx::instrument {

	//template<fms::iterable::input IU, fms::iterable::input IC>
	//concept input = fms::iterable::input<cash_flow<typename IU::value_type, typename IC::value_type>>;

	// Fixed income instrument from time and cash flow iterables.
	template<fms::iterable::input IU, fms::iterable::input IC>
	class iterable	{
		IU u;
		IC c;
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = cash_flow<typename IU::value_type, typename IC::value_type>;

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

		explicit operator bool() const
		{
			return u && c;
		}
		value_type operator*() const
		{
			return cash_flow(*u, *c);
		}
		iterable& operator++()
		{
			++u;
			++c;

			return *this;
		}
	};
	// E.g., make_iterable({1,2,3}, {.2,.3,.4})
	template<class U, class C>
	inline auto make_iterable(const std::initializer_list<U>& u, const std::initializer_list<C>& c)
	{
		return iterable(fms::iterable::list(u), fms::iterable::list(c));
	}
#ifdef _DEBUG
	inline int iterable_test()
	{
		{
			auto u = std::vector({ 1,2,3 });
			auto c = std::vector({ 2,3,4 });
			auto i = iterable(fms::iterable::make_interval(u), fms::iterable::make_interval(c));
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
			auto ii = iterable(fms::iterable::make_interval(u), fms::iterable::make_interval(c));
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
		{
			auto z = make_iterable({ 1 }, { 2 });
			assert(z);
			assert(*z == cash_flow(1, 2));
			++z;
			assert(!z);
		}

		return 0;
	}
#endif // _DEBUG

	// TODO: forward_rate_agreement
}
