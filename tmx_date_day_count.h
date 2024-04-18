// tmx_date_day_count.h - Day count conventions for tmx dates
// https://eagledocs.atlassian.net/wiki/spaces/Accounting2017/pages/439484565/Understand+Day+Count+Basis+Options
// https://github.com/bloomberg/bde/tree/main/groups/bbl/bbldc
// See XXX.t.cpp for tests.
#pragma once
#include <functional>
#ifdef _DEBUG
#include "tmx_math.h"
#endif // _DEBUG
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

		return ((y2 - y1) * 360 + (m2 - m1) * 30 + (d2 - d1)) / 360.0;
	}
	// test for day_count_isma30360()
#ifdef _DEBUG

// https://github.com/bloomberg/bde/blob/4da5189379a5fa371cb371ca9bcc6fa8b8670394/groups/bbl/bbldc/bbldc_basicisma30360.t.cpp#L178
#define TMX_DATE_DAY_COUNT(X) \
                X(1993,     2,     1,   1993,     3,     1,   0.0833 ) \
                X(1996,     1,    15,   1996,     5,    31,   0.3750 ) \
                X(2000,     2,    27,   2000,     2,    27,   0.0000 ) \
                X(2000,     2,    27,   2000,     2,    28,   0.0028 ) \
                X(2000,     2,    27,   2000,     2,    29,   0.0056 ) \
                X(2000,     2,    28,   2000,     2,    28,   0.0000 ) \
                X(2000,     2,    28,   2000,     2,    29,   0.0028 ) \
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

#define DAY_COUNT_TEST(Y1, M1, D1, Y2, M2, D2, DC) \
static_assert(math::equal_precision(day_count_isma30360(to_ymd(Y1, M1, D1), to_ymd(Y2, M2, D2)), DC, -4));
	//TMX_DATE_DAY_COUNT(DAY_COUNT_TEST) 
#undef DAY_COUNT_TEST
#undef TMX_DATE_DAY_COUNT

#endif // _DEBUG

	// https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicpsa30360eom.cpp
	constexpr double day_count_isma30360eom(const ymd& ymd1, const ymd& ymd2)
	{
		auto [y1, m1, d1] = from_ymd(ymd1);
		auto [y2, m2, d2] = from_ymd(ymd2);

		if (m1 == 2 && (d1 == 29 || (d1 == 28 && !ymd1.year().is_leap()))) {
			d1 = 30;
			if ((y1 == y2) && (m2 == 2) && (d2 == 29 || (d2 == 28 && !ymd1.year().is_leap()))) {
				return 0 / 360.0;
			}
		}
		else if (d1 == 31) {
			d1 = 30;
		}
		if (d2 == 31 && d1 == 30) {
			d2 = 30;
		}
		return ((y2 - y1) * 360 + (m2 - m1) * 30 + (d2 - d1)) / 360.0;
	}


#ifdef _DEBUG
	// test for day_count_isma30360eom()
