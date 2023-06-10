// tmx_bond.h - Bonds
#pragma once
#include <algorithm>
#include <vector>
#include "ensure.h"
#include "tmx_date.h"
#include "tmx_instrument.h"
#include "tmx_value.h"

namespace tmx::bond {

	template<class C = double>
	struct simple {
		std::chrono::years maturity;
		C coupon;
		std::chrono::months frequency;
		date::dcf_t& day_count_fraction;
	};

	// Return pair of time, cash vectors
	template<class U = double, class C = double>
	constexpr instrument_vector<U,C> instrument(const simple<C>& bond, const std::chrono::sys_days& dated)
	{
		instrument_vector<U,C> i;
		
		auto d0 = dated;
		auto d1 = date::as_days(d0 + bond.frequency);
		while (d1 <= dated + bond.maturity) {
			U u = date::sub_years(d1, dated);
			C c = bond.coupon * bond.day_count_fraction(d0, d1);
			if (d1 == dated + bond.maturity) {
				c += 1;
			}
			i.push_back(u, c);
			d0 = d1;
			d1 = date::as_days(d0 + bond.frequency);
		}
		
		return i;
	}
	/*
	template<class C>
	constexpr auto accrued(const simple<C>& bond, const date::ymd& dated, const date::ymd& valuation)
	{
		auto d = dated;

		while (d + bond.frequency <= valuation) {
			d += bond.frequency;
		}

		return bond.coupon * bond.day_count_fraction(d, valuation);
	}
	//!!! add tests
	*/
	/*
	// price given yield
	template<class Y>
	constexpr auto price(const simple<Y>& bond, const date::ymd& dated, Y y)
	{
		double p = 0;

		auto mat = dated + bond.maturity;
		date::ymd d0 = dated;
		date::ymd d1 = d0 + bond.frequency;
		Y D = 1;
		while (d1 <= mat) {
			D /= 1 + y * bond.frequency.count() / 12;
			p += bond.day_count_fraction(d0, d1) * D;
			d0 = d1;
			d1 = d0 + bond.frequency;
		}

		return bond.coupon * p + D;
	}
	*/

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 4455)
	inline int bond_basic_test()
	{
		{
			using std::chrono::sys_days;
			using std::chrono::year;
			using std::chrono::month;
			using std::chrono::day;
			using std::literals::chrono_literals::operator""y;

			auto d = sys_days{ 2023y / 1 / 1 };
			bond::simple<> bond{std::chrono::years(10), 0.05, date::frequency::semiannually, date::dcf_30_360};
			const auto i = instrument(bond, d);
			ensure(20 == i.size());
			const auto u = i.time();
			const auto c = i.cash();
			ensure(c[0] == bond.coupon * bond.day_count_fraction(d, date::as_days(d + bond.frequency)));
			ensure(c[19] == c[0] + 1);
			ensure(u[19] == date::dcf_years(d, date::as_days(d + bond.maturity)));
		}

		return 0;
	}
#pragma warning(pop)
#endif // _DEBUG
}

// class callable : public basic { ... };


