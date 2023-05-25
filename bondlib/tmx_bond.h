// tmx_bond.h - Bonds
#pragma once
#include <algorithm>
#include <vector>
#include "tmx_date.h"

namespace tmx::bond {

	namespace frequency {
		constexpr auto annually = std::chrono::months(12);
		constexpr auto semiannually = std::chrono::months(6);
		constexpr auto quarterly = std::chrono::months(3);
		constexpr auto monthly = std::chrono::months(1);
	}

	struct basic {
		date::ymd dated; // calculation start date
		double maturity; // years
		double coupon;
		std::chrono::months frequency;
		double (*day_count_fraction)(const date::ymd&, const date::ymd&);
	};

#ifdef _DEBUG

	inline int test_bond_basic()
	{
		{
			using std::chrono::year;
			using std::chrono::month;
			using std::chrono::day;

			date::ymd d(year(2023), month(1), day(1));
			basic bond{d, 10, 0.05, frequency::semiannually, date::dcf_30_360};
			ensure(0.5 == bond.day_count_fraction(bond.dated, bond.dated + bond.frequency));
			ensure(0.5 == bond.day_count_fraction(bond.dated - bond.frequency, bond.dated));
		}

		return 0;
	}

#endif // _DEBUG

	template<class U = double, class C = double>
	inline std::pair<std::vector<U>, std::vector<C>> cashflows(const date::ymd& valuation, const basic& bond)
	{
		std::vector<U> u;
		std::vector<C> c;

		auto maturity = date::add_years(valuation, bond.maturity);
		auto d1 = maturity;
		auto d0 = d1 - bond.frequency;
		u.push_back(date::dcf_years(valuation, d1));
		// clean price ??? correct accrued interest
		c.push_back(bond.coupon * bond.day_count_fraction(std::max(d0, valuation), d1));

		// Work back from maturity and reverse.
		while (d0 > valuation) {
			d1 = d0;
			d0 = d1 - bond.frequency;
			u.push_back(date::dcf_years(valuation, d1));
			c.push_back(bond.coupon * bond.day_count_fraction(std::max(d0, valuation), d1));
		}

		std::reverse(u.begin(), u.end());
		std::reverse(c.begin(), c.end());

		return { u, c };
	}

}