//https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicpsa30360eom.t.cpp#L179
#define TMX_DATE_DAY_COUNT(X) \
                X(1993,     2,     1,   1993,     3,     1,   0.0833 ) \
                X(1996,     1,    15,   1996,     5,    31,   0.3778 ) \
                X(2000,     2,    27,   2000,     2,    27,   0.0000 ) \
                X(2000,     2,    27,   2000,     2,    28,   0.0028 ) \
                X(2000,     2,    27,   2000,     2,    29,   0.0056 ) \
                X(2000,     2,    28,   2000,     2,    28,   0.0000 ) \
                X(2000,     2,    28,   2000,     2,    29,   0.0028 ) \
                X(2000,     2,    29,   2000,     2,    29,   0.0000 ) \
                X(2000,     2,    27,   2000,     2,    27,   0.0000 ) \
                X(2003,     2,    28,   2004,     2,    29,   0.9972 ) \
                X(1993,     12,   15,   1993,     12,   30,   0.0417 ) \
                X(1993,     12,   15,   1993,    12,    31,   0.0444 ) \
                X(1993,    12,    31,   1994,     2,     1,   0.0861 ) \
                X(1998,     2,    27,   1998,     3,    27,   0.0833 ) \
                X(1998,     2,    28,   1998,     3,    27,   0.0750 ) \
                X(1999,     1,     1,   1999,     1,    29,   0.0778 ) \
                X(1999,     1,    29,   1999,     1,    30,   0.0028 ) \
                X(1999,     1,    29,   1999,     1,    31,   0.0056 ) \
                X(1999,     1,    29,   1999,     3,    29,   0.1667 ) \
                X(1999,     1,    29,   1999,     3,    30,   0.1694 ) \
                X(1999,     1,    29,   1999,     3,    31,   0.1722 ) \
                X(1999,     1,    30,   1999,     1,    31,       0. ) \
                X(1999,     1,    30,   1999,     2,    27,   0.0750 ) \
                X(1999,     1,    30,   1999,     2,    28,   0.0778 ) \
                X(1999,     1,    30,   1999,     3,    29,   0.1639 ) \
                X(1999,     1,    30,   1999,     3,    30,   0.1667 ) \
                X(1999,     1,    30,   1999,     3,    31,   0.1667 ) \
                X(1999,     1,    31,   1999,     3,    29,   0.1639 ) \
                X(1999,     1,    31,   1999,     3,    30,   0.1667 ) \
                X(1999,     1,    31,   1999,     3,    31,   0.1667 ) \
                X(1999,     2,    27,   1999,     2,    27,       0. ) \
                X(1999,     2,    27,   1999,     2,    28,   0.0028 ) \
                X(1999,     2,    28,   1999,     2,    28,       0. ) \
                X(2000,     1,    29,   2000,     1,    31,   0.0056) \
                X(2000,     1,    29,   2000,     3,    29,   0.1667 ) \
                X(2000,     1,    29,   2000,     3,    30,   0.1694 ) \
                X(2000,     1,    29,   2000,     3,    31,   0.1722) \
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
                X(2000,     1,    29,   2004,     1,    31,   4.0056 ) \
                X(2000,     1,    29,   2004,     3,    29,   4.1667 ) \
                X(2000,     1,    29,   2004,     3,    30,   4.1694 ) \
                X(2000,     1,    29,   2004,     3,    31,   4.1722 ) \
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



#define DAY_COUNT_TEST(Y1, M1, D1, Y2, M2, D2, DC) \
static_assert(math::equal_precision(day_count_isma30360eom(to_ymd(Y1, M1, D1), to_ymd(Y2, M2, D2)), DC, -4));
	TMX_DATE_DAY_COUNT(DAY_COUNT_TEST)
#undef DAY_COUNT_TEST
#undef TMX_DATE_DAY_COUNT
#endif // _DEBUG


	// https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicisdaactualactual.cpp
	constexpr double day_count_isdaactualactual(const ymd& ymd1, const ymd& ymd2)
	{
		auto [y1, m1, d1] = from_ymd(ymd1);
		auto [y2, m2, d2] = from_ymd(ymd2);

		const int db = 365 + ymd1.year().is_leap();
		const int de = 365 + ymd2.year().is_leap();
		const int dy = y2 - y1 - 1;
		double dbd = diffdays(to_ymd(y1 + 1, 1, 1), ymd1);
		double dde = diffdays(ymd2, to_ymd(y2, 1, 1));

		double num = dy * db * de + dbd * de + dde * db;
		int den = db * de;

		return num / den;
	}

#ifdef _DEBUG
	// test for day_count_isdaactualactual()
