// tmx_bond.h - Bonds
#pragma once
#include "tmx_date_day_count.h"
#include "tmx_instrument.h"

namespace tmx::bond {

	// Basic bond indicative data.
	template<class C = double>
	struct basic 
	{
		date::ymd         dated; // when interest starts accruing
		date::ymd         maturity;
		C                 coupon; // not in percent
		date::frequency   frequency = date::frequency::semiannually;
		date::day_count_t day_count = date::day_count_isma30360;
		C                 face = 100;
	};

	// Return instrument cash flows for basic bond from present value date.
	template<class C>
	inline auto instrument(const basic<C>& bond, const date::ymd& pvdate)
	{
		using namespace fms::iterable;

		// pvdate might be before dated
		const auto d0 = std::max(bond.dated, pvdate);
		// first cash flow date after d0
		const auto [fpd, _] = date::first_payment_date(bond.frequency, d0, bond.maturity);
		// apply(adjust, apply(holiday, d))
		// payment dates
		const auto pd = date::periodic(bond.frequency, fpd, bond.maturity);
		// convert dates to time in years from pvdate (cache is gcc workaround)
		const auto u = cache(apply([pvdate](const date::ymd& d) { return d - pvdate; }, pd));

		// day count fractions
		const auto dt = nabla(concatenate(once(pvdate), pd), bond.day_count);
		// cash flows
		const auto c = constant(bond.face * bond.coupon) * dt;

		// face value at maturity
		const auto f = instrument::zero_coupon_bond(bond.maturity - pvdate, bond.face);

		return cache(merge(instrument::iterable(u, c), f));
	}
#ifdef _DEBUG

	inline int basic_test()
	{
		using namespace std::literals::chrono_literals;
		using namespace std::chrono;
		using namespace date;
		auto d = 2023y / 1 / 1;
		bond::basic<> bond{ d, d + years(10), 0.05 };

		{
			auto i = instrument(bond, d);
			//assert(20 == length(i));
			auto c0 = *i;
			assert(c0.c == 2.5);
			assert(c0.u == d + date::period(bond.frequency) - d);

			i = drop(i, 19);
			auto cn = *i;
			assert(cn.u == (bond.maturity - bond.dated));
			assert(cn.c == 102.5);
		}
		{
			auto pvdate = d + months(1);
			auto i = instrument(bond, pvdate);
			assert(20 == length(i));
			auto c0 = *i;
			assert(c0.c != 2.5);
			assert(c0.u == bond.dated + date::period(bond.frequency) - pvdate);

			++i;
			auto c1 = *i;
			assert(c1.c == 2.5);
			i = drop(i, 18);
			auto cn = *i;
			assert(cn.u == (bond.maturity - pvdate));
			assert(cn.c == 102.5);
		}

		return 0;
	}

#endif // _DEBUG

} // namespace tmx::bond

// class callable : public basic { ... };

