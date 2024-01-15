// tmx_bond.h - Bonds
#pragma once
#include <algorithm>
#include <vector>
#include "ensure.h"
#include "tmx_date_day_count.h"
#include "tmx_instrument_value.h"
#include "tmx_value.h"

namespace tmx::bond {

	template<class T, class dT>
	class iota {
		T t;
		dT dt;
	public:
		iota(T t, dT dt)
			: t(t), dt(dt)
		{ }
		T operator*() const
		{
			return t;
		}
		iota& operator++()
		{
			t += dt;

			return *this;
		}
		iota& operator--()
		{
			t -= dt;

			return *this;
		}
	};

	// Basic bond indicative data.
	template<class C = double>
	struct basic {
		unsigned maturity;
		C coupon;
		date::frequency frequency = date::frequency::semiannually;
		date::day_count_t day_count = date::day_count_isma30360;
		C redemption = 1;
	};

	// Return instrument cash flows for unit notional with time based on issue date.
	template<class C = double>
	constexpr instrument::value<double, C> instrument(const basic<C>& bond, const date::ymd& dated, date::ymd issue = date::ymd{})
	{
		instrument::value<double, C> i;

		if (!issue.ok()) {
			issue = dated;
		}

		date::ymd maturity = dated + std::chrono::years(bond.maturity);
		std::chrono::months period = date::period(bond.frequency);
		auto us = iota(maturity, period);
		auto d1 = *us;
		auto d0 = *--us;
		// increment backward from maturity to dated
		while (d0 >= dated) {
			i.push_front(date::diffyears(d1, issue), bond.coupon * bond.day_count(d0, d1));
			d1 = d0;
			d0 = *--us;
		}
		i.push_back(i.back().first, bond.redemption);

		return i;
	}
	/*
	template<class C>
	constexpr auto accrued(const basic<C>& bond, const date::ymd& dated, const date::ymd& valuation)
	{
		auto d = dated;

		while (d + bond.frequency <= valuation) {
			d += bond.frequency;
		}

		return bond.coupon * bond.day_count_fraction(d, valuation);
	}
	//!!! add tests
	*/

#ifdef _DEBUG

	inline int basic_test()
	{
		{
			using namespace std::literals::chrono_literals;
			using namespace date;

			auto d = 2023y / 1 / 1;
			bond::basic<> bond{ 10, 0.05, frequency::semiannually, day_count_isma30360 };
			auto i = instrument(bond, d, d);
			ensure(20 == i.size());
			auto u = i.time();
			auto c = i.cash();
			ensure(u[0] != 0);
			ensure(c[0] == 0.05 / 2);
			ensure(std::fabs(-c[19] + c[0] + 1) < 1e-15);
			ensure(std::fabs(-u[19] + 10) <= 1/date::days_per_year);

			bond::basic<> bond2{ 10, 0.05 };
			auto i2 = instrument(bond2, d);
			ensure(i.size() == i2.size());
			auto u2 = i2.time();
			auto c2 = i2.cash();
			for (size_t n = 0; n < i.size(); ++n) {
				ensure(u[n] == u2[n]);
				ensure(c[n] == c2[n]);
			}
		}

		return 0;
	}

#endif // _DEBUG

}

// class callable : public basic { ... };