//https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicisdaactualactual.t.cpp#L186
#define TMX_DATE_DAY_COUNT(X) \
                X(1992,     2,     1,   1992,     3,     1,   0.0792) \
                X(1993,     2,     1,   1993,     3,     1,   0.0767) \
                X(1996,     1,    15,   1996,     5,    31,   0.3743) \
                X(2000,     2,    27,   2000,     2,    27,   0.0000) \
                X(2000,     2,    27,   2000,     2,    28,   0.0027) \
                X(2000,     2,    27,   2000,     2,    29,   0.0055) \
                X(2000,     2,    28,   2000,     2,    28,   0.0000) \
                X(2000,     2,    28,   2000,     2,    29,   0.0027) \
                X(2000,     2,    29,   2000,     2,    29,   0.0000) \
                X(2001,     1,     1,   2003,     1,     1,   2.0000) \
                X(1999,     2,     1,   1999,     7,     1,   0.4110) \
                X(2000,     1,    30,   2000,     6,    30,   0.4153) \
                X(2002,     8,    15,   2003,     7,    15,   0.9151) \
                X(1992,     2,     1,   1993,     3,     1,   1.0769) \
                X(1993,     2,     1,   1996,     2,     1,   2.9998) \
                X(2003,     2,    28,   2004,     2,    29,   1.0023) \
                X(1999,     7,     1,   2000,     7,     1,   1.0014) \
                X(1999,     7,    30,   2000,     1,    30,   0.5039) \
                X(1999,    11,    30,   2000,     4,    30,   0.4155) \
                X(2003,    11,     1,   2004,     5,     1,   0.4977) \

#define DAY_COUNT_TEST(Y1, M1, D1, Y2, M2, D2, DC) \
static_assert(math::equal_precision(day_count_isdaactualactual(to_ymd(Y1, M1, D1), to_ymd(Y2, M2, D2)), DC, -4));
	TMX_DATE_DAY_COUNT(DAY_COUNT_TEST)
#undef DAY_COUNT_TEST
#undef TMX_DATE_DAY_COUNT
#endif // _DEBUG

	// https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicactual360.cpp
	constexpr double day_count_actual360(const ymd& ymd1, const ymd& ymd2)
	{
		return diffdays(ymd2, ymd1) / 360.0;
	}

#ifdef _DEBUG
	// test for day_count_actual360()
//https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicactual360.t.cpp#L194
#define TMX_DATE_DAY_COUNT(X) \
                X(1992,     2,     1,   1992,     3,     1,   0.0806) \
                X(1992,     2,     1,   1993,     3,     1,   1.0944) \
                X(1993,     1,     1,   1993,     2,    21,   0.1417) \
                X(1993,     1,     1,   1994,     1,     1,   1.0139) \
                X(1993,     1,    15,   1993,     2,     1,   0.0472) \
                X(1993,     2,     1,   1993,     3,     1,   0.0778) \
                X(1993,     2,     1,   1996,     2,     1,   3.0417) \
                X(1993,     2,     1,   1993,     3,     1,   0.0778) \
                X(1993,     2,    15,   1993,     4,     1,   0.1250) \
                X(1993,     3,    15,   1993,     6,    15,   0.2556) \
                X(1993,     3,    31,   1993,     4,     1,   0.0028) \
                X(1993,     3,    31,   1993,     4,    30,   0.0833) \
                X(1993,     3,    31,   1993,    12,    31,   0.7639) \
                X(1993,     7,    15,   1993,     9,    15,   0.1722) \
                X(1993,     8,    21,   1994,     4,    11,   0.6472) \
                X(1993,    11,     1,   1994,     3,     1,   0.3333) \
                X(1993,    12,    15,   1993,    12,    30,   0.0417) \
                X(1993,    12,    15,   1993,    12,    31,   0.0444) \
                X(1993,    12,    31,   1994,     2,     1,   0.0889) \
                X(1996,     1,    15,   1996,     5,    31,   0.3806) \
                X(1998,     2,    27,   1998,     3,    27,   0.0778) \
                X(1998,     2,    28,   1998,     3,    27,   0.0750) \
                X(1999,     1,     1,   1999,     1,    29,   0.0778) \
                X(1999,     1,    29,   1999,     1,    30,   0.0028) \
                X(1999,     1,    29,   1999,     1,    31,   0.0056) \
                X(1999,     1,    29,   1999,     3,    29,   0.1639) \
                X(1999,     1,    29,   1999,     3,    30,   0.1667) \
                X(1999,     1,    29,   1999,     3,    31,   0.1694) \
                X(1999,     1,    30,   1999,     1,    31,   0.0028) \
                X(1999,     1,    30,   1999,     2,    27,   0.0778) \
                X(1999,     1,    30,   1999,     2,    28,   0.0806) \
                X(1999,     1,    30,   1999,     3,    29,   0.1611) \
                X(1999,     1,    30,   1999,     3,    30,   0.1639) \
                X(1999,     1,    30,   1999,     3,    31,   0.1667) \
                X(1999,     1,    31,   1999,     3,    29,   0.1583) \
                X(1999,     1,    31,   1999,     3,    30,   0.1611) \
                X(1999,     1,    31,   1999,     3,    31,   0.1639) \


