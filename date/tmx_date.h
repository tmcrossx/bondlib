// tmx_date.h - Convert year/month/day to time_t and back.
#pragma once
#ifdef _DEBUG
#include <cassert>
#endif // _DEBUG
#include <tuple>
#include <chrono>

namespace tmx::date {

	using std::literals::chrono_literals::operator""y;
	using ymd = std::chrono::year_month_day; // E.g., ymd d = 2024y / 5 / 6
	
	constexpr time_t seconds_per_year = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::years{ 1 }).count();

	// Time in seconds between d0 and d1
	constexpr time_t operator-(const ymd& d1, const ymd& d0)
	{
		using namespace std::chrono;

		return duration_cast<seconds>(sys_days(d1) - sys_days(d0)).count();
	}
	static_assert(2023y / 4 / 5 - 2023y / 4 / 5 == 0);
	static_assert(2023y / 4 / 6 - 2023y / 4 / 5 == 86400);
	
	constexpr double diffyears(const ymd& d1, const ymd& d0)
	{
		return static_cast<double>(d1 - d0) / seconds_per_year;
	}

	// d0 + (d1 - d0) = d1
	constexpr ymd operator+(const ymd& d, time_t s)
	{
		using namespace std::chrono;

		return ymd{ floor<days>(sys_days(d) + seconds(s)) };
	}
	static_assert(2023y / 4 / 5 + (2023y / 4 / 6 - 2023y / 4 / 5) == 2023y / 4 / 6);
	static_assert(2023y / 4 / 5 + (2023y / 5 / 6 - 2023y / 4 / 5) == 2023y / 5 / 6);
	static_assert(2023y / 4 / 5 + (2024y / 5 / 6 - 2023y / 4 / 5) == 2024y / 5 / 6);

	// Break down ymd to tuple.
	constexpr auto split(const ymd& d) // spread?
	{
		return std::tuple<int, unsigned, unsigned>(d.year(), d.month(), d.day());
	}

	// Difference in days from d0 to d1.
	constexpr auto diffdays(const ymd& d1, const ymd& d0)
	{
		using namespace std::chrono;

		return (sys_days(d1) - sys_days(d0)).count();
	}
	/*
	// Difference in months from d0 to d1.
	constexpr auto diffmonths(const ymd& d1, const ymd& d0)
	{
		using namespace std::chrono;

		return d1.month() - d0.month();
	}
	*/
#if 0
	// Broken down date to ymd. TODO: remove?
	constexpr ymd to_ymd(int y, unsigned int m, unsigned int d)
	{
		return ymd(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
	}
#ifdef _DEBUG
	static_assert(to_ymd(2023, 4, 5) == 2023y / 4 / 5);
#endif // _DEBUG

	// Break down ymd to tuple.
	constexpr auto from_ymd(const ymd& d) // spread?
	{
		return std::tuple<int, unsigned, unsigned>(d.year(), d.month(), d.day());
	}
#ifdef _DEBUG
	static_assert(from_ymd(to_ymd(2023, 4, 5)) == std::tuple(2023, 4u, 5u));
#endif // _DEBUG

	// year/month/day to UTC time_t
	constexpr time_t to_time_t(const ymd& ymd)
	{
		return sys_days(ymd).time_since_epoch().count() * seconds_per_day;
	}
#ifdef _DEBUG
	// Not guaranteed to be true.
	static_assert(to_time_t(to_ymd(1970, 1, 1)) == 0);
    static_assert(to_time_t(to_ymd(1970, 1, 2)) == seconds_per_day);
#endif // _DEBUG

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
	static_assert(diffseconds(2023y / 4 / 5), 2023y/ 4/ 4) == seconds_per_day);
	static_assert(diffseconds(to_ymd(2023, 4, 5), to_ymd(2023, 4, 6)) == -seconds_per_day);
#endif // _DEBUG

	// Time in years from d0 to d1.
	constexpr double diffyears(ymd d1, ymd d0)
	{
		return static_cast<double>(diffseconds(d1, d0)) / seconds_per_year;
	}
	constexpr ymd addyears(ymd d, double ys)
	{
		return from_time_t(to_time_t(d) + static_cast<time_t>(ys * seconds_per_year));
	}

	constexpr int diffmonths(ymd d1, ymd d0)
	{
		int y1{ d1.year() };
		int y0{ d0.year() };
		unsigned m1{ d1.month() };
		unsigned m0{ d0.month() };

		return 12 * (y1 - y0) + m1 - m0;
	}
#ifdef _DEBUG
	static_assert(diffmonths(2023y / 5 / 5, 2023y / 4 / 5) == 1);
	static_assert(diffmonths(2023y / 5 / 5, 2023y / 6 / 5) == -1);
	static_assert(diffmonths(2024y / 5 / 5, 2023y / 6 / 5) == 11);
	static_assert(diffmonths(2024y / 5 / 5, 2023y / 5 / 5) == 12);
	static_assert(diffmonths(2024y / 5 / 5, 2023y / 7 / 5) == 10);
#endif // _DEBUG
#endif // 0
} // namespace tmx::date
