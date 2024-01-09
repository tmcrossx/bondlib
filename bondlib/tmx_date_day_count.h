// tmx_date_day_count.h - Day count conventions for tmx dates
// https://github.com/bloomberg/bde/tree/main/groups/bbl/bbldc
#pragma once
#include "tmx_date.h"

namespace tmx::date {

	// https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicisma30360.cpp
	inline double day_count_isma30E360(const ymd& ymd1, const ymd& ymd2)
	{
		auto [y1, m1, d1] = from_ymd(ymd1);
		auto [y2, m2, d2] = from_ymd(ymd2);

		if (d1 == 31) {
			d1 = 30;
		}
		if (d2 == 31) {
			d2 = 30;
		}

		return (y2 - y1) * 360. + (m2 - m1) * 30. + (d2 - d1);
	}

	// https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicpsa30360eom.cpp
	inline double day_count_isma30360eom(const ymd& ymd1, const ymd& ymd2) 
	{
		auto [y1, m1, d1] = from_ymd(ymd1);
		auto [y2, m2, d2] = from_ymd(ymd2);

		if (m1 == 2 && (d1 == 29 || (d1 == 28 && ymd1.year().is_leap()))) {
			d1 = 30;
		}
		else if (d1 == 31) {
			d1 = 30;
		}
		if (d2 == 31 && d1 == 30) {
			d2 = 30;
		}

		return (y2 - y1)*360. + (m2 - m1)*30. + (d2 - d1);
	}

} // namespace tmx::date
