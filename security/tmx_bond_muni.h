// tmx_bond_muni.h - Municipal bond securities
#pragma once
#include "ensure.h"
#include "security/tmx_bond.h"

namespace tmx::security {

	// Treasury bond.
	template<class C = double, class F = double>
	struct muni : public security::bond<C, F> {
		muni(date::ymd dated, date::ymd maturity, C coupon = 0.05, F face = 100)
			: security::bond<C, F>{ dated, maturity, coupon }
		{
			this->face = face;
		}
	};

} // namespace tmx::security 