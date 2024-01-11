// tmx_date.h - Date and time calculation
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <chrono>
#include <iterator>
#include <tuple>

#include "tmx_datetime.h"

namespace tmx::date {

	// Calendar year/month/day date.
	using ymd = std::chrono::year_month_day;

	constexpr ymd to_ymd(int y, unsigned int m, unsigned int d)
	{
		return ymd(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
	}
	static_assert(to_ymd(2021, 1, 1) == ymd(std::chrono::year(2021), std::chrono::January, std::chrono::day(1)));
	static_assert(to_ymd(2021, 1, 1) == std::chrono::year(2021)/1/1);

	constexpr std::tuple<int, unsigned, unsigned> from_ymd(const ymd& d)
	{
		return { d.year().operator int(), d.month().operator unsigned int(), d.day().operator unsigned int() };
	}
	static_assert(from_ymd(to_ymd(2021, 1, 1)) == std::tuple(2021, 1u, 1u));

	// time_t cannot be constexpr
	inline time_t to_time_t(const ymd& d)
	{
		return std::chrono::system_clock::to_time_t(std::chrono::sys_days(d));
	}

	// d0 - d1 in years to system clock precision.
	// If dt = d0 - d1 then d1 = d0 - dt and d0 = d1 + dt.
	inline auto diffyears(ymd d1, ymd d0)
	{
		return datetime::diffyears(to_time_t(d1), to_time_t(d0));
	}
	
/*
#ifdef _DEBUG
	int basic_date_test()
	{
		using namespace std::chrono;
		{
			constexpr auto d0 = to_ymd(2023, 4, 5);
			constexpr auto d1 = to_ymd(2024, 4, 5);
			auto dd = diffyears(d1, d0);// .count();
			dd = dd;
		assert(d1 - std::chrono::years(1) == d0);
			assert(sys_days(d1) + dd == sys_days(d0));
			assert(sys_days(d0) - dd == sys_days(d1));
		}

		return 0;
	}
#endif // _DEBUG
*/
#if 0
	// TODO: remove???
	// Periodic times in [effective, termination] working backwards from termination in month steps.
	class periodic {
		ymd effective, termination;
		int months;
		ymd current;
	public:
		// STL iterator
		using iterator_category = std::forward_iterator_tag;
		using value_type = ymd;

		constexpr periodic(ymd effective, ymd termination, int months, bool end = false)
			: effective{ effective }, termination{ termination }, months{ months }, current{ termination }
		{
			if (end) {
				operator++();
			}
			else if (valid()) {
				reset();
			}
		}
		constexpr periodic(const periodic&) = default;
		constexpr periodic& operator=(const periodic&) = default;
		constexpr ~periodic() = default;

		constexpr bool operator==(const periodic&) const = default;

		// order and direction of period are compatible
		constexpr bool valid() const
		{
			if (effective == termination) {
				return months == 0;
			}

			return (effective < termination) == (months > 0);
		}

		constexpr auto begin() const
		{
			return periodic(effective, termination, months);
		}
		constexpr auto end() const
		{
			periodic(effective, termination, months, true);
		}

		constexpr explicit operator bool() const
		{
			return current <= termination;
		}
		constexpr value_type operator*() const
		{
			return current;
		}
		constexpr periodic& operator++()
		{
			if (*this) {
				current += std::chrono::months(months);
			}

			return *this;
		}
	protected:
		constexpr void reset()
		{
			while (current - std::chrono::months(months) >= effective) {
				current -= std::chrono::months(months);
			}
		}
	};
#ifdef _DEBUG
	static int periodic_test()
	{
		{
			constexpr auto eff = to_ymd(2023, 1, 2);
			constexpr auto ter = to_ymd(2025, 1, 2);
			constexpr auto pi = periodic(eff, ter, 12);
			static_assert(pi);
			constexpr auto pi2{ pi };
			static_assert(pi2 == pi);
			constexpr auto pi3 = pi2;
			static_assert(!(pi3 != pi2));
			static_assert(eff == *pi3);
		}
		{
			constexpr auto eff = to_ymd(2023, 1, 1);
			constexpr auto ter = to_ymd(2025, 1, 2);
			constexpr auto pi = periodic(eff, ter, 12);
			static_assert(*pi == to_ymd(2023, 1, 2));
		}
		{
			constexpr auto eff = to_ymd(2023, 1, 3);
			constexpr auto ter = to_ymd(2025, 1, 2);
			constexpr auto pi = periodic(eff, ter, 12);
			static_assert(*pi == to_ymd(2024, 1, 2));
		}
		{
			constexpr auto eff = to_ymd(2023, 1, 1);
			constexpr auto ter = to_ymd(2025, 2, 1);
			constexpr auto pi = periodic(eff, ter, 12);
			static_assert(*pi == to_ymd(2023, 2, 1));
		}
		{
			constexpr auto eff = to_ymd(2023, 3, 1);
			constexpr auto ter = to_ymd(2025, 2, 1);
			constexpr auto pi = periodic(eff, ter, 12);
			static_assert(*pi == to_ymd(2024, 2, 1));
		}
		{
			auto d0 = to_ymd(2023, 4, 5);
			auto d1 = to_ymd(2025, 4, 5);
			auto pi = periodic(d0, d1, 12);
			ymd ps[3];
			ps[0] = *pi;
			assert(ps[0] == d0);
			ps[1] = *++pi;
			assert(ps[1] == d0 + std::chrono::years(1));
			assert(ps[1] == d1 - std::chrono::years(1));
			ps[2] = *++pi;
			assert(ps[2] == d1);
			++pi;
			assert(!pi);
		}


		return 0;
	}
#endif // _DEBUG

	// Day count fraction appoximately equal to time in year between dates.
	// https://eagledocs.atlassian.net/wiki/spaces/Accounting2017/pages/439484565/Understand+Day+Count+Basis+Options

	namespace dcf {
		// Day count fraction in years from d0 to d1.
		constexpr years _years(const ymd& d0, const ymd& d1)
		{
			return d1 - d0;
		}
		// NASD
		constexpr years _30_360(const ymd& t0, const ymd& t1)
		{
			using std::chrono::day;
			auto d0 = (t0.day() == day(31))
				? day(30)
				: t0.day();
			auto d1 = (t1.day() == day(31) and d0 > day(29))
				? day(30)
				: t1.day();

			int dy = (int)t1.year() - (int)t0.year();
			int dm = (unsigned)t1.month() - (unsigned)t0.month();
			int dd = (unsigned)d1 - (unsigned)d0;

			return years(360 * dy + 30 * dm + dd) / 360.;
		}
		// ISMA
		constexpr years _30E_360(const ymd& t0, const ymd& t1)
		{
			using std::chrono::day;
			auto d0 = (t0.day() == day(31))
				? day(30)
				: t0.day();
			auto d1 = (t1.day() == day(31))
				? day(30)
				: t1.day();

			int dy = (int)t1.year() - (int)t0.year();
			int dm = (unsigned)t1.month() - (unsigned)t0.month();
			int dd = (unsigned)d1 - (unsigned)d0;

			return years(360 * dy + 30 * dm + dd) / 360.;
		}
		constexpr years _actual_360(const ymd& t0, const ymd& t1)
		{
			using std::chrono::sys_days;

			return years((sys_days(t1) - sys_days(t0)).count() / 360.);
		}
		constexpr years _actual_365(const ymd& t0, const ymd& t1)
		{
			using std::chrono::sys_days;

			return years((sys_days(t1) - sys_days(t0)).count() / 365.);
		}

#ifdef _DEBUG
#define DATE_DCF_TEST(X) \
	X(to_ymd(2003, 12, 29), to_ymd(2004, 1, 31), 31, 32, 33) \
	X(to_ymd(2003, 12, 30), to_ymd(2004, 1, 31), 30, 30, 32) \
	X(to_ymd(2003, 12, 31), to_ymd(2004, 1, 31), 30, 30, 31) \
	X(to_ymd(2004, 1, 1), to_ymd(2004, 1, 31), 29, 30, 30) \
	X(to_ymd(2003, 12, 29), to_ymd(2004, 2, 1), 32, 32, 34) \
	X(to_ymd(2003, 12, 30), to_ymd(2004, 2, 1), 31, 31, 33) \
	X(to_ymd(2003, 12, 31), to_ymd(2004, 2, 1), 31, 31, 32) \
	X(to_ymd(2004, 1, 1), to_ymd(2004, 2, 1), 30, 30, 31) \

		static int test()
		{
			using namespace std::chrono;
#define TEST_DATE_DCF(d0, d1, a, b, c) static_assert(dcf::_30E_360(d0, d1) == years(a/360.));
			DATE_DCF_TEST(TEST_DATE_DCF)
#undef TEST_DATE_DCF
#define TEST_DATE_DCF(d0, d1, a, b, c) static_assert(dcf::_30_360(d0, d1) == years(b/360.));
				DATE_DCF_TEST(TEST_DATE_DCF)
#undef TEST_DATE_DCF
#define TEST_DATE_DCF(d0, d1, a, b, c) static_assert(dcf::_actual_360(d0, d1) == years(c/360.));
				DATE_DCF_TEST(TEST_DATE_DCF)
#undef TEST_DATE_DCF
			{
				constexpr auto t0 = year(2023) / 1 / 2;
				constexpr auto t1 = year(2024) / 1 / 2;
				constexpr auto y2 = dcf::_30_360(t0, t1);
				static_assert(y2 == years(1));
				constexpr auto y3 = dcf::_actual_360(t0, t1);
				constexpr auto dd = sys_days(t1) - sys_days(t0);
				static_assert(dd == std::chrono::days(365));
				constexpr auto yy = years(dd.count() / 360.);
				static_assert(y3 == yy);// years(dd.count() / 360.));
			}
			{
				constexpr auto t0 = year(2023) / 1 / 2;
				constexpr auto t1 = year(2024) / 1 / 4;
				constexpr auto y2 = dcf::_30_360(t0, t1);
				constexpr auto y2_ = years(1 + 0 / 30. + 2 / 360.);
				static_assert(y2 == y2_);
				constexpr auto y3 = dcf::_actual_365(t0, t1);
				constexpr auto dd = sys_days(t1) - sys_days(t0);
				static_assert(dd == std::chrono::days(367));
				constexpr auto yy = years(dd.count() / 365.);
				static_assert(y3 == yy);
			}

			return 0;
		}
#endif // _DEBUG
	}
	// Roll to business day conventions.
	enum class roll {
		none,
		following, // following business day
		previous,  // previous business day
		modified_following, // following business day unless different month
		modified_previous,  // previous business day unless different month
	};

	constexpr ymd adjust(const ymd& date, roll convention, const calendar& cal = calendars::weekday)
	{
		using std::chrono::sys_days;

		if (!cal(date)) {
			return date;
		}

		switch (convention) {
		case roll::none:
			return date;
		case roll::previous:
			return adjust(sys_days(date) - std::chrono::days(1), convention, cal);
		case roll::following:
			return adjust(sys_days(date) + std::chrono::days(1), convention, cal);
		case roll::modified_following:
		{
			const auto date_ = adjust(sys_days(date), roll::following, cal);
			return date_.month() == date.month()
				? date_
				: adjust(date, roll::previous, cal);
		}
		case roll::modified_previous:
		{
			const auto date_ = adjust(sys_days(date), roll::previous, cal);
			return date_.month() == date.month()
				? date_
				: adjust(date, roll::following, cal);
		}
		default:
			return ymd{};
		}

		return date;
	}
#endif // 0

	enum class frequency {
		annually = 1,
		semiannually = 2,
		quarterly = 4,
		monthly = 12,
	};
	constexpr auto tenor(frequency f)
	{
		return 12 / (int)f;
	}

} // namespace tmx::date
