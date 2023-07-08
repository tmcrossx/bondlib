// tmx_bond.h - Bonds
#pragma once
#include <algorithm>
#include <vector>
#include "ensure.h"
#include "tmx_date.h"
#include "tmx_instrument_value.h"
#include "tmx_value.h"

namespace tmx::bond {

	template<class C = double>
	struct simple {
		std::chrono::years maturity;
		C coupon;
		unsigned short frequency;
		date::dcf_t& day_count;
	};

	// Return instrument cash flows for unit notional
	template<class U = double, class C = double>
	constexpr instrument_value<U, C> instrument(const simple<C>& bond, const date::ymd& dated)
	{
		instrument_value<U, C> i;

		auto mat = dated + bond.maturity;
		auto d0 = dated;
		auto d1 = d0 + std::chrono::months(12 / bond.frequency);
		while (d1 <= mat) {
			U u = date::dcf_years(dated, d1);
			C c = bond.coupon * bond.day_count(d0, d1);
			i.push_back(u, c);
			d0 = d1;
			d1 = d0 + std::chrono::months(12 / bond.frequency);
		}
		auto [_u, _c] = i.back();
		i.push_back(_u, 1);

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

			auto d = 2023y / 1 / 1;
			bond::simple<> bond{ std::chrono::years(10), 0.05, date::frequency::semiannually, date::dcf_30_360 };
			auto i = instrument(bond, d);
			ensure(20 == i.size());
			auto u = i.time();
			auto c = i.cash();
			ensure(u[0] != 0);
			ensure(c[0] == 0.05 / 2);
			ensure(std::fabs(-c[19] + c[0] + 1) < 1e-15);
			ensure(std::fabs(-u[19] + 10) < 1e-2);
		}

		return 0;
	}
#pragma warning(pop)
#endif // _DEBUG
}

// class callable : public basic { ... };


