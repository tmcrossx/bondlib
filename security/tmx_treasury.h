// tmx_treasury.h - Treasury instruments
#pragma once
#include "ensure.h"
#include "security/tmx_bond.h"

namespace tmx::security::treasury {

	// T-bill price from coupon and term.
	template<class F>
	constexpr F bill_price(unsigned weeks, F f)
	{
		return 1 - 7 * weeks * f / 360;
	}

	// T-bill price is face.
	template<class C = double, class F = double>
	struct bill : public security::bond<C, F> {
		bill(date::ymd dated, unsigned weeks, C coupon, F face = 100)
			: security::bond<C, F>{
				dated,
				std::chrono::sys_days(dated) + std::chrono::days(7*weeks),
				C(0),
				date::frequency::none,
				date::day_count_actual360,
				date::business_day::roll::following,
				date::holiday::calendar::FED,
				face / bill_price(weeks, coupon)
		}
		{
			ENSURE(weeks <= 52 || !"Treasury bill must have 52 weeks or less to maturity.");
		}
	};
	// Treasury note
	template<class C = double, class F = double>
	struct note : public security::bond<C, F> {
		note(date::ymd dated, date::ymd maturity, C coupon, F face = 100)
			: security::bond<C, F>{
				dated,
				maturity,
				coupon,
				date::frequency::semiannually,
				date::day_count_isdaactualactual,
				date::business_day::roll::following,
				date::holiday::calendar::FED,
				face
		}
		{
			auto mat = (maturity.year() - dated.year()).count();

			ENSURE(mat == 2 || mat == 3 || mat == 5 || mat == 7 || mat == 10
				|| !"Treasury note must have 2, 3, 5, 7, or 10 years to maturity.");
		}
	};
	// Treasury bond.
	template<class C = double, class F = double>
	struct bond : public security::bond<C,F> {
		bond(date::ymd dated, date::ymd maturity, C coupon, F face = 100)
			: security::bond<C, F>{ 
				dated, 
				maturity, 
				coupon, 
				date::frequency::semiannually, 
				date::day_count_isdaactualactual, 
				date::business_day::roll::following, 
				date::holiday::calendar::FED, 
				face 
			}
		{
			auto mat = (maturity.year() - dated.year()).count();

			ENSURE(mat > 10 || !"Treasury bond must have at least 10 years to maturity.");
		}
	};

} // namespace tmx::instrument::treasury