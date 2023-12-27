// tmx_date.h - Date and time calculation
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <chrono>
#include <iterator>
#include <tuple>

namespace tmx::date {

	// Calendar year/month/day date.
	using ymd = std::chrono::year_month_day;

	constexpr auto y_m_d(const ymd& d)
	{
		return std::make_tuple(d.year(), d.month(), d.day());
	}

	// duration as double in years
	using years = std::chrono::duration<double, std::chrono::years::period>;

	// d0 - d1 in years to system clock precision.
	// If dt = d0 - d1 then d1 = d0 - dt and d0 = d1 + dt.
	constexpr years operator-(ymd d0, ymd d1)
	{
		return years(std::chrono::sys_days(d0) - std::chrono::sys_days(d1));
	}

#ifdef _DEBUG
	int basic_date_test()
	{
		using namespace std::chrono;
		{
			constexpr auto d0 = 2023y / 4 / 5;
			// structured binding cannot be constexpr
			auto [y, m, d] = y_m_d(d0);
			assert(y == 2023y);
			assert(m == month(4)); //??? no string literal
			assert(d == 5d);
		}
		{
			constexpr auto d0 = 2023y/4/5;
			constexpr auto d1 = 2024y/4/5;
			constexpr auto dd = d0 - d1;
			static_assert(d1 - std::chrono::years(1) == d0);
			static_assert(sys_days(d1) + dd == sys_days(d0));
			static_assert(sys_days(d0) - dd == sys_days(d1));
		}
		{
			constexpr auto d0 = 2023y/4/5;
			constexpr auto d1 = 2024y/6/7;
			constexpr auto dd = d0 - d1;
			static_assert(sys_days(d1) + dd == sys_days(d0));
			static_assert(sys_days(d0) - dd == sys_days(d1));
		}
		{
			constexpr auto d0 = 2023y/4/5;
			constexpr auto d1 = 2024y/7/6;
			constexpr auto dd = d0 - d1;
			static_assert(sys_days(d1) + dd == sys_days(d0));
			static_assert(sys_days(d0) - dd == sys_days(d1));
		}

		return 0;
	}
#endif // _DEBUG

	// Periodic times in [effective, termination] working backwards from termination in period steps.
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
		using namespace std::chrono;
		{
			constexpr auto eff = 2023y/ 1/ 2;
			constexpr auto ter = 2025y/ 1/ 2;
			constexpr auto pi = periodic(eff, ter, 12);
			static_assert(pi);
			constexpr auto pi2{ pi };
			static_assert(pi2 == pi);
			constexpr auto pi3 = pi2;
			static_assert(!(pi3 != pi2));
			static_assert(eff == *pi3);
		}
		{
			constexpr auto eff = 2023y/ 1/ 1;
			constexpr auto ter = 2025y/ 1/ 2;
			constexpr auto pi = periodic(eff, ter, 12);
			static_assert(*pi == 2023y/ 1/ 2);
		}
		{
			constexpr auto eff = 2023y/ 1/ 3;
			constexpr auto ter = 2025y/ 1/ 2;
			constexpr auto pi = periodic(eff, ter, 12);
			static_assert(*pi == 2024y/ 1/ 2);
		}
		{
			constexpr auto eff = 2023y/ 1/ 1;
			constexpr auto ter = 2025y/ 2/ 1;
			constexpr auto pi = periodic(eff, ter, 12);
			static_assert(*pi == 2023y/ 2/ 1);
		}
		{
			constexpr auto eff = 2023y/ 3/ 1;
			constexpr auto ter = 2025y/ 2/ 1;
			constexpr auto pi = periodic(eff, ter, 12);
			static_assert(*pi == 2024y/ 2/ 1);
		}
		{
			auto d0 = 2023y/ 4/ 5;
			auto d1 = 2025y/ 4/ 5;
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
	using dcf_t = years(*)(const ymd&, const ymd&);
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
		/*
	X(std::chrono::year(2003)/ 12/ 2, std::chrono::year(2004)/ 1/ 3, 31, 32, 33) \
	X(make_ymd(2003, 12, 30), make_ymd(2004, 1, 31), 30, 30, 32) \
	X(make_ymd(2003, 12, 31), make_ymd(2004, 1, 31), 30, 30, 31) \
	X(make_ymd(2004, 1, 1), make_ymd(2004, 1, 31), 29, 30, 30) \
	X(make_ymd(2003, 12, 29), make_ymd(2004, 2, 1), 32, 32, 34) \
	X(make_ymd(2003, 12, 30), make_ymd(2004, 2, 1), 31, 31, 33) \
	X(make_ymd(2003, 12, 31), make_ymd(2004, 2, 1), 31, 31, 32) \
	X(make_ymd(2004, 1, 1), make_ymd(2004, 2, 1), 30, 30, 31) \
	*/

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

	// Return true if date is a holiday.
	using holiday = bool(*)(const ymd&);
	namespace holidays {

		// Yearly holiday on month and day
		constexpr bool month_day(const ymd& d, const std::chrono::month& month, const std::chrono::day& day)
		{
			return (d.month() == month) and (d.day() == day);
		}

		constexpr bool new_year_day(const ymd& d)
		{
			return month_day(d, std::chrono::January, std::chrono::day(1));
		}

		// ...

		constexpr bool christmas_day(const ymd& d)
		{
			return month_day(d, std::chrono::December, std::chrono::day(25));
		}

	} // namespace holiday

	// Return true on non-trading days.
	using calendar = bool(*)(const ymd&);
	namespace calendars {

		constexpr bool weekday(const ymd& d)
		{
			auto w = std::chrono::year_month_weekday(d);
			auto wd = w.weekday();

			return (wd == std::chrono::Saturday) or (wd == std::chrono::Sunday);
		}
		constexpr bool example(const ymd& d)
		{
			return weekday(d) or holidays::new_year_day(d);
		}

		// constexpr bool nyse(const ymd& d) ...

	} // namespace calendars

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

#ifdef _DEBUG
	int test()
	{
		using year = std::chrono::year;
		using days = std::chrono::sys_days;
		//using seconds = std::chrono::seconds;
		{
			constexpr auto d0 = year{ 2023 } / 1 / 1;
			constexpr auto d1 = year{ 2023 } / 1 / 2;
			constexpr auto dt = days(d1) - days(d0);
			constexpr auto dy = years(dt);
			static_assert(0 != dy.count());
			static_assert(1 == dt.count());
			constexpr auto y0 = dcf::_years(d0, d1);
			static_assert(dy == y0);
		}

		return 0;
	}
#endif // _DEBUG

} // namespace tmx::date
