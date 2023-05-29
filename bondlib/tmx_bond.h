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

	struct basic {
		date::ymd dated; // calculation start date
		unsigned int maturity; // years
		double coupon;
		std::chrono::months frequency;
		double (*day_count_fraction)(const date::ymd&, const date::ymd&);
	private:
		mutable std::vector<double> u, c; // times and cash flows
	public:
		basic(date::ymd dated, unsigned int maturity, double coupon,
			std::chrono::months frequency = frequency::semiannually,
			double (*day_count_fraction)(const date::ymd&, const date::ymd&) = date::dcf_30_360)
		: dated{ dated }, maturity{ maturity }, coupon{ coupon },
			frequency{ frequency }, day_count_fraction{ day_count_fraction }
		{
			auto mat = dated + std::chrono::years(maturity);
			auto d0 = dated;
			auto d1 = d0 + frequency;
			do {
				u.push_back(date::dcf_years(dated, d1));
				c.push_back(coupon * day_count_fraction(d0, d1));
				d0 = d1;
				d1 = d0 + frequency;
			} while (d1 <= mat);

			c.back() += 1; // notional
		}
		basic(const basic&) = default;
		basic& operator=(const basic&) = default;
		virtual ~basic()
		{ }

		constexpr double accrued(date::ymd valuation) const
		{
			auto d = dated;

			while (d <= valuation) {
				d += frequency;
			}
			
			return coupon * day_count_fraction(valuation, d);
		}

		// price given yield
		double price(double y) const
		{
			double p = 0;

			auto mat = dated + std::chrono::years(maturity);
			date::ymd d0 = dated;
			double D = 1;
			for (auto d1 = d0 + frequency; d1 <= mat; d0 = d1, d1 = d0 + frequency) {
				D /= 1 + y * frequency.count() / 12;
				p += day_count_fraction(d0, d1) * D;
			}
			
			return coupon * p + D;
		}

		template<class D>
		double present_value(date::ymd valuation, D d) const
		{
			auto du = date::dcf_years(dated, valuation);
			tmx::translate _u(du, std::span(u));

			return value::present(_u, std::span(c).last(_u.size()), d);
		}

		template<class D>
		double duration(date::ymd valuation, D d) const
		{
			auto du = date::dcf_years(dated, valuation);
			tmx::translate _u(du, std::span(u));

			return value::duration(_u, std::span(c).last(_u.size()), d);
		}
		//!!! yield
	};

	// class callable : public basic { ... };

#ifdef _DEBUG

	inline int bond_basic_test()
	{
		{
			using std::chrono::year;
			using std::chrono::month;
			using std::chrono::day;

			date::ymd d(year(2023), month(1), day(1));
			basic bond(d, 10, 0.05, frequency::semiannually, date::dcf_30_360);
			ensure(0.5 == bond.day_count_fraction(bond.dated, bond.dated + bond.frequency));
			ensure(0.5 == bond.day_count_fraction(bond.dated - bond.frequency, bond.dated));
		}

		return 0;
	}

#endif // _DEBUG

}
