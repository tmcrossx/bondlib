// tmx_date.h - Convert year/month/day to time_t and back.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <tuple>
#include <chrono>

using std::literals::chrono_literals::operator""y;

namespace tmx::date {

	// Use calendar year/month/day from <chrono> for dates.
	using ymd = std::chrono::year_month_day; // E.g., ymd d = 2024y / 5 / 6

	// Short names for constants from <chrono>	
	constexpr time_t seconds_per_year = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::years{ 1 }).count();
	constexpr time_t seconds_per_day = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::days{ 1 }).count();
	constexpr double days_per_year = static_cast<double>(seconds_per_year) / seconds_per_day;

	// Broken down date to ymd. TODO: remove?
	constexpr ymd to_ymd(int y, unsigned int m, unsigned int d)
	{
		return ymd(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
	}
	static_assert(to_ymd(2023, 4, 5) == 2023y / 4 / 5);

	// Break down ymd to tuple.
	constexpr auto from_ymd(const ymd& d) // spread?
	{
		return std::tuple<int, unsigned, unsigned>(d.year(), d.month(), d.day());
	}
	static_assert(from_ymd(to_ymd(2023, 4, 5)) == std::tuple(2023, 4u, 5u));

	// year/month/day to UTC time_t
	constexpr time_t to_time_t(const ymd& ymd)
	{
		return std::chrono::sys_days(ymd).time_since_epoch().count() * seconds_per_day;
	}
	// Not guaranteed to be true.
	static_assert(to_time_t(to_ymd(1970, 1, 1)) == 0);
    static_assert(to_time_t(to_ymd(1970, 1, 2)) == seconds_per_day);

	// UTC time_t to year/month/day
	constexpr ymd from_time_t(time_t t)
	{
		using namespace std::chrono;

		return ymd{ floor<days>(time_point<system_clock>{seconds{t}}) };
	}
#ifdef _DEBUG
	static_assert(from_time_t(0) == to_ymd(1970, 1, 1));
	static_assert(from_time_t(0) == to_ymd(1970, 1, 1));
	static_assert(from_time_t(seconds_per_day) == to_ymd(1970, 1, 2));
	static_assert(from_time_t(-seconds_per_day) == to_ymd(1969, 12, 31));
#endif // _DEBUG

	// Time in seconds from d0 to d1.
	constexpr time_t diffseconds(ymd d1, ymd d0)
	{
		return to_time_t(d1) - to_time_t(d0);
	}
#ifdef _DEBUG
	static_assert(diffseconds(to_ymd(2023, 4, 5), to_ymd(2023, 4, 4)) == seconds_per_day);
	static_assert(diffseconds(to_ymd(2023, 4, 5), to_ymd(2023, 4, 6)) == -seconds_per_day);
#endif // _DEBUG

	// Time in days from d0 to d1.
	constexpr double diffdays(ymd d1, ymd d0)
	{
		return static_cast<double>(diffseconds(d1, d0)) / seconds_per_day;
	}
	// Time in years from d0 to d1.
	constexpr double diffyears(ymd d1, ymd d0)
	{
		return static_cast<double>(diffseconds(d1, d0)) / seconds_per_year;
	}

	constexpr ymd adddays(ymd d, double ds)
	{
		return from_time_t(to_time_t(d) + static_cast<time_t>(ds * seconds_per_day));
	}
	constexpr ymd addyears(ymd d, double ys)
	{
		return from_time_t(to_time_t(d) + static_cast<time_t>(ys * seconds_per_year));
	}

} // namespace tmx::date

// Date difference in years.
constexpr double operator-(const tmx::date::ymd& d1, const tmx::date::ymd& d0)
{
	return tmx::date::diffyears(d1, d0);
}
// Add years to date.
constexpr tmx::date::ymd operator+(const tmx::date::ymd& d, double y)
{
	return tmx::date::addyears(d, y);
}

#ifdef _DEBUG
static_assert(2023y / 4 / 5 - 2023y / 4 / 5 == 0);
static_assert(2024y / 4 / 5 - 2023y / 4 / 5 >= 1);
static_assert(2024y / 4 / 5 - 2023y / 4 / 5 <= 1.01);
static_assert(2023y / 4 / 5 + (2024y / 4 / 5 - 2023y / 4 / 5) == 2024y / 4 / 5);
#endif // _DEBUG