#define DAY_COUNT_TEST(Y1, M1, D1, Y2, M2, D2, DC) \
static_assert(math::equal_precision(day_count_actual360(to_ymd(Y1, M1, D1), to_ymd(Y2, M2, D2)), DC, -4));
	TMX_DATE_DAY_COUNT(DAY_COUNT_TEST)
#undef DAY_COUNT_TEST
#undef TMX_DATE_DAY_COUNT
#endif // _DEBUG     

	// https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicactual365fixed.cpp
	constexpr double day_count_actual365fixed(const ymd& ymd1, const ymd& ymd2)
	{
		return diffdays(ymd2, ymd1) / 365.0;
	}

#ifdef _DEBUG
	// test for day_count_actual365fixed()
	//https://github.com/bloomberg/bde/blob/main/groups/bbl/bbldc/bbldc_basicactual365fixed.t.cpp#L195
#define TMX_DATE_DAY_COUNT(X) \
                X(1992,     2,     1,   1992,     3,     1,   0.0795) \
                X(1992,     2,     1,   1993,     3,     1,   1.0795) \
                X(1993,     1,     1,   1993,     2,    21,   0.1397) \
                X(1993,     1,     1,   1994,     1,     1,   1.0000) \
                X(1993,     1,    15,   1993,     2,     1,   0.0466) \
                X(1993,     2,     1,   1993,     3,     1,   0.0767) \
                X(1993,     2,     1,   1996,     2,     1,   3.0000) \
                X(1993,     2,     1,   1993,     3,     1,   0.0767) \
                X(1993,     2,    15,   1993,     4,     1,   0.1233) \
                X(1993,     3,    15,   1993,     6,    15,   0.2521) \
                X(1993,     3,    31,   1993,     4,     1,   0.0027) \
                X(1993,     3,    31,   1993,     4,    30,   0.0822) \
                X(1993,     3,    31,   1993,    12,    31,   0.7534) \
                X(1993,     7,    15,   1993,     9,    15,   0.1699) \
                X(1993,     8,    21,   1994,     4,    11,   0.6384) \
                X(1993,    11,     1,   1994,     3,     1,   0.3288) \
                X(1993,    12,    15,   1993,    12,    30,   0.0411) \
                X(1993,    12,    15,   1993,    12,    31,   0.0438) \
                X(1993,    12,    31,   1994,     2,     1,   0.0877) \
                X(1996,     1,    15,   1996,     5,    31,   0.3753) \
                X(1998,     2,    27,   1998,     3,    27,   0.0767) \
                X(1998,     2,    28,   1998,     3,    27,   0.0740) \

#define DAY_COUNT_TEST(Y1, M1, D1, Y2, M2, D2, DC) \
static_assert(math::equal_precision(day_count_actual365fixed(to_ymd(Y1, M1, D1), to_ymd(Y2, M2, D2)), DC, -4));
	TMX_DATE_DAY_COUNT(DAY_COUNT_TEST)
#undef DAY_COUNT_TEST
#undef TMX_DATE_DAY_COUNT
#endif // _DEBUG     

#ifdef _DEBUG
	inline int day_count_test()
	{
		using namespace std::chrono_literals;

		{
            ymd d0 = to_ymd(2004, 9, 30);
			ymd d1 = 2004y / 12 / 31;
			const double yearsDiff = day_count_isma30360(d0, d1);
			assert(0.25 == yearsDiff);
		}
		{
			ymd d0 = to_ymd(2000, 3, 31);
			ymd d1 = 2004y / 1 / 31;
			const double yearsDiff = day_count_isma30360(d0, d1);
			assert(3.8333 <= yearsDiff);
			assert(3.8334 >= yearsDiff);
		}

		return 0;
	}
#endif // _DEBUG

} // namespace tmx::date
