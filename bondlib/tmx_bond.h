// tmx_bond.h - Bonds
#pragma once
#include <algorithm>
#include <vector>
#include "tmx_date.h"
#include "tmx_value.h"

namespace tmx::bond {

	namespace frequency {
		constexpr auto annually = std::chrono::months(12);
		constexpr auto semiannually = std::chrono::months(6);
		constexpr auto quarterly = std::chrono::months(3);
		constexpr auto monthly = std::chrono::months(1);
	}

	template<class C = double>
	struct simple {
		std::chrono::years maturity;
		C coupon;
		std::chrono::months frequency;
		C (*day_count_fraction)(const date::ymd&, const date::ymd&);
	};

	// Return pair of time, cash vectors
	template<class U = double, class C = double>
	constexpr std::pair<std::vector<U>, std::vector<C>> cashflows(const simple<C>& bond, const date::ymd& dated)
	{
		std::vector<U> u;
		std::vector<C> c;

		date::ymd d0 = dated;
		date::ymd d1 = d0 + bond.frequency;
		while (d1 <= dated + bond.maturity) {
			u.push_back(date::dcf_years(dated, d1));
			c.push_back(bond.coupon * bond.day_count_fraction(d0, d1));
			d0 = d1;
			d1 = d0 + bond.frequency;
		}
		c.back() += 1; // principal

		return { u, c };
	}

	template<class C>
	constexpr auto accrued(const simple<C>& bond, const date::ymd& dated, const date::ymd& valuation)
	{
		auto d = dated;

		while (d + bond.frequency <= valuation) {
			d += bond.frequency;
		}

		return bond.coupon * bond.day_count_fraction(d, valuation);
	}

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

	inline int bond_basic_test()
	{
		{
			using std::chrono::year;
			using std::chrono::month;
			using std::chrono::day;

			date::ymd d(year(2023), month(1), day(1));
			bond::simple bond(std::chrono::years(10), 0.05, bond::frequency::semiannually, date::dcf_30_360);
			const auto& [u, c] = cashflows(bond, d);
			assert(20 == u.size());
			assert(c[0] == bond.coupon * bond.day_count_fraction(d, d + bond.frequency));
			assert(c[19] == c[0] + 1);
			assert(u[19] == date::dcf_years(d, d + bond.maturity));
		}

		return 0;
	}

#endif // _DEBUG
}

// class callable : public basic { ... };


