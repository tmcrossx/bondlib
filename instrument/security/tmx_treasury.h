// tmx_treasury.h - Treasury instruments
#pragma once
#include "instrument/security/tmx_bond.h"

namespace tmx::instrument::bond {

	// class bill
	// class note
	template<class C = double, class F = double>
	struct treasury : public tmx::instrument::bond::basic<C,F> {
		treasury(date::ymd dated, date::ymd maturity, C coupon, F face = 100)
			: tmx::instrument::bond::basic<C, F>{ 
				dated, 
				maturity, 
				coupon, 
				date::frequency::semiannually, 
				date::day_count_isdaactualactual, 
				date::business_day::roll::following, 
				date::holiday::calendar::FED, 
				face 
			}
		{ }
	};

} // namespace tmx::instrument::treasury