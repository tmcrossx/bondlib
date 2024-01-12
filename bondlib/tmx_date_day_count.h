// tmx_date_day_count.h - Day count conventions for tmx dates
// https://eagledocs.atlassian.net/wiki/spaces/Accounting2017/pages/439484565/Understand+Day+Count+Basis+Options
// https://github.com/bloomberg/bde/tree/main/groups/bbl/bbldc
// See XXX.t.cpp for tests.
#pragma once
#include "tmx_date.h"

namespace tmx::date {

	using day_count_t = double(*)(const ymd&, const ymd&);

	// https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicisma30360.cpp
	constexpr double day_count_isma30360(const ymd& ymd1, const ymd& ymd2)
	{
		auto [y1, m1, d1] = from_ymd(ymd1);
		auto [y2, m2, d2] = from_ymd(ymd2);

		if (d1 == 31) {
			d1 = 30;
		}
		if (d2 == 31) {
			d2 = 30;
		}

		return ((y2 - y1) * 360 + (m2 - m1) * 30 + (d2 - d1))/360.0;
	}

#ifdef _DEBUG

// https://github.com/bloomberg/bde/blob/4da5189379a5fa371cb371ca9bcc6fa8b8670394/groups/bbl/bbldc/bbldc_basicisma30360.t.cpp#L178
#define TMX_DATE_DAY_COUNT(X) \
                X(1993,     2,     1,   1993,     3,     1,   0.0833 ) \
                X(1996,     1,    15,   1996,     5,    31,   0.3750 ) \
                X(2000,     2,    27,   2000,     2,    27,   0.0000 ) \
                X(2000,     2,    27,   2000,     2,    28,   0.0028 ) \
                X(2000,     2,    27,   2000,     2,    29,   0.0056 ) \
                X(2000,     2,    28,   2000,     2,    27,  -0.0028 ) \
                X(2000,     2,    28,   2000,     2,    28,   0.0000 ) \
                X(2000,     2,    28,   2000,     2,    29,   0.0028 ) \
                X(2000,     2,    29,   2000,     2,    27,  -0.0056 ) \
                X(2000,     2,    29,   2000,     2,    28,  -0.0028 ) \
                X(2000,     2,    29,   2000,     2,    29,   0.0000 ) \
                X(2003,     2,    28,   2004,     2,    29,   1.0028 ) \
                X(2000,     1,    29,   2000,     1,    30,   0.0028 ) \
                X(2000,     1,    29,   2000,     1,    31,   0.0028 ) \
                X(2000,     1,    29,   2000,     3,    29,   0.1667 ) \
                X(2000,     1,    29,   2000,     3,    30,   0.1694 ) \
                X(2000,     1,    29,   2000,     3,    31,   0.1694 ) \
                X(2000,     1,    30,   2000,     1,    31,   0.0000 ) \
                X(2000,     1,    30,   2000,     2,    27,   0.0750 ) \
                X(2000,     1,    30,   2000,     2,    28,   0.0778 ) \
                X(2000,     1,    30,   2000,     2,    29,   0.0806 ) \
                X(2000,     1,    30,   2000,     3,    29,   0.1639 ) \
                X(2000,     1,    30,   2000,     3,    30,   0.1667 ) \
                X(2000,     1,    30,   2000,     3,    31,   0.1667 ) \
                X(2000,     1,    31,   2000,     3,    29,   0.1639 ) \
                X(2000,     1,    31,   2000,     3,    30,   0.1667 ) \
                X(2000,     1,    31,   2000,     3,    31,   0.1667 ) \
                X(2000,     1,    29,   2004,     1,    30,   4.0028 ) \
                X(2000,     1,    29,   2004,     1,    31,   4.0028 ) \
                X(2000,     1,    29,   2004,     3,    29,   4.1667 ) \
                X(2000,     1,    29,   2004,     3,    30,   4.1694 ) \
                X(2000,     1,    29,   2004,     3,    31,   4.1694 ) \
                X(2000,     1,    30,   2004,     1,    31,   4.0000 ) \
                X(2000,     1,    30,   2004,     2,    27,   4.0750 ) \
                X(2000,     1,    30,   2004,     2,    28,   4.0778 ) \
                X(2000,     1,    30,   2004,     2,    29,   4.0806 ) \
                X(2000,     1,    30,   2004,     3,    29,   4.1639 ) \
                X(2000,     1,    30,   2004,     3,    30,   4.1667 ) \
                X(2000,     1,    30,   2004,     3,    31,   4.1667 ) \
                X(2000,     1,    31,   2004,     3,    29,   4.1639 ) \
                X(2000,     1,    31,   2004,     3,    30,   4.1667 ) \
                X(2000,     1,    31,   2004,     3,    31,   4.1667 ) \
                X(2004,     1,    29,   2000,     1,    30,  -3.9972 ) \
                X(2004,     1,    29,   2000,     1,    31,  -3.9972 ) \
                X(2004,     1,    29,   2000,     3,    29,  -3.8333 ) \
                X(2004,     1,    29,   2000,     3,    30,  -3.8306 ) \
                X(2004,     1,    29,   2000,     3,    31,  -3.8306 ) \
                X(2004,     1,    30,   2000,     1,    31,  -4.0000 ) \
                X(2004,     1,    30,   2000,     2,    27,  -3.9250 ) \
                X(2004,     1,    30,   2000,     2,    28,  -3.9222 ) \
                X(2004,     1,    30,   2000,     2,    29,  -3.9194 ) \
                X(2004,     1,    30,   2000,     3,    29,  -3.8361 ) \
                X(2004,     1,    30,   2000,     3,    30,  -3.8333 ) \
                X(2004,     1,    30,   2000,     3,    31,  -3.8333 ) \
                X(2004,     1,    31,   2000,     3,    29,  -3.8361 ) \
                X(2004,     1,    31,   2000,     3,    30,  -3.8333 ) \
                X(2004,     1,    31,   2000,     3,    31,  -3.8333 ) \

    // ??? where should this go
    // fabs(a - b) <= eps
    constexpr bool approx(double a, double b, double eps) {
		return -eps <= a - b && a - b <= eps;
	}

// TODO: bisect to find tests not passing
#define DAY_COUNT_TEST(Y1, M1, D1, Y2, M2, D2, DC) \
static_assert(approx(day_count_isma30360(to_ymd(Y1, M1, D1), to_ymd(Y2, M2, D2)), DC, 1e-4));
//	TMX_DATE_DAY_COUNT(DAY_COUNT_TEST)
#undef DAY_COUNT_TEST
#undef TMX_DATE_DAY_COUNT

#endif // _DEBUG

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

	// TODO: actual/actual
	// TODO: actual/360
	// TODO: actual/365

} // namespace tmx::date
