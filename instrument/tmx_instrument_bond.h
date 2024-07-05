// tmx_bond.h - Bonds
#pragma once
#include "date/tmx_date_business_day.h"
#include "date/tmx_date_day_count.h"
#include "date/tmx_date_holiday_calendar.h"
#include "date/tmx_date_periodic.h"
#include "tmx_instrument.h"

namespace tmx::bond {

	// Basic bond indicative data.
	template<class C = double, class F = double>
	struct basic
	{
		date::ymd dated; // when interest starts accruing
		date::ymd maturity; // when last coupon and principal is repaid
		C coupon; // not in percent, e.g., 0.05 instead of 5%
		date::frequency frequency = date::frequency::semiannually;
		date::day_count_t day_count = date::day_count_isma30360;
		date::business_day::roll roll = date::business_day::roll::none;
		date::holiday::calendar::calendar_t cal = date::holiday::calendar::none;
		F face = 100;
	};

	// Return instrument cash flows for basic bond from present value date.
	template<class C>
	inline auto instrument(const basic<C>& bond, const date::ymd& pvdate)
	{
		using namespace fms::iterable;

		// If pvdate is before dated use dated in first payment date.
		const auto d0 = std::max(bond.dated, pvdate);
		// payment dates
		const auto pd = date::periodic(bond.frequency, d0, bond.maturity);
		// adjust payment dates with roll convention and holiday calendar
		const auto apd = apply(date::adjust(bond.roll, bond.cal), pd);
		// convert dates to time in years from pvdate
		const auto u = apply([pvdate](const date::ymd& d) {
			return tmx::date::diffyears(d, pvdate); }, apd);

		// day count fractions
		const auto dcf = delta(concatenate(single(d0), apd), bond.day_count);
		// cash flows
		const auto c = constant(bond.face * bond.coupon) * dcf;

		// face value at maturity
		const auto u_ = concatenate(u, single(bond.maturity - pvdate));
		const auto c_ = concatenate(c, single(bond.face));

		return instrument::iterable(u_, c_);
	}

#ifdef _DEBUG

	inline int basic_test()
	{
		using namespace std::literals::chrono_literals;
		using namespace std::chrono;
		using namespace tmx::date;
		using namespace fms::iterable;

		auto d = 2023y / 1 / 1;
		bond::basic<> bond{ d, d + years(10), 0.05 };
		{
			bond::basic b(d, d + months(6), 0.05);
			auto i = instrument(b, d);
			cash_flow<> uc;
			uc = *i;
			++i;
			uc = *i;
			++i;
			assert(!i);
		}
		{
			auto i = instrument(bond, d);
			assert(21 == size(i));
			auto c0 = *i;
			assert(c0.c == 2.5);
			//assert(c0.u == d + date::to_period(bond.frequency) - d);

			i = drop(i, 20);
			auto cn = *i;
			assert(cn.u == (bond.maturity - d));
			assert(cn.c == 100);
		}
		{
			auto pvdate = d + months(1);
			auto i = instrument(bond, pvdate);
			assert(21 == size(i));
			auto c0 = *i;
			assert(c0.c < 2.5);
			//assert(c0.u == bond.dated + date::to_period(bond.frequency) - pvdate);

			++i;
			auto c1 = *i;
			assert(c1.c == 2.5);
			i = drop(i, 19);
			auto cn = *i;
			assert(cn.u == (bond.maturity - pvdate));
			assert(cn.c == 100);
		}
		{
			auto pvdate = d - months(1);
			auto i = instrument(bond, pvdate);
			assert(21 == size(i));
			auto c0 = *i;
			assert(c0.c == 2.5); // accrue from dated date
			//assert(c0.u == bond.dated + date::to_period(bond.frequency) - pvdate);

			++i;
			auto c1 = *i;
			assert(c1.c == 2.5);
			i = drop(i, 19);
			auto cn = *i;
			assert(cn.u == (bond.maturity - pvdate));
			assert(cn.c == 100);
		}

		return 0;
	}
#endif // _DEBUG
} // namespace tmx::bond
