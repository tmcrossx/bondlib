// fms_date_periodic.h - Periodic dates with roll and holiday adjustments
#pragma once
#include "tmx_date_business_day.h"

namespace tmx::date {

	// Adjust a periodic date to a business day.
	class adjust {
		periodic p;
		business_day::roll convention;
		holiday::calendar::calendar_t cal;
	public:
		adjust(const periodic& p, business_day::roll convention, holiday::calendar::calendar_t cal = holiday::weekend)
			: p(p), convention(convention), cal(cal)
		{ }

		explicit operator bool() const
		{
			return true;
		}
		ymd operator*() const
		{
			return business_day::adjust(*p, convention, cal);
		}
		adjust& operator++()
		{
			++p;

			return *this;
		}
	};

} // namespace tmx::date

