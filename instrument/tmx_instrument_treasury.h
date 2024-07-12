// tmx_instrument_treasury.h - Treasury instruments
#pragma once
#include "tmx_instrument_bond.h"

namespace tmx::intrument::treasury {

	// class bill
	// class note
	template<class U = double, class F = double>
	struct bond : public tmx::instrument::bond<U, C> {
		bond(date::ymd dated, date::ymd maturity, C coupon, F face = 100)
			: tmx::instrument::bond<U, C>{ 
				dated, 
				maturity, 
				coupon, 
				date::frequency::semiannually, 
				date::isdaactualactual, 
				date::business_day::roll::following, 
				date::holiday::calendar::FED, 
				face 
			}
		{ }
	};

} // namespace tmx::instrument::treasury